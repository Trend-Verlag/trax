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

#include "Cargo_Imp.h"

namespace trax{
///////////////////////////////////////
std::unique_ptr<Cargo> Cargo::Make( CargoType type ) noexcept{
	return std::make_unique<Cargo_Imp>( type );
}

Cargo_Imp::Cargo_Imp( CargoType type ) noexcept
	: m_Type( type )
{}

const char* Cargo_Imp::TypeName() const noexcept{
	return "Cargo";
}

CargoType Cargo_Imp::GetType() const{
	return m_Type;
}

void Cargo_Imp::SetShape( std::shared_ptr<Shape> pShape ) noexcept{
	m_Shape = pShape;
}

std::shared_ptr<Shape> Cargo_Imp::GetShape() const noexcept{
	return m_Shape;
}

void Cargo_Imp::UserData( CargoUserData* pData ) noexcept{
	m_UserData = pData;
}

CargoUserData* Cargo_Imp::UserData() const noexcept{
	return m_UserData;
}
///////////////////////////////////////
std::unique_ptr<MoveableCargo> MoveableCargo::Make( CargoType type ) noexcept{
	return std::make_unique<MoveableCargo_Imp>( type );
}

MoveableCargo_Imp::MoveableCargo_Imp( CargoType type ) noexcept
	: Cargo_Imp{ type }
{}

const char* MoveableCargo_Imp::TypeName() const noexcept{
	return "MoveableCargo";
}

void MoveableCargo_Imp::SetGestalt( std::shared_ptr<Gestalt> pGestalt ) noexcept{
	m_Gestalt = pGestalt;
}

std::shared_ptr<Gestalt> MoveableCargo_Imp::GetGestalt() const noexcept{
	return m_Gestalt;
}
///////////////////////////////////////
}
