//	trax track library
//	AD 2023 
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

#include "Body_Imp.h"

namespace trax
{
	using namespace spat;

///////////////////////////////////////
void Body_Imp::SetMass( Mass mass ){
	if( mass <= 0_kg )
		throw std::invalid_argument( "Mass has to have positive value" );
}
//
//Shape& Body_Imp::GetGeomList() noexcept{
//	return *this;
//}
///////////////////////////////////////
SquareMatrix<Area,3> SpecificInertiaTensorFor( const Box<Length>& box ){
	SquareMatrix<Area,3> iTensor;
	iTensor.SetIdentity();

	iTensor(0,0) = (pow<2>(box.EdgeY()) + pow<2>(box.EdgeZ())) / 12;
	iTensor(1,1) = (pow<2>(box.EdgeX()) + pow<2>(box.EdgeZ())) / 12;
	iTensor(2,2) = (pow<2>(box.EdgeY()) + pow<2>(box.EdgeX())) / 12;

	return iTensor;
}

SquareMatrix<Area,3> SpecificInertiaTensorFor( const Sphere<Length>& sphere ){
	SquareMatrix<Area,3> iTensor;
	iTensor.SetIdentity();

	iTensor(0,0) = iTensor(1,1) = iTensor(2,2) = 0.4f * pow<2>(sphere.r);

	return iTensor;
}

SquareMatrix<Area,3> SpecificInertiaTensorForCylinder( Length radius, Length length ){
	SquareMatrix<Area,3> iTensor;
	iTensor.SetIdentity();

	iTensor(0,0) = iTensor(1,1) = 0.25f*pow<2>(radius) + pow<2>(length)/12;
	iTensor(2,2) = 0.5f*pow<2>(radius);

	return iTensor;
}
///////////////////////////////////////
}