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

#include "ModuleSet_Imp.h"
#include "../Module.h"

namespace trax{

ModuleSet_Imp::ModuleSet_Imp()
{
}

void ModuleSet_Imp::AddModule( std::unique_ptr<Module> module )
{
	if( module )
		m_Modules.push_back( std::move( module ) );
}

void ModuleSet_Imp::RemoveModule( Module& module )
{
	auto it = std::find_if( m_Modules.begin(), m_Modules.end(), 
		[&module]( const std::unique_ptr<Module>& m ){ return m.get() == &module; } );
	if( it != m_Modules.end() )
		m_Modules.erase( it );
}

void ModuleSet_Imp::ClearModules()
{
	m_Modules.clear();
}

}
