// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "Jack_Imp.h"
#include "trax/Plug.h"

#include <cassert>

namespace trax{
///////////////////////////////////////
Jack_Imp::Jack_Imp( Jack_Imp&& jack ) noexcept
	:	ObjectID_Imp<Jack>	{ std::move(jack) },
		m_pPlug				{ jack.m_pPlug },
		m_bPulsing			{ jack.m_bPulsing },
		m_RefPlugID			{ jack.m_RefPlugID }/*,
		m_Parent			{ jack.m_Parent }*/
{
	assert( !m_bPulsing );

	if( m_pPlug )
		m_pPlug->ConnectTo( *this );

	jack.m_pPlug = nullptr;
}

Jack_Imp::Jack_Imp( const std::string& name )
	:	m_pPlug		{ nullptr },
		m_bPulsing	{ false }
{
	Reference( "name", name );
}
//
//iJack_Imp& Jack_Imp::operator=( Jack_Imp&& jack ){
//	ObjectID_Imp<Jack>::operator=( std::move(jack) );
//
//	m_pPlug = jack.m_pPlug;
//	m_bPulsing = jack.m_bPulsing;
//	m_RefPlugID = jack.m_RefPlugID;
//
//	assert( !m_bPulsing );
//
//	if( m_pPlug )
//		m_pPlug->ConnectTo( *this );
//
//	jack.m_pPlug = nullptr;
//}

Jack_Imp::~Jack_Imp() noexcept{
	DoClear();
}

const char* Jack_Imp::TypeName() const{
	return "Jack";
}

IDType Jack_Imp::RefPlugID() const noexcept{
	return m_RefPlugID;
}

void Jack_Imp::RefPlugID( IDType id ) noexcept{
	m_RefPlugID = id;
}

void Jack_Imp::Pulse() noexcept{
	if( m_bPulsing )
		return; // break recursions

	if( m_pPlug ){
		m_bPulsing = true;
		m_pPlug->Pulse( true );
		m_bPulsing = false;
	}
}

void Jack_Imp::Insert( Plug* pPlug ) noexcept{
	if( m_pPlug == pPlug )
		return;

	if( pPlug && pPlug->Plugged() )
		pPlug->Plugged()->Clear();

	Clear();

	m_pPlug = pPlug;
	if( m_pPlug )
		m_pPlug->ConnectTo( *this );
}

void Jack_Imp::InsertAtTail( Plug* pPlug ){
	if( !Plugged() )
		Insert( pPlug );
	else
		m_pPlug->Tail().JackOnPulse().Insert( pPlug );
}

void Jack_Imp::InsertAndAppend( Plug* pPlug ) noexcept{
	if( m_pPlug == pPlug )
		return;

	Plug* pOldPulg = m_pPlug;

	Insert( pPlug );

	if( pPlug && pOldPulg )
		pPlug->JackOnPulse().Insert( pOldPulg );
}

Plug* Jack_Imp::GetPlug() const noexcept{
	return m_pPlug;
}

bool Jack_Imp::Plugged() const noexcept{
	return m_pPlug ? true : false;
}

void Jack_Imp::Clear() noexcept{
	DoClear();
}

void Jack_Imp::DoClear() noexcept{
	assert( !m_bPulsing );

	if( m_pPlug ){
		m_pPlug->Release();
		m_pPlug = nullptr;
	}

	m_RefPlugID = 0u;
}

}