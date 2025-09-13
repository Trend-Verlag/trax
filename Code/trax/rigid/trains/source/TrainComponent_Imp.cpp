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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "TrainComponent_Imp.h"


namespace trax{

bool Couple( const TrainComponent::Coupling& coupling ) noexcept
{
	if( coupling.pTrainComponentA && coupling.pTrainComponentB )
	{
		return coupling.pTrainComponentA->Couple( coupling.endA, *coupling.pTrainComponentB, coupling.endB );
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