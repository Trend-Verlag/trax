// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2019 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "Units.h"
#include "common/Interval.h"

//#define BOOST_MATH_INSTRUMENT
#include <boost/math/tools/roots.hpp>

namespace common{
	template<typename> struct Interval;
}

namespace trax{

	/// \name Dimensionated Values boost math tools.
	///@{
	template<class F, typename DomainType/*, typename CoDomainType*/, typename Tol>
	common::Interval<DomainType> bracket_and_solve_root(F f, const DomainType& guess, const One& factor, bool rising, Tol tol, boost::uintmax_t& max_iter){
		const auto function = [&f]( Real s ) -> Real{
			const auto result = f( DomainType{s} );
			return result.Units();
		};

		const std::pair<Real,Real> bracket = boost::math::tools::bracket_and_solve_root(
			function, 
			guess.Units(),
			factor.Units(),																			
			rising,
			tol,
			max_iter );

		return { DomainType{bracket.first}, DomainType{bracket.second} };
	}

	template<class F, typename DomainType/*, typename CoDomainType*/>
	DomainType newton_raphson_iterate( F f, DomainType guess, common::Interval<DomainType> limits, int digits, boost::uintmax_t& max_iter ){
		const auto function = [&f]( Real s ) -> std::pair<Real,Real>{
			const auto result = f( DomainType{s} );
			return { result.first.Units(), result.second.Units() };
		};

		return DomainType{ boost::math::tools::newton_raphson_iterate( 
			function, 
			guess.Units(), 
			limits.Near().Units(), 
			limits.Far().Units(), 
			digits, 
			max_iter ) };
	}
	///@}

}