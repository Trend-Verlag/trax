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

#pragma once


#include "../Geom.h"
#include "../Material.h"

namespace trax{

	class Body_Imp;
	class Shape_ImpBase;

	class Geom_Imp : public virtual Geom{
	public:
		Geom_Imp() noexcept;

		// Geom:
		
		void SetName( const char* name ) noexcept override;

		const char* GetName() const noexcept override;

		void SetMaterial( const Material& material ) noexcept override;

		const Material& GetMaterial() const noexcept override;

		void TypeFilter( Filters typeFlags ) noexcept override;

		Filters TypeFilter() const noexcept override;

		void CollisionFilter( unsigned int collideWith ) noexcept override;

		unsigned int CollisionFilter() const noexcept override;


		virtual void OnAttach( const Shape_ImpBase& ){};

		virtual void OnDetach(){};
	private:
		std::string		m_Name;
		Material		m_Material;
		Filters			m_TypeFilter;
		unsigned int	m_CollisionFilter;
	};
}