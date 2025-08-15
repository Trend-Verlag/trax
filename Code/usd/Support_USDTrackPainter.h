// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "trax/TrackPainter.h"

#include "SupportUSD_Spat.h"

#if defined(_MSC_VER)
#	pragma warning( push )
#	pragma warning( disable: 4275 ) // non dll-interface class used as base for dll-interface class
#endif
#include <pxr/base/vt/array.h>
#if defined(_MSC_VER)
#	pragma warning( pop )
#endif

namespace trax{

class BufferedUSDTrackPainter : public trax::SectionPainter{
public:
	BufferedUSDTrackPainter(	pxr::VtArray<pxr::GfVec3f>& points, 
								pxr::VtArray<int>& indices,
								pxr::VtArray<int>& faceVertexCounts,
								Real usdStageMetersPerUnit,
								int mode = TrackPainter::Mode::mode_default, 
								common::Interval<Length> segmentLimits = {trax::epsilon__length,trax::plausible_maximum_length}, 
								Length e = trax::epsilon__length );

protected:
	void StartPaint( const spat::Frame<Length,One>& rFrame, Length offset, const trax::Section& section ) override;
	void AddVertex( const spat::Frame<Length,One>& rFrame, const trax::Section::SectionPoint& pt, const spat::Position2D<One>& /*textcoord*/ ) override;
	void AddTriangle( unsigned int i0, unsigned int i1, unsigned int i2 ) override;

	pxr::VtArray<pxr::GfVec3f>& m_Points;
private:
	pxr::VtArray<int>&	m_Indices;
	int					m_IndexOffset;
	pxr::VtArray<int>&  m_FaceVertexCounts;
	Real				m_UsdStageMetersPerUnit;
};

}
