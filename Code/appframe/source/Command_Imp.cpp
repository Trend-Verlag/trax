//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "Nicht was Dir geschieht ist wichtig, sondern wie du darauf reagierst"
//
//							Marc Aurel



#include "appframe/Command.h"


namespace cmnd{
///////////////////////////////////////
const char* Command::Name() const{
	return typeid(*this).name();
}

const char* Command::Call() const{
    thread_local std::string buffer;
    buffer = Name();
    buffer += "()";
    return buffer.c_str(); 
}

void Command::ParameterFrom( std::istream& istream ){
	//char openBracket;
	//istream >> openBracket;
	//if( !istream || openBracket != '(' )
	//	throw std::runtime_error( "Missing opening parenthesis!" );

	ReadParameterList(istream);
	if( !istream )
		throw std::runtime_error( "Invalid parameter!" );

	char closingBracket;
	istream >> closingBracket;
	if( !istream || closingBracket != ')' )
		throw std::runtime_error( "Missing closing parenthesis!" );
}

void Command::TimeStamp( long time ){
	m_TimeStamp = time;
}

long Command::TimeStamp() const noexcept{
	return m_TimeStamp;
}

void Command::SetAllTimeStamps( long time ){
	TimeStamp( time );
}

void Command::Freeze() noexcept{
	m_bFrozen = true;
}

bool Command::IsFrozen() const noexcept{
	return m_bFrozen;
}

void* Command::dll_alloc( std::size_t n ){
	return std::malloc(n);
}

void Command::dll_free( void* p ) noexcept{
	std::free( p );
}
//
//std::unique_ptr<Command> Command::Clone( trax::Real value ) const{
//	auto pCommand = Clone();
//	pCommand->SetValue( value );
//	return pCommand;
//}
//
//std::unique_ptr<Command> Command::Clone( int value ) const{
//	auto pCommand = Clone();
//	pCommand->SetValue( value );
//	return pCommand;
//}
//
//std::unique_ptr<Command> Command::Clone( bool value ) const{
//	auto pCommand = Clone();
//	pCommand->SetValue( value );
//	return pCommand;
//}
//
//std::unique_ptr<Command> Command::Clone(trax::IDType value) const{
//	auto pCommand = Clone();
//	pCommand->SetValue( value );
//	return pCommand;
//}
//
//std::unique_ptr<Command> Command::Clone( const std::string& value ) const{
//	auto pCommand = Clone();
//	pCommand->SetValue( value );
//	return pCommand;
//}
//
//std::unique_ptr<Command> Command::Clone( spat::Vector<trax::Real>& value ) const{
//	auto pCommand = Clone();
//	pCommand->SetValue( value );
//	return pCommand;
//}
//
//std::unique_ptr<Command> Command::Clone( const spat::Frame<trax::Real>& value ) const{
//	auto pCommand = Clone();
//	pCommand->SetValue( value );
//	return pCommand;
//}
///////////////////////////////////////
const Macro* Cast( const Command* command ) noexcept{
	return dynamic_cast<const Macro*>( command );
}

Macro* Cast( Command* command ) noexcept{
	return dynamic_cast<Macro*>( command );
}
///////////////////////////////////////

///////////////////////////////////////

}

