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

#include "Gestalt_Imp.h"

#include <numeric>
#include <iostream>

namespace trax
{

void Gestalt_Imp::SetName( const char* name ) noexcept
{
	Shape_ImpBase::SetName( name );
}

const char* Gestalt_Imp::GetName() const noexcept
{
	return Shape_ImpBase::GetName();
}
/*
int Gestalt_Imp::Attach( std::unique_ptr<Geom> pGeom )
{
	Mass residue = GetMass() - std::accumulate( m_Masses.begin(), m_Masses.end(), 0_kg );
	if( residue < 0_kg )
		residue = 0_kg;

	return Attach( std::move( pGeom ), residue );
}

int Gestalt_Imp::Attach( std::vector<std::unique_ptr<Geom>>& geoms )
{
	if( geoms.size() == 0 )
		return -1;

	Mass residue = GetMass() - std::accumulate( m_Masses.begin(), m_Masses.end(), 0_kg );
	if( residue < 0_kg )
		residue = 0_kg;

	Volume totalVolume = std::accumulate( geoms.begin(), geoms.end(), 0_m3,
		[]( Volume sum, const std::unique_ptr<Geom>& pGeom ){
			if( pGeom == nullptr )
				return sum;
			return sum + pGeom->GetVolume();
	} );

	int retval = -1;

	for( auto iter = geoms.begin(); iter != geoms.end(); ++iter )
	{
		if( *iter == nullptr )
			continue;

		Mass mass = residue * (*iter)->GetVolume() / totalVolume;
		if( int idx = Shape_ImpBase::Attach( std::move( *iter ) ); idx >= 0 )
		{
			if( retval < 0 )
				retval = idx;
			m_Masses.push_back( mass );
		}
	}

	if( retval >= 0 )
		DoCalculateMassProperties();

	return retval;
}

std::unique_ptr<Geom> Gestalt_Imp::Remove( int idx )
{
	if( std::unique_ptr<Geom> pRetval = Shape_ImpBase::Remove(idx); pRetval )
	{
		m_Masses.erase( m_Masses.begin() + idx );
		DoCalculateMassProperties();
		return pRetval;
	}

	return nullptr;
}
*/

int Gestalt_Imp::Attach( std::unique_ptr<Geom> pGeom, Mass mass )
{
	if( mass < 0_kg )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): mass must not be negative." );

	spat::Frame<Length,One> frame;
	pGeom->GetFrame( frame );
	const spat::SquareMatrix<MomentOfInertia,3> inertialTensor = mass * pGeom->SpecificInertiaTensor();

	int idx = Attach( std::move( pGeom ) );
	if( idx >= 0 )
		AddMassProperties( mass, frame, inertialTensor );

	return idx;
}

int Gestalt_Imp::Attach(
	std::unique_ptr<Geom> pGeom, 
	Mass mass, 
	const spat::Frame<Length,One>& massLocalPose, 
	const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor )
{
	if( mass < 0_kg )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): mass must not be negative." );
	if( !massLocalPose.IsOrthoNormal() )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): massLocalPose must be orthonormal." );
	if( inertiaTensor(0,0) < 0_kgm2 ||
		inertiaTensor(1,1) < 0_kgm2 ||
		inertiaTensor(2,2) < 0_kgm2 )
		throw std::invalid_argument("Gestalt_Imp::Attach(): diagonal elements of inertial tensor must not be negative.");
	if( !inertiaTensor.IsSymmetric() )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): inertial tensor must be symmetric." );

	int idx = Attach( std::move( pGeom ) );
	if( idx >= 0 )
		AddMassProperties( mass, massLocalPose, inertiaTensor );

	return idx;
}

int Gestalt_Imp::Attach( std::vector<std::pair<std::unique_ptr<Geom>,Mass>>& geoms ) noexcept
{
	int idx = Count();

	Mass mass = Mass();
	spat::Frame<Length, One> massLocalPose = CenterOfMassLocalPose();
	spat::Vector<MomentOfInertia> principalMoments = PrincipalMomentsOfInertia();
	std::vector<Mass> addedMasses;

	while( geoms.size() )
	{
		try{
			Attach( std::move( geoms.front().first ), geoms.front().second );
			addedMasses.push_back( geoms.front().second );
			geoms.erase( geoms.begin() );
		}
		catch( const std::exception& e ){	
			std::cerr << "Can not attach a geom to gestalt: " << (GetName() ? GetName() : "unknown") << ". Exception: " << e.what() << std::endl;

			// rollback:
			while( Count() > idx + 1 ){
				geoms.insert( geoms.begin(), std::make_pair( Remove( Count() - 1 ), addedMasses.back() ) );
				addedMasses.pop_back();
			}

			// restore mass properties:
			SetMass( mass );
			CenterOfMassLocalPose( massLocalPose );
			PrincipalMomentsOfInertia( principalMoments );
		}
	}

	return idx < Count() ? idx : - 1;
}

int Gestalt_Imp::Attach(
	std::vector<std::unique_ptr<Geom>>& geoms, 
	Mass mass, 
	const spat::Frame<Length,One>& massLocalPose, 
	const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor )
{
	if( mass < 0_kg )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): mass must not be negative." );
	if( !massLocalPose.IsOrthoNormal() )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): massLocalPose must be orthonormal." );
	if( inertiaTensor(0,0) < 0_kgm2 ||
		inertiaTensor(1,1) < 0_kgm2 ||
		inertiaTensor(2,2) < 0_kgm2 )
		throw std::invalid_argument("Gestalt_Imp::Attach(): diagonal elements of inertial tensor must not be negative.");
	if( !inertiaTensor.IsSymmetric() )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): inertial tensor must be symmetric." );
	
	int idx = Attach( geoms );
	if( idx >= 0 )
		AddMassProperties( mass, massLocalPose, inertiaTensor );

	return idx;
}

}
