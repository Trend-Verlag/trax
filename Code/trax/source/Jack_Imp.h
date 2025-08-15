// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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
