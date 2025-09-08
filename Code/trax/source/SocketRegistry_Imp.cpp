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

#include "SocketRegistry_Imp.h"

#include "trax/Jack.h"

#include <cassert>
#include <iostream>

namespace trax{
///////////////////////////////////////
std::unique_ptr<SocketRegistry> SocketRegistry::Make() noexcept{
	try{
		return std::make_unique<SocketRegistry_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
void SocketRegistry_Imp::RegisterPlug( Plug& plug ){
	if( plug.ID() )
		Connect(plug);
	else{
		m_Jacks.clear(); // no jacks can get connected if there are 
						 // zero id plugs, wich have to be assigned an id first.

		//std::clog << "Plug with zero ID! No jacks can get connected if there are zero id plugs, which have to be assigned an id first. This will not be a problem on import." << std::endl;
	}

	m_Plugs.insert( &plug );
}

void SocketRegistry_Imp::UnRegisterPlug( const Plug& plug ){
	m_Plugs.erase( plug.ID() );
}

Plug* SocketRegistry_Imp::GetPlug( IDType id ) const{
	const auto iter = m_Plugs.find( id );
	if( iter != m_Plugs.end() )
		return (*iter).second;

	return nullptr;
}

IDType SocketRegistry_Imp::MaxValidPlugID() const noexcept{
	return m_Plugs.GetMaxValid();
}

void SocketRegistry_Imp::ConnectJack( Jack& jack ){
	if( !jack.RefPlugID() || jack.GetPlug() )
		return; // can not connect 

	assert( std::find( m_Jacks.begin(), m_Jacks.end(), &jack ) == m_Jacks.end() );

	const auto iter = m_Plugs.find( jack.RefPlugID() );
	if( iter != m_Plugs.end() )
		jack.InsertAtTail( iter->second );
	else
		m_Jacks.push_back( &jack );
}

void SocketRegistry_Imp::RemoveJack( Jack& jack ) noexcept{
	m_Jacks.erase( std::remove( m_Jacks.begin(), m_Jacks.end(), &jack ), m_Jacks.end() );
}

void SocketRegistry_Imp::Clear() noexcept
{
	m_Plugs.clear();
	m_Jacks.clear();
}

void SocketRegistry_Imp::Connect( Plug& plug ){
	auto iter = std::find_if( m_Jacks.begin(), m_Jacks.end(),
		[&plug]( Jack* pJack ) noexcept{ return pJack->RefPlugID() == plug.ID(); } );

	if( iter != m_Jacks.end() ){
		(*iter)->InsertAtTail( &plug );
		m_Jacks.erase( iter );
	}

	//for( auto iter = m_Jacks.begin(); iter != m_Jacks.end(); ++iter ){
	//	if( (*iter)->RefPlugID() == plug.ID() ){
	//		(*iter)->Insert( &plug );
	//		m_Jacks.erase( iter );
	//		return;
	//	}
	//}
}
///////////////////////////////////////
} // namespace trax