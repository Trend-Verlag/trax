//	trax track library
//	AD 2019 
//
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

#pragma once

/// \page docu_dim Dimensionated Values
///
/// \section dim_xml XML Support
/// The dim library provides support for reading and writing dimensionated 
/// values in XML format, using the Boost Property Tree library.

#include <boost/property_tree/ptree.hpp>

#include "dim/support/DimSupportStream.h"

namespace dim{
namespace ptreesupport{

	/// \brief Reads a dimensionated value from pt and path, converting it correctly.
	///
	/// If the value in the property tree carries a unit, this will be respected. Otherwise
	/// it will be interpreted as to be the default unit of trax, unless defaultUnits
	/// specifies explicitly which unit it is to interpret as.
	/// \param pt boost property tree base.
	/// \param path path to the value relative to pt.
	/// \param defaultValue The value to be returned if the path does not exist.
	/// \param defaultUnits The units to be used if there is no unit specified.
	template<int L,int M,int T>
	Value<Dimension<L,M,T>> get( 
		const boost::property_tree::ptree& pt, 
		boost::property_tree::ptree::path_type path, 
		Value<Dimension<L,M,T>> defaultValue = Value<Dimension<L,M,T>>{0}, 
		Value<Dimension<L,M,T>> (*defaultUnits)( Real ) = DefaultUnit<L,M,T>::GetDefaultStreamInDimension() )
	{
		common::Resetter<typename DefaultUnit<L,M,T>::StreamInDimension> resetter{ DefaultUnit<L,M,T>::GetDefaultStreamInDimension(), defaultUnits };
		if( auto result = pt.get_optional<Value<Dimension<L,M,T>>>( path ) )
			return *result;
		return defaultValue;
	}

	template<int L,int M,int T>
	boost::optional<Value<Dimension<L,M,T>>> get_optional( 
		const boost::property_tree::ptree& pt, 
		boost::property_tree::ptree::path_type path, 
		Value<Dimension<L,M,T>> (*defaultUnits)( Real ) )
	{
		common::Resetter<typename DefaultUnit<L,M,T>::StreamInDimension> resetter{ DefaultUnit<L,M,T>::GetDefaultStreamInDimension(), defaultUnits };
		return pt.get_optional<Value<Dimension<L,M,T>>>( path );
	}

} // namespace ptreesupport
} // namespace dim
