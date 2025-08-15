// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2015
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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