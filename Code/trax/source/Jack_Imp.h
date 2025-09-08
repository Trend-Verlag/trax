//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "trax/Jack.h"
#include "trax/Units.h"

#include <sstream>

namespace trax{

	template<class Base>
	class JackEnumerator_Imp :	public Base,
								public JackEnumerator{
	public:

		int CountJacks() const override{
			int count = 0;
			for( const auto& element : *this )
				count += dynamic_cast<const JackEnumerator&>(element).CountJacks();

			return count;
		}
	protected:
		const Jack& _GetJack( int idx ) const override{
			for( const auto& element : *this ){
				const JackEnumerator& jackEnumerator = dynamic_cast<const JackEnumerator&>(element);
				const int countJacks = jackEnumerator.CountJacks();
				if( idx < countJacks )
					return jackEnumerator.GetJack( idx );
				else
					idx -= countJacks;
			}

			std::ostringstream stream;
			stream << "Out of range!" << std::endl;
			stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
			throw std::range_error( stream.str() );
		}
	};

}
