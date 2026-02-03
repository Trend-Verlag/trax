//	trax track library
//	AD 2013 
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

#include "trax/support/Fixtures.h"

#include "trax/Plug.h"
#include "trax/Jack.h"
#include "trax/Event.h"


#ifndef TRAX_FIXTURES_DIR
#define TRAX_FIXTURES_DIR 
#pragma message("TRAX_FIXTURES_DIR not defined; fixure path might not be found.")
#endif // !TRAX_FIXTURES_DIR

using namespace dim::literals;

///////////////////////////////////////
std::filesystem::path FixtureBase::FixturePath()
{
	//std::filesystem::path fixturePath = std::filesystem::current_path();
	//fixturePath = fixturePath.parent_path();
	//fixturePath = fixturePath.parent_path();
	//fixturePath /= "Fixtures";

	//return fixturePath;

	return std::filesystem::path{ std::string{TRAX_FIXTURES_DIR} };
}
///////////////////////////////////////
TrackFixture::TrackFixture()
{
	m_pTrack = trax::TrackBuilder::Make();
	std::shared_ptr<trax::ArcP> pArc = trax::ArcP::Make();
	pArc->Create( { {0_m,0_m,0_m}, {1,0,0}, {0,_m(100_m),0} } );
	m_pTrack->Attach( pArc, {0_m,100_m*dim::pi/2}  );
}

TrackFixture::~TrackFixture(){
	m_pTrack->DeCouple();
}
///////////////////////////////////////
ThreeTracksInALineFixture::ThreeTracksInALineFixture()
	: m_pTrack1	{ trax::TrackBuilder::Make() }
	, m_pTrack2	{ trax::TrackBuilder::Make() }
	, m_pTrack3{ trax::TrackBuilder::Make() }
{
	std::shared_ptr<trax::Line> pLine = trax::Line::Make();

	m_pTrack1->ID( 1 );
	m_pTrack2->ID( 2 );
	m_pTrack3->ID( 3 );

	m_pTrack1->Attach( pLine, { 0_m,10_m } );
	m_pTrack2->Attach( pLine, { 0_m,10_m } );
	m_pTrack3->Attach( pLine, { 0_m,10_m } );

	spat::Frame<dim::Length,dim::One> frame = spat::Identity<dim::Length,dim::One>;
	m_pTrack1->SetFrame( frame );
	frame.TransportTan( 10_m );
	m_pTrack2->SetFrame( frame);
	frame.TransportTan( 10_m );
	m_pTrack3->SetFrame( frame );

	m_pTrack1->Couple( std::make_pair( m_pTrack1,trax::EndType::south ), std::make_pair( m_pTrack2, trax::EndType::north ) );
	m_pTrack2->Couple( std::make_pair( m_pTrack2, trax::EndType::south ), std::make_pair( m_pTrack3, trax::EndType::north ) );
}

ThreeTracksInALineFixture::~ThreeTracksInALineFixture()
{
	m_pTrack2->DeCouple();
}
///////////////////////////////////////
SwichFixture::SwichFixture()
{	//		 
	//	1		2
	// ----- ------
	//		\3
	//		 \
	//
	m_pTrack1 = trax::TrackBuilder::Make();
 	std::shared_ptr<trax::LineP> pLine = trax::LineP::Make();
 	pLine->Create( spat::Origin3D<trax::Length>, spat::Position<trax::Length>(1_m,0_m,0_m), spat::Vector<trax::One>(0,0,1) );
	m_pTrack1->Attach( pLine, {0_m,10_m} );

	m_pTrack2 = trax::TrackBuilder::Make();
 	std::shared_ptr<trax::ArcP> pArc1 = trax::ArcP::Make();
	pArc1->Create( { {10_m,0_m,0_m}, {1,0,0} }, {20_m,1_m,0_m} );
	m_pTrack2->Attach( pArc1, {0_m,10_m} );

	m_pTrack3 = trax::TrackBuilder::Make();
 	std::shared_ptr<trax::ArcP> pArc2 = trax::ArcP::Make();
	pArc2->Create( { {10_m,0_m,0_m}, {1,0,0} }, {20_m,-10_m,0_m} );
	m_pTrack3->Attach( pArc2, {0_m,10_m} );
}

SwichFixture::~SwichFixture()
{
	m_pTrack1->DeCouple();
	m_pTrack2->DeCouple();
	m_pTrack3->DeCouple();
}

TrackAndLocation::TrackAndLocation()
{
	m_LocationOnTrack.PutOn( m_pTrack, trax::TrackLocation( m_pTrack->GetLength()/2, true ) );
}
///////////////////////////////////////
TrackCircle::TrackCircle()
	:	m_pTrack1	{ trax::TrackBuilder::Make() },
		m_pTrack2	{ trax::TrackBuilder::Make() },
		m_pTrack3	{ trax::TrackBuilder::Make() },
		m_pTrack4	{ trax::TrackBuilder::Make() },
		m_pArc1		{ trax::ArcP::Make() },
		m_pArc2		{ trax::ArcP::Make() },
		m_pArc3		{ trax::ArcP::Make() },
		m_pArc4		{ trax::ArcP::Make() },
		R			{ 100_m }
{
	m_pTrack1->ID( 1 );
	m_pTrack2->ID( 2 );
	m_pTrack3->ID( 3 );
	m_pTrack4->ID( 4 );

	// this fixture should give a circle with r == 10...
	m_pArc1->Create( { spat::Origin3D<dim::Length>, {1,0,0}, {0,_m(R),0} } );
	m_pTrack1->Attach( m_pArc1, {0_m,R*dim::pi/2} );
	m_pTrack1->Attach( trax::LinearTwist::Make() );

	m_pArc2->Create( { spat::Origin3D<dim::Length>, {0,1,0}, {_m(-R),0,0} } );
	m_pTrack2->Attach( m_pArc2, {0_m,R*dim::pi/2} );
	m_pTrack2->Attach( trax::LinearTwist::Make() );

	m_pArc3->Create( { spat::Origin3D<dim::Length>, {-1,0,0}, {0,_m(-R),0} } );
	m_pTrack3->Attach( m_pArc3, {0_m,R*dim::pi/2} );
	m_pTrack3->Attach( trax::LinearTwist::Make() );

	m_pArc4->Create( { spat::Origin3D<dim::Length>, {0,-1,0}, {_m(R),0,0} } );
	m_pTrack4->Attach( m_pArc4,{0_m, R*dim::pi/2} );
	m_pTrack4->Attach( trax::LinearTwist::Make() );

	m_pTrack1->Couple( std::make_pair(m_pTrack1, trax::EndType::south), std::make_pair(m_pTrack2, trax::EndType::north) );
	m_pTrack2->Couple( std::make_pair(m_pTrack2, trax::EndType::south), std::make_pair(m_pTrack3, trax::EndType::north) );
	m_pTrack3->Couple( std::make_pair(m_pTrack3, trax::EndType::south), std::make_pair(m_pTrack4, trax::EndType::north) );
	m_pTrack4->Couple( std::make_pair(m_pTrack4, trax::EndType::south), std::make_pair(m_pTrack1, trax::EndType::north) );

	m_Location.PutOn( m_pTrack1, trax::TrackLocation( 0_m, true ) );
}

TrackCircle::~TrackCircle(){
	m_pTrack1->DeCouple();
	m_pTrack3->DeCouple();
}
/////////////////////////////////////
SensorFixture::SensorFixture()
	:	TrackCircle		(),
		m_pSensor		( trax::Sensor::Make() ),
		m_pPulseCounter	( trax::PulseCounter::Make() ),
		m_pEvent		( trax::Event::Make() )
{
	m_pSensor->JackOnTrigger().Insert( &m_pPulseCounter->PlugToCountUp() );
}

SensorFixture::~SensorFixture(){
}
/////////////////////////////////////
