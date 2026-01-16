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

		using Shape::Attach;


		// Gestalt:
		int Attach( std::unique_ptr<Geom> pGeom, Mass mass ) override;

		int Attach( std::unique_ptr<Geom> pGeom, Mass mass, const spat::Frame<Length,One>& massLocalPose, const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor ) override;
		
		int Attach( std::vector<std::pair<std::unique_ptr<Geom>,Mass>>& geoms ) noexcept override;

		int Attach( std::vector<std::unique_ptr<Geom>>& geoms, Mass mass, const spat::Frame<Length,One>& massLocalPose, const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor ) override;

	protected:
		virtual void AddMassProperties( Mass mass, const spat::Frame<Length,One>& massLocalPose, const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor ) noexcept = 0;
	
	private:
	};
}
