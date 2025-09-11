//	trax track library
//	AD 2014 
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

#include "Geom_Imp.h"
#include "../GeomType.h"

#include "common/NarrowCast.h"

namespace trax{
	using namespace common;
///////////////////////////////////////
Geom_Imp::Geom_Imp() noexcept
	:	m_Material			{ Material::Type::wood },
		m_TypeFilter		{ none },
		m_CollisionFilter	{ CollisionFilterFor(none) }
{}

void Geom_Imp::SetName( const char* name ) noexcept{
	m_Name = name;
}

const char* Geom_Imp::GetName() const noexcept{
	return m_Name.c_str();
}

void Geom_Imp::SetMaterial( const Material& material ) noexcept{
	m_Material = material;
}

const Material& Geom_Imp::GetMaterial() const noexcept{
	return m_Material;
}

void Geom_Imp::TypeFilter( Filters typeFlags ) noexcept{
	m_TypeFilter = typeFlags;
}

Geom::Filters Geom_Imp::TypeFilter() const noexcept{
	return m_TypeFilter;
}

void Geom_Imp::CollisionFilter( unsigned int collideWith ) noexcept{
	m_CollisionFilter = collideWith;
}

unsigned int Geom_Imp::CollisionFilter() const noexcept{
	return m_CollisionFilter;
}
///////////////////////////////////////
std::string ToString( GeomType type ){
	switch( type ){
	case GeomType::unknown:
		return "unknown";
	case GeomType::none:
		return "none";
	case GeomType::box:
		return "box";
	case GeomType::cylinder:
		return "cylinder";
	case GeomType::capsule:
		return "capsule";
	case GeomType::sphere:
		return "sphere";
	case GeomType::heightField:
		return "heightField";
	case GeomType::convexMesh:
		return "convexMesh";
	case GeomType::triangleMesh:
		return "triangleMesh";
	default:
		assert( !"unknown geom type!" );
		return "unknown";
	};
}

GeomType ToGeomType( const std::string& name ){
	if( name == "box" )
		return GeomType::box;
	else if( name == "capsule" )
		return GeomType::capsule;
	else if( name == "convexMesh" )
		return GeomType::convexMesh;
	else if( name == "triangleMesh" )
		return GeomType::triangleMesh;
	else if( name == "cylinder" )
		return GeomType::cylinder;
	else if( name == "sphere" )
		return GeomType::sphere;
	else if( name == "heightField" )
		return GeomType::heightField;
	else if( name == "none" )
		return GeomType::none;
	else
		return GeomType::unknown;
}

std::string ToString( Geom::Filters filter ){
	switch( filter ){
	case Geom::none:
		return "none";
	case Geom::fBogie:
		return "bogie";
	case Geom::fChildBogie:
		return "childBogie";
	case Geom::fWheelset:
		return "wheelset";
	case Geom::fCargo:
		return "cargo";
	case Geom::fTerrain:
		return "terrain";
	case Geom::fBogieSpace:
		return "bogieSpace";
	case Geom::fCrane:
		return "crane";
	case Geom::fConstruction:
		return "construction";
	case Geom::fHook:
		return "hook";
	case Geom::fHookFixed:
		return "hookFixed";
	case Geom::fCouplingNorth:
		return "couplingNorth";
	case Geom::fCouplingSouth:
		return "couplingSouth";
	case Geom::fTriggerShape:
		return "triggerShape";
	case Geom::fCabin:
		return "cabin";
	case Geom::fBogieNoCabin:
		return "bogieNoCabin";
	case Geom::fBuffer:
		return "buffer";
	case Geom::fTrack:
		return "track";
	case Geom::fMovableTrack:
		return "movableTrack";
	default:
		return "unknown";
	}
}

Geom::Filters GeomFilter( const std::string& filter ){
	if( filter == "bogie" )
		return Geom::fBogie;
	else if( filter == "childBogie" )
		return Geom::fChildBogie;
	else if( filter == "wheelset" )
		return Geom::fWheelset;
	else if( filter == "cargo" )
		return Geom::fCargo;
	else if( filter == "terrain" )
		return Geom::fTerrain;
	else if( filter == "bogieSpace" )
		return Geom::fBogieSpace;
	else if( filter == "crane" )
		return Geom::fCrane;
	else if( filter == "construction" )
		return Geom::fConstruction;
	else if( filter == "hook" )
		return Geom::fHook;
	else if( filter == "hookFixed" )
		return Geom::fHookFixed;
	else if( filter == "couplingNorth" )
		return Geom::fCouplingNorth;
	else if( filter == "couplingSouth" )
		return Geom::fCouplingSouth;
	else if( filter == "triggerShape" )
		return Geom::fTriggerShape;
	else if( filter == "cabin" )
		return Geom::fCabin;
	else if( filter == "bogieNoCabin" )
		return Geom::fBogieNoCabin;
	else if( filter == "buffer" )
		return Geom::fBuffer;
	else if( filter == "track" )
		return Geom::fTrack;
	else if( filter == "movableTrack" )
		return Geom::fMovableTrack;
	else 
		return Geom::none;
}

unsigned int CollisionFilterFor( Geom::Filters filter ) noexcept{
	switch( filter ){
	case Geom::fBogie:
	case Geom::fCabin:
	case Geom::fBogieNoCabin:
		return Geom::fAll ^ (Geom::fChildBogie | Geom::fCouplingNorth | Geom::fCouplingSouth );
	case Geom::fChildBogie:
		return Geom::fAll ^ (Geom::fBogie | Geom::fCouplingNorth | Geom::fCouplingSouth);
	case Geom::fWheelset:
	case Geom::fCargo:
	case Geom::fBogieSpace:
		return Geom::fAll ^ (Geom::fCouplingNorth | Geom::fCouplingSouth);
	case Geom::fTerrain:
		return Geom::fAll ^ (Geom::fCouplingNorth | Geom::fCouplingSouth | Geom::fTerrain);
	case Geom::fCrane:
		return Geom::fAll ^ (Geom::fCouplingNorth | Geom::fCouplingSouth | Geom::fHook | Geom::fHookFixed);
	case Geom::fConstruction:
		return Geom::fAll ^ (Geom::fCouplingNorth | Geom::fCouplingSouth | Geom::fConstruction);
	case Geom::fHook:
		return Geom::fAll ^ (Geom::fCouplingNorth | Geom::fCouplingSouth | Geom::fCrane | Geom::fHook | Geom::fHookFixed);
	case Geom::fHookFixed:
		return Geom::fAll ^ (Geom::fCouplingNorth | Geom::fCouplingSouth | Geom::fCrane | Geom::fHook | Geom::fHookFixed);
	case Geom::fCouplingNorth:
	case Geom::fCouplingSouth:
		return Geom::fCouplingNorth | Geom::fCouplingSouth;
	default:
		return Geom::none;
	}
}
///////////////////////////////////////
} // namespace trax
