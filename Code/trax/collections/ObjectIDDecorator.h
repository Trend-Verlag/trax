// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "it's the end of the world as we know it"
//
//								R.E.M.

#pragma once

#include "trax/IDType.h"
#include "trax/Units.h"
#include "spat/Frame.h"

#include <memory>

#include "trax/Identified.h"

namespace trax{


	/// \brief The base for decorator implementations. 
	/// Needed as BaseDecorator to combine other decorator implementers.
	template<class InterfaceT>
	class Decorator : public InterfaceT{
	public:
		/// \brief Type of the interface the decorator is decorating.
		typedef InterfaceT InterfaceType;

		Decorator() = delete;
		Decorator( const Decorator& ) = delete; // no copy constructor (at least as long m_pComponent stays unique)
		Decorator( Decorator&& ) = delete;

		/// \brief Decorator constructor.
		/// \param pComponent Pointer to object that is to be decorated.
		Decorator( std::shared_ptr<InterfaceType> pComponent )
			:	InterfaceT	{},
				m_pComponent{ pComponent }
		{
			assert( m_pComponent );
			m_pComponent->SetDecorator( this );
		}

		Decorator& operator=( const Decorator& ) = delete; // no copy assignment
		Decorator& operator=( Decorator&& ) = delete; // no move assignment

		/// \brief Decorator member function forwarding.
		InterfaceType* Component() const noexcept{
			return m_pComponent.get();
		}
	protected:
		std::shared_ptr<InterfaceType> m_pComponent; ///< Pointer to decorated object.

		void SetDecorator( InterfaceType* pDecorator ) noexcept override{
			m_pComponent->SetDecorator( pDecorator );
		}
	};


	/// \brief Cast to a decorator type in the decorator chain or to the
	/// component itself.
	/// \param pInterface An interface type. Supposed not to be a decorator
	/// type but the decorated interface.
	template<class DecoratorPointerType, class InterfaceType>
	DecoratorPointerType decorator_cast( InterfaceType* pInterface ) noexcept{
		if( auto pDecorator = dynamic_cast<Decorator<InterfaceType>*>(pInterface) ){
			if( auto target = dynamic_cast<DecoratorPointerType>(pDecorator) )
				return target;

			return decorator_cast<DecoratorPointerType>( pDecorator->Component() );
		}
		
		// it is no decorator
		return dynamic_cast<DecoratorPointerType>(pInterface);
	}


	///// \brief Finds the decoree
	//template<class DecoreePointerType, class InterfaceType>
	//DecoreePointerType* decoree_cast( InterfaceType* pInterface ){


	//}


	/// \brief A decorator for identification part of trax objects.
	template<class BaseDecorator>
	class ObjectIDDecorator : public BaseDecorator{
	protected:
		using BaseDecorator::m_pComponent;
	public:
		/// \brief Type of the interface the decorator is decorating.
		typedef typename BaseDecorator::InterfaceType InterfaceType;

		/// \brief Decorator constructor.
		/// \param pComponent Pointer to object that is to be decorated.
		ObjectIDDecorator( std::shared_ptr<InterfaceType> pComponent )
			: BaseDecorator{ pComponent }
		{}

		const std::string& Reference( const std::string& name ) const override{
			return m_pComponent->Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return m_pComponent->Reference( name, reference );
		}

		const std::vector<char const *>& ReferenceNames( const std::string& namePart ) const override{
			return m_pComponent->ReferenceNames( namePart );
		}

		/// \brief ObjectIDDecorator member function forwarding.
		IDType ID() const noexcept override{
			return m_pComponent->ID();
		}

		/// \brief ObjectIDDecorator member function forwarding.
		void ID	( IDType id ) noexcept override{
			return m_pComponent->ID(id);
		}
	};


	/// \brief A decorator for pose part of trax objects.
	template<class BaseDecorator>
	class PoseDecorator : public BaseDecorator{
	protected:
		using BaseDecorator::m_pComponent;
	public:
		/// \brief Type of the interface the decorator is decorating.
		typedef typename BaseDecorator::InterfaceType InterfaceType;

		/// \brief Decorator constructor.
		/// \param pComponent Pointer to object that is to be decorated.
		PoseDecorator( std::shared_ptr<InterfaceType> pComponent )
			: BaseDecorator{ pComponent }
		{}

		/// \brief PoseDecorator member function forwarding.
		void SetFrame( const spat::Frame<Length,One>& frame ) noexcept/*(noexcept(m_pComponent->SetFrame(frame)))*/ override{
			return m_pComponent->SetFrame(frame);
		}

		/// \brief PoseDecorator member function forwarding.
		const spat::Frame<Length,One>& GetFrame() const noexcept/*(noexcept(m_pComponent->GetFrame()))*/ override{
			return m_pComponent->GetFrame();
		}
	};

}