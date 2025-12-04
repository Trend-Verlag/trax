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

namespace trax
{

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

int Gestalt_Imp::Attach( std::unique_ptr<Geom> pGeom, Mass mass )
{
	if( pGeom == nullptr )
		return -1;
	if( mass < 0_kg )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): mass must not be negative." );
	if( pGeom->GetVolume() <= 0_m3 )
		throw std::invalid_argument( "Gestalt_Imp::Attach(): Geom must have a volume > 0." );

	if( int idx = Shape_ImpBase::Attach( std::move( pGeom ) ); idx >= 0 )
	{
		m_Masses.push_back( mass );
		DoCalculateMassProperties();
		return idx;
	}
	else
		return idx;
}

Mass Gestalt_Imp::GeomMass( int idx ) const
{
	return m_Masses.at(idx);
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

}
