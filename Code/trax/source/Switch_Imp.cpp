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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "Switch_Imp.h"
#include "trax/Location.h"


namespace trax{
///////////////////////////////////////
std::unique_ptr<Switch> Switch::Make() noexcept
{
	try{
		return std::make_unique<Switch_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
std::unique_ptr<ThreeWaySwitch> ThreeWaySwitch::Make() noexcept
{
	try{
		return std::make_unique<ThreeWaySwitch_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
std::unique_ptr<SingleSlipSwitch> SingleSlipSwitch::Make() noexcept
{
	try{
		return std::make_unique<SingleSlipSwitch_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
std::unique_ptr<DoubleSlipSwitch> DoubleSlipSwitch::Make() noexcept
{
	try{
		return std::make_unique<DoubleSlipSwitch_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
std::unique_ptr<NarrowSwitch> NarrowSwitch::Make( unsigned char numBranches ) noexcept
{
	if( numBranches == 1 )
		return std::make_unique<Switch_Imp>();
	if( numBranches == 2 )
		return std::make_unique<ThreeWaySwitch_Imp>();

	return std::make_unique<NarrowSwitch_Imp>( numBranches );
}

NarrowSwitch_Imp::NarrowSwitch_Imp( unsigned char numBranches )
	:	Connector_Imp		{ 2 + numBranches },
		m_Status			{ Status::go },
		m_JacksOnBranches	{ numBranches },
		m_PlugToGo			{ *this },
		m_PlugsToBranches	{ numBranches }
{
	if( static_cast<int>(numBranches) > static_cast<int>(Status::maxBranches) )
		throw std::range_error( "NarrowSwitch can not have more than " + std::to_string(static_cast<int>(Status::maxBranches)) + " branches!" );

	m_PlugToGo.SetStatus( Status::go );
	m_PlugToGo.Reference( "name", "PlugToGo" );

	for( unsigned char i = 0; i < numBranches; ++i ){	
		std::string ordinal = numBranches == 1 ? "" : std::to_string(i+1); //no ordinal for simple switch, due to compatibility reasons ...
		m_JacksOnBranches[i] = std::make_unique<Jack_Imp>( "JackOnBranch" + ordinal );
		m_PlugsToBranches[i] = std::make_unique<MultiPlug_Imp<StatusHolder_Plug<NarrowSwitch_Imp>>>(*this);
		m_PlugsToBranches[i]->SetStatus( static_cast<Status>(i+1) );
		m_PlugsToBranches[i]->Reference( "name", "PlugToBranch" + ordinal );
	}
}

ConnectorType NarrowSwitch_Imp::GetConnectorType() const noexcept{
	return ConnectorType::NarrowSwitch;
}

const char* NarrowSwitch_Imp::TypeName() const noexcept{
	return "NarrowSwitch";
}

void NarrowSwitch_Imp::Toggle( bool pulse ){
	int status = static_cast<int>(m_Status);
	if( ++status > CntDivergedTracks() )
		status = static_cast<int>(Status::go);

	Set( static_cast<Status>(status), pulse );
}

void NarrowSwitch_Imp::Set( const Track& trackA, Track::EndType trackendA, const Track& trackB, Track::EndType trackendB, bool pulse ){
	const int slotA = Slot( trackA, trackendA );
	const int slotB = Slot( trackB, trackendB );

	if( slotA < 0 || slotB < 0 )
		throw std::invalid_argument( "Track not in a narrow switches slot!" );

	if( slotA == slotB )
		throw std::logic_error( "Cannot connect a slot of a narrow switch with itself!" );

	if( slotA * slotB != 0 )
		throw std::logic_error( "Cannot connect those two slots of a narrow switch!" );

	Set( static_cast<Status>(slotA != 0 ? slotA-1 : slotB-1), pulse );
}

int NarrowSwitch_Imp::Slot( 
	int slot, 
	std::shared_ptr<TrackBuilder> pTrack, 
	Track::EndType trackend, 
	bool connectAnonymous )
{
	const int retval = Connector_Imp::Slot( slot, pTrack, trackend, connectAnonymous );

	switch( slot ){
	case 0:
		for( int i = 1; i < CntSlots(); ++i )
			if( Slot(i).first )
				Slot(i).first->Couple( Slot(i), Slot(slot_0) );
		break;
	default:
		if( Slot(slot_0).first )
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot) );
	}

	Set( m_Status, false );
	return retval;
}

bool NarrowSwitch_Imp::Check( std::ostream& os, Length e_distance, Angle e_kink, Angle e_twist ) const noexcept
{
	if( !IsComplete() ){
		os << Verbosity::detailed << "Switch is incomplete! SwitchID: " << ID() << std::endl;
		return false;
	}

	bool bOk = true;
	for( int i = slot_1; i < CntSlots(); ++i )
		if( !CheckSlot( i, os, e_distance, e_kink, e_twist ) )
			bOk = false;

	return bOk;
}

void NarrowSwitch_Imp::GetCenter( Frame<Length, One>& center ) const
{
	if( NarrowTrack().first ){
		if( NarrowTrack().second == Track::EndType::end )
			NarrowTrack().first->Transition( NarrowTrack().first->GetLength(), center );
		else if( NarrowTrack().second == Track::EndType::front ){
			NarrowTrack().first->Transition( 0_m, center );
			center.T *= -1;
			center.N *= -1;
		}
	}
	else if( StraightTrack().first ){
		if( StraightTrack().second == Track::EndType::end ){
			StraightTrack().first->Transition( StraightTrack().first->GetLength(), center );
			center.T *= -1;
			center.N *= -1;
		}
		else if( StraightTrack().second == Track::EndType::front )
			StraightTrack().first->Transition( 0_m, center );
	}
	else{	
		for( int i = 0; i < CntDivergedTracks(); ++i )
			if( DivergedTrack(i).first ){
				if( DivergedTrack(i).second == Track::EndType::end ){
					DivergedTrack(i).first->Transition( DivergedTrack(i).first->GetLength(), center );
					center.T *= -1;
					center.N *= -1;
				}
				else if( DivergedTrack(i).second == Track::EndType::front )
					DivergedTrack(i).first->Transition( 0_m, center );

				break;
			}
	}
}

void NarrowSwitch_Imp::RegisterSockets( SocketRegistry& modul ){
	Connector_Imp::RegisterSockets( modul );

	modul.RegisterPlug( m_PlugToGo );
	for( auto& plug : m_PlugsToBranches )
		modul.RegisterPlug( *plug );

	modul.ConnectJack( m_JackOnGo );
	for( auto& jack : m_JacksOnBranches )
		modul.ConnectJack( *jack );
}

void NarrowSwitch_Imp::UnregisterSockets( SocketRegistry& modul ){
	for( auto riter = m_JacksOnBranches.rbegin(); riter != m_JacksOnBranches.rend(); ++riter )
		modul.RemoveJack( **riter );
	modul.RemoveJack( m_JackOnGo );

	for( auto riter = m_PlugsToBranches.rbegin(); riter != m_PlugsToBranches.rend(); ++riter )
		modul.UnRegisterPlug( **riter );
	modul.UnRegisterPlug( m_PlugToGo );

	Connector_Imp::UnregisterSockets( modul );
}

NarrowSwitch::Status NarrowSwitch_Imp::Set( Status to, bool pulse ){
	const Status retval = m_Status;

	m_Status = to;

	if( Slot(slot_0).first ){
		switch( m_Status ){
		case Status::none:
			Slot(slot_0).first->DeCouple( Slot(slot_0).second, true );
			break;
		case Status::go:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_1) );

			if( pulse )
				m_JackOnGo.Pulse();

			break;
		case Status::toggle:
		case Status::change:
			assert( !"Invalid switch status value" );
			break;
		case Status::empty:
			if( !Slot(slot_1).first ){
				Slot(slot_0).first->DeCouple( Slot(slot_0).second, true );
				m_Status = Status::go;

				if( pulse )
					m_JackOnGo.Pulse();

				break;
			}

			m_Status = retval;
			for( int i = 0; i < CntDivergedTracks(); ++i )
				if( !Slot(slot_2+i).first ){
					Slot(slot_0).first->DeCouple( Slot(slot_0).second, true );
					m_Status = static_cast<Status>(static_cast<int>(Status::branch1) + i);

					if( pulse )
						m_JacksOnBranches[i]->Pulse();

					break;
				}

			break;
		default:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(static_cast<int>(m_Status)+1) );

			if( pulse )
				m_JacksOnBranches[static_cast<std::size_t>(m_Status)-1]->Pulse();

			break;
		}

		if( pulse )
			JackOnChange().Pulse();
	}

	return retval;
}

NarrowSwitch::Status NarrowSwitch_Imp::Get() const noexcept{
	return m_Status;
}

void NarrowSwitch_Imp::NarrowTrack( 
	std::shared_ptr<TrackBuilder> pNarrowTrack, 
	Track::EndType trackend )
{
	Slot( slot_0, pNarrowTrack, trackend );
}

std::pair<std::shared_ptr<TrackBuilder>, Track::EndType> NarrowSwitch_Imp::NarrowTrack() const noexcept{
	return Slot(slot_0);
}

void NarrowSwitch_Imp::ClearNarrowTrack(){
	Clear(slot_0);
}

void NarrowSwitch_Imp::StraightTrack( 
	std::shared_ptr<TrackBuilder> pStaightTrack, 
	Track::EndType trackend )
{
	Slot( slot_1, pStaightTrack, trackend );
}

std::pair<std::shared_ptr<TrackBuilder>, Track::EndType> NarrowSwitch_Imp::StraightTrack() const noexcept{
	return Slot(slot_1);
}

void NarrowSwitch_Imp::ClearStraightTrack(){
	Clear(slot_1);
}

void NarrowSwitch_Imp::DivergedTrack( 
	int divTrackID, 
	std::shared_ptr<TrackBuilder> pDivergedTrack, Track::EndType trackend )
{
	Slot( slot_2 + divTrackID, pDivergedTrack, trackend );
}

std::pair<std::shared_ptr<TrackBuilder>, Track::EndType> NarrowSwitch_Imp::DivergedTrack( int divTrackID ) const noexcept{
	return Slot(slot_2 + divTrackID);
}

void NarrowSwitch_Imp::ClearDivergedTrack( int divTrackID ){
	Clear(slot_2 + divTrackID);
}

int NarrowSwitch_Imp::CntDivergedTracks() const noexcept{
	return static_cast<int>(m_JacksOnBranches.size());
}

Location NarrowSwitch_Imp::Bifurcation() const{
	if( !NarrowTrack().first )
		return Location{};

	TrackLocation loc;
	if( NarrowTrack().second == Track::EndType::end ){
		loc.parameter	= NarrowTrack().first->GetLength();
		loc.orientation = Orientation::Value::para;
	}
	else{
		loc.parameter	= 0_m;
		loc.orientation = Orientation::Value::anti;
	}

	return Location{ NarrowTrack().first, loc };
}

Jack& NarrowSwitch_Imp::JackOnGo() noexcept{
	return m_JackOnGo;
}

MultiPlug& NarrowSwitch_Imp::PlugToGo() noexcept{
	return m_PlugToGo;
}

Jack& NarrowSwitch_Imp::JackOn( Status status ){
	if( status >= Status::branch1 && static_cast<int>(status) <= CntDivergedTracks() )
		return *m_JacksOnBranches[static_cast<std::size_t>(status)-static_cast<std::size_t>(Status::branch1)];

	switch( status ){
	case Status::go:
		return JackOnGo();
	case Status::change:
		return JackOnChange();
	default:
		throw std::invalid_argument( "Unknown Switch::Status enumeration value!" );
	}
}

MultiPlug& NarrowSwitch_Imp::PlugTo( Status status ){
	if( status >= Status::branch1 && static_cast<int>(status) <= CntDivergedTracks() )
		return *m_PlugsToBranches[static_cast<std::size_t>(status)-static_cast<std::size_t>(Status::branch1)];

	switch( status ){
	case Status::go:
		return PlugToGo();
	case Status::toggle:
		return PlugToToggle();
	default:
		throw std::invalid_argument( "Unknown Switch::Status enumeration value!" );
	}
}

Jack& NarrowSwitch_Imp::JackOnDivergedTrack( int divTrackID ) noexcept{
	return *m_JacksOnBranches[divTrackID];
}

MultiPlug& NarrowSwitch_Imp::PlugToDivergedTrack( int divTrackID ) noexcept{
	return *m_PlugsToBranches[divTrackID];
}

int NarrowSwitch_Imp::CountPlugs() const{
	int count = m_PlugToGo.CountPlugs();

	for( const auto& plug : m_PlugsToBranches )
		count += plug->CountPlugs();

	count += Connector_Imp::CountPlugs();
	return count;
}

int NarrowSwitch_Imp::CountJacks() const noexcept{
	return 1 + static_cast<int>(m_JacksOnBranches.size()) + Connector_Imp::CountJacks();
}

const Plug& NarrowSwitch_Imp::_GetPlug( int idx ) const{
	if( idx < m_PlugToGo.CountPlugs() )
		return m_PlugToGo.GetPlug( idx );
	idx -= m_PlugToGo.CountPlugs();

	for( const auto& plug : m_PlugsToBranches ){
		if( idx < plug->CountPlugs() )
			return plug->GetPlug( idx );
		idx -= plug->CountPlugs();
	}
	
	return Connector_Imp::_GetPlug( idx );
}

const Jack& NarrowSwitch_Imp::_GetJack( int idx ) const{
	if( idx == 0 )
		return m_JackOnGo;
	
	idx -= 1;
	if( idx < static_cast<int>(m_JacksOnBranches.size()) )
		return *m_JacksOnBranches[idx];

	idx -= static_cast<int>(m_JacksOnBranches.size());
	return Connector_Imp::_GetJack( idx );
}
///////////////////////////////////////
std::string ToString( NarrowSwitch::Status status ){
	if( status >= NarrowSwitch::Status::branch1 && status <= NarrowSwitch::Status::branchN )
		return "branch" + std::to_string( static_cast<int>(status) );

	switch( status ){
	case NarrowSwitch::Status::none:
		return "none";
	case NarrowSwitch::Status::go:
		return "go";
	case NarrowSwitch::Status::toggle:
		return "toggle";
	case NarrowSwitch::Status::change:
		return "change";
	default:
		assert( !"Unknown Switch::Status enumerator!" );
		return "unknown";
	}
}

NarrowSwitch::Status ToNarrowSwitchStatus( const std::string& status ){
	if( status == "go" )
		return NarrowSwitch::Status::go;
	else if( status == "branch" )
		return NarrowSwitch::Status::branch1;
	else if( status.substr( 0, 6 ) == "branch" )
		return static_cast<NarrowSwitch::Status>(std::stoi( status.substr( 6 ) ));
	else if( status == "toggle" )
		return NarrowSwitch::Status::toggle;
	else if( status == "change" )
		return NarrowSwitch::Status::change;
	else if( status == "none" )
		return NarrowSwitch::Status::none;

	std::ostringstream stream;
	stream << "Invalid Switch::Status string!" << status << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

NarrowSwitch::Status NarrowSwitchStatusFrom( const std::string& socketName )
{
	if( socketName == "JackOnGo" ||
		socketName == "PlugToGo" )
		return Switch::Status::go;
	else if( socketName == "JackOnBranch" ||
			 socketName == "PlugToBranch" )
		return NarrowSwitch::Status::branch1;
	else if( socketName.substr( 0, 12 ) == "JackOnBranch" ||
			 socketName.substr( 0, 12 ) == "PlugToBranch" )
		return static_cast<NarrowSwitch::Status>(std::stoi( socketName.substr( 12 ) ));
	else if( socketName == "JackOnChange" )
		return NarrowSwitch::Status::change;
	else if( socketName == "PlugToToggle" )
		return NarrowSwitch::Status::toggle;

	std::ostringstream stream;
	stream << "Invalid Switch::Status socketName string!" << socketName << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}
///////////////////////////////////////
Switch_Imp::Switch_Imp()
	:	NarrowSwitch_Imp{1}
{
}

ConnectorType Switch_Imp::GetConnectorType() const noexcept{
	return ConnectorType::TwoWaySwitch;
}

const char* Switch_Imp::TypeName() const noexcept{
	return "Switch";
}

void Switch_Imp::DivergedTrack( 
	std::shared_ptr<TrackBuilder> pDivergedTrack, 
	Track::EndType trackend )
{
	Slot( slot_2, pDivergedTrack, trackend );
}

std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> Switch_Imp::DivergedTrack() const noexcept{
	return Slot(slot_2);
}

void Switch_Imp::ClearDivergedTrack(){
	Clear(slot_2);
}

void Switch_Imp::SwapTracks(){
	SwapSlots( slot_1, slot_2 );
	Set( Get(), false );
}

bool Switch_Imp::IsNormal( bool bStopAtFirstTrack, Length maxDistance ) const{
	if( !IsComplete() )
		throw std::runtime_error( "Switch_Imp::IsNormal call invalid" );

	Position<Length> EndPosStraight;
	Position<Length> EndPosDiverged;
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged, bStopAtFirstTrack, maxDistance ) )
		return EndPosStraight.y * EndPosStraight.y + EndPosStraight.z * EndPosStraight.z < EndPosDiverged.y * EndPosDiverged.y + EndPosDiverged.z * EndPosDiverged.z;

	return false;
}

bool Switch_Imp::Normalize(){
	if( !IsNormal() ){
		SwapTracks();
		return true;
	}

	return false;
}

bool Switch_Imp::IsY( bool bStopAtFirstTrack, Length maxDistance, AnglePerLength margin ) const{
	Position<Length> EndPosStraight;
	Position<Length> EndPosDiverged;
	if( const Length length = MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged, bStopAtFirstTrack, maxDistance ) )
	{
		const AnglePerLength deviationStraight = atan2( EndPosStraight.y, EndPosStraight.x ) / length;
		const AnglePerLength deviationDiverged = atan2( EndPosDiverged.y, EndPosDiverged.x ) / length;

		if( abs(deviationStraight) < margin ||
			abs(deviationDiverged) < margin )
			return false; // one branch qualifies for running straight along.

		return deviationStraight * deviationDiverged < 0_1Im2;
	}

	return false;
}

bool Switch_Imp::IsHorizontal( bool bStopAtFirstTrack, Length maxDistance ) const{
	if( !IsComplete() ){
		std::ostringstream stream;
		stream << "A switch is incomplete";
		StreamSwitch( stream );
		throw std::runtime_error( stream.str() );
	}

	Position<Length> EndPosStraight;		
	Position<Length> EndPosDiverged;		
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged, bStopAtFirstTrack, maxDistance ) )
		return abs(EndPosDiverged.y - EndPosStraight.y) > abs(EndPosDiverged.z - EndPosStraight.z);

	throw std::runtime_error( "Switch_Imp::IsHorizontal call invalid" );
}

bool Switch_Imp::IsVertical( bool bStopAtFirstTrack, Length maxDistance ) const{
	if( !IsComplete() ){
		std::ostringstream stream;
		stream << "A switch is incomplete";
		StreamSwitch( stream );
		throw std::runtime_error( stream.str() );
	}

	Position<Length> EndPosStraight;		
	Position<Length> EndPosDiverged;		
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged, bStopAtFirstTrack, maxDistance ) )
		return abs(EndPosDiverged.y - EndPosStraight.y) < abs(EndPosDiverged.z - EndPosStraight.z);

	throw std::runtime_error( "Switch_Imp::IsVertical call invalid" );
}

bool Switch_Imp::BranchLeftOrRight( bool bStopAtFirstTrack, Length maxDistance ) const{
	if( !IsComplete() ){
		std::ostringstream stream;
		stream << "A switch is incomplete";
		StreamSwitch( stream );
		throw std::runtime_error( stream.str() );
	}

	Position<Length> EndPosStraight;		
	Position<Length> EndPosDiverged;		
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged, bStopAtFirstTrack, maxDistance ) )
		return EndPosStraight.y <= EndPosDiverged.y;

	throw std::runtime_error( "Switch_Imp::BranchLeftOrRight call invalid" );
}

bool Switch_Imp::BranchUpOrDown( bool bStopAtFirstTrack, Length maxDistance ) const{
	if( !IsComplete() ){
		std::ostringstream stream;
		stream << "A switch is incomplete";
		StreamSwitch( stream );
		throw std::runtime_error( stream.str() );
	}

	Position<Length> EndPosStraight;	
	Position<Length> EndPosDiverged;	
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged, bStopAtFirstTrack, maxDistance ) )
		return EndPosStraight.z <= EndPosDiverged.z;

	throw std::runtime_error( "Switch_Imp::BranchUpOrDown call invalid" );
}

Jack& Switch_Imp::JackOnBranch() noexcept{
	return JackOnDivergedTrack( 0 );
}

MultiPlug& Switch_Imp::PlugToBranch() noexcept{
	return PlugToDivergedTrack( 0 );
}

Length Switch_Imp::MinimumOutgoingTrackEndpoints( 
	Position<Length>& posStraight, 
	Position<Length>& posDiverged,
	bool bStopAtFirstTrack, 
	Length maxDistance ) const
{
	if( !IsComplete() )
		return 0_m;

	Location location1 = GetEndLocation( *this, 1 ); 
	if( location1.IsOnTrack() ){
		Location location2 = GetEndLocation( *this, 2 ); 
		if( location2.IsOnTrack() )
		{
			Length minlength = bStopAtFirstTrack ? std::min( Slot(1).first->GetLength(), Slot(2).first->GetLength() ) : maxDistance;

			if( const Length rest = location1.Move( minlength ).first )
				minlength -= rest;
			if( const Length rest = location2.Move( minlength ).first ){
				minlength -= rest;
				location1 = GetEndLocation( *this, 1 );
				location1.Move( minlength );
			}

			location1.Transition( posStraight );
			location2.Transition( posDiverged );

			Frame<Length,One> frame;
			Bifurcation().Transition( frame );
			frame.FromParent( posStraight );
			frame.FromParent( posDiverged );
			return minlength;
		}
	}

	return 0_m;
}

void Switch_Imp::StreamSwitch( std::ostream& ostream ) const{
	ostream << ", narrow track id: " << (NarrowTrack().first ? NarrowTrack().first->ID() : IDType{0});
	ostream << ", straight track id: " << (StraightTrack().first ? StraightTrack().first->ID() : IDType{0});
	ostream << ", diverged track id: " << (DivergedTrack().first ? DivergedTrack().first->ID() : IDType{0}) << std::endl;
}
///////////////////////////////////////
ThreeWaySwitch_Imp::ThreeWaySwitch_Imp()
	:	NarrowSwitch_Imp{2}
{
}

ConnectorType ThreeWaySwitch_Imp::GetConnectorType() const noexcept{
	return ConnectorType::ThreeWaySwitch;
}

const char* ThreeWaySwitch_Imp::TypeName() const noexcept{
	return "ThreeWaySwitch";
}

void ThreeWaySwitch_Imp::DivergedTrack1(
	std::shared_ptr<TrackBuilder> pDivergedTrack1,
	Track::EndType trackend )
{
	Slot( slot_2, pDivergedTrack1, trackend );
}

std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> ThreeWaySwitch_Imp::DivergedTrack1() const noexcept{
	return Slot(slot_2);
}

void ThreeWaySwitch_Imp::ClearDivergedTrack1(){
	Clear(slot_2);
}

void ThreeWaySwitch_Imp::DivergedTrack2(
	std::shared_ptr<TrackBuilder> pDivergedTrack,
	Track::EndType trackend )
{
	Slot( slot_3, pDivergedTrack, trackend );
}

std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> ThreeWaySwitch_Imp::DivergedTrack2() const noexcept{
	return Slot(slot_3);
}

void ThreeWaySwitch_Imp::ClearDivergedTrack2(){
	Clear(slot_3);
}

void ThreeWaySwitch_Imp::SwapStraightWithDiverged(){
	SwapSlots( slot_1, slot_2 );
	Set( Get(), false );
}

void ThreeWaySwitch_Imp::SwapStraightWithDiverged2(){
	SwapSlots( slot_1, slot_3 );
	Set( Get(), false );
}

void ThreeWaySwitch_Imp::SwapDivergedWithDiverged2(){
	SwapSlots( slot_2, slot_3 );
	Set( Get(), false );
}

bool ThreeWaySwitch_Imp::IsNormal() const{
	if( !IsComplete() )
		throw std::runtime_error( "ThreeWaySwitch_Imp::IsNormal call invalid" );

	Position<Length> EndPosStraight;		
	Position<Length> EndPosDiverged1;		
	Position<Length> EndPosDiverged2;		
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged1, EndPosDiverged2 ) )
		return EndPosDiverged1.y >= EndPosStraight.y && EndPosStraight.y >= EndPosDiverged2.y;

	return false;
}

bool ThreeWaySwitch_Imp::Normalize(){
	Position<Length> EndPosStraight;		
	Position<Length> EndPosDiverged1;		
	Position<Length> EndPosDiverged2;		
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged1, EndPosDiverged2 ) ){
		if( EndPosDiverged1.y < EndPosStraight.y ){
			if( EndPosStraight.y >= EndPosDiverged2.y ){
				if( EndPosDiverged1.y >= EndPosDiverged2.y )
					SwapSlots( slot_0, slot_1 );
				else{
					SwapSlots( slot_1, slot_2 );
					SwapSlots( slot_0, slot_1 );
				}
			}
			else
				SwapSlots( slot_1, slot_2 );
		}
		else{
			if( EndPosStraight.y < EndPosDiverged2.y ){
				if( EndPosDiverged1.y >= EndPosDiverged2.y )
					SwapSlots( slot_0, slot_2 );
				else{
					SwapSlots( slot_1, slot_2 );
					SwapSlots( slot_0, slot_2 );
				}
			}
			else
				return false;
		}

		assert( IsNormal() );
		return true;
	}

	return false;
}

ThreeWaySwitch::Status ThreeWaySwitch_Imp::StatusToLeft() const{
	Position<Length> EndPosStraight;		
	Position<Length> EndPosDiverged1;		
	Position<Length> EndPosDiverged2;		
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged1, EndPosDiverged2 ) ){
		if( EndPosStraight.y >= EndPosDiverged1.y && EndPosStraight.y >= EndPosDiverged2.y )
			return Status::go;
		if( EndPosDiverged1.y >= EndPosStraight.y && EndPosDiverged1.y >= EndPosDiverged2.y )
			return Status::branch1;
		if( EndPosDiverged2.y >= EndPosStraight.y && EndPosDiverged2.y >= EndPosDiverged1.y )
			return Status::branch2;
	}

	return Status::none;
}

ThreeWaySwitch::Status ThreeWaySwitch_Imp::StatusToMiddle() const{
	Position<Length> EndPosStraight;		
	Position<Length> EndPosDiverged1;		
	Position<Length> EndPosDiverged2;		
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged1, EndPosDiverged2 ) ){
		if( (EndPosStraight.y >= EndPosDiverged1.y && EndPosStraight.y < EndPosDiverged2.y) ||
			(EndPosStraight.y >= EndPosDiverged2.y && EndPosStraight.y < EndPosDiverged1.y) )
			return Status::go;
		if( (EndPosDiverged1.y >= EndPosStraight.y && EndPosDiverged1.y < EndPosDiverged2.y) || 
			(EndPosDiverged1.y >= EndPosDiverged2.y && EndPosDiverged1.y < EndPosStraight.y) )
			return Status::branch1;
		if( (EndPosDiverged2.y >= EndPosStraight.y && EndPosDiverged2.y < EndPosDiverged1.y) || 
			(EndPosDiverged2.y >= EndPosDiverged1.y && EndPosDiverged2.y < EndPosStraight.y))
			return Status::branch2;
	}

	return Status::none;
}

ThreeWaySwitch::Status ThreeWaySwitch_Imp::StatusToRight() const{
	Position<Length> EndPosStraight;
	Position<Length> EndPosDiverged1;
	Position<Length> EndPosDiverged2;
	if( MinimumOutgoingTrackEndpoints( EndPosStraight, EndPosDiverged1, EndPosDiverged2 ) ){
		if( EndPosStraight.y < EndPosDiverged1.y && EndPosStraight.y < EndPosDiverged2.y )
			return Status::go;
		if( EndPosDiverged1.y < EndPosStraight.y && EndPosDiverged1.y < EndPosDiverged2.y )
			return Status::branch1;
		if( EndPosDiverged2.y < EndPosStraight.y && EndPosDiverged2.y < EndPosDiverged1.y )
			return Status::branch2;
	}

	return Status::none;
}

Jack& ThreeWaySwitch_Imp::JackOnBranch1() noexcept{
	return JackOnDivergedTrack( 0 );
}

MultiPlug& ThreeWaySwitch_Imp::PlugToBranch1() noexcept{
	return PlugToDivergedTrack( 0 );
}

Jack& ThreeWaySwitch_Imp::JackOnBranch2() noexcept{
	return JackOnDivergedTrack( 1 );
}

MultiPlug& ThreeWaySwitch_Imp::PlugToBranch2() noexcept{
	return PlugToDivergedTrack( 1 );
}

bool ThreeWaySwitch_Imp::MinimumOutgoingTrackEndpoints(
	Position<Length>& posStraight,
	Position<Length>& posDiverged1,
	Position<Length>& posDiverged2 ) const
{
	if( !IsComplete() )
		return false;

	Length minlength = std::min( std::min( Slot(1).first->GetLength(), Slot(2).first->GetLength() ), Slot(3).first->GetLength() );
	
	Location location = GetEndLocation( *this, 1 ); 
	if( location.IsOnTrack() ){
		location.Move( minlength );
		location.Transition( posStraight );
	}

	location = GetEndLocation( *this, 2 ); 
	if( location.IsOnTrack() ){
		location.Move( minlength );
		location.Transition( posDiverged1 );
	}

	location = GetEndLocation( *this, 3 ); 
	if( location.IsOnTrack() ){
		location.Move( minlength );
		location.Transition( posDiverged2 );
	}

	Frame<Length,One> frame;
	Bifurcation().Transition( frame );
	frame.FromParent( posStraight );
	frame.FromParent( posDiverged1 );
	frame.FromParent( posDiverged2 );
	return true;
}
///////////////////////////////////////
SingleSlipSwitch_Imp::SingleSlipSwitch_Imp()
	:	Connector_Imp	{slot_count},
		m_Status		{Status::go1},
		m_PlugToGo1		{*this},
		m_PlugToGo2		{*this},
		m_PlugToBranch	{*this}
{
	m_PlugToGo1.SetStatus( Status::go1 );
	m_PlugToGo1.Reference( "name", "PlugToGo1" );
	m_PlugToGo2.SetStatus( Status::go2 );
	m_PlugToGo2.Reference( "name", "PlugToGo2" );
	m_PlugToBranch.SetStatus( Status::branch );
	m_PlugToBranch.Reference( "name", "PlugToBranch" );

	m_Center.Init();
}

ConnectorType SingleSlipSwitch_Imp::GetConnectorType() const noexcept{
	return ConnectorType::SingleSlipSwitch;
}

const char* SingleSlipSwitch_Imp::TypeName() const noexcept{
	return "SingleSlipSwitch";
}

void SingleSlipSwitch_Imp::Toggle( bool pulse ){
	int status = static_cast<int>(m_Status);
	if( ++status >= static_cast<int>(Status::count) )
		status = static_cast<int>(Status::go1);

	Set( static_cast<Status>(status), pulse );
}

void SingleSlipSwitch_Imp::Set( 
	const Track& /*trackA*/, 
	Track::EndType /*trackendA*/, 
	const Track& /*trackB*/, 
	Track::EndType /*trackendB*/, 
	bool /*pulse*/ )
{
	assert( !"Not implemented yet!" );
}

bool SingleSlipSwitch_Imp::Check( std::ostream& os, Length e_distance, Angle e_kink, Angle e_twist ) const noexcept
{
	if( !IsComplete() ){
		os << Verbosity::detailed << "SingleSlipSwitch is incomplete! SwitchID: " << ID() << std::endl;
		return false;
	}

	bool bOk = true;
	if( !CheckSlot( slot_2, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_3, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_4, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_5, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	return bOk;
}

void SingleSlipSwitch_Imp::GetCenter( Frame<Length,One>& center ) const noexcept{
	center = m_Center;
}

SingleSlipSwitch::Status SingleSlipSwitch_Imp::Set( Status to, bool pulse )
{
	const Status retval = m_Status;

	m_Status = to;

	if( IsComplete() ){
		switch( m_Status ){
		case Status::go1:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_2) );
			Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_3) );

			if( pulse )
				m_JackOnGo1.Pulse();

			break;
		case Status::go2:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_2) );
			Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_3) );

			if( pulse )
				m_JackOnGo2.Pulse();

			break;
		case Status::branch:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_4) );
			Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_5) );

			if( pulse )
				m_JackOnBranch.Pulse();

			break;
		default:
			assert( !"Unknown switch status value" );
		}

		if( pulse )
			JackOnChange().Pulse();
	}

	return retval;
}

SingleSlipSwitch::Status SingleSlipSwitch_Imp::Get() const noexcept{
	return m_Status;
}

void SingleSlipSwitch_Imp::SetCenter( const Frame<Length,One>& center ) noexcept{
	m_Center = center;
}

Jack& SingleSlipSwitch_Imp::JackOn( Status status ){
	switch( status ){
	case Status::go1:
		return m_JackOnGo1;
	case Status::go2:
		return m_JackOnGo2;
	case Status::branch:
		return m_JackOnBranch;
	case Status::change:
		return JackOnChange();
	default:
		throw std::invalid_argument( "Unknown ThreeWaySwitch::Status enumeration value!" );
	}
}

MultiPlug& SingleSlipSwitch_Imp::PlugTo(Status status){
	switch( status ){
	case Status::go1:
		return m_PlugToGo1;
	case Status::go2:
		return m_PlugToGo2;
	case Status::branch:
		return m_PlugToBranch;
	case Status::toggle:
		return PlugToToggle();
	default:
		throw std::invalid_argument( "Unknown Switch::Status enumeration value!" );
	}
}

int SingleSlipSwitch_Imp::Slot(
	int slot,
	std::shared_ptr<TrackBuilder> pTrack,
	Track::EndType trackend, 
	bool connectAnonymous )
{
	const int retval = Connector_Imp::Slot( slot, pTrack, trackend, connectAnonymous );

	if( IsComplete() ){
		Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_2) );
		Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_4) );
		Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_3) );
		Slot(slot_1).first->Couple( Slot(slot_1), Slot(5) );

		Set( m_Status, false );
	}

	return retval;
}

void SingleSlipSwitch_Imp::RegisterSockets( SocketRegistry& modul ){
	Connector_Imp::RegisterSockets( modul );

	modul.RegisterPlug( m_PlugToGo1 );
	modul.RegisterPlug( m_PlugToGo2 );
	modul.RegisterPlug( m_PlugToBranch );
	modul.ConnectJack( m_JackOnGo1 );
	modul.ConnectJack( m_JackOnGo2 );
	modul.ConnectJack( m_JackOnBranch );
}

void SingleSlipSwitch_Imp::UnregisterSockets( SocketRegistry& modul ){
	modul.RemoveJack( m_JackOnBranch );
	modul.RemoveJack( m_JackOnGo2 );
	modul.RemoveJack( m_JackOnGo1 );
	modul.UnRegisterPlug( m_PlugToBranch );
	modul.UnRegisterPlug( m_PlugToGo2 );
	modul.UnRegisterPlug( m_PlugToGo1 );

	Connector_Imp::UnregisterSockets( modul );
}

int SingleSlipSwitch_Imp::CountPlugs() const{
	return	m_PlugToGo1.CountPlugs() + 
			m_PlugToGo2.CountPlugs() + 
			m_PlugToBranch.CountPlugs() + 
			Connector_Imp::CountPlugs();
}

int SingleSlipSwitch_Imp::CountJacks() const noexcept{
	return Connector_Imp::CountJacks() + 3;
}

const Plug& SingleSlipSwitch_Imp::_GetPlug(int idx) const{
	if( idx < m_PlugToGo1.CountPlugs() )
		return m_PlugToGo1.GetPlug( idx );

	idx -= m_PlugToGo1.CountPlugs();
	if( idx < m_PlugToGo2.CountPlugs() )
		return m_PlugToGo2.GetPlug( idx );

	idx -= m_PlugToGo2.CountPlugs();
	if( idx < m_PlugToBranch.CountPlugs() )
		return m_PlugToBranch.GetPlug( idx );

	idx -= m_PlugToBranch.CountPlugs();
	return Connector_Imp::_GetPlug( idx );
}

const Jack& SingleSlipSwitch_Imp::_GetJack(int idx) const{
	switch( idx ){
	case 0:
		return m_JackOnGo1;
	case 1:
		return m_JackOnGo2;
	case 2:
		return m_JackOnBranch;
	case 3:
		return JackOnChange();
	default:
		std::ostringstream stream;
		stream << "Out of range!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	}
}

std::string ToString( SingleSlipSwitch::Status status ){
	switch( status ){
	case SingleSlipSwitch::Status::none:
		return "none";
	case SingleSlipSwitch::Status::go1:
		return "go1";
	case SingleSlipSwitch::Status::go2:
		return "go2";
	case SingleSlipSwitch::Status::branch:
		return "branch";
	case SingleSlipSwitch::Status::toggle:
		return "toggle";
	case SingleSlipSwitch::Status::change:
		return "change";
	default:
		assert( !"Unknown Switch::Status enumerator!" );
		return "unknown";
	}
}

SingleSlipSwitch::Status ToSingleSlipSwitchStatus( const std::string& status ){
	if( status == "go" )
		return SingleSlipSwitch::Status::go;
	if( status == "go1" )
		return SingleSlipSwitch::Status::go1;
	if( status == "go2" )
		return SingleSlipSwitch::Status::go2;
	if( status == "branch" )
		return SingleSlipSwitch::Status::branch;
	if( status == "toggle" )
		return SingleSlipSwitch::Status::toggle;
	if( status == "change" )
		return SingleSlipSwitch::Status::change;
	if( status == "none" )
		return SingleSlipSwitch::Status::none;

	std::ostringstream stream;
	stream << "Invalid SingleSlipSwitch::Status string!" << status << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

SingleSlipSwitch::Status SingleSlipSwitchStatusFrom( const std::string& socketName ){
	if( socketName == "JackOnGo1" ||
		socketName == "PlugToGo1" )
		return SingleSlipSwitch::Status::go1;
	else if(	socketName == "JackOnGo2" ||
				socketName == "PlugToGo2" )
		return SingleSlipSwitch::Status::go2;
	else if(	socketName == "JackOnBranch" ||
				socketName == "PlugToBranch" )
		return SingleSlipSwitch::Status::branch;
	else if( socketName == "JackOnChange" )
		return SingleSlipSwitch::Status::change;
	else if( socketName == "PlugToToggle" )
		return SingleSlipSwitch::Status::toggle;

	std::ostringstream stream;
	stream << "Invalid SingleSlipSwitch::Status socketName string!" << socketName << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}
///////////////////////////////////////
DoubleSlipSwitch_Imp::DoubleSlipSwitch_Imp()
	:	Connector_Imp	{slot_count},
		m_Status		{Status::go1},
		m_PlugToGo1		{*this},
		m_PlugToGo2		{*this},
		m_PlugToBranch1	{*this},
		m_PlugToBranch2	{*this}
{
	m_PlugToGo1.SetStatus( Status::go1 );
	m_PlugToGo1.Reference( "name", "PlugToGo1" );
	m_PlugToGo2.SetStatus( Status::go2 );
	m_PlugToGo2.Reference( "name", "PlugToGo2" );
	m_PlugToBranch1.SetStatus( Status::branch1 );
	m_PlugToBranch1.Reference( "name", "PlugToBranch1" );
	m_PlugToBranch2.SetStatus( Status::branch2 );
	m_PlugToBranch2.Reference( "name", "PlugToBranch2" );

	m_Center.Init();
}

ConnectorType DoubleSlipSwitch_Imp::GetConnectorType() const noexcept{
	return ConnectorType::DoubleSlipSwitch;
}

const char* DoubleSlipSwitch_Imp::TypeName() const noexcept{
	return "DoubleSlipSwitch";
}

void DoubleSlipSwitch_Imp::Toggle( bool pulse ){
	int status = static_cast<int>(m_Status);
	if( ++status >= static_cast<int>(Status::count) )
		status = static_cast<int>(Status::go1);

	Set( static_cast<Status>(status), pulse );
}

void DoubleSlipSwitch_Imp::Set(
	const Track& /*trackA*/,
	Track::EndType /*trackendA*/,
	const Track& /*trackB*/,
	Track::EndType /*trackendB*/,
	bool /*pulse*/)
{
	assert( !"Not implemented yet!" );
}

bool DoubleSlipSwitch_Imp::Check( std::ostream& os, Length e_distance, Angle e_kink, Angle e_twist ) const noexcept
{
	if( !IsComplete() ){
		os << Verbosity::detailed << "DoubleSlipSwitch is incomplete! SwitchID: " << ID() << std::endl;
		return false;
	}

	bool bOk = true;
	if( !CheckSlot( slot_4, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_5, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_6, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_7, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_8, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_9, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_10, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	if( !CheckSlot( slot_11, os, e_distance, e_kink, e_twist ) )
		bOk = false;

	return bOk;
}

void DoubleSlipSwitch_Imp::GetCenter( Frame<Length,One>& center ) const noexcept{
	center = m_Center;
}

DoubleSlipSwitch::Status DoubleSlipSwitch_Imp::Set( Status to, bool pulse ){
	const Status retval = m_Status;

	m_Status = to;

	if( IsComplete() ){
		switch( m_Status ){
		case Status::go1:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_4) );
			Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_5) );
			Slot(slot_2).first->Couple( Slot(slot_2), Slot(slot_6) );
			Slot(slot_3).first->Couple( Slot(slot_3), Slot(slot_7) );

			if( pulse )
				m_JackOnGo1.Pulse();

			break;
		case Status::go2:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_4) );
			Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_5) );
			Slot(slot_2).first->Couple( Slot(slot_2), Slot(slot_6) );
			Slot(slot_3).first->Couple( Slot(slot_3), Slot(slot_7) );

			if( pulse )
				m_JackOnGo2.Pulse();

			break;
		case Status::branch1:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_8) );
			Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_11) );
			Slot(slot_2).first->Couple( Slot(slot_2), Slot(slot_10) );
			Slot(slot_3).first->Couple( Slot(slot_3), Slot(slot_9) );

			if( pulse )
				m_JackOnBranch1.Pulse();

			break;
		case Status::branch2:
			Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_8) );
			Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_11) );
			Slot(slot_2).first->Couple( Slot(slot_2), Slot(slot_10) );
			Slot(slot_3).first->Couple( Slot(slot_3), Slot(slot_9) );

			if( pulse )
				m_JackOnBranch2.Pulse();

			break;
		default:
			assert( !"Unknown switch status value" );
		}

		if( pulse )
			JackOnChange().Pulse();
	}

	return retval;
}

DoubleSlipSwitch::Status DoubleSlipSwitch_Imp::Get() const noexcept{
	return m_Status;
}

void DoubleSlipSwitch_Imp::SetCenter( const Frame<Length,One>& center ) noexcept{
	m_Center = center;
}

Jack& DoubleSlipSwitch_Imp::JackOn(Status status){
	switch( status ){
	case Status::go1:
		return m_JackOnGo1;
	case Status::go2:
		return m_JackOnGo2;
	case Status::branch1:
		return m_JackOnBranch1;
	case Status::branch2:
		return m_JackOnBranch2;
	case Status::change:
		return JackOnChange();
	default:
		throw std::invalid_argument( "Unknown ThreeWaySwitch::Status enumeration value!" );
	}
}

MultiPlug& DoubleSlipSwitch_Imp::PlugTo(Status status){
	switch( status ){
	case Status::go1:
		return m_PlugToGo1;
	case Status::go2:
		return m_PlugToGo2;
	case Status::branch1:
		return m_PlugToBranch1;
	case Status::branch2:
		return m_PlugToBranch2;
	case Status::toggle:
		return PlugToToggle();
	default:
		throw std::invalid_argument( "Unknown Switch::Status enumeration value!" );
	}
}

int DoubleSlipSwitch_Imp::Slot(
	int slot,
	std::shared_ptr<TrackBuilder> pTrack,
	Track::EndType trackend, 
	bool connectAnonymous )
{
	const int retval = Connector_Imp::Slot( slot, pTrack, trackend, connectAnonymous );

	if( IsComplete() ){
		Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_4) );
		Slot(slot_0).first->Couple( Slot(slot_0), Slot(slot_8) );
		Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_5) );
		Slot(slot_1).first->Couple( Slot(slot_1), Slot(slot_11) );
		Slot(slot_2).first->Couple( Slot(slot_2), Slot(slot_6) );
		Slot(slot_2).first->Couple( Slot(slot_2), Slot(slot_10) );
		Slot(slot_3).first->Couple( Slot(slot_3), Slot(slot_7) );
		Slot(slot_3).first->Couple( Slot(slot_3), Slot(slot_9) );

		Set( m_Status, false );
	}

	return retval;
}

void DoubleSlipSwitch_Imp::RegisterSockets( SocketRegistry& modul ){
	Connector_Imp::RegisterSockets( modul );

	modul.RegisterPlug( m_PlugToGo1 );
	modul.RegisterPlug( m_PlugToGo2 );
	modul.RegisterPlug( m_PlugToBranch1 );
	modul.RegisterPlug( m_PlugToBranch2 );
	modul.ConnectJack( m_JackOnGo1 );
	modul.ConnectJack( m_JackOnGo2 );
	modul.ConnectJack( m_JackOnBranch1 );
	modul.ConnectJack( m_JackOnBranch2 );
}

void DoubleSlipSwitch_Imp::UnregisterSockets( SocketRegistry& modul ){
	modul.RemoveJack( m_JackOnBranch2 );
	modul.RemoveJack( m_JackOnBranch1 );
	modul.RemoveJack( m_JackOnGo2 );
	modul.RemoveJack( m_JackOnGo1 );
	modul.UnRegisterPlug( m_PlugToBranch2 );
	modul.UnRegisterPlug( m_PlugToBranch1 );
	modul.UnRegisterPlug( m_PlugToGo2 );
	modul.UnRegisterPlug( m_PlugToGo1 );

	Connector_Imp::UnregisterSockets( modul );
}

int DoubleSlipSwitch_Imp::CountPlugs() const{
	return	m_PlugToGo1.CountPlugs() + 
			m_PlugToGo2.CountPlugs() + 
			m_PlugToBranch1.CountPlugs() + 
			m_PlugToBranch2.CountPlugs() + 
			Connector_Imp::CountPlugs();
}

int DoubleSlipSwitch_Imp::CountJacks() const noexcept{
	return Connector_Imp::CountJacks() + 4;
}

const Plug& DoubleSlipSwitch_Imp::_GetPlug(int idx) const{
	if( idx < m_PlugToGo1.CountPlugs() )
		return m_PlugToGo1.GetPlug( idx );

	idx -= m_PlugToGo1.CountPlugs();
	if( idx < m_PlugToGo2.CountPlugs() )
		return m_PlugToGo2.GetPlug( idx );

	idx -= m_PlugToGo2.CountPlugs();
	if( idx < m_PlugToBranch1.CountPlugs() )
		return m_PlugToBranch1.GetPlug( idx );

	idx -= m_PlugToBranch1.CountPlugs();
	if( idx < m_PlugToBranch2.CountPlugs() )
		return m_PlugToBranch2.GetPlug( idx );

	idx -= m_PlugToBranch2.CountPlugs();
	return Connector_Imp::_GetPlug( idx );
}

const Jack& DoubleSlipSwitch_Imp::_GetJack(int idx) const{
	switch( idx ){
	case 0:
		return m_JackOnGo1;
	case 1:
		return m_JackOnGo2;
	case 2:
		return m_JackOnBranch1;
	case 3:
		return m_JackOnBranch2;
	case 4:
		return JackOnChange();
	default:
		std::ostringstream stream;
		stream << "Out of range!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	}
}

std::string ToString( DoubleSlipSwitch::Status status ){
	switch( status ){
	case DoubleSlipSwitch::Status::none:
		return "none";
	case DoubleSlipSwitch::Status::go1:
		return "go1";
	case DoubleSlipSwitch::Status::go2:
		return "go2";
	case DoubleSlipSwitch::Status::branch1:
		return "branch1";
	case DoubleSlipSwitch::Status::branch2:
		return "branch2";
	case DoubleSlipSwitch::Status::toggle:
		return "toggle";
	case DoubleSlipSwitch::Status::change:
		return "change";
	default:
		assert( !"Unknown Switch::Status enumerator!" );
		return "unknown";
	}
}

DoubleSlipSwitch::Status ToDoubleSlipSwitchStatus( const std::string& status ){
	if( status == "go" )
		return DoubleSlipSwitch::Status::go;
	if( status == "go1" )
		return DoubleSlipSwitch::Status::go1;
	if( status == "go2" )
		return DoubleSlipSwitch::Status::go2;
	if( status == "branch" )
		return DoubleSlipSwitch::Status::branch;
	if( status == "branch1" )
		return DoubleSlipSwitch::Status::branch1;
	if( status == "branch2" )
		return DoubleSlipSwitch::Status::branch2;
	if( status == "toggle" )
		return DoubleSlipSwitch::Status::toggle;
	if( status == "change" )
		return DoubleSlipSwitch::Status::change;
	if( status == "none" )
		return DoubleSlipSwitch::Status::none;

	std::ostringstream stream;
	stream << "Invalid DoubleSlipSwitch::Status string!" << status << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

DoubleSlipSwitch::Status DoubleSlipSwitchStatusFrom( const std::string& socketName ){
	if( socketName == "JackOnGo1" ||
		socketName == "PlugToGo1" )
		return DoubleSlipSwitch::Status::go1;
	else if(	socketName == "JackOnGo2" ||
				socketName == "PlugToGo2" )
		return DoubleSlipSwitch::Status::go2;
	else if(	socketName == "JackOnBranch1" ||
				socketName == "PlugToBranch1" )
		return DoubleSlipSwitch::Status::branch1;
	else if(	socketName == "JackOnBranch2" ||
				socketName == "PlugToBranch2" )
		return DoubleSlipSwitch::Status::branch2;
	else if( socketName == "JackOnChange" )
		return DoubleSlipSwitch::Status::change;
	else if( socketName == "PlugToToggle" )
		return DoubleSlipSwitch::Status::toggle;

	std::ostringstream stream;
	stream << "Invalid DoubleSlipSwitch::Status socketName string!" << socketName << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}
///////////////////////////////////////
}