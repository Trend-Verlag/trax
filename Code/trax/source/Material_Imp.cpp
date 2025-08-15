//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2015 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel




#include "trax/Material.h"

namespace trax{

Material::Type operator++( Material::Type& material ) noexcept{
	material = static_cast<Material::Type>(static_cast<int>(material) + 1);
	return material;
}

std::string ToString( Material::Type type ){
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

Material::Type MaterialType( const std::string& string ){
	if( string == "steel" )
		return Material::Type::steel;
	else if( string == "tin" )
		return Material::Type::tin;
	else if( string == "aluminium" )
		return Material::Type::aluminium;
	else if( string == "copper" )
		return Material::Type::copper;
	else if( string == "stone" )
		return Material::Type::stone;
	else if( string == "coal" )
		return Material::Type::coal;
	else if( string == "soil" )
		return Material::Type::soil;
	else if( string == "wood" )
		return Material::Type::wood;
	else if( string == "asphalt" )
		return Material::Type::asphalt;
	else if( string == "rubber" )
		return Material::Type::rubber;
	else if( string == "filt" )
		return Material::Type::filt;
	else if( string == "glass" )
		return Material::Type::glass;
	else if( string == "paper" )
		return Material::Type::paper;
	else if( string == "concrete" )
		return Material::Type::concrete;
	else if( string == "water" )
		return Material::Type::water;
	else if( string == "plastic" )
		return Material::Type::plastic;
	else if( string == "cloth" )
		return Material::Type::cloth;
	else if( string == "chalk" )
		return Material::Type::chalk;
	else if( string == "diamond" )
		return Material::Type::diamond;
	else if( string == "plasma" )
		return Material::Type::plasma;
	else
		return Material::Type::none;
}

}