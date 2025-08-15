// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "Plug_Imp.h"

#include "trax/ImplementationHelper.h"

namespace trax{
///////////////////////////////////////
Plug_Imp::Plug_Imp( Plug_Imp&& plug ) noexcept
	:	ObjectID_Imp<Plug>	{ std::move(plug) },
		m_pPlugged			{ nullptr },
		m_JackOnPulse		{ std::move(plug.m_JackOnPulse) },
		m_Parent			{ plug.m_Parent }
{
	if( plug.m_pPlugged )
		plug.m_pPlugged->Insert( this );
}

Plug_Imp::Plug_Imp( PlugEnumerator& parent )
	:	m_pPlugged		{ nullptr },
		m_Parent		{ parent }
{}

Plug_Imp::Plug_Imp( PlugEnumerator& parent, Plug_Imp&& plug )
	:	m_pPlugged		{ nullptr },
		m_JackOnPulse	{ std::move(plug.m_JackOnPulse) },
		m_Parent		{ parent }
{	
	if( plug.m_pPlugged )
		plug.m_pPlugged->Insert( this );
}

Plug_Imp::~Plug_Imp() noexcept{
	Remove();
}

Jack* Plug_Imp::Plugged() const noexcept{
	return m_pPlugged;
}

bool Plug_Imp::IsCyclic() const noexcept{
	Plug* pPlug = m_JackOnPulse.GetPlug();
	while( pPlug ){
		if( pPlug == this )
			return true;

		pPlug = pPlug->JackOnPulse().GetPlug();
	}

	return false;
}

const Plug& Plug_Imp::Tail() const{
	if( !m_JackOnPulse.Plugged() )
		return *this;

	if( IsCyclic() )
		throw std::logic_error( "Cyclic connection. No tail available!" );

	return m_JackOnPulse.GetPlug()->Tail();
}

Plug& Plug_Imp::Tail(){
	if( !m_JackOnPulse.Plugged() )
		return *this;

	if( IsCyclic() )
		throw std::logic_error( "Cyclic connection. No tail available!" );

	return m_JackOnPulse.GetPlug()->Tail();
}

Jack& Plug_Imp::JackOnPulse() noexcept{
	return m_JackOnPulse;
}

const Jack& Plug_Imp::JackOnPulse() const noexcept{
	return m_JackOnPulse;
}

PlugEnumerator& Plug_Imp::Parent() const noexcept{
	return m_Parent;
}

void Plug_Imp::Remove() noexcept
{
	if( m_pPlugged )
	// reconnect the Jack  with the plug 
	// plugged into this Plug's Jack
	{
		m_pPlugged->Insert( m_JackOnPulse.GetPlug() );
		m_pPlugged = nullptr;
	}
}

int Plug_Imp::CountJacks() const noexcept{
	return 1;
}

void Plug_Imp::Pulse( bool bForewardToJack ) noexcept{
	if( bForewardToJack )
		m_JackOnPulse.Pulse();
}

const Jack& Plug_Imp::_GetJack( int idx ) const{
	if( idx == 0 )
		return m_JackOnPulse;

	assert( 0 );
	std::ostringstream stream;
	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
	throw std::range_error( stream.str() );
}

void Plug_Imp::Release() noexcept{
	m_pPlugged = nullptr;
}

void Plug_Imp::ConnectTo( Jack& jack ) noexcept{
	m_pPlugged = &jack;
}
///////////////////////////////////////
const Plug* GetFirstNonZeroIDPlugInChain( const Plug& plug ){
	if( plug.ID() )
		return &plug;

	if( !plug.JackOnPulse().Plugged() )
		return nullptr;

	return GetFirstNonZeroIDPlugInChain( *plug.JackOnPulse().GetPlug() );
}
///////////////////////////////////////
}