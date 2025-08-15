// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



#include "Signal_Imp.h"
#include "trax/Location.h"

namespace trax{
///////////////////////////////////////
Signal_Imp::Signal_Imp()
	: m_Status( Status::stop ),
	m_pTrackBuilder( nullptr ),
	m_PlugToToggle( *this )
{
	m_PlugToToggle.Reference( "name", "PlugToToggle" );
}

const char*	Signal_Imp::TypeName() const noexcept{
	return "Signal";
}

Signal::Status Signal_Imp::Set( Status status, bool pulse ){
	const Status retval = m_Status;
	m_Status = status;

	if( pulse ){
		JackOn( m_Status ).Pulse();

		if( retval != m_Status ){
			JackOnChange().Pulse();
			JackOnChangeTo( m_Status ).Pulse();
		}
	}

	return retval;
}

void Signal_Imp::Toggle( bool pulse ){
	Status status = static_cast<Status>(static_cast<int>(m_Status) + 1);
	if( status == Status::count )
		status = Status::stop;

	Set( status, pulse );
}

Signal::Status Signal_Imp::Get() const noexcept{
	return m_Status;
}

int Signal_Imp::CountStatus() const{
	return 0;
}

void Signal_Imp::Attach( TrackBuilder* pTrackBuilder ) noexcept{
	m_pTrackBuilder = pTrackBuilder;
}

TrackBuilder* Signal_Imp::TrackAttached() const noexcept{
	return m_pTrackBuilder;
}

void Signal_Imp::GetTrackRange( TrackRange& trackRange ) const{
	if( m_pTrackBuilder )
		m_pTrackBuilder->Attached( *this, &trackRange );
}

Location Signal_Imp::GetLocation() const{
	if( const TrackBuilder* pTrackBuilder = TrackAttached() ){
		if( auto pTrack = pTrackBuilder->This() ){
			TrackRange tr;
			if( pTrack->Attached( *this, &tr ) )
				return { pTrack, TrackLocation( tr.range.Far(), tr.range.Normal() ) };
		}
	}

	return {};
}

MultiPlug& Signal_Imp::PlugToToggle() noexcept{
	return m_PlugToToggle;
}

Jack& Signal_Imp::JackOnChange() noexcept{
	return m_JackOnChange;
}

Jack& Signal_Imp::JackOn( Status status ){
	switch( status ){
	case Status::change:
		return JackOnChange();
	case Status::clear:
	case Status::stop:
	case Status::caution:
	case Status::slow:
	case Status::doors_open:
	case Status::doors_close:
	case Status::uncouple:
	case Status::lights_on:
	case Status::jump:
	{
		std::ostringstream stream;
		stream << "Unapplicable enumeration value! (" << ToString( status ) << ')' << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}
	case Status::none:
	{
		std::ostringstream stream;
		stream << "Invalid enumeration value Signal::none. No Jack available! ("<< ToString( status ) << ')' << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}
	default:
	{
		std::ostringstream stream;
		stream << "Unknown enumeration value! ("<< static_cast<int>(status) << ')' << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}
	}
}

Jack& Signal_Imp::JackOnChangeTo( Status status ){
	std::ostringstream stream;
	stream << "Unknown JackOnChangeTo for enumeration value! ("<< ToString( status ) << ')' << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

MultiPlug& Signal_Imp::PlugTo( Status status ){
	switch( status ){
	case Status::toggle:
		return PlugToToggle();
	case Status::clear:
	case Status::stop:
	case Status::caution:
	case Status::slow:
	case Status::doors_open:
	case Status::doors_close:
	case Status::uncouple:
	case Status::lights_on:
	case Status::jump:
	{
		std::ostringstream stream;
		stream << "Unapplicable enumeration value! (" << ToString( status ) << ')' << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}
	case Status::none:
	{
		std::ostringstream stream;
		stream << "Invalid enumeration value Signal::none. No Plug available! ("<< ToString( status ) << ')' << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}
	default:
	{
		std::ostringstream stream;
		stream << "Unknown enumeration value! ("<< static_cast<int>(status) << ')' << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}
	}
}

void Signal_Imp::RegisterSockets( SocketRegistry& registry ){
	registry.RegisterPlug( m_PlugToToggle );
	registry.ConnectJack( m_JackOnChange );
}

void Signal_Imp::UnregisterSockets( SocketRegistry& registry ){
	registry.UnRegisterPlug( m_PlugToToggle );
	registry.RemoveJack( m_JackOnChange );
}

const Plug& Signal_Imp::_GetPlug( int idx ) const{
	return m_PlugToToggle.GetPlug( idx );
}

const Jack& Signal_Imp::_GetJack( int idx ) const{
	switch( idx ){
	case 0:
		return m_JackOnChange;
	default:
		std::ostringstream stream;
		stream << "Out of range!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	}
}

int Signal_Imp::CountPlugs() const{
	return m_PlugToToggle.CountPlugs();
}

int Signal_Imp::CountJacks() const noexcept{
	return 1;
}
///////////////////////////////////////
SignalClear_Imp::SignalClear_Imp()
	:	m_PlugToClear			{ *this }
{
	m_PlugToClear.Reference( "name", "PlugToClear" );
	m_PlugToClear.SetStatus( Status::clear );
}

int SignalClear_Imp::CountStatus() const noexcept{
	return 1;
}

Jack& SignalClear_Imp::JackOn( Status status ){
	if( status == Status::clear )
		return JackOnClear();

	return Signal_Imp::JackOn( status );
}

Jack& SignalClear_Imp::JackOnChangeTo(Status status){
	if( status == Status::clear )
		return JackOnChangeToClear();

	return Signal_Imp::JackOnChangeTo( status );
}

MultiPlug& SignalClear_Imp::PlugTo( Status status ){
	if( status == Status::clear )
		return PlugToClear();

	return Signal_Imp::PlugTo( status );
}

Jack& SignalClear_Imp::JackOnClear() noexcept{
	return m_JackOnClear;
}

Jack& SignalClear_Imp::JackOnChangeToClear() noexcept{
	return m_JackOnChangeToClear;
}

MultiPlug& SignalClear_Imp::PlugToClear() noexcept{
	return m_PlugToClear;
}

void SignalClear_Imp::RegisterSockets( SocketRegistry& registry ){
	registry.RegisterPlug( m_PlugToClear );
	registry.ConnectJack( m_JackOnClear );

	Signal_Imp::RegisterSockets( registry );
}

void SignalClear_Imp::UnregisterSockets( SocketRegistry& registry ){
	registry.UnRegisterPlug( m_PlugToClear );
	registry.RemoveJack( m_JackOnClear );

	Signal_Imp::UnregisterSockets( registry );
}

int SignalClear_Imp::CountPlugs() const{
	return Signal_Imp::CountPlugs() + m_PlugToClear.CountPlugs();
}

int SignalClear_Imp::CountJacks() const noexcept{
	return Signal_Imp::CountJacks() + 2;
}

const Plug& SignalClear_Imp::_GetPlug( int idx ) const{
	if( idx < Signal_Imp::CountPlugs() )
		return Signal_Imp::_GetPlug( idx );
	idx -= Signal_Imp::CountPlugs();

	if( idx < m_PlugToClear.CountPlugs() )
		return m_PlugToClear.GetPlug( idx );
	//idx -= m_PlugToClear.CountPlugs();

	throw std::range_error( "Unknown index for plug" );
}

const Jack& SignalClear_Imp::_GetJack( int idx ) const{
	if( idx < Signal_Imp::CountJacks() )
		return Signal_Imp::_GetJack( idx );
	idx -= Signal_Imp::CountJacks();

	if( idx-- == 0 )
		return m_JackOnClear;
	if( idx-- == 0 )
		return m_JackOnChangeToClear;

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}
///////////////////////////////////////
std::unique_ptr<VelocityControl> VelocityControl::Make() noexcept{
	try{
		return std::make_unique<VelocityControl_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

VelocityControl_Imp::VelocityControl_Imp() noexcept
	: m_StopDistance{ 0 }
{
}

const char* VelocityControl_Imp::TypeName() const noexcept{
	return "VelocityControl";
}

Signal::Status VelocityControl_Imp::Set( Status status, bool pulse )
{
	if( status != Status::none && m_ControlStates.find( status ) == m_ControlStates.end() ){
		assert( 0 );
		std::ostringstream stream;
		stream << "Status not available for velocity control signal: " << ToString( status ) << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}

	return Signal_Imp::Set( status, pulse );
}

Jack& VelocityControl_Imp::JackOn( Status status ){
	if( IsVelocityStatus( status ) )
		return m_ControlStates.at( status ).m_JackOnVelocity;

	return Signal_Imp::JackOn( status );
}

Jack& VelocityControl_Imp::JackOnChangeTo( Status status ){
	if( IsVelocityStatus( status ) )
		return m_ControlStates.at( status ).m_JackOnChangeToVelocity;

	return Signal_Imp::JackOnChangeTo( status );
}

MultiPlug& VelocityControl_Imp::PlugTo( Status status ){
	if( IsVelocityStatus( status ) )
		return m_ControlStates.at( status ).m_PlugToVelocity;

	return Signal_Imp::PlugTo( status );
}

void VelocityControl_Imp::Toggle( bool pulse ){
	auto iter = m_ControlStates.begin();
	for(; iter != m_ControlStates.end(); ++iter ){
		if( iter->first == Get() ){
			if( ++iter == m_ControlStates.end() )
				iter = m_ControlStates.begin();

			break;
		}
	}

	assert( iter != m_ControlStates.end() );
	Set( iter->first, pulse );
}

int VelocityControl_Imp::CountStatus() const{
	return common::narrow_cast<int>(m_ControlStates.size());
}

void VelocityControl_Imp::RegisterSockets( SocketRegistry& registry ){
	Signal_Imp::RegisterSockets( registry );

	for( auto& pair : m_ControlStates ){
		registry.RegisterPlug( pair.second.m_PlugToVelocity );
		registry.ConnectJack( pair.second.m_JackOnVelocity );
		registry.ConnectJack( pair.second.m_JackOnChangeToVelocity );
	}
}

void VelocityControl_Imp::UnregisterSockets( SocketRegistry& registry ){
	Signal_Imp::UnregisterSockets( registry );

	for( auto& pair : m_ControlStates ){
		registry.UnRegisterPlug( pair.second.m_PlugToVelocity );
		registry.RemoveJack( pair.second.m_JackOnVelocity );
		registry.RemoveJack( pair.second.m_JackOnChangeToVelocity );
	}
}

int VelocityControl_Imp::CountPlugs() const{
	int count = Signal_Imp::CountPlugs();
	
	for( const auto& pair : m_ControlStates )
		count += pair.second.m_PlugToVelocity.CountPlugs();

	return count;
}

int VelocityControl_Imp::CountJacks() const noexcept{
	return common::narrow_cast<int>(Signal_Imp::CountJacks() + 2*m_ControlStates.size());
}

bool VelocityControl_Imp::AffectTravelVelocity( Status status, bool bTravel ){
	if( !IsVelocityStatus( status ) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );

	const bool retval = m_ControlStates.at( status ).m_bAffectTravelVelocity;
	m_ControlStates.at( status ).m_bAffectTravelVelocity = bTravel;
	return retval;
}

bool VelocityControl_Imp::AffectTravelVelocity( Status status ) const{
	if( !IsVelocityStatus(status) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );

	return m_ControlStates.at( status ).m_bAffectTravelVelocity;
}

void VelocityControl_Imp::SetVelocity( Status status, Velocity velocityMin, Velocity velocityMax ){
	if( !IsVelocityStatus( status ) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );
	if( abs( velocityMin ) > abs( velocityMax ) )
		throw std::invalid_argument( "velocityMin has to be nearer to zero then velocityMax!" );
	
	// insert into map in a way that does not copy or move.
	// This is needed, to supply the parent by reference and to
	// keep our references to Plugs and Jacks valid.
	m_ControlStates.emplace(	std::piecewise_construct,
								std::forward_as_tuple( status ),
								std::forward_as_tuple( *this, status, velocityMin, velocityMax ) );

#ifdef _DEBUG
	//test what happens if element already existed...
	const Jack* pPointer1 = &m_ControlStates.at( status ).m_JackOnVelocity;

	m_ControlStates.emplace( std::piecewise_construct,
		std::forward_as_tuple( status ),
		std::forward_as_tuple( *this, status, velocityMin, velocityMax ) );

	const Jack* pPointer2 = &m_ControlStates.at( status ).m_JackOnVelocity;

	assert( pPointer1 == pPointer2 );
#endif
}

void VelocityControl_Imp::SetVelocity( Status status, Velocity velocity ){
	SetVelocity( status,  velocity, velocity);
}

void VelocityControl_Imp::VelocityMin( Status status, Velocity velocityMin ){
	SetVelocity( status, velocityMin, velocityMin < 0_mIs ? std::numeric_limits<Velocity>::lowest() : std::numeric_limits<Velocity>::max() );
}

void VelocityControl_Imp::VelocityMax( Status status, Velocity velocityMax ){
	SetVelocity( status, 0_mIs, velocityMax);
}

Velocity VelocityControl_Imp::VelocityMin( Status status ) const{
	if( !IsVelocityStatus( status ) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );

	return m_ControlStates.at( status ).m_MinVelocity;
}

Velocity VelocityControl_Imp::VelocityMax( Status status ) const{
	if( !IsVelocityStatus( status ) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );

	return m_ControlStates.at( status ).m_MaxVelocity;
}

bool VelocityControl_Imp::IsValidState( Status status ) const{
	if( !IsVelocityStatus( status ) )
		return false;

	return m_ControlStates.find( status ) != m_ControlStates.end();
}

void VelocityControl_Imp::Remove( Status status ){
	if( !IsVelocityStatus( status ) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );

	m_ControlStates.erase( status );
}

void VelocityControl_Imp::Clear() noexcept{
	m_ControlStates.clear();
}

void VelocityControl_Imp::StopDistance( Length distance ) noexcept{
	m_StopDistance = distance;
}

Length VelocityControl_Imp::StopDistance() const noexcept{
	return m_StopDistance;
}

Jack& VelocityControl_Imp::JackOnVelocity( Status status ){
	if( !IsVelocityStatus( status ) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );

	return m_ControlStates.at( status ).m_JackOnVelocity;
}

MultiPlug & VelocityControl_Imp::PlugToVelocity( Status status ){
	if( !IsVelocityStatus( status ) )
		throw std::invalid_argument( "Signal::Status has to be a velocity kind of status!" );

	return m_ControlStates.at( status ).m_PlugToVelocity;
}

const Plug& VelocityControl_Imp::_GetPlug( int idx ) const{
	if( idx < Signal_Imp::CountPlugs() )
		return Signal_Imp::_GetPlug( idx );
	idx -= Signal_Imp::CountPlugs();

	for( const auto& pair : m_ControlStates ){
		if( idx < pair.second.m_PlugToVelocity.CountPlugs() )
			return pair.second.m_PlugToVelocity.GetPlug( idx );
		idx -= pair.second.m_PlugToVelocity.CountPlugs();
	}

	assert( 0 );
	std::ostringstream stream;
	stream << "Unknown index for plug!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

const Jack& VelocityControl_Imp::_GetJack( int idx ) const{
	if( idx < Signal_Imp::CountJacks() )
		return Signal_Imp::_GetJack( idx );
	idx -= Signal_Imp::CountJacks();

	for( const auto& pair : m_ControlStates ){
		if( idx-- == 0 )
			return pair.second.m_JackOnVelocity;
	}

	for( const auto& pair : m_ControlStates ){
		if( idx-- == 0 )
			return pair.second.m_JackOnChangeToVelocity;
	}

	assert( 0 );
	std::ostringstream stream;
	stream << "Unknown index for jack!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

VelocityControl_Imp::ControlState::ControlState(
	VelocityControl_Imp& parent, 
	Status status,
	Velocity minVelocity,
	Velocity maxVelocity )
	:	m_MinVelocity			{minVelocity},
		m_MaxVelocity			{maxVelocity},
		m_bAffectTravelVelocity	{false},
		m_JackOnVelocity		{"JackOnVelocity" + std::to_string( static_cast<int>(status) - static_cast<int>(Status::velocity0) ) },
		m_JackOnChangeToVelocity{"JackOnChangeToVelocity" + std::to_string( static_cast<int>(status) - static_cast<int>(Status::velocity0) ) },
		m_PlugToVelocity		{parent}
{
	m_PlugToVelocity.Reference( "name", "PlugToVelocity" + std::to_string( static_cast<int>(status) - static_cast<int>(Status::velocity0) ) );
	m_PlugToVelocity.SetStatus( status );
}
///////////////////////////////////////
std::unique_ptr<JumpSite> trax::JumpSite::Make() noexcept
{
	try{
		return std::make_unique<JumpSite_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

JumpSite_Imp::JumpSite_Imp()
	:	m_bTrackLocation(false),
		m_PlugToJump( *this )
{
	m_PlugToJump.Reference( "name", "PlugToJump" );
	m_PlugToJump.SetStatus( Status::jump );
}

const char*	JumpSite_Imp::TypeName() const noexcept{
	return "JumpSignal";
}

Signal::Status JumpSite_Imp::Set( Status status, bool pulse ){
	switch( status ){
	case Status::clear:
	case Status::jump:
		break;
	default:
		assert( 0 );
		std::ostringstream stream;
		stream << "Status not available for jump signal: " << ToString( status ) << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}

	return SignalClear_Imp::Set( status, pulse );
}

void JumpSite_Imp::Toggle( bool ){
	Set( Get() == Status::clear ? Status::jump : Status::clear );
}

int JumpSite_Imp::CountStatus() const noexcept{
	return 2;
}

void JumpSite_Imp::Target( const Location& target ) noexcept{
	m_TargetLocation = target;
	m_bTrackLocation = true;
	m_pTargetDepot = nullptr;
}

void JumpSite_Imp::Target( std::shared_ptr<Depot> pDepot ) noexcept{
	m_pTargetDepot = pDepot;
	m_bTrackLocation = false;
}

bool JumpSite_Imp::IsTargetLocation() const noexcept{
	return m_bTrackLocation;
}

const Location& JumpSite_Imp::TargetLocation() const noexcept{
	return m_TargetLocation;
}

std::shared_ptr<Depot> JumpSite_Imp::TargetDepot() const noexcept{
	return m_pTargetDepot;
}

Jack& JumpSite_Imp::JackOnJump() noexcept{
	return m_JackOnJump;
}

Jack& JumpSite_Imp::JackOnChangeToJump() noexcept{
	return m_JackOnChangeToJump;
}

MultiPlug& JumpSite_Imp::PlugToJump() noexcept{
	return m_PlugToJump;
}

Jack& JumpSite_Imp::JackOnClear() noexcept{
	return SignalClear_Imp::JackOnClear();
}

Jack & JumpSite_Imp::JackOnChangeToClear() noexcept{
	return SignalClear_Imp::JackOnChangeToClear();
}

MultiPlug& JumpSite_Imp::PlugToClear() noexcept{
	return SignalClear_Imp::PlugToClear();
}

Jack& JumpSite_Imp::JackOn( Status status ){
	if( status == Status::jump )
		return JackOnJump();

	return SignalClear_Imp::JackOn( status );
}

Jack& JumpSite_Imp::JackOnChangeTo( Status status ){
	if( status == Status::jump )
		return JackOnChangeToJump();

	return SignalClear_Imp::JackOnChangeTo( status );
}

MultiPlug& JumpSite_Imp::PlugTo( Status status ){
	if( status == Status::jump )
		return PlugToJump();

	return SignalClear_Imp::PlugTo( status );
}

void JumpSite_Imp::RegisterSockets( SocketRegistry& module ){
	module.RegisterPlug( m_PlugToJump );

	SignalClear_Imp::RegisterSockets( module );

	module.ConnectJack( m_JackOnJump );
}

void JumpSite_Imp::UnregisterSockets( SocketRegistry& module ){
	module.UnRegisterPlug( m_PlugToJump );

	SignalClear_Imp::UnregisterSockets( module );

	module.RemoveJack( m_JackOnJump );
}

const Plug& JumpSite_Imp::_GetPlug( int idx ) const{
	if( idx < SignalClear_Imp::CountPlugs() )
		return SignalClear_Imp::_GetPlug( idx );

	idx -= SignalClear_Imp::CountPlugs();
	if( idx < m_PlugToJump.CountPlugs() )
		return m_PlugToJump.GetPlug( idx );

	throw std::range_error( "Unknown index for plug" );
}

const Jack& JumpSite_Imp::_GetJack( int idx ) const{
	if( idx < SignalClear_Imp::CountJacks() )
		return SignalClear_Imp::_GetJack( idx );

	idx -= SignalClear_Imp::CountJacks();
	if( idx-- == 0 )
		return m_JackOnJump;
	if( idx-- == 0 )
		return m_JackOnChangeToJump;

	assert( 0 );
	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

int JumpSite_Imp::CountPlugs() const{
	return SignalClear_Imp::CountPlugs() + m_PlugToJump.CountPlugs();
}

int JumpSite_Imp::CountJacks() const noexcept{
	return SignalClear_Imp::CountJacks() + 2;
}
///////////////////////////////////////
std::unique_ptr<TrackEndTransition> TrackEndTransition::Make( Track* pToTrack, Track::EndType toEnd ) noexcept{
	try{
		return std::make_unique<TrackEndTransition_Imp>( pToTrack, toEnd );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

TrackEndTransition_Imp::TrackEndTransition_Imp( Track* pToTrack, Track::EndType toEnd )
	: m_pFromTrack( nullptr ),
	m_FromEnd( Track::EndType::front ),
	m_pToTrack( pToTrack ),
	m_ToEnd( toEnd )
{
	if( m_pToTrack ){
		Track::TrackEnd fromEnd = m_pToTrack->TransitionEnd( m_ToEnd );
		m_pFromTrack = fromEnd.first.get();
		m_FromEnd = fromEnd.second;
	}
}

int TrackEndTransition_Imp::CountStatus() const noexcept{
	return 2;
}

void TrackEndTransition_Imp::Set( Track* pToTrack, Track::EndType toEnd ) noexcept{
	m_pToTrack = pToTrack;
	m_ToEnd = toEnd;

	if( m_pToTrack ){
		Track::TrackEnd fromEnd = m_pToTrack->TransitionEnd( m_ToEnd );
		m_pFromTrack = fromEnd.first.get();
		m_FromEnd = fromEnd.second;
	}
}

Track* TrackEndTransition_Imp::From( Track::EndType& end ) const noexcept{
	end = m_FromEnd;
	return m_pFromTrack;
}

Track* TrackEndTransition_Imp::From() const noexcept{
	return m_pFromTrack;
}

Track* TrackEndTransition_Imp::To( Track::EndType& end ) const noexcept{
	end = m_ToEnd;
	return m_pToTrack;
}

Track* TrackEndTransition_Imp::To() const noexcept{
	return m_pToTrack;
}
///////////////////////////////////////
std::string ToString( Signal::Status status ){
	switch( status ){
	case Signal::Status::none:
		return "none";
	case Signal::Status::stop:
		return "stop";
	case Signal::Status::clear:
		return "clear";
	case Signal::Status::slow:
		return "slow";
	case Signal::Status::velocity3:
		return "velocity3";
	case Signal::Status::velocity4:
		return "velocity4";
	case Signal::Status::velocity5:
		return "velocity5";
	case Signal::Status::velocity6:
		return "velocity6";
	case Signal::Status::velocity7:
		return "velocity7";
	case Signal::Status::velocity8:
		return "velocity8";
	case Signal::Status::velocity9:
		return "velocity9";
	case Signal::Status::velocity10:
		return "velocity10";
	case Signal::Status::velocity11:
		return "velocity11";
	case Signal::Status::velocity12:
		return "velocity12";
	case Signal::Status::caution:
		return "caution";
	case Signal::Status::doors_open:
		return "doors_open";
	case Signal::Status::doors_close:
		return "doors_close";
	case Signal::Status::uncouple:
		return "uncouple";
	case Signal::Status::lights_on:
		return "lights_on";
	case Signal::Status::jump:
		return "jump";
	case Signal::Status::toggle:
		return "toggle";
	case Signal::Status::change:
		return "change";
	default:
		assert( !"Unknown Signal::Status enumerator!" );
		return "unknown";
	}
}

constexpr bool IsVelocityStatus( Signal::Status status ){
	return Signal::Status::velocity0 <= status && status <= Signal::Status::velocityMax;
}

Signal::Status ToSignalStatus( const std::string& status ){
	if( status == "none" )
		return Signal::Status::none;
	else if( status == "stop" )
		return Signal::Status::stop;
	else if( status == "clear" )
		return Signal::Status::clear;
	else if( status == "slow" )
		return Signal::Status::slow;
	else if( status == "velocity0" )
		return Signal::Status::velocity0;
	else if( status == "velocity1" )
		return Signal::Status::velocity1;
	else if( status == "velocity2" )
		return Signal::Status::velocity2;
	else if( status == "velocity3" )
		return Signal::Status::velocity3;
	else if( status == "velocity4" )
		return Signal::Status::velocity4;
	else if( status == "velocity5" )
		return Signal::Status::velocity5;
	else if( status == "velocity6" )
		return Signal::Status::velocity6;
	else if( status == "velocity7" )
		return Signal::Status::velocity7;
	else if( status == "velocity8" )
		return Signal::Status::velocity8;
	else if( status == "velocity9" )
		return Signal::Status::velocity9;
	else if( status == "velocity10" )
		return Signal::Status::velocity10;
	else if( status == "velocity11" )
		return Signal::Status::velocity11;
	else if( status == "velocity12" )
		return Signal::Status::velocity12;
	else if( status == "caution" )
		return Signal::Status::caution;
	else if( status == "doors_open" )
		return Signal::Status::doors_open;
	else if( status == "doors_close" )
		return Signal::Status::doors_close;
	else if( status == "uncouple" )
		return Signal::Status::uncouple;
	else if( status == "lights_on" )
		return Signal::Status::lights_on;
	else if( status == "jump" )
		return Signal::Status::jump;
	else if( status == "toggle" )
		return Signal::Status::toggle;
	else if( status == "change" )
		return Signal::Status::change;

	std::ostringstream stream;
	stream << "Invalid Signal::Status string: " << status << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

Signal::Status SignalStatusFrom( const std::string& socketName ){
	if( socketName == "JackOnStop" ||
		socketName == "JackOnChangeToStop" ||
		socketName == "PlugToStop" )
		return Signal::Status::stop;
	else if( socketName == "JackOnClear" ||
		socketName == "JackOnChangeToClear" ||
		socketName == "PlugToClear" )
		return Signal::Status::clear;
	else if( socketName == "JackOnSlow" ||
		socketName == "JackOnChangeToSlow" ||
		socketName == "PlugToSlow" )
		return Signal::Status::slow;
	else if( socketName == "JackOnVelocity0" ||
		socketName == "JackOnChangeToVelocity0" ||
		socketName == "PlugToVelocity0" )
		return Signal::Status::velocity0;
	else if( socketName == "JackOnVelocity1" ||
		socketName == "JackOnChangeToVelocity1" ||
		socketName == "PlugToVelocity1" )
		return Signal::Status::velocity1;
	else if( socketName == "JackOnVelocity2" ||
		socketName == "JackOnChangeToVelocity2" ||
		socketName == "PlugToVelocity2" )
		return Signal::Status::velocity2;
	else if( socketName == "JackOnVelocity3" ||
		socketName == "JackOnChangeToVelocity3" ||
		socketName == "PlugToVelocity3" )
		return Signal::Status::velocity3;
	else if( socketName == "JackOnVelocity4" ||
		socketName == "JackOnChangeToVelocity4" ||
		socketName == "PlugToVelocity4" )
		return Signal::Status::velocity4;
	else if( socketName == "JackOnVelocity5" ||
		socketName == "JackOnChangeToVelocity5" ||
		socketName == "PlugToVelocity5" )
		return Signal::Status::velocity5;
	else if( socketName == "JackOnVelocity6" ||
		socketName == "JackOnChangeToVelocity6" ||
		socketName == "PlugToVelocity6" )
		return Signal::Status::velocity6;
	else if( socketName == "JackOnVelocity7" ||
		socketName == "JackOnChangeToVelocity7" ||
		socketName == "PlugToVelocity7" )
		return Signal::Status::velocity7;
	else if( socketName == "JackOnVelocity8" ||
		socketName == "JackOnChangeToVelocity8" ||
		socketName == "PlugToVelocity8" )
		return Signal::Status::velocity8;
	else if( socketName == "JackOnVelocity9" ||
		socketName == "JackOnChangeToVelocity9" ||
		socketName == "PlugToVelocity9" )
		return Signal::Status::velocity9;
	else if( socketName == "JackOnVelocity10" ||
		socketName == "JackOnChangeToVelocity10" ||
		socketName == "PlugToVelocity10" )
		return Signal::Status::velocity10;
	else if( socketName == "JackOnVelocity11" ||
		socketName == "JackOnChangeToVelocity11" ||
		socketName == "PlugToVelocity11" )
		return Signal::Status::velocity11;
	else if( socketName == "JackOnVelocity12" ||
		socketName == "JackOnChangeToVelocity12" ||
		socketName == "PlugToVelocity12" )
		return Signal::Status::velocity12;
	else if( socketName == "JackOnCaution" ||
		socketName == "JackOnChangeToCaution" ||
		socketName == "PlugToCaution" )
		return Signal::Status::caution;
	else if( socketName == "JackOnDoorsOpen" ||
		socketName == "JackOnChangeToDoorsOpen" ||
		socketName == "PlugToDoorsOpen" )
		return Signal::Status::doors_open;
	else if( socketName == "JackOnDoorsClose" ||
		socketName == "JackOnChangeToDoorsClose" ||
		socketName == "PlugToDoorsClose" )
		return Signal::Status::doors_close;
	else if( socketName == "JackOnUncouple" ||
		socketName == "JackOnChangeToUncouple" ||
		socketName == "PlugToUncouple" )
		return Signal::Status::uncouple;
	else if( socketName == "JackOnLightsOn" ||
		socketName == "JackOnChangeToLightsOn" ||
		socketName == "PlugToLightsOn" )
		return Signal::Status::lights_on;
	else if( socketName == "JackOnJump" ||
		socketName == "JackOnChangeToJump" ||
		socketName == "PlugToJump" )
		return Signal::Status::jump;
	else if( socketName == "JackOnChange" )
		return Signal::Status::change;
	else if( socketName == "PlugToToggle" )
		return Signal::Status::toggle;

	std::ostringstream stream;
	stream << "Invalid Signal::Status name string: " << socketName << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}
///////////////////////////////////////
}
