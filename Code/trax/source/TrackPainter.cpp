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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "trax/TrackPainter.h"
#include "trax/Track.h"

#include <iostream>

namespace trax{
	using namespace common;
	using namespace spat;

TrackPainter::TrackPainter() noexcept
	:	m_Mode			{mode_default},
		m_Epsilon		{epsilon__length},
		m_SegmentLimits	{epsilon__length,plausible_maximum_length}
{
}

TrackPainter::TrackPainter( int mode, Interval<Length> segmentLimits, Length e )
	:	m_Mode			{mode},
		m_Epsilon		{e},
		m_SegmentLimits	{segmentLimits}
{
	if( m_Epsilon <= 0_m )
		throw std::invalid_argument( "The epsilon value must be > 0_m!" );

	if( !m_SegmentLimits.Normal() )
		throw std::invalid_argument( "The segment limits have to specify the lower limit as \
the Near value and the upper limit as the Far() value." );

	if( !(m_SegmentLimits > Interval<Length>{-infinite__length,0_m}) )
		throw std::invalid_argument( "The limits both must be > 0." );
}

static Length Segment_Constant( const Track& track, Length s, Length, common::Interval<Length> segmentLimits, bool ) noexcept{
	if( s + segmentLimits.Far() > track.GetLength() )
		return track.GetLength() - s;

	return segmentLimits.Far();
}

Length TrackPainter::Paint( const TrackBuilder& track, const Section& section, Length offset )
{
	if( !track.IsValid() )
		throw std::invalid_argument( "Invalid track!" );
	if( section.CountPoints() < 2 )
		throw std::invalid_argument( "Invalid section for track! Count profile points < 2." );
	if( track.GetLength() == infinite__length )
		throw std::invalid_argument( "Invalid track! Length is infinite." );

	m_CountSegmentsLastPaint = 0;

	if( GetMode() & mode_localFrame )
		m_Transformation = track.GetAbsoluteFrame() * track.GetFrame();
	else if( GetMode() & mode_startFrame )
		track.Transition( 0_m, m_Transformation );
	else
		m_Transformation.Init();

	Frame<Length,One> frame;
	track.Transition( 0_m, frame );
	if( GetMode() & (mode_localFrame | mode_startFrame) ) 
		m_Transformation.FromParent( frame );

	StartPaint( frame, offset, section );

	Length ds, s = 0_m;
	using SegmentFuncType = Length(*)(const Track&, Length, Length, common::Interval<Length>, bool);
	SegmentFuncType SegmentFunction = Segment;	
	if( m_SegmentLimits.Length() == 0_m )
		SegmentFunction = Segment_Constant;
	else
	{
		if( GetMode() & mode_constantSegmentLength )
			SegmentFunction = Segment_Constant;
		if( GetMode() & mode_checkedSegmentLength )
			SegmentFunction = Segment_Checked;
		if( GetMode() & mode_totallycheckedSegmentLength )
			SegmentFunction = Segment_TotallyChecked;
	}

	while( (ds = SegmentFunction( track, s, m_Epsilon, m_SegmentLimits, GetMode() & mode_ignoreCuvesTorsion )) )
	{
		track.Transition( s += ds, frame );
		if( GetMode() & (mode_localFrame | mode_startFrame) )
			m_Transformation.FromParent( frame );

		PaintSegment( frame, ds, section );

		++m_CountSegmentsLastPaint;
	}

	EndPaint(frame);

	return track.GetLength() + offset;
}
//
//void TrackPainter::operator()( const StaticTrack& track ){
//	if( track.GetSection() )
//		Paint( track, *track.GetSection() );
//	else
//		std::cerr << "TrackPainter: No section for track!" << std::endl;
//}
///////////////////////////////////////
SectionPainter::SectionPainter() noexcept
	:	TrackPainter	{},
		m_NextIndex		{0},
		m_TexScaleU		{1_1Im},
		m_TexCoordU		{0}
{
}

SectionPainter::SectionPainter( int mode, common::Interval<Length> segmentLimits, Length e )
	:	TrackPainter	{mode,segmentLimits,e},
		m_NextIndex		{0},
		m_TexScaleU		{1_1Im},
		m_TexCoordU		{0}
{
}

void SectionPainter::StartPaint( const Frame<Length,One>& rFrame, Length offset, const Section& section ){
	m_TexScaleU = section.TextureExtent().Length() / section.PolygonChainLength();

	m_NextIndex	= 0;
	m_TexCoordU	= offset * m_TexScaleU;

	//	First row of profile vertices
	AddVertexRowAt( rFrame, section );
}

void SectionPainter::PaintSegment( const Frame<Length,One>& rFrame, Length segmentlength, const Section& section ){
	m_TexCoordU += segmentlength * m_TexScaleU;

	AddVertexRowAt( rFrame, section );

	for( int i = m_NextIndex - section.CountPoints(); i < m_NextIndex - 1; ++i )
	{
		if( GetMode() & mode_leftHandedFaces ){
			AddTriangle( i, i - section.CountPoints() + 1, i + 1 );
			AddTriangle( i, i - section.CountPoints(), i - section.CountPoints() + 1 );
		}
		else{
			AddTriangle( i, i + 1, i - section.CountPoints() + 1 );
			AddTriangle( i, i - section.CountPoints() + 1, i - section.CountPoints() );
		}
	}
}

void SectionPainter::AddVertexRowAt( const Frame<Length,One>& rFrame, const Section& section ){
	for( int i = 0; i < section.CountPoints(); ++i )
	{
		const Section::SectionPoint& pt = section.Get( i );

		AddVertex( rFrame, pt, { m_TexCoordU, pt.t } );

		++m_NextIndex;
	}
}
///////////////////////////////////////
}