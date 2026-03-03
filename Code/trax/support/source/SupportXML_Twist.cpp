//	trax track library
//	AD 2022 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "trax/support/TraxSupportXML.h"
#include "spat/support/SpatSupportXML.h"
#include "dim/support/DimSupportXML.h"

#include "trax/RoadwayTwist.h"

namespace trax
{
namespace ptreesupport
{
	using namespace dim::ptreesupport;
	using namespace spat::ptreesupport;

	std::unique_ptr<RoadwayTwist> CreateTwist( const boost::property_tree::ptree& pt )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "ConstantTwist" )
			{
				if( std::unique_ptr<ConstantTwist> pConstantTwist = ConstantTwist::Make(); pConstantTwist )
				{
					pair.second >> *pConstantTwist;
					return pConstantTwist;
				}
			}

			else if( pair.first == "LinearTwist" )
			{
				if( std::unique_ptr<LinearTwist> pLinearTwist = LinearTwist::Make(); pLinearTwist )
				{
					pair.second >> *pLinearTwist;
					return pLinearTwist;
				}
			}

			else if( pair.first == "PiecewiseTwist" )
			{
				if( std::unique_ptr<PiecewiseTwist> pPiecewiseTwist = PiecewiseTwist::Make(); pPiecewiseTwist )
				{
					pair.second >> *pPiecewiseTwist;
					return pPiecewiseTwist;
				}
			}

			else if( pair.first == "PiecewiseLinear" )
			{
				if( std::unique_ptr<PiecewiseTwist> pPiecewiseLinear = PiecewiseTwist::Make( RoadwayTwist::TwistType::PiecewiseLinear ); pPiecewiseLinear )
				{
					pair.second >> *pPiecewiseLinear;
					return pPiecewiseLinear;
				}
			}

			else if( pair.first == "PiecewiseCircularTwist" )
			{
				if( std::unique_ptr<PiecewiseTwist> pPiecewiseCircularTwist = PiecewiseTwist::Make( RoadwayTwist::TwistType::PiecewiseCircular ); pPiecewiseCircularTwist )
				{
					pair.second >> *pPiecewiseCircularTwist;
					return pPiecewiseCircularTwist;
				}
			}

			else if( pair.first == "DirectionalTwist" )
			{
				if( std::unique_ptr<DirectionalTwist> pDirectionalTwist = DirectionalTwist::Make(); pDirectionalTwist )
				{
					pair.second >> *pDirectionalTwist;
					return pDirectionalTwist;
				}
			}

			else if( pair.first == "PiecewiseDirectionalTwist" )
			{
				if( std::unique_ptr<PiecewiseDirectionalTwist> pPiecewiseDirectionalTwist = PiecewiseDirectionalTwist::Make(); pPiecewiseDirectionalTwist )
				{
					pair.second >> *pPiecewiseDirectionalTwist;
					return pPiecewiseDirectionalTwist;
				}
			}

			else if( pair.first == "CombinedTwist" )
			{
				if( std::unique_ptr<CombinedTwist> pCombinedTwist = CombinedTwist::Make(); pCombinedTwist )
				{
					pair.second >> *pCombinedTwist;
					return pCombinedTwist;
				}
			}
		}

		return nullptr;
	}

	const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, ConstantTwist& constantTwist )
	{
		constantTwist.TwistValue( pt.get( "<xmlattr>.angle", 0_rad ) );
		return pt;
	}

	const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, LinearTwist& linearTwist )
	{
		linearTwist.From( pt.get( "<xmlattr>.startangle", 0_rad ) );
		linearTwist.To( pt.get( "<xmlattr>.endangle", 0_rad ) );
		return pt;
	}

	const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, PiecewiseTwist& piecewiseTwist )
	{
		PiecewiseTwist::Data data;

		for( const auto& pair : pt )
		{
			if( pair.first == "TwistAngle" )
				data.push_back( std::make_pair( 
					get( pair.second, "<xmlattr>.s", 0_m, _m ),
					pair.second.get( "<xmlattr>.value", 0_rad ) ) );
		}
		piecewiseTwist.Create( data );

		return pt;
	}

	const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, DirectionalTwist& directionalTwist )
	{
		directionalTwist.Freeze( pt.get( "<xmlattr>.bFrozen", false ) );

		for( const auto& pair : pt )
		{
			if( pair.first == "Vector" ){
				spat::Vector<One> attractor;
				ReadVector( pair.second, attractor );
				directionalTwist.Attractor( attractor );
			}
		}

		return pt;
	}

	const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, PiecewiseDirectionalTwist& piecewiseDirectionalTwist )
	{

		return pt;
	}

	const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, CombinedTwist& combinedTwist )
	{
		std::vector<std::unique_ptr<RoadwayTwist>> twists;
		twists.reserve(2);

		for( const auto& pair : pt )
		{
			if( pair.first == "Twist" )
			{
				twists.push_back( CreateTwist( pair.second ) );
			}
		}

		if( twists.size() > 0 && twists.at(0) )
			combinedTwist.AttachTwist1( std::move(twists.at(0)) );

		if( twists.size() > 1 && twists.at(1) )
			combinedTwist.AttachTwist2( std::move(twists.at(1)) );

		return pt;
	}


	std::unique_ptr<RoadwayTwist> CreateConstantTwist( const boost::property_tree::ptree& pt ){
		if( auto pConstantTwist = ConstantTwist::Make() ){

			pConstantTwist->TwistValue( pt.get( "<xmlattr>.angle", 0_rad ) );

			return pConstantTwist;
		}

		return nullptr;
	}

	std::unique_ptr<RoadwayTwist> CreateLinearTwist( const boost::property_tree::ptree& pt ){
		if( auto pLinearTwist = LinearTwist::Make() ){

			pLinearTwist->From( pt.get( "<xmlattr>.startangle", 0_rad ) );
			pLinearTwist->To( pt.get( "<xmlattr>.endangle", 0_rad ) );

			return pLinearTwist;
		}

		return nullptr;
	}
		
	static void CreatePiecewiseTwist( const boost::property_tree::ptree& pt, PiecewiseTwist& twist )
	{
		PiecewiseTwist::Data data;

		for( const auto& pair : pt )
		{
			if( pair.first == "TwistAngle" )
				data.push_back( std::make_pair( 
					get( pair.second, "<xmlattr>.s", 0_m, _m ),
					pair.second.get( "<xmlattr>.value", 0_rad ) ) );
		}

		twist.Create( data );
	}
		
	std::unique_ptr<RoadwayTwist> CreatePiecewiseTwist( const boost::property_tree::ptree& pt ){
		if( auto pTwist = PiecewiseTwist::Make() )
		{
			CreatePiecewiseTwist( pt, *pTwist );

			return pTwist;
		}

		return nullptr;
	}

	std::unique_ptr<RoadwayTwist> CreatePiecewiseLinearTwist( const boost::property_tree::ptree& pt ){
		if( auto pTwist = PiecewiseTwist::Make( RoadwayTwist::TwistType::PiecewiseLinear ) )
		{
			CreatePiecewiseTwist( pt, *pTwist );

			return pTwist;
		}

		return nullptr;
	}
		
	std::unique_ptr<RoadwayTwist> CreatePiecewiseCircularTwist( const boost::property_tree::ptree& pt ){
		if( auto pTwist = PiecewiseTwist::Make( RoadwayTwist::TwistType::PiecewiseCircular ) )
		{
			CreatePiecewiseTwist( pt, *pTwist );

			return pTwist;
		}

		return nullptr;
	}
		
	std::unique_ptr<RoadwayTwist> CreateDirectionalTwist( const boost::property_tree::ptree& pt ){
		if( auto pTwist = DirectionalTwist::Make() )
		{
			pTwist->Freeze( pt.get( "<xmlattr>.bFrozen", false ) );

			for( const auto& pair : pt )
			{
				if( pair.first == "Vector" ){
					spat::Vector<One> attractor;
					ReadVector( pair.second, attractor );
					pTwist->Attractor( attractor );
				}
			}

			return pTwist;
		}

		return nullptr;
	}
}
}