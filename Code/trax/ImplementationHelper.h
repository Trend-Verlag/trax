//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "trax/ObjectID.h"
#include "trax/Units.h"

#include <stack>
#include <map>
#include <memory>

#include "spat/Frame.h"

namespace trax{

	template<class Base>
	class Pose_Imp : public Base{
	public:
		Pose_Imp() noexcept{
			m_Frame.Init();
			m_AbsoluteFrame.Init();
		}

		void SetFrame( const spat::Frame<Length,One>& frame ) noexcept override{
			//assert( frame.IsOrthoNormal() );
			m_Frame = frame;
			PropagateAbsoluteFrameToClients();
		}

		const spat::Frame<Length,One>& GetFrame() const noexcept override{
			return m_Frame;
		}

#if defined(__clang__)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif 
		virtual void SetAbsoluteFrame( const spat::Frame<Length,One>& frame ) noexcept
		 // no 'override' here, since the template might introduce the method for some Bases.
		{
			//assert( frame.IsOrthoNormal() );
			m_AbsoluteFrame = frame;
			PropagateAbsoluteFrameToClients();
		}

		virtual const spat::Frame<Length,One>& GetAbsoluteFrame() const noexcept
		 // no 'override' here, since the template might introduce the method for some Bases.
		{
			return m_AbsoluteFrame;
		}
#if defined(__clang__)
#	pragma GCC diagnostic pop
#endif

		void SetFrames( const spat::Frame<Length,One>& frame, const spat::Frame<Length,One>& absolutFrame ) noexcept{
			m_Frame = frame;
			m_AbsoluteFrame = absolutFrame;
			PropagateAbsoluteFrameToClients();
		}
	protected:
		// Override this to brief clients about a change of its
		// absolute frame of reference.
		virtual void PropagateAbsoluteFrameToClients() noexcept{}
	private:
		spat::Frame<Length,One> m_Frame;
		spat::Frame<Length,One> m_AbsoluteFrame;
	};


	template<class TraxType, class Base>
	class Container_Imp : public Base{
	public:
		Container_Imp() noexcept;

		using Base::Add;
		using Base::Remove;
		using Base::Take;
		using Base::GetNext;
		using Base::IsMember;

		bool										IsValid		() const noexcept override;

		IDType										Add			( std::shared_ptr<TraxType> pTraxType ) override;

		IDType										AddRelaxed	( std::shared_ptr<TraxType> pTraxType ) override;

		bool										Remove		( TraxType* pTraxType, bool zeroIDs = false ) override;

		int											Take 		( typename Base::collection_type& collection ) override;

		void										Clear		() noexcept override;

		int											Count		() const noexcept override;

		typename Base::collection_type::iterator	begin		() noexcept override;

		typename Base::collection_type::iterator	end			() noexcept override;

		typename Base::collection_type::const_iterator begin() const noexcept override;

		typename Base::collection_type::const_iterator cbegin() const noexcept override;

		typename Base::collection_type::const_iterator end() const noexcept override;

		typename Base::collection_type::const_iterator cend() const noexcept override;

		std::shared_ptr<TraxType>					GetFirst	() const noexcept override;

		std::shared_ptr<TraxType>					GetNext		( const std::shared_ptr<TraxType>& pTraxType ) const override;

		std::shared_ptr<TraxType>					GetLast		() const override;

		std::shared_ptr<TraxType>					GetPrevious	( const std::shared_ptr<TraxType>& pTraxType ) const override;

		std::shared_ptr<TraxType>					Get			( IDType id ) const override;

		std::shared_ptr<TraxType>					Get			( const std::string& name ) const override;

		void										PushActive	( IDType id ) override;

		void										PopActive	() override;

		std::shared_ptr<TraxType>					GetActive	() const override;

		bool										IsMember	( const TraxType& item ) const override;

		bool										IsMember	( IDType id  ) const override;

		void										ShiftIDs	( int offset ) override;

		IDType										MaxID		() const override;

		IDType										MinID		() const override;


	protected:
		void SetDecorator( Base* pDecorator ) noexcept override; // has to derive immediatley from the collection interface
		Base* Decorator() const noexcept{ return m_pDecorator; }

		using ContainerType = std::map<IDType,std::shared_ptr<TraxType>>;
		ContainerType m_Container;
	private:
		Base* m_pDecorator;

		bool IsFree( IDType id ) const{
			return m_Container.find( id ) == m_Container.end();
		}

		std::stack<IDType> m_ActiveStack;

		IDType GetFree() const{
			IDType x = 1;
			for( auto citer = m_Container.begin();
				citer != m_Container.end(); ++x, ++citer )
			{
				if( x == (*citer).first )
					continue;

				return x;
			}

			return x;
		}
	};


// inlines:
///////////////////////////////////////
template<class TraxType, class Base>
inline Container_Imp<TraxType, Base>::Container_Imp() noexcept
	:	Base{}
{
	SetDecorator( this );
}

template<class TraxType, class Base>
inline bool Container_Imp<TraxType, Base>::IsValid() const noexcept
{
	return false;
}

template<class TraxType, class Base>
IDType Container_Imp<TraxType,Base>::Add( std::shared_ptr<TraxType> pTraxType ){
	if( !pTraxType )
		return 0;

	if( Decorator()->IsMember( *pTraxType ) ){
		Decorator()->PushActive( pTraxType->ID() );
		return pTraxType->ID();
	}
		
	if( pTraxType->ID() ){
		if( !IsFree( pTraxType->ID() ) )
			throw std::invalid_argument( "The item ID is already used in the container! ID: " + std::to_string( pTraxType->ID().GetID() ) );
	}
	else
		pTraxType->ID( GetFree() );

	m_Container.insert( std::make_pair( pTraxType->ID(), pTraxType ) );
	Decorator()->PushActive(pTraxType->ID());

	return pTraxType->ID();
}

template<class TraxType, class Base>
IDType Container_Imp<TraxType,Base>::AddRelaxed( std::shared_ptr<TraxType> pTraxType ){
	if( !pTraxType )
		return 0;

	if( Decorator()->IsMember( *pTraxType ) ){
		Decorator()->PushActive( pTraxType->ID() );
		return pTraxType->ID();
	}
		
	if( pTraxType->ID() ){
		if( !IsFree( pTraxType->ID() ) )
			pTraxType->ID( GetFree() );
	}
	else
		pTraxType->ID( GetFree() );

	m_Container.insert( std::make_pair( pTraxType->ID(), pTraxType ) );
	Decorator()->PushActive(pTraxType->ID());

	return pTraxType->ID();
}

template<class TraxType, class Base>
bool Container_Imp<TraxType,Base>::Remove( TraxType* pTraxType, bool zeroIDs ){
	assert( pTraxType );

	if( pTraxType ){
		if( zeroIDs ){
			const IDType id = pTraxType->ID();
			pTraxType->ID( 0 );
			if( m_Container.erase( id ) > 0 )
				return true; // the object might be deleted here
			else
				pTraxType->ID( id );
		}
		else
			return m_Container.erase( pTraxType->ID() ) > 0;
	}

	return false;
}

template<class TraxType, class Base>
int Container_Imp<TraxType,Base>::Take( typename Base::collection_type& collection ){
	const int offset = MaxID();
	collection.ShiftIDs( offset ); // make sure the ids stay unique.

	auto pTraxType = collection.GetFirst();
	while( pTraxType ){
		collection.Remove( pTraxType.get() );
		Decorator()->Add( pTraxType );

		pTraxType = collection.GetFirst();
	}

	return offset;
}

template<class TraxType, class Base>
std::shared_ptr<TraxType> Container_Imp<TraxType,Base>::GetNext( const std::shared_ptr<TraxType>& pTraxType ) const{
	assert( pTraxType );
	auto iter = m_Container.find( pTraxType->ID() );
	if( iter != m_Container.end() &&
		++iter != m_Container.end() )
		return (*iter).second;

	return nullptr;
}

template<class TraxType, class Base>
std::shared_ptr<TraxType> Container_Imp<TraxType,Base>::GetPrevious( const std::shared_ptr<TraxType>& pTraxType ) const{
	assert( pTraxType );
	auto iter = m_Container.find( pTraxType->ID() );
	if( iter != m_Container.end() &&
		iter != m_Container.begin() )
		return (*--iter).second;

	return nullptr;
}

template<class TraxType, class Base>
bool Container_Imp<TraxType,Base>::IsMember( const TraxType& item ) const{
	if( !item.ID() )
		return false;

	const auto iter = m_Container.find( item.ID() );
	if( iter == m_Container.end() )
		return false;

	return iter->second.get() == &item;
}

template<class TraxType, class Base>
void Container_Imp<TraxType,Base>::Clear() noexcept{
	m_Container.clear();
	while( !m_ActiveStack.empty() )
		m_ActiveStack.pop();
}

template<class TraxType, class Base>
int Container_Imp<TraxType,Base>::Count() const noexcept{
	return static_cast<int>(m_Container.size());
}

template<class TraxType, class Base>
typename Base::collection_type::iterator Container_Imp<TraxType,Base>::begin() noexcept{
	return { m_Container.begin() };
}

template<class TraxType, class Base>
typename Base::collection_type::iterator Container_Imp<TraxType,Base>::end() noexcept{
	return { m_Container.end() };
}

template<class TraxType, class Base>
typename Base::collection_type::const_iterator Container_Imp<TraxType,Base>::begin() const noexcept{
	return { m_Container.begin() };
}

template<class TraxType, class Base>
typename Base::collection_type::const_iterator Container_Imp<TraxType,Base>::end() const noexcept{
	return { m_Container.end() };
}

template<class TraxType, class Base>
typename Base::collection_type::const_iterator Container_Imp<TraxType,Base>::cbegin() const noexcept{
	return { m_Container.begin() };
}

template<class TraxType, class Base>
typename Base::collection_type::const_iterator Container_Imp<TraxType,Base>::cend() const noexcept{
	return { m_Container.end() };
}

template<class TraxType, class Base>
std::shared_ptr<TraxType> Container_Imp<TraxType,Base>::GetFirst() const noexcept{
	const auto iter = m_Container.begin();
	if( iter != m_Container.end() )
		return (*iter).second;

	return nullptr;
}

template<class TraxType, class Base>
std::shared_ptr<TraxType> Container_Imp<TraxType,Base>::GetLast() const{
	const auto iter = m_Container.rbegin();
	if( iter != m_Container.rend() )
		return (*iter).second;

	return nullptr;
}

template<class TraxType, class Base>
std::shared_ptr<TraxType> Container_Imp<TraxType,Base>::Get( IDType id ) const{
	const auto iter = m_Container.find( id );
	if( iter != m_Container.end() )
		return (*iter).second;

	return nullptr;
}

template<class TraxType, class Base>
std::shared_ptr<TraxType> Container_Imp<TraxType,Base>::Get( const std::string& name ) const{
	for( auto iter = m_Container.begin(); iter != m_Container.end(); ++iter )
		if( (*iter).second->Reference( "name" ).compare( name ) == 0 )
			return (*iter).second;

	return nullptr;
}

template<class TraxType, class Base>
void Container_Imp<TraxType,Base>::PushActive( IDType id ){
	m_ActiveStack.push(id);
}

template<class TraxType, class Base>
void Container_Imp<TraxType,Base>::PopActive(){
	if( m_ActiveStack.empty() )
		throw std::underflow_error( "Popped an empty stack!" );

	m_ActiveStack.pop();
}

template<class TraxType, class Base>
std::shared_ptr<TraxType> Container_Imp<TraxType,Base>::GetActive() const{
	if( m_ActiveStack.empty() )
		return nullptr;

	return Decorator()->Get(m_ActiveStack.top());
}

template<class TraxType, class Base>
bool Container_Imp<TraxType,Base>::IsMember( IDType id ) const{
	return !IsFree( id );
}

template<class TraxType, class Base>
inline void Container_Imp<TraxType, Base>::ShiftIDs( int offset ){
	if( m_Container.empty() )
		return;
	if( MinID().GetID() + offset <= 0 )
		throw std::out_of_range( "Can not shift id space." );

	std::stack<IDType> tempStack;
	while( !m_ActiveStack.empty() ){
		tempStack.push( m_ActiveStack.top() + offset );
		m_ActiveStack.pop();
	}
	while( !tempStack.empty() ){
		m_ActiveStack.push( tempStack.top() );
		tempStack.pop();
	}

	ContainerType tempContainer;
	for( const auto& pair : m_Container ){
		pair.second->ID( pair.second->ID() + offset );
		tempContainer[pair.first+offset] = pair.second;
	}

	m_Container.swap( tempContainer );
}

template<class TraxType, class Base>
inline IDType Container_Imp<TraxType, Base>::MaxID() const{
	if( m_Container.empty() )
		return 0;

	return m_Container.rbegin()->first;
}

template<class TraxType, class Base>
inline IDType Container_Imp<TraxType, Base>::MinID() const{
	if( m_Container.empty() )
		return 0;

	return m_Container.begin()->first;
}

template<class TraxType, class Base>
void Container_Imp<TraxType, Base>::SetDecorator( Base* pDecorator ) noexcept{
	if( pDecorator )
		m_pDecorator = pDecorator;
	else
		m_pDecorator = this;
}

template<class ContainerType>
inline bool IsValid_Imp( const ContainerType& container ){
	bool bOK = true;
	for( const auto& pair : container ){
		auto& element = *(pair.second);
		if( !element.IsValid() ){
			bOK = false;
		}
	}

	return bOK;
}

}

