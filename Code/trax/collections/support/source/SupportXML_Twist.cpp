// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "trax/collections/support/CollectionSupportXML.h"
#include "spat/support/SpatSupportXML.h"
#include "dim/support/DimSupportXML.h"

#include "trax/RoadwayTwist.h"

namespace trax
{
namespace ptreesupport
{
	using namespace dim::ptreesupport;
	using namespace spat::ptreesupport;

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

	void RegisterAllTwistReaders( class PTreeReader& toReader )
	{
        toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Constant ), CreateConstantTwist );
        toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Linear ), CreateLinearTwist );
        toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Piecewise ), CreatePiecewiseTwist );
        toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::PiecewiseLinear ), CreatePiecewiseLinearTwist );
        toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::PiecewiseCircular ), CreatePiecewiseCircularTwist );
        toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Directional ), CreateDirectionalTwist );
	}
}
}