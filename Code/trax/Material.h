//	trax track library
//	AD 2015
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

#pragma once

#include "Configuration.h"
#include "Units.h"

namespace trax{

	struct Material{
		enum class Type : int{
			none = -1,
			steel = 0,
			tin,
			aluminium,
			copper,
			stone,
			coal,
			soil,
			wood,
			asphalt,
			rubber,
			filt,
			glass,
			paper,
			concrete,
			water,
			plastic,
			cloth,
			chalk,
			diamond,
			plasma,
			count_materials
		} type;

		bool bRinging;
		One staticFriction;
		One dynamicFriction;
		One restitution;

		Material() noexcept
			:	type			(Type::steel),
				bRinging		(false),
				staticFriction	(0.6f), 
				dynamicFriction	(0.4f), 
				restitution		(0.1f)
		{}

		Material( Type type ) noexcept
			:	type			(type),
				bRinging		(false),
				staticFriction	(0.6f), 
				dynamicFriction	(0.4f), 
				restitution		(0.1f)
		{}
	};

	Material::Type operator++( Material::Type& material ) noexcept;

	struct Materials{
		Materials( Material::Type a, Material::Type b ) noexcept
			: matA(a), matB(b)
		{}
		Materials( unsigned int a, unsigned int b ) noexcept
			: matA(static_cast<Material::Type>(a)), matB(static_cast<Material::Type>(b))
		{}

		void Swap() noexcept{
			std::swap( matA, matB );
		}

		Material::Type matA;
		Material::Type matB;
	};

	dclspc std::string ToString( Material::Type type );

	dclspc Material::Type MaterialType( const std::string& string );
}