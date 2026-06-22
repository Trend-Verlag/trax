//	trax track library
//	AD 2025 
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

#include "../Cargo.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	typedef ObjectID_Imp<Cargo> CargoBase;

	class Cargo_Imp : public CargoBase
	{
	public:
		Cargo_Imp( CargoType type ) noexcept;

		const char*	TypeName() const noexcept override;

		CargoType GetType() const override;

		void SetShape( std::shared_ptr<Shape> pShape ) noexcept override;

		std::shared_ptr<Shape> GetShape() const noexcept override;

		void UserData( CargoUserData* pData ) noexcept override;

		CargoUserData* UserData() const noexcept override;
	private:
		CargoType m_Type;
		std::shared_ptr<Shape> m_Shape;
		CargoUserData* m_UserData;
	};


	class MoveableCargo_Imp : public MoveableCargo,
							  public Cargo_Imp{
	public:
		MoveableCargo_Imp( CargoType type ) noexcept;

		const char*	TypeName() const noexcept override;

		void SetGestalt( std::shared_ptr<Gestalt> pGestalt ) noexcept override;

		std::shared_ptr<Gestalt> GetGestalt() const noexcept override;

	private:
		std::shared_ptr<Gestalt> m_Gestalt;

	};

}
