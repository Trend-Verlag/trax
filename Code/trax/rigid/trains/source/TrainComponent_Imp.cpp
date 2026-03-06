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

#include "TrainComponent_Imp.h"


namespace trax{

bool CanCouple( const TrainComponent::Coupling& coupling ) noexcept
{
	if( coupling.coupledA.first && coupling.coupledB.first )
	{
		if( coupling.coupledA.first->IsCoupled( coupling.coupledA.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanCouple: cannot couple component A because the end is coupled." << std::endl;
			return false;
		}

		if( !coupling.coupledA.first->IsActivated( coupling.coupledA.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanCouple: cannot couple component A because the end is not activated." << std::endl;
			return false;
		}

		if( coupling.coupledB.first->IsCoupled( coupling.coupledB.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanCouple: cannot couple component B because the end is coupled." << std::endl;
			return false;
		}

		if( !coupling.coupledB.first->IsActivated( coupling.coupledB.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanCouple: cannot couple component B because the end is not activated." << std::endl;
			return false;
		}

		if( coupling.coupledA.first->GetCouplingTypeIndex( coupling.coupledA.second ) != coupling.coupledB.first->GetCouplingTypeIndex( coupling.coupledB.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanCouple: cannot couple component because the coupling types do not match." << std::endl;
			return false;
		}
	}

	return true;
}

bool Couple( const TrainComponent::Coupling& coupling ) noexcept
{
	if( coupling.coupledA.first && coupling.coupledB.first )
	{
		return coupling.coupledA.first->Couple( coupling.coupledA.second, *coupling.coupledB.first, coupling.coupledB.second );
	}

	return false;
}

bool CanReCouple( const TrainComponent::Coupling& coupling ) noexcept
{
	if( coupling.coupledA.first && coupling.coupledB.first )
	{
		if( !coupling.coupledA.first->IsCoupled( coupling.coupledA.second ) && !coupling.coupledA.first->IsActivated( coupling.coupledA.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanReCouple: cannot couple component A because the end is neither coupled nor active." << std::endl;
			return false;
		}

		if( !coupling.coupledB.first->IsCoupled( coupling.coupledB.second ) && !coupling.coupledB.first->IsActivated( coupling.coupledB.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanReCouple: cannot couple component B because the end is neither coupled nor active." << std::endl;
			return false;
		}

		if( coupling.coupledA.first->GetCouplingTypeIndex( coupling.coupledA.second ) != coupling.coupledB.first->GetCouplingTypeIndex( coupling.coupledB.second ) )
		{
			std::cout << Verbosity::detailed << "TrainComponent::CanReCouple: cannot couple component because the coupling types do not match." << std::endl;
			return false;
		}
	}

	return true;
}

bool ReCouple( const TrainComponent::Coupling& coupling ) noexcept
{
	if( coupling.coupledA.first && coupling.coupledB.first )
	{
		if( coupling.coupledA.first->IsCoupled( coupling.coupledA.second ) )
		{
			coupling.coupledA.first->Uncouple( coupling.coupledA.second );
			coupling.coupledA.first->ActivateCoupling( coupling.coupledA.second );
		}

		if( coupling.coupledB.first->IsCoupled( coupling.coupledB.second ) )
		{
			coupling.coupledB.first->Uncouple( coupling.coupledB.second );
			coupling.coupledB.first->ActivateCoupling( coupling.coupledB.second );
		}

		return coupling.coupledA.first->Couple( coupling.coupledA.second, *coupling.coupledB.first, coupling.coupledB.second );
	}

	return false;
}

Train* CommonParent( const TrainComponent& a, const TrainComponent& b ) noexcept
{
	Train* pParentB = b.GetTrain();
	while( pParentB )
	{
		Train* pParentA = a.GetTrain();
		while( pParentA )
		{
			if( pParentA == pParentB )
				return pParentA;

			pParentA = pParentA->GetTrain();
		}

		pParentB = pParentB->GetTrain();
	}

	return nullptr;
}

}