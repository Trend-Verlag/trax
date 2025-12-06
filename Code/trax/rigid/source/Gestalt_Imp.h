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

#pragma once

#include "../Gestalt.h"
#include "Body_Imp.h"
#include "Shape_Imp.h"

#ifdef _WIN32
#	pragma warning( disable : 4250 ) // inherits via dominance warning
#endif


namespace trax
{
	class Gestalt_Imp :	public Gestalt,
						public virtual Body_Imp,
						public virtual Shape_ImpBase
	{
	public:
		
		// Shape:
		void SetName( const char* name ) noexcept override;

		const char* GetName() const noexcept override;

		int Attach( std::unique_ptr<Geom> pGeom ) override;

		int Attach( std::vector<std::unique_ptr<Geom>>& geoms ) override;

		using Shape::Attach;

		std::unique_ptr<Geom> Remove( int idx ) override;


		// Gestalt:
		int Attach( std::unique_ptr<Geom> pGeom, Mass mass ) override;

		Mass GeomMass( int idx = 0 ) const override;


	protected:

		virtual void DoCalculateMassProperties() = 0;
	private:
		std::vector<Mass> m_Masses;
	};
}
