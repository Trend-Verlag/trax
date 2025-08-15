// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//	trax track library
//	AD 2019 
//
//  "Ein Drittel Heizoel, zwei Drittel Benzin."
//
//							Casper
//

#pragma once

#include <boost/version.hpp>

//static_assert(BOOST_VERSION >= 107000, "Boost version too old");

#if BOOST_VERSION < 107000
#   include <boost/math/tools/numerical_differentiation.hpp>
#else
#   include <boost/math/differentiation/finite_difference.hpp>
#endif // BOOST_VERSION < 107000


namespace trax{

template<class ResultType, class F, typename Valtype>
inline ResultType finite_difference_derivative6( const F f, Valtype t )   
// Based on Boost:
//
//  (C) Copyright Nick Thompson 2018.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
{
    using rtype = typename Valtype::real_type;
    constexpr rtype eps = (std::numeric_limits<rtype>::epsilon)();
    // Error bound ~eps^6/7
    // Error: h^6f^(7)(x)/140 + 5|f(x)|eps/h
    Valtype h{ std::pow( eps / rtype{168}, rtype{1} / rtype{7} ) };

#if BOOST_VERSION < 107000
    h = Valtype{ boost::math::tools::detail::make_xph_representable(t.Units(), h.Units()) };
#else
    h = Valtype{ boost::math::differentiation::detail::make_xph_representable(t.Units(), h.Units()) };
#endif // BOOST_VERSION < 107000

    const auto y1 = f(t + h) - f(t - h);
    const auto y2 = f(t - 2 * h) - f(t + 2 * h);
    const auto y3 = f(t + 3 * h) - f(t - 3 * h);

    return (y3 + 9_1 * y2 + 45_1 * y1) / (60 * h);
}

template<class ResultType, class F, typename Valtype>
inline ResultType finite_difference_derivative8( const F f, Valtype t )      
// Based on Boost:
//
//  (C) Copyright Nick Thompson 2018.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
{
    using rtype = typename Valtype::real_type;

    const rtype eps = (std::numeric_limits<rtype>::epsilon)();
    // Error bound ~eps^8/9.
    // In double precision, we only expect to lose two digits of precision while using this formula, at the cost of 8 function evaluations.
    // Error: h^8|f^(9)(x)|/630 + 7|f(x)|eps/h assuming 7 unstabilized additions.
    // Mathematica code to get the error:
    // Series[(f[x+h]-f[x-h])*(4/5) + (1/5)*(f[x-2*h] - f[x+2*h]) + (4/105)*(f[x+3*h] - f[x-3*h]) + (1/280)*(f[x-4*h] - f[x+4*h]), {h, 0, 9}]
    // If we used Kahan summation, we could get the max error down to h^8|f^(9)(x)|/630 + |f(x)|eps/h.
    Valtype h{ std::pow( rtype{551.25f} * eps, rtype{1} / rtype{9} ) };

#if BOOST_VERSION < 107000
    h = Valtype{ boost::math::tools::detail::make_xph_representable(t.Units(), h.Units()) };
#else
    h = Valtype{ boost::math::differentiation::detail::make_xph_representable(t.Units(), h.Units()) };
#endif // BOOST_VERSION < 107000

    const auto y1 = f(t + h) - f(t - h);
    const auto y2 = f(t - 2 * h) - f(t + 2 * h);
    const auto y3 = f(t + 3 * h) - f(t - 3 * h);
    const auto y4 = f(t - 4 * h) - f(t + 4 * h);

    const auto tmp1 = 3 * y4 / 8 + 4 * y3;
    const auto tmp2 = 21 * y2 + 84 * y1;

    return (tmp1 + tmp2) / (105 * h);
}

/// /brief Simplifies a polygonal chain, by the Douglas-Peucker algorithm 
/// 
/// https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm.
template<class ContainerType,typename Valtype, typename IteratorToPositionConverterType >
void DouglasPeucker( 
    ContainerType& data, 
    typename ContainerType::iterator start, 
    typename ContainerType::iterator end, 
    Valtype maxDeviation, 
    IteratorToPositionConverterType P ){

	const spat::VectorBundle<Length,One> B{
		P(start),
		spat::Normalize( P(end) - P(start) ).second };

    Valtype maxDistance = Valtype{0};
	typename ContainerType::iterator iterMax;
	for( typename ContainerType::iterator iter = start + 1; iter < end; ++iter ){
		spat::Vector<Valtype> O{ P(iter) - B.P };
		O -= O * B.T * B.T;
		const Valtype distance = O.Length();
		if( maxDistance < distance ){
			maxDistance = distance;
			iterMax = iter;
		}
	}

	if( maxDistance < maxDeviation )
		data.erase( start+1, end );
	else{
		DouglasPeucker( data, iterMax, end, maxDeviation, P );
		DouglasPeucker( data, start, iterMax, maxDeviation, P );
	}
}

}
