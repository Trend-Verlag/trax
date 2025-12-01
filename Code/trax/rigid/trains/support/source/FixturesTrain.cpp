//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
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

#include "../FixturesTrain.h"
#include "trax/Section.h"
#include "trax/rigid/StaticTrack.h"
#include "trax/collections/TrackSystem.h"

using namespace dim::literals;

///////////////////////////////////////
TrainFixture::TrainFixture( bool bVisualDebugger )
	:	m_pSimulator{ trax::Simulator::Make( bVisualDebugger ? trax::Simulator::Type::PhysX_VisualDebugger : trax::Simulator::Type::PhysX ) },
		m_pScene	{ trax::Scene::Make(*m_pSimulator) },
		m_pTrack1	{ trax::StaticTrack::Make(*m_pScene) },
		m_pTrack2	{ trax::StaticTrack::Make(*m_pScene) },
		m_pTrack3	{ trax::StaticTrack::Make(*m_pScene) },
		m_pTrack4	{ trax::StaticTrack::Make(*m_pScene) },
		m_pTrack5	{ trax::StaticTrack::Make(*m_pScene) },
		m_pTrack6	{ trax::StaticTrack::Make(*m_pScene) },
		m_pArc1		{ trax::ArcP::Make() },
		m_pArc2		{ trax::ArcP::Make() },
		m_pArc3		{ trax::ArcP::Make() },
		m_pArc4		{ trax::ArcP::Make() },
		m_pLine1	{ trax::Line::Make() },
		R			{ 100_m }
{
	std::shared_ptr<trax::Section> pSection = trax::Section::Make( trax::Section::SpecialSections::vignol_UIC60 );

	// this fixture should give a circle with r == 10...
	m_pArc1->Create( { spat::Origin3D<dim::Length>, {1,0,0}, {0,_m(R),0} } );
	m_pTrack1->Attach( pSection );
	m_pTrack1->Attach( m_pArc1, {0_m,R*dim::pi/2} );
	m_pTrack1->Attach( trax::LinearTwist::Make() );

	m_pArc2->Create( { spat::Origin3D<dim::Length>, {0,1,0}, {_m(-R),0,0} } );
	m_pTrack2->Attach( pSection );
	m_pTrack2->Attach( m_pArc2, {0_m,R*dim::pi/2} );
	m_pTrack2->Attach( trax::LinearTwist::Make() );

	m_pArc3->Create( { spat::Origin3D<dim::Length>, {-1,0,0}, {0,_m(-R),0} } );
	m_pTrack3->Attach( pSection );
	m_pTrack3->Attach( m_pArc3, {0_m,R*dim::pi/2} );
	m_pTrack3->Attach( trax::LinearTwist::Make() );

	m_pArc4->Create( { spat::Origin3D<dim::Length>, {0,-1,0}, {_m(R),0,0} } );
	m_pTrack4->Attach( pSection );
	m_pTrack4->Attach( m_pArc4,{0_m, R*dim::pi/2} );
	m_pTrack4->Attach( trax::LinearTwist::Make() );

	m_pTrack5->Attach( pSection );
	m_pTrack5->Attach( m_pLine1, {0_m, R} );
	m_pTrack5->Attach( trax::LinearTwist::Make() );

	m_pTrack6->Attach( pSection );
	m_pTrack6->Attach( m_pLine1, {0_m, R} );
	m_pTrack6->Attach( trax::LinearTwist::Make() );
	spat::Frame<dim::Length,dim::One> frame;
	frame.Init();
	frame.T *= -1;
	frame.N *= -1;
	m_pTrack6->SetFrame( frame );

	m_pTrack1->Couple( std::make_pair(m_pTrack1, trax::Track::EndType::end), std::make_pair(m_pTrack2, trax::Track::EndType::front) );
	m_pTrack2->Couple( std::make_pair(m_pTrack2, trax::Track::EndType::end), std::make_pair(m_pTrack3, trax::Track::EndType::front) );
	m_pTrack3->Couple( std::make_pair(m_pTrack3, trax::Track::EndType::end), std::make_pair(m_pTrack4, trax::Track::EndType::front) );
	m_pTrack4->Couple( std::make_pair(m_pTrack4, trax::Track::EndType::end), std::make_pair(m_pTrack1, trax::Track::EndType::front) );

	m_pTrack5->Couple( std::make_pair( m_pTrack5, trax::Track::EndType::front ), std::make_pair( m_pTrack6, trax::Track::EndType::front ) );

	m_Location.PutOn( m_pTrack1, trax::TrackLocation{ 0_m, true } );
}

TrainFixture::~TrainFixture(){
	m_pTrack1->DeCouple();
	m_pTrack3->DeCouple();
	m_pTrack5->DeCouple();
}
/////////////////////////////////////
///////////////////////////////////////
MultiTrackSystemFixture::MultiTrackSystemFixture( bool bVisualDebugger )
	:	m_pSimulator{ trax::Simulator::Make( bVisualDebugger ? trax::Simulator::Type::PhysX_VisualDebugger : trax::Simulator::Type::PhysX ) },
		m_pScene	{ trax::Scene::Make(*m_pSimulator) }
{
}

MultiTrackSystemFixture::~MultiTrackSystemFixture()
{
	if( m_pTrackSystem )
		m_pTrackSystem->DeCoupleAll();
}

void MultiTrackSystemFixture::BuildFixture( int nTrackCollections )
{
	m_pTrackSystem = trax::TrackSystem::Make();

	// this fixture should give a circle with r == 10...
	dim::Length R = 50_m;
	dim::Length D = 2*R + 5_m;
	dim::Length H = 10_m;
	std::shared_ptr<trax::ArcP> pArc1 = trax::ArcP::Make();
	std::shared_ptr<trax::ArcP> pArc2 = trax::ArcP::Make();
	std::shared_ptr<trax::ArcP> pArc3 = trax::ArcP::Make();
	std::shared_ptr<trax::ArcP> pArc4 = trax::ArcP::Make();

	pArc1->Create( { spat::Origin3D<dim::Length>, {1,0,0}, {0,_m(R),0} } );
	pArc2->Create( { spat::Origin3D<dim::Length>, {0,1,0}, {_m(-R),0,0} } );
	pArc3->Create( { spat::Origin3D<dim::Length>, {-1,0,0}, {0,_m(-R),0} } );
	pArc4->Create( { spat::Origin3D<dim::Length>, {0,-1,0}, {_m(R),0,0} } );

	std::shared_ptr<trax::Section> pSection = trax::Section::Make( trax::Section::SpecialSections::vignol_UIC60 );

	int length = static_cast<int>(cbrt( nTrackCollections ));
	spat::Frame<dim::Length,dim::One> frame;
	frame.Init();
	frame.TransportTan( -D * length/2 );
	frame.TransportNor( -D * length/2 );
	frame.TransportBin( -H * length/2 );

	for( int i = 0; i < nTrackCollections; ++i )
	{
		trax::IDType id = m_pTrackSystem->CreateCollection();
		m_pTrackSystem->SetCollectionFrame( id, frame );

		std::shared_ptr<trax::StaticTrack>	pTrack1 = trax::StaticTrack::Make(*m_pScene);
		std::shared_ptr<trax::StaticTrack>	pTrack2 = trax::StaticTrack::Make(*m_pScene);
		std::shared_ptr<trax::StaticTrack>	pTrack3 = trax::StaticTrack::Make(*m_pScene);
		std::shared_ptr<trax::StaticTrack>	pTrack4 = trax::StaticTrack::Make(*m_pScene);

		//std::shared_ptr<TrackBuilder>	pTrack1 = TrackBuilder::Make();
		//std::shared_ptr<TrackBuilder>	pTrack2 = TrackBuilder::Make();
		//std::shared_ptr<TrackBuilder>	pTrack3 = TrackBuilder::Make();
		//std::shared_ptr<TrackBuilder>	pTrack4 = TrackBuilder::Make();

		pTrack1->Attach( pSection );
		pTrack1->Attach( pArc1, {0_m,R*dim::pi/2} );
		pTrack1->Attach( trax::LinearTwist::Make() );

		pTrack2->Attach( pSection );
		pTrack2->Attach( pArc2, {0_m,R*dim::pi/2} );
		pTrack2->Attach( trax::LinearTwist::Make() );

		pTrack3->Attach( pSection );
		pTrack3->Attach( pArc3, {0_m,R*dim::pi/2} );	
		pTrack3->Attach( trax::LinearTwist::Make() );

		pTrack4->Attach( pSection );
		pTrack4->Attach( pArc4, {0_m,R*dim::pi/2} );
		pTrack4->Attach( trax::LinearTwist::Make() );

		pTrack1->Couple( std::make_pair(pTrack1, trax::Track::EndType::end), std::make_pair(pTrack2, trax::Track::EndType::front) );
		pTrack2->Couple( std::make_pair(pTrack2, trax::Track::EndType::end), std::make_pair(pTrack3, trax::Track::EndType::front) );
		pTrack3->Couple( std::make_pair(pTrack3, trax::Track::EndType::end), std::make_pair(pTrack4, trax::Track::EndType::front) );
		pTrack4->Couple( std::make_pair(pTrack4, trax::Track::EndType::end), std::make_pair(pTrack1, trax::Track::EndType::front) );

		m_pTrackSystem->Add( pTrack1 );
		m_pTrackSystem->Add( pTrack2 );
		m_pTrackSystem->Add( pTrack3 );
		m_pTrackSystem->Add( pTrack4 );

		if( (i+1) % length ){
			frame.TransportTan( D );
		}
		else{
			frame.TransportTan( -(length-1)*D );

			if( (i+1) % (length*length) ){
				frame.TransportNor( D );
			}
			else{
				frame.TransportNor( -(length-1)*D );
				frame.TransportBin( H );
			}
		}
	}
}
///////////////////////////////////////