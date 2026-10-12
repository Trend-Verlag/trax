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

#include "trax/rigid/HeightField.h"

#include "Geom_Imp.h"

namespace trax{

	class HeightField_Imp : public HeightField
						  , public Geom_Imp
	{
	public:
		HeightField_Imp();

		// Geom:
		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		void SetFrame( const spat::Frame<Length,One>& frame ) noexcept override;

		void GetFrame( spat::Frame<Length,One>& frame ) const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		spat::SquareMatrix<Area,3> SpecificInertiaTensor() const override;

		Volume GetVolume() const noexcept override;

		GeomType GetGeomType() const noexcept override;

		bool IsOverlapping( const Geom& other ) const noexcept override;


		// HeightField:
		bool Create( common::Span<const short> samples, common::Span<const bool> holes, const spat::Vector2D<int>& rasterExtent, Length rasterScale, Length heightScale ) override;
		
		bool Create( common::Span<const Length> samples, common::Span<const bool> holes, const spat::Vector2D<int>& rasterExtent, Length rasterScale ) override;

		bool Create( const spat::Vector2D<int>& rasterExtent, const Length rasterScale ) override;

		spat::Vector2D<int> GetRasterExtent() const noexcept override;

		Length GetRasterScale() const noexcept override;

		Length GetHeight( const spat::Position2D<int>& atRasterPoint ) const override;

		Length GetHeight( const spat::Position<Length>& atShapeLocalPoint ) const noexcept override;

		common::Interval<Length> HeightRange() const noexcept override;

		common::Interval<Length> HeightRange( const spat::Rect<int>& inRect ) const override;

		void SetHeight( const spat::Position2D<int>& atRasterPoint, Length height ) override;

		void SetHeight( const spat::Rect<int>& atRasterArea, Length height ) override;

		bool IsHole( const spat::Position2D<int>& atRasterPoint ) const override;

		void SetHole( const spat::Position2D<int>& atRasterPoint, bool hole ) override;

	private:
		spat::Frame<Length,One> m_Frame;

		struct Data{
			Data();

			std::vector<Length> m_Samples;
			std::vector<bool> m_Holes;
			spat::Rect<int> m_RasterExtent;
			Length m_RasterScale;

			spat::Box<Length> GetExtent() const noexcept;
			void Create( const spat::Vector2D<int>& rasterExtent, const Length rasterScale );
			inline Length GetHeight( const spat::Position2D<int>& atRasterPoint ) const noexcept{
				return m_Samples[ atRasterPoint.y * m_RasterExtent.Width() + atRasterPoint.x ];
			}
			Length GetHeight(const spat::Position<Length>& atLocalPoint ) const noexcept;
			common::Interval<Length> HeightRange( const spat::Rect<int>& inRect ) const;
		};

		std::shared_ptr<Data> m_pData;

		HeightField_Imp( std::shared_ptr<Data> pData );
	};

}