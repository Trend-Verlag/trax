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


#include "trax/rigid/support/SupportTerrain.h"

#include "dim/support/DimSupportStream.h"
#include "trax/rigid/HeightField.h"
#include "stb_image.h"

#include <iostream>

namespace trax{

//static inline sure3::sureRGB floatTosureRGB( float f ){
//	int val = (int)f;
//	
//	unsigned uval = val + 10000;	//Aus negativem Bereich herausheben
//	sure3::sureRGB rgb;
//
//	rgb.r = (uval & 0x00FF0000) >> 16;
//	rgb.g = (uval & 0x0000FF00) >> 8;
//	rgb.b = (uval & 0x000000FF);
//
//	return rgb;
//}
//
//static inline float sureRGBTofloat( sure3::sureRGB rgb )
//{
//		//Farbwerte, die jeweils von 0 - 255 laufen so
//		//in eine vier Byte große Variable schieben:
//		// 0x00RRGGBB:
//
//	unsigned Rot	= rgb.r;
//	Rot <<= 16;
//	unsigned Gruen	= rgb.g;
//	Gruen <<= 8;
//	unsigned Blau	= rgb.b;
//
//	unsigned	uval	= Rot | Gruen | Blau;
//
//		//Variable als int interpretieren und 10000 abziehen:
//	int			val		= (int)uval - 10000;
//
//		//Die vier Byte stellen jetzt unseren float - Wert dar:
//	return (float)val;
//}

bool LoadFromEEPHeightFieldFile( HeightField& heightField, const std::filesystem::path& fullPath, Length resolution )
{
	spat::Vector2D<int> rasterRect{ 0, 0 };
	int channels = 0;
	stbi_uc* pData = stbi_load( fullPath.string().c_str(), &rasterRect.dx, &rasterRect.dy, &channels, 4 ); // RGBA, 8 bit each
	if( !pData )
		return false;

	const size_t count = rasterRect.dx * rasterRect.dy;
	std::vector<Length> heights( count );
	std::unique_ptr<bool[]> holes = std::make_unique<bool[]>( count );

	for( size_t idx = 0; idx < count; ++idx )
	{
		const stbi_uc* px = pData + 4 * idx;
		const unsigned int red = px[2], green = px[1], blue = px[0], alpha = px[3];

		// EEP encoding: 0x00RRGGBB - 10000 (see sureRGBTofloat)
		const int val = static_cast<int>((red << 16) | (green << 8) | blue) - 10000;
		heights[idx] = _cm(val);
		holes[idx] = ( alpha == 0 );

		if( isnan(heights[idx]) )
		{
			std::clog << Verbosity::normal << "HeightFiled sample height is nan at: " << idx << std::endl;
		}
		else if( abs(heights[idx]) > maximum__length )
		{
			std::clog << Verbosity::normal << "HeightFiled sample height extraordinaire: height==" << heights[idx] << " at: " << idx << std::endl;
		}
	}

	stbi_image_free( pData );

	return heightField.Create(
		common::MakeConstSpan( heights ),
		common::Span<const bool>{ holes.get(), count },
		rasterRect,
		resolution );
}

}