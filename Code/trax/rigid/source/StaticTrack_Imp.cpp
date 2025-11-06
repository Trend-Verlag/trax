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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "StaticTrack_Imp.h"

#include "../Shape.h"
#include "../Scene.h"
#include "trax/TrackPainter.h"

#include <iostream>

namespace trax{
	using namespace spat;

///////////////////////////////////////
std::shared_ptr<StaticTrack> StaticTrack::Make( const Scene& scene ) noexcept{
	try{
		std::shared_ptr<StaticTrack_Imp> pRetval = std::make_shared<StaticTrack_Imp>( scene );

		if( pRetval )
			pRetval->SetWeakPointerToSelf( pRetval );

		return pRetval;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

StaticTrack_Imp::StaticTrack_Imp( const Scene& scene )
	: m_Scene	{ scene }
	, m_pShape	{ scene.CreateShape() }
{
}

Track::TrackType StaticTrack_Imp::GetTrackType() const noexcept{
	return TrackType::withGeoms;
}

bool StaticTrack_Imp::IsValid() const noexcept
{
	if( !SectionTrack_Imp::IsValid() )
		return false;

	if( GetLength() == +infinite__length )
		return false;

	if( !m_pShape )
		return false;

	return true;
}

void StaticTrack_Imp::SetMaterial( const Material& material ) noexcept{
	m_Material = material;
}

const Material& StaticTrack_Imp::GetMaterial() const noexcept{
	return m_Material;
}

const Shape& StaticTrack_Imp::GetShape() const noexcept{
	return *m_pShape;
}

void StaticTrack_Imp::PropagateAbsoluteFrameToClients() noexcept
{
	SectionTrack_Imp::PropagateAbsoluteFrameToClients();
	m_pShape->SetFrame( GetAbsoluteFrame() * GetFrame() );
}

void StaticTrack_Imp::OnGeometryChanged() noexcept
{
	m_pShape->Clear();

	if( IsValid() )
		CreateShape();
}

void StaticTrack_Imp::CreateShape() noexcept
{
	try{
		if( std::unique_ptr<GeomMesh> pGeomMesh = m_Scene.CreateGeomTriangleMesh(); pGeomMesh ){
			std::vector<Position<Length>> points;
			std::vector<int> indices;

			BufferedPainter<Position<Length>> painter{ points, indices, TrackPainter::Mode::mode_localFrame, {1_m,1_m} };

			if( GetSection() ){
				painter.Paint( *this, *GetSection() );

				if( pGeomMesh->Create( points, indices ) ){
					pGeomMesh->SetMaterial( m_Material );
					pGeomMesh->TypeFilter( Geom::fTrack );
					m_pShape->Attach( std::move(pGeomMesh) );
				}
			}
			else
				std::cerr << "TrackPainter: No section for track!" << std::endl;
		}
	}
	catch( const std::exception& e ){
		std::cerr << e.what() << std::endl;
	}
}
///////////////////////////////////////
spat::Box<Length> GetBoxFor( const TrackBuilder& track, const Section* pSection )
{
	Length s = 0_m;	
	Position<Length> position;
	track.Transition( s, position );
	spat::Box<Length> box{ position, position };

	while( const Length ds = Segment_Checked( track, s, epsilon__length, { 1_m, +infinite__length } ) )
	{
		track.Transition( s += ds, position );
		box.Expand( position );
	}

	if( pSection ){
		const Rect<Length> clearance = pSection->Clearance();

		Length d = std::max( abs(clearance.Left()), abs(clearance.Right()) );
		d = std::max( d, abs(clearance.Bottom()) );
		d = std::max( d, abs(clearance.Top()) );

		box.Inflate( d, d, d );
	}
 
	return box;
}
///////////////////////////////////////
}