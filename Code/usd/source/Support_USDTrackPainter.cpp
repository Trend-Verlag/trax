// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "usd/Support_USDTrackPainter.h"

namespace trax{

BufferedUSDTrackPainter::BufferedUSDTrackPainter(	
	pxr::VtArray<pxr::GfVec3f>& points, 
	pxr::VtArray<int>& indices,
	pxr::VtArray<int>& faceVertexCounts,
	Real usdStageMetersPerUnit,
	int mode, 
	common::Interval<Length> segmentLimits, 
	Length e )
	: SectionPainter			{mode,segmentLimits,e}
	, m_Points					{points}
	, m_Indices					{indices}
	, m_IndexOffset				{common::narrow_cast<int>(m_Points.size())}
	, m_FaceVertexCounts		{faceVertexCounts}
	, m_UsdStageMetersPerUnit	{usdStageMetersPerUnit}
{
}

void BufferedUSDTrackPainter::StartPaint( const spat::Frame<Length,One>& rFrame, Length offset, const trax::Section& section ){
	m_IndexOffset = common::narrow_cast<int>(m_Points.size());
	SectionPainter::StartPaint( rFrame, offset, section );
}

void BufferedUSDTrackPainter::AddVertex( const spat::Frame<Length,One>& rFrame, const trax::Section::SectionPoint& pt, const spat::Position2D<One>& /*textcoord*/ ){
	spat::Position<Length> V = rFrame.P + pt.p.x * rFrame.N + pt.p.y * rFrame.B;
	m_Points.push_back( From<pxr::GfVec3f>(V,m_UsdStageMetersPerUnit) );
}
	
void BufferedUSDTrackPainter::AddTriangle( unsigned int i0, unsigned int i1, unsigned int i2 ){
	m_Indices.push_back( m_IndexOffset + i0 );
	m_Indices.push_back( m_IndexOffset + i1 );
	m_Indices.push_back( m_IndexOffset + i2 );
	m_FaceVertexCounts.push_back( 3 );
}

}