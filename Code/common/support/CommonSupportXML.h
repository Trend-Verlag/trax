//	trax track library
//	AD 2014 
//
//  "You are too clever to be mentally ill."
//
//						The Indelicates
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include <boost/property_tree/ptree.hpp>

#include "common/Interval.h"


namespace common{

	/// \brief ptree operator support
	///
	/// If you want to use these in a namespace that itself declares overloads
	/// of these operators, use using declarations to bring them into scope 
	/// (e.g.: using common::ptreesupport::operator<<;); otherwise they will be
	/// hidden by the namespace's own overloads.
	namespace ptreesupport{

		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const Interval<Valtype>& i );
		template<typename Valtype>
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, Interval<Valtype>& i );


		template<typename Valtype> inline
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const Interval<Valtype>& i ){
			boost::property_tree::ptree ptInterval;

			ptInterval.add( "<xmlattr>.near", i.Near() );
			ptInterval.add( "<xmlattr>.far", i.Far() );

			opt.add_child( "Interval", ptInterval );
			return opt;
		}

		template<typename Valtype> inline
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, Interval<Valtype>& i ){
			for( auto iter = ipt.begin(); iter != ipt.end(); ++iter ){
				if( (*iter).first == "Interval" )
				{
					i.m_Near = (*iter).second.get( "<xmlattr>.near", Valtype{0} );
					i.m_Far = (*iter).second.get( "<xmlattr>.far", Valtype{0} );
					ipt.erase( iter );
					return ipt;
				}
			}

			assert( !"Syntax error: no Interval tag!" );
			return ipt;
		}


		template<typename Valtype> inline
		void ReadInterval( const boost::property_tree::ptree& pt, common::Interval<Valtype>& interval ){
			interval.Near( pt.get( "<xmlattr>.near", Valtype{0} ) );
			interval.Far( pt.get( "<xmlattr>.far", Valtype{0} ) );
		}
	} // namespace ptreesupport
} // namespace common

