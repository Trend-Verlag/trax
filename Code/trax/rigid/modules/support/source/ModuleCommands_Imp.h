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

#include "trax/rigid/modules/support/ModuleCommands.h"
#include "trax/rigid/modules/Module.h"
#include "appframe/Command.h"

namespace trax{

	class CommandModule : public cmnd::Command
	{
	public:
		CommandModule( Module& modul ) noexcept;
	protected:
		Module& m_Module; ///< Module this command is operating on.

	};


	struct RailRunner;

	class CommandRailRunner : public CommandModule
	{
	public:
		CommandRailRunner( Module& modul, IDType railRunnerID, bool bTrain ) noexcept;

	protected:
		RailRunner& GetRailRunner() const;

		IDType m_RailRunnerID;
		bool m_bTrain;
	};


	class CommandThrust : public CommandRailRunner
	{
	public:
		CommandThrust( Module& modul, IDType railRunnerID, bool bTrain, One thrust );

		void Execute() override;
		const char* Name() const override;
		const char* Call() const override;
		std::unique_ptr<cmnd::Command> Clone() const override;
	private:
		One m_Thrust;
	};


	class CommandBrake : public CommandRailRunner
	{
	public:
		CommandBrake( Module& modul, IDType railRunnerID, bool bTrain, One brake );

		void Execute() override;
		const char* Name() const override;
		const char* Call() const override;
		std::unique_ptr<cmnd::Command> Clone() const override;
	private:
		One m_Brake;
	};


	class CommandTargetVelocity : public CommandRailRunner
	{
	public:
		CommandTargetVelocity( Module& modul, IDType railRunnerID, bool bTrain, Velocity targetVelocity );

		void Execute() override;
		const char* Name() const override;
		const char* Call() const override;
		std::unique_ptr<cmnd::Command> Clone() const override;
	private:
		Velocity m_TargetVelocity;
	};


	class CommandToggleCoupling : public CommandRailRunner
	{
	public:
		CommandToggleCoupling( Module& modul, IDType railRunnerID, bool bTrain, EndType atEnd );

		void Execute() override;
		const char* Name() const override;
		const char* Call() const override;
		std::unique_ptr<cmnd::Command> Clone() const override;
	//	bool Reversible() const noexcept override { return false; }
	private:
		EndType m_AtEnd;
	};


}
