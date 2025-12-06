//	trax track library
//	AD 2023 
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


#include "Shape_Imp.h"

#include "common/NarrowCast.h"
#include <iostream>

namespace trax{
///////////////////////////////////////
Shape_ImpBase::Shape_ImpBase() noexcept
{
}

void Shape_ImpBase::SetName( const char* name ) noexcept
{
	if( name )
		m_Name = name;
	else
		m_Name.clear();
}

const char* Shape_ImpBase::GetName() const noexcept
{
	return m_Name.c_str();
}

spat::Box<Length> Shape_ImpBase::GetExtent() const noexcept
{
	spat::Box<Length> extent{ -infinite__length, -infinite__length, -infinite__length };
	for( const auto& pGeom : m_Geoms )
	{
		extent.Union( pGeom->GetExtent() );
	}

	return extent;
}

int Shape_ImpBase::Attach( std::unique_ptr<Geom> pGeom ){
	if( pGeom )
	// due to the fact that we take an unique_pointer, that geom can not be already attached.
	{
		if( Geom_Imp* pGeom_Imp = dynamic_cast<Geom_Imp*>(pGeom.get()); pGeom_Imp )
		{
			pGeom_Imp->OnAttach( *this );
			m_Geoms.push_back( std::move(pGeom) );
			return common::narrow_cast<int>(m_Geoms.size()) - 1;
		}
		else{
			throw std::invalid_argument( "Unknown Geom object type!" );
		}
	}

	return -1;
}

int Shape_ImpBase::Attach( std::vector<std::unique_ptr<Geom>>& geoms ){
	for( auto& pGeom : geoms )
	{
		try{
			Attach( std::move( pGeom ) );
		}
		catch( const std::exception& e ){
			std::cerr << "Can not attach a geom to shape: " << (GetName() ? GetName() : "unknown") << ". Exception: " << e.what() << std::endl;
		}
	}

	geoms.clear();
	return common::narrow_cast<int>(m_Geoms.size()) - 1;
}

std::unique_ptr<Geom> Shape_ImpBase::Remove( int idx ){
	if( idx >= 0 && static_cast<std::size_t>(idx) < m_Geoms.size() ){
		auto it = m_Geoms.begin() + idx;
		std::unique_ptr<Geom> p = std::move( *it );
		m_Geoms.erase( it );
		dynamic_cast<Geom_Imp*>(p.get())->OnDetach();
		return p;
	}

	return nullptr;
}

void Shape_ImpBase::Clear() noexcept{
	while( !m_Geoms.empty() )
		Remove( Count() -1 );
}

int Shape_ImpBase::Count() const{
	return common::narrow_cast<int>(m_Geoms.size());
}

Geom& Shape_ImpBase::Get( int idx ) const{
	return *m_Geoms.at(idx);
}
///////////////////////////////////////
} // namespace trax