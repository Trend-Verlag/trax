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

#include "trax/rigid/Geom.h"

#include "dim/DimensionedValues.h"
#include "spat/Rect.h"
#include "spat/Position2D.h"
#include "spat/Vector2D.h"

namespace trax{

	/// \brief A HeightField is a 2D surface in 3D space that can be used for collision detection.
	///
	/// The HeightField is defined by a grid of sample points, each with a height value. The surface 
	/// is formed by connecting these points with triangles. Holes can be defined in the HeightField 
	/// to create areas where no collision occurs.
	/// The raster extent is defined by the number of columns (dx) and rows (dy) of cells in the grid; 
	/// the number of raster points is one more in each dimension. A Rect<int> in raster space is defined 
	/// by the indices of the sample points, with (0,0) being the bottom-left corner, all the edges
	/// are inclusive.
	struct HeightField : virtual Geom{

		/// \brief Makes a memory HeightField object.
		static dclspc std::unique_ptr<HeightField> Make() noexcept;


		/// \brief Gets a height value of the heightfield.
		/// \param atRasterPoint The raster point in the heightfield to get or set the height for.
		/// \throws std::out_of_range if atRasterPoint is outside the raster extent of the heightfield.
		/// \returns the height at the given point.
		inline Length operator()( const spat::Position2D<int>& rasterPoint ) const{
			return GetHeight( rasterPoint );
		}


		/// \brief Gets a height value of the heightfield.
		/// \param atParentLocalPoint The point in the heightfield's parent coordinate system to get the 
		/// projected height for.
		/// \returns the height at the given point in the heightfield's parent coordinate system.
		inline Length operator()( const spat::Position<Length>& heightFieldPoint ) const{
			return GetHeight( heightFieldPoint );
		}


		/// \name Construction
		/// 
		/// \brief Creates a heightfield from data
		/// The quads are tesselated as [i],[i+nCols+1],[i+1] and [i],[i+nCols],[i+nCols+1].
		/// If all three adjacent samples of a triangle have the corresponding hole information flag set to true,
		/// the triangle would not be created as a collision shape.
		/// \param samples Array with the elevation data of the sample points; size must be at least nRows*nCols.
		/// \param holes Array with the hole information; size must be at least nRows*nCols.
		/// \param rasterExtent Number of columns (dx) and rows (dy) of cells in the heightfield.
		/// \param rasterScale Horizontal scale factor. The width and height of each cell.
		/// \param heightScale Vertical scale factor. samples[x] * heightScale will be the height of a sample x.
		/// @{
		
		virtual bool Create( common::Span<const short> samples, common::Span<const bool> holes, const spat::Vector2D<int>& rasterExtent, Length rasterScale, Length heightScale ) = 0;
		
		virtual bool Create( common::Span<const Length> samples, common::Span<const bool> holes, const spat::Vector2D<int>& rasterExtent, Length rasterScale ) = 0;

		virtual bool Create( const spat::Vector2D<int>& rasterExtent, const Length rasterScale ) = 0;
		/// @}


		/// \brief Gets the raster extent of the heightfield.
		virtual spat::Vector2D<int> GetRasterExtent() const noexcept = 0;


		/// \brief Gets the distance between raster points of the heightfield.
		virtual Length GetRasterScale() const noexcept = 0;


		/// \brief Gets a height value of the heightfield.
		/// \param atRasterPoint The raster point in the heightfield to get or set the height for.
		/// \throws std::out_of_range if atRasterPoint is outside the raster extent of the heightfield.
		/// \returns the height at the given point.
		virtual Length GetHeight( const spat::Position2D<int>& atRasterPoint ) const = 0;


		/// \brief Gets a height value of the heightfield.
		/// \param atParentLocalPoint The point in the heightfield's parent coordinate system to get the 
		/// projected height for.
		/// \returns the height at the given point in the heightfield's parent coordinate system.
		virtual Length GetHeight( const spat::Position<Length>& atParentLocalPoint ) const noexcept = 0;


		/// \return The minimum and maximum height values in the total heightfield.
		virtual common::Interval<Length> HeightRange() const noexcept = 0;


		/// \brief Gets the range of height values in a given area of the heightfield.
		/// \throws std::out_of_range if atRasterArea is outside the raster extent of the heightfield.
		/// \return The minimum and maximum height values in the atRasterArea, edges inclusive.
		virtual common::Interval<Length> HeightRange( const spat::Rect<int>& atRasterArea ) const = 0;


		/// \brief Sets a height value of the heightfield.
		/// \param atRasterPoint The raster point in the heightfield to set the height for.
		/// \param height The height to set at the raster point.
		virtual void SetHeight( const spat::Position2D<int>& atRasterPoint, Length height ) = 0;


		/// \brief Sets the height values of the heightfield.
		/// \param atRasterArea The area in raster space to set the height for. The edges are inclusive.
		/// \param height The height to set in the raster area.
		virtual void SetHeight( const spat::Rect<int>& atRasterArea, Length height ) = 0;


		/// \brief Checks if a raster point in the heightfield is a hole.
		/// 
		/// A created triangle with all three adjacent sample points being holes will not be 
		/// created as a collision shape.
		/// \param atRasterPoint The raster point in the heightfield to check.
		/// \return True if the raster point is a hole, false otherwise.
		virtual bool IsHole( const spat::Position2D<int>& atRasterPoint ) const = 0;


		/// \brief Sets a raster point in the heightfield as a hole or not.
		/// \param atRasterPoint The raster point in the heightfield to set.
		/// \param bHole True to set the raster point as a hole, false otherwise.
		virtual void SetHole( const spat::Position2D<int>& atRasterPoint, bool bHole ) = 0;
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
