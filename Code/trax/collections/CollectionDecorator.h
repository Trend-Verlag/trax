//	trax track library
//	AD 2014 
//
//  "Were do the children play?"
//
//						Cat Stevens
//
// Copyright (c) 2025 Trend Redaktions- und Verlagsgesellschaft mbH
// Copyright (c) 2019 Marc-Michael Horstmann
//
// Permission is hereby granted to any person obtaining a copy of this software 
// and associated source code (the "Software"), to use, view, and study the 
// Software for personal or internal business purposes, subject to the following 
// conditions:
//
// 1. Redistribution, modification, sublicensing, or commercial use of the 
// Software is NOT permitted without prior written consent from the copyright 
// holder.
//
// 2. The Software is provided "AS IS", without warranty of any kind, express 
// or implied.
//
// 3. All copies of the Software must retain this license notice.
//
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/IDType.h"

#include <string>
#include <memory>

namespace trax{


	/// \brief A decorator for trax collections.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library insted of the original object.
	template<class BaseDecorator>
	class CollectionDecorator : public BaseDecorator{
	public:

		/// \name Type definitions
		/// \brief These are the type definitions expected to be defined
		/// by base decorators or the decoree itself.
		///@{
		typedef typename BaseDecorator::InterfaceType InterfaceType;
		typedef typename BaseDecorator::collection_type collection_type;
		typedef typename BaseDecorator::value_type value_type;
		///@}

		/// \brief Decorator constructor.
		/// \param pComponent Pointer to object that is to be decorated.
		CollectionDecorator( std::shared_ptr<InterfaceType> pComponent )
			: BaseDecorator{ pComponent }
		{}

		/// \name Collection member function forwarding.
		///@{
		const char* TypeName() const noexcept override{
			return m_pComponent->TypeName();
		}

		IDType Add( std::shared_ptr<value_type> pItem ) override{
			return m_pComponent->Add(pItem);
		}

		IDType AddRelaxed( std::shared_ptr<value_type> pItem ) override{
			return m_pComponent->AddRelaxed(pItem);
		}

		bool Remove( value_type* pItem, bool zeroIDs = false ) override{
			return m_pComponent->Remove( pItem, zeroIDs );
		}
		
		int Take( collection_type& ct ) override{
			return m_pComponent->Take(ct);
		}

		void Clear() override{
			return m_pComponent->Clear();
		}

		int Count() const override{
			return m_pComponent->Count();
		}

		typename collection_type::iterator begin() override{
			return m_pComponent->begin();
		}

		typename collection_type::const_iterator begin() const override{
			return m_pComponent->cbegin();
		}

		typename collection_type::const_iterator cbegin() const override{
			return m_pComponent->cbegin();
		}

		typename collection_type::iterator end() override{
			return m_pComponent->end();
		}

		typename collection_type::const_iterator end() const override{
			return m_pComponent->cend();
		}

		typename collection_type::const_iterator cend() const override{
			return m_pComponent->cend();
		}

		std::shared_ptr<value_type> GetFirst() const override{
			return m_pComponent->GetFirst();
		}

		std::shared_ptr<value_type> GetNext( const std::shared_ptr<value_type>& pItem ) const override{
			return m_pComponent->GetNext(pItem);
		}

		std::shared_ptr<value_type> GetLast() const override{
			return m_pComponent->GetLast();
		}

		std::shared_ptr<value_type> GetPrevious( const std::shared_ptr<value_type>& pItem ) const override{
			return m_pComponent->GetPrevious(pItem);
		}

		std::shared_ptr<value_type> Get( IDType id ) const override{
			return m_pComponent->Get(id);
		}

		std::shared_ptr<value_type> Get( const std::string& name ) const override{
			return m_pComponent->Get(name);
		}

		void PushActive( IDType id ) override{
			return m_pComponent->PushActive(id);
		}
		
		void PopActive() override{
			return m_pComponent->PopActive();
		}

		std::shared_ptr<value_type> GetActive() const override{
			return m_pComponent->GetActive();
		}

		bool IsMember( const value_type& item ) const override{
			return m_pComponent->IsMember(item);
		}

		bool IsMember( IDType id ) const override{
			return m_pComponent->IsMember(id);
		}

		void ShiftIDs( int offset ) override{
			return m_pComponent->ShiftIDs( offset );
		}

		IDType MaxID() const override{
			return m_pComponent->MaxID();
		}

		IDType MinID() const override{
			return m_pComponent->MinID();
		}
		///@}
	protected:
		using BaseDecorator::m_pComponent;
	};

}