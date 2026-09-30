//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "trax/Configuration.h"

#include "dim/DimensionedValues.h"
#include "spat/Rect.h"
#include "spat/Position2D.h"
#include "spat/Vector2D.h"

namespace trax{

	using namespace dim;

	/// \brief A HeightField is a 2D surface in 3D space that can be used for collision detection.
	///
	struct HeightField{

		/// \name Construction
		/// 
		/// \brief Creates a heightfield from data
		/// The quads are tesselated as [i],[i+nCols+1],[i+1] and [i],[i+nCols],[i+nCols+1].
		/// If all three adjacent samples of a triangle have the corresponding hole information flag set to true,
		/// the triangle would not be created as a collision shape.
		/// \param pSamples Array with the elevation data of the sample points. Length must be at least nRows*nCols.
		/// \param pbHoles Array with the hole information. Length must be at least nRows*nCols.
		/// \param rasterExtent Number of columns (dx) and rows (dy) in the heightfield.
		/// \param rasterScale Horizontal scale factor. 1 * rasterScale * meters_per_unit will be the grid distance in meters.
		/// \param heightScale Vertical scale factor. pSamples[x] * heightScale * meters_per_unit will be the height in meters.
		/// @{
		
		virtual bool Create( const short* pSamples, const bool* pbHoles, const spat::Vector2D<int>& rasterExtent, const spat::Vector2D<Length>& rasterScale, Length heightScale ) = 0;
		
		virtual bool Create( const Length* pSamples, const bool* pbHoles, const spat::Vector2D<int>& rasterExtent, const spat::Vector2D<Length>& rasterScale, Length heightScale ) = 0;

		virtual bool Create( const spat::Vector2D<int>& rasterExtent, const spat::Vector2D<Length>& rasterScale, Length heightScale ) = 0;
		/// @}


		virtual spat::Vector2D<int> GetRasterExtent() const noexcept = 0;

		virtual spat::Vector2D<Length> GetRasterScale() const noexcept = 0;


		virtual Length GetHeight( const spat::Position2D<int>& atRasterPoint ) const = 0;

		virtual Length GetHeight( const spat::Position2D<Length>& atPoint ) const = 0;

		virtual common::Interval<Length> HeightRange() const noexcept = 0;

		virtual void SetHeight( const spat::Position2D<int>& atRasterPoint, Length height ) = 0;

		virtual void SetHeight( const spat::Rect<int>& atRasterArea, Length height ) = 0;


		virtual bool IsHole( const spat::Position2D<int>& atRasterPoint ) const = 0;

		virtual void SetHole( const spat::Position2D<int>& atRasterPoint, bool hole ) = 0;

	};

//	inline spat::Position2D<Length> RasterSpaceToHeightFieldSpace( const HeightField& forHeightField, const spat::Position2D<int> rasterPoint );
//	inline spat::Position2D<int> HeightFieldSpaceToRasterSpace( const HeightField& forHeightField, const spat::Position2D<Length> heightFieldPoint );
//	inline spat::Rect<Length> RasterSpaceToHeightFieldSpace( const HeightField& forHeightField, const spat::Rect<int> rasterPoint );
//	inline spat::Rect<int> HeightFieldSpaceToRasterSpace( const HeightField& forHeightField, const spat::Rect<Length> heightFieldPoint );
//
//
/////////////////////////////////////////
//inline spat::Position2D<Length> RasterSpaceToHeightFieldSpace( const HeightField& forHeightField, const spat::Position2D<int> rasterPoint ){
//	return forHeightField.GetRasterScale().dx * spat::Position2D<One>{ rasterPoint };
//}
//
//inline spat::Position2D<int> HeightFieldSpaceToRasterSpace( const HeightField& forHeightField, const spat::Position2D<Length> heightFieldPoint ){
//	return spat::Position2D<int>{ heightFieldPoint / forHeightField.GetRasterScale() };
//}
//
//inline spat::Rect<Length> RasterSpaceToHeightFieldSpace( const HeightField& forHeightField, const spat::Rect<int> rasterPoint ){
//	return spat::Rect<Length>{
//		forHeightField.GetRasterScale() * spat::Position2D<One>{ rasterPoint.LeftTop() },
//		forHeightField.GetRasterScale() * spat::Position2D<One>{ rasterPoint.RightBottom() }
//	};
//}
//
//inline spat::Rect<int> HeightFieldSpaceToRasterSpace( const HeightField& forHeightField, const spat::Rect<Length> heightFieldPoint ){
//	return spat::Rect<int>{
//		spat::Position2D<int>{ heightFieldPoint.LeftTop() / forHeightField.GetRasterScale() },
//		spat::Position2D<int>{ heightFieldPoint.RightBottom() / forHeightField.GetRasterScale() }
//	}; 
//}
///////////////////////////////////////
}
