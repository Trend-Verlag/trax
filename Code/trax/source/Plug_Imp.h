//	trax track library
//	AD 2014 
//
//  "Oh! Hark!"
//
//			Lisa Mitchell
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

#include "trax/Plug.h"
#include "trax/ObjectID.h"
#include "trax/SocketRegistry.h"
#include "Jack_Imp.h"


namespace trax{


	template<class ParentPlugType>
	class MultiPlug_Imp :	public MultiPlug,
							public ParentPlugType{
	public:
		MultiPlug_Imp( typename ParentPlugType::TargetType& parent )
			: ParentPlugType{ parent }
		{}

		MultiPlug_Imp( typename ParentPlugType::TargetType& parent, void (ParentPlugType::TargetType::*pPulseFunction)() )
			: ParentPlugType{ parent, pPulseFunction }
		{}

		template<typename ValueType>
		MultiPlug_Imp( typename ParentPlugType::TargetType& parent, void (ParentPlugType::TargetType::*pPulseFunction)(ValueType), ValueType value )
			: ParentPlugType{ parent, pPulseFunction, value }
		{}

		const char*	TypeName() const noexcept override{
			return "MultiPlug";
		}

		const std::string& Reference( const std::string& name ) const override{
			return ParentPlugType::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return ParentPlugType::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return ParentPlugType::ID();
		}

		void ID( IDType id ) noexcept override{
			return ParentPlugType::ID( id );
		}

		Jack* Plugged() const noexcept override{
			return ParentPlugType::Plugged();
		}

		const Plug& Tail() const override{
			return ParentPlugType::Tail();
		}

		Plug& Tail() override{
			return ParentPlugType::Tail();
		}

		Jack& JackOnPulse() noexcept override{
			return ParentPlugType::JackOnPulse();
		}

		const Jack& JackOnPulse() const noexcept override{
			return ParentPlugType::JackOnPulse();
		}

		PlugEnumerator& Parent() const noexcept override{
			return ParentPlugType::Parent();
		}

		Plug& Make( SocketRegistry* pRegistry = nullptr ) override{
			m_PlugClones.push_back( std::make_unique<Plug_Clone>( *this ) );
			Plug& plug = *m_PlugClones.back().get();

			plug.Reference( "name", Reference( "name" ) );
			if( pRegistry )
				pRegistry->RegisterPlug( plug );

			return plug;
		}

		Plug& Unplugged( SocketRegistry* pRegistry = nullptr ) override{
			if( !Plugged() )
				return *this;

			auto fiter = std::find_if( m_PlugClones.begin(), m_PlugClones.end(),
				[]( std::unique_ptr<Plug_Clone>& pPlug ) noexcept{ return !pPlug->Plugged(); } );
			if( fiter != m_PlugClones.end() )
				return *(fiter->get());

			return Make( pRegistry );
		}

		void Release( Plug& plug ) override{
			auto fiter = std::find_if( m_PlugClones.begin(), m_PlugClones.end(),
				[&plug]( std::unique_ptr<Plug_Clone>& c ) noexcept{ return c.get() == &plug; } );
			if( fiter != m_PlugClones.end() )
				m_PlugClones.erase( fiter );
		}

		void Clear() noexcept override{
			m_PlugClones.clear();
		}

		int CountPlugs() const override{
			return common::narrow_cast<int>(m_PlugClones.size() + 1);
		}
	protected:
		using MultiPlug::Release;

		const Plug& _GetPlug( int idx ) const override{
			if( idx == 0 )
				return *this;

			return *m_PlugClones.at(--idx).get();
		}

		void Pulse( bool bForewardToJack ) noexcept override{
			ParentPlugType::Pulse( bForewardToJack );
		}

	private:
		class Plug_Clone : public Plug_Imp{
		public:
			Plug_Clone( MultiPlug_Imp& parent )
				: Plug_Imp( parent.Parent() ),
				m_Parent( parent )
			{}

		protected:
			void Pulse( bool bForewardToJack ) noexcept override{
				m_Parent.Pulse( false );
				Plug_Imp::Pulse( bForewardToJack );
			}

		private:
			MultiPlug_Imp& m_Parent;
		};

		std::vector<std::unique_ptr<Plug_Clone>> m_PlugClones;
	};




	template<class ParentType, typename ValueType>
	class Plug_Imp_ParentPointerValue : public Plug_Imp_Parent<ParentType>{
#if defined(_MSC_VER) && !defined(_WIN64)
#	pragma pack(push, 4) // alignment of a member was sensitive to packing
#endif
		void (ParentType::*m_pPulseFunction)( ValueType );
		ValueType m_Value;
#if defined(_MSC_VER) && !defined(_WIN64)
#	pragma pack(pop)
#endif
	public:
		using TargetType = ParentType;

		Plug_Imp_ParentPointerValue( const Plug_Imp_ParentPointerValue& plug ) = delete;
		Plug_Imp_ParentPointerValue( Plug_Imp_ParentPointerValue&& plug ) = default;
		Plug_Imp_ParentPointerValue( ParentType& parent, void (ParentType::*pPulseFunction)( ValueType ), ValueType value )
			:	Plug_Imp_Parent<ParentType>	{parent},
				m_pPulseFunction			{pPulseFunction},
				m_Value						{value}
		{
			assert(m_pPulseFunction);
		}

		void Pulse( bool bForewardToJack ) noexcept override{
			(Plug_Imp_Parent<ParentType>::m_Parent.*m_pPulseFunction)( m_Value );
			Plug_Imp_Parent<ParentType>::Pulse( bForewardToJack );
		}
	};


	template<class _TargetType, class StatusProvider = _TargetType>
	class StatusHolder_Plug : public Plug_Imp{
	public:
		using TargetType = _TargetType;

		StatusHolder_Plug( TargetType& target )
			:	Plug_Imp	( target ),
				m_TargetType( target ),
				m_Status	( StatusProvider::Status::none )
		{}

		StatusHolder_Plug( TargetType& target, typename StatusProvider::Status status )
			:	Plug_Imp	(target),
				m_TargetType(target),
				m_Status	(status)
		{}

		StatusHolder_Plug& operator=( const StatusHolder_Plug& ) = delete; // no assignment
		
		void SetStatus( typename StatusProvider::Status status ) noexcept{
			m_Status = status;
		}
	protected:
		void Pulse( bool bForewardToJack ) noexcept override{
			try{
				m_TargetType.Set( m_Status );
			}
			catch( const std::exception& ){
				assert( !"Try to set incomplete or ill formed item!" );
			}
			Plug_Imp::Pulse( bForewardToJack );
		}

	private:
		TargetType&						m_TargetType;
		typename StatusProvider::Status	m_Status;
	};

	template<class _TargetType>
	class Toggle_Pug : public Plug_Imp{
	public:
		using TargetType = _TargetType;

		Toggle_Pug( TargetType& target )
			:	Plug_Imp(target),
				m_Target(target)
		{}

		Toggle_Pug& operator=( const Toggle_Pug& ) = delete; // no assignment

	protected:
		void Pulse( bool bForewardToJack ) noexcept override{
			Plug_Imp::Pulse( bForewardToJack );
			try{
				m_Target.Toggle();
			}
			catch( const std::exception& ){
				assert( !"Try to set incomplete or ill formed item!" );
			}
		}
	private:
		TargetType& m_Target;
	};


	/// \brief A Jack that provides a MultiPlug to collect pulses from
	/// multiple sources.
	class MultiPlugJack_Imp :	public Jack_Imp,
								public PlugEnumerator{
	public:
		MultiPlugJack_Imp( const std::string& name )
		: Jack_Imp		{ name }
		, m_PlugToPulse	{ *this, &MultiPlugJack_Imp::Pulse }
		{
			m_PlugToPulse.Reference( "name", "PlugToPulse" );
		}

		const char*	TypeName() const noexcept override{
			return "MultiPlugJack";
		}

		MultiPlug& PlugToPulse() noexcept{
			return m_PlugToPulse;
		}

		int CountPlugs() const override{
			return m_PlugToPulse.CountPlugs();
		}

		/// \brief Removes all plugs created to trigger this jack.
		void ClearPlugs() noexcept{
			m_PlugToPulse.Clear();
		}
	protected:
		const Plug& _GetPlug( int idx ) const override{
			return m_PlugToPulse.GetPlug( idx );
		}
	private:
		void Pulse() noexcept{
			Jack_Imp::Pulse();
		}

		MultiPlug_Imp<Plug_Imp_ParentPointer<MultiPlugJack_Imp>> m_PlugToPulse;
	};
}