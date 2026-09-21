//	trax track library
//	AD 2019 
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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "Units.h"
#include "common/Interval.h"

//#define BOOST_MATH_INSTRUMENT
#include <boost/math/tools/roots.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/lu.hpp>

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

    namespace ublas = boost::numeric::ublas;

    // Solve a general N-dimensional nonlinear system F(x) = 0.
    //   F      : function mapping a length-N vector to a length-N residual vector
    //   guess  : initial estimate (also receives the result)
    //   tol    : convergence tolerance on ||F(x)||
    //   maxIt  : maximum Newton iterations
    // Returns true on convergence, false otherwise (singular Jacobian / no convergence).
    template<typename Valtype>
    inline bool NewtonSolve(
        const std::function<ublas::vector<Valtype>(const ublas::vector<Valtype>&)>& F,
        ublas::vector<Valtype>& guess,
        Valtype tol   = Valtype{1e-10},
        std::size_t maxIt = 100)
    {
        const std::size_t n = guess.size();
        const Valtype h = Valtype{1e-7}; // finite-difference step for the Jacobian

        for (std::size_t it = 0; it < maxIt; ++it)
        {
            ublas::vector<Valtype> f = F(guess);

            // Check convergence: ||f||_2
            if (ublas::norm_2(f) < tol)
                return true;

            // Build Jacobian J via forward finite differences
            ublas::matrix<double> J(n, n);
            for (std::size_t j = 0; j < n; ++j)
            {
                ublas::vector<Valtype> xp = guess;
                xp(j) += h;
                ublas::vector<Valtype> fp = F(xp);
                for (std::size_t i = 0; i < n; ++i)
                    J(i, j) = (fp(i) - f(i)) / h;
            }

            // Solve J * dx = f  (LU factorization, in place)
            ublas::permutation_matrix<std::size_t> pm(n);
            if (ublas::lu_factorize(J, pm) != 0)
                return false; // singular Jacobian

            ublas::vector<Valtype> dx = f;
            ublas::lu_substitute(J, pm, dx);

            // Newton update: x <- x - dx
            guess -= dx;
        }

        // Final tolerance check after the loop
        return ublas::norm_2(F(guess)) < tol;
    }
}