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

#include "ModuleCommands_Imp.h"

#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/collections/Fleet.h"
#include "trax/support/TraxSupportStream.h"

#include <sstream>	

namespace trax{
///////////////////////////////////////
CommandModule::CommandModule( Module& modul ) noexcept
	: m_Module{ modul }
{}
///////////////////////////////////////
CommandRailRunner::CommandRailRunner( Module& modul, IDType railRunnerID, bool bTrain ) noexcept
	: CommandModule	{ modul }
	, m_RailRunnerID{ railRunnerID }
	, m_bTrain		{ bTrain }
{}

RailRunner& CommandRailRunner::GetRailRunner() const
{
	if( std::shared_ptr<Fleet> pFleet = m_Module.GetFleet(); pFleet )
	{
		if( m_bTrain ){
			if( std::shared_ptr<Train> pTrain = pFleet->Get( m_RailRunnerID ); pTrain )
				return *pTrain;
		}
		else{
			if( std::shared_ptr<RollingStock> pRollingStock = pFleet->GetRollingStock( m_RailRunnerID ); pRollingStock )
				return *pRollingStock;
		}
	}

	throw std::logic_error{ "CommandRailRunner: No such RailRunner with given ID." };
}
///////////////////////////////////////
std::unique_ptr<cmnd::Command> MakeCommandThrust(
	Module& modul, 
	IDType railRunnerID, 
	bool bTrain, 
	One thrust )
{
	return std::make_unique<CommandThrust>( modul, railRunnerID, bTrain, thrust );
}

CommandThrust::CommandThrust( 
	Module& modul, 
	IDType railRunnerID, 
	bool bTrain, 
	One thrust )
	: CommandRailRunner	{ modul, railRunnerID, bTrain }
	, m_Thrust			{ thrust }
{}

void CommandThrust::Execute()
{
	RailRunner& railRunner = GetRailRunner();
	One oldThrust = railRunner.Thrust();
	railRunner.Thrust( m_Thrust );
	m_Thrust = oldThrust;
}

const char* CommandThrust::Name() const
{
	if( m_bTrain )
		return "TrainThrust";
	else
		return "RollingStockThrust";
}

const char* CommandThrust::Call() const{
	thread_local std::string result;
	std::ostringstream stream;
	stream << Name() << "( ";
	stream << m_RailRunnerID << ", ";
	stream << m_Thrust << " )";
	result = stream.str();
	return result.c_str();
}

std::unique_ptr<cmnd::Command> CommandThrust::Clone() const{
	return std::make_unique<CommandThrust>( *this );
}
///////////////////////////////////////
std::unique_ptr<cmnd::Command> MakeCommandBrake( Module& modul, IDType railRunnerID, bool bTrain, One brake )
{
	return std::make_unique<CommandBrake>( modul, railRunnerID, bTrain, brake );
}

CommandBrake::CommandBrake( Module & modul, IDType railRunnerID, bool bTrain, One brake )
	: CommandRailRunner	{ modul, railRunnerID, bTrain }
	, m_Brake			{ brake }
{}

void CommandBrake::Execute()
{
	RailRunner& railRunner = GetRailRunner();
	One oldBrake = railRunner.Brake();
	railRunner.Brake( m_Brake );
	m_Brake = oldBrake;
}

const char* CommandBrake::Name() const
{
	if( m_bTrain )
		return "TrainBrake";
	else
		return "RollingStockBrake";
}

const char* CommandBrake::Call() const
{
	thread_local std::string result;
	std::ostringstream stream;
	stream << Name() << "( ";
	stream << m_RailRunnerID << ", ";
	stream << m_Brake << " )";
	result = stream.str();
	return result.c_str();
}

std::unique_ptr<cmnd::Command> CommandBrake::Clone() const
{
	return std::make_unique<CommandBrake>( *this );
}
///////////////////////////////////////
std::unique_ptr<cmnd::Command> MakeCommandTargetVelocity( Module& modul, IDType railRunnerID, bool bTrain, Velocity targetVelocity )
{
	return std::make_unique<CommandTargetVelocity>( modul, railRunnerID, bTrain, targetVelocity );
}

CommandTargetVelocity::CommandTargetVelocity( Module& modul, IDType railRunnerID, bool bTrain, Velocity targetVelocity )
	: CommandRailRunner{ modul, railRunnerID, bTrain }
	, m_TargetVelocity{ targetVelocity }
{}

void CommandTargetVelocity::Execute()
{
	RailRunner& railRunner = GetRailRunner();
	Velocity oldTargetVelocity = railRunner.TargetVelocity();
	railRunner.TargetVelocity( m_TargetVelocity );
	m_TargetVelocity = oldTargetVelocity;
}

const char* CommandTargetVelocity::Name() const
{
	if( m_bTrain )
		return "TrainTargetVelocity";
	else
		return "RollingStockTargetVelocity";
}

const char * CommandTargetVelocity::Call() const
{
	thread_local std::string result;
	std::ostringstream stream;
	stream << Name() << "( ";
	stream << m_RailRunnerID << ", ";
	stream << m_TargetVelocity << " )";
	result = stream.str();
	return result.c_str();
}

std::unique_ptr<cmnd::Command> CommandTargetVelocity::Clone() const
{
	return std::make_unique<CommandTargetVelocity>( *this );
}
///////////////////////////////////////
std::unique_ptr<cmnd::Command> MakeCommandToggleCoupling( Module& modul, IDType railRunnerID, bool bTrain, EndType atEnd )
{
	return std::make_unique<CommandToggleCoupling>( modul, railRunnerID, bTrain, atEnd );
}

CommandToggleCoupling::CommandToggleCoupling( 
	Module& modul, 
	IDType railRunnerID, 
	bool bTrain, 
	EndType atEnd )
	: CommandRailRunner{ modul, railRunnerID, bTrain }
	, m_AtEnd{ atEnd }
{}

void CommandToggleCoupling::Execute()
{
	RailRunner& railRunner = GetRailRunner();

	if( railRunner.IsCoupled( m_AtEnd ) )
	{
		railRunner.Uncouple( m_AtEnd );
	}
	else if( railRunner.IsActivated( m_AtEnd ))
	{
		railRunner.DeactivateCoupling( m_AtEnd );
	}
	else
	{
		railRunner.ActivateCoupling( m_AtEnd );
	}
}

const char* CommandToggleCoupling::Name() const
{
	if( m_bTrain )
		return "ToggleTrainCoupling";
	else
		return "ToggleRollingStockCoupling";
}

const char* CommandToggleCoupling::Call() const
{
	thread_local std::string result;
	std::ostringstream stream;
	stream << Name() << "( ";
	stream << m_RailRunnerID << ", ";
	stream << m_AtEnd << " )";
	result = stream.str();
	return result.c_str();
}

std::unique_ptr<cmnd::Command> CommandToggleCoupling::Clone() const
{
	return std::make_unique<CommandToggleCoupling>( *this );
}
///////////////////////////////////////
}