//	trax track library
//	AD 2015 
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

#include "../Material.h"

namespace trax{

Material::Type operator++( Material::Type& material ) noexcept{
	material = static_cast<Material::Type>(static_cast<int>(material) + 1);
	return material;
}

const char* ToString( Material::Type type ){
	switch( type ){
	case Material::Type::none:
		return "none";
	case Material::Type::steel:
		return "steel";
	case Material::Type::tin:
		return "tin";
	case Material::Type::aluminium:
		return "aluminium";
	case Material::Type::copper:
		return "copper";
	case Material::Type::stone:
		return "stone";
	case Material::Type::coal:
		return "coal";
	case Material::Type::soil:
		return "soil";
	case Material::Type::wood:
		return "wood";
	case Material::Type::asphalt:
		return "asphalt";
	case Material::Type::rubber:
		return "rubber";
	case Material::Type::filt:
		return "filt";
	case Material::Type::glass:
		return "glass";
	case Material::Type::paper:
		return "paper";
	case Material::Type::concrete:
		return "concrete";
	case Material::Type::water:
		return "water";
	case Material::Type::plastic:
		return "plastic";
	case Material::Type::cloth:
		return "";
	case Material::Type::chalk:
		return "chalk";
	case Material::Type::diamond:
		return "diamond";
	case Material::Type::plasma:
		return "plasma";
	default:
		return "unknown";
	}
}

Material::Type MaterialType( const char* string ){
	if( strcmp(string, "steel") == 0 )
		return Material::Type::steel;
	else if( strcmp(string, "tin") == 0 )
		return Material::Type::tin;
	else if( strcmp(string, "aluminium") == 0 )
		return Material::Type::aluminium;
	else if( strcmp(string, "copper") == 0 )
		return Material::Type::copper;
	else if( strcmp(string, "stone") == 0 )
		return Material::Type::stone;
	else if( strcmp(string, "coal") == 0 )
		return Material::Type::coal;
	else if( strcmp(string, "soil") == 0 )
		return Material::Type::soil;
	else if( strcmp(string, "wood") == 0 )
		return Material::Type::wood;
	else if( strcmp(string, "asphalt") == 0 )
		return Material::Type::asphalt;
	else if( strcmp(string, "rubber") == 0 )
		return Material::Type::rubber;
	else if( strcmp(string, "filt") == 0 )
		return Material::Type::filt;
	else if( strcmp(string, "glass") == 0 )
		return Material::Type::glass;
	else if( strcmp(string, "paper") == 0 )
		return Material::Type::paper;
	else if( strcmp(string, "concrete") == 0 )
		return Material::Type::concrete;
	else if( strcmp(string, "water") == 0 )
		return Material::Type::water;
	else if( strcmp(string, "plastic") == 0 )
		return Material::Type::plastic;
	else if( strcmp(string, "cloth") == 0 )
		return Material::Type::cloth;
	else if( strcmp(string, "chalk") == 0 )
		return Material::Type::chalk;
	else if( strcmp(string, "diamond") == 0 )
		return Material::Type::diamond;
	else if( strcmp(string, "plasma") == 0 )
		return Material::Type::plasma;
	else
		return Material::Type::none;
}

}