//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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


#include "trax/End.h"
#include "trax/IDType.h"
#include "appframe/Command.h"
#include "dim/DimensionedValues.h"


namespace trax{

	using namespace dim;

	struct Module;

	// Command RailRunner:
	std::unique_ptr<cmnd::Command> dclspc MakeCommandThrust( Module& modul, IDType railRunnerID, bool bTrain, One thrust );
	std::unique_ptr<cmnd::Command> dclspc MakeCommandBrake( Module& modul, IDType railRunnerID, bool bTrain, One brake );
	std::unique_ptr<cmnd::Command> dclspc MakeCommandTargetVelocity( Module& modul, IDType railRunnerID, bool bTrain, Velocity targetVelocity );
	std::unique_ptr<cmnd::Command> dclspc MakeCommandToggleCoupling( Module& modul, IDType railRunnerID, bool bTrain, EndType atEnd );

	// Command Train:
	std::unique_ptr<cmnd::Command> dclspc MakeCommandRail( Module& modul, IDType trainID, IDType trackID, Length atParameter );
	std::unique_ptr<cmnd::Command> dclspc MakeCommandReduce( Module& modul, IDType trainID );


}