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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "Gestalt_Imp.h"

namespace trax
{

int Gestalt_Imp::Attach( std::unique_ptr<Geom> pGeom )
{
	return Attach( std::move( pGeom ), 0_kg );
}

int Gestalt_Imp::Attach( std::unique_ptr<Geom> pGeom, Mass mass )
{
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
