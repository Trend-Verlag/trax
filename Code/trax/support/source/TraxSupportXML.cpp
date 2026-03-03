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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "trax/support/TraxSupportXML.h"

#include "trax/Section.h"
#include "trax/SectionTrack.h"
#include "trax/TrackLocation.h"
//#include "trax/TractionForceCharacteristic.h"
//#include "trax/Wheelset.h"

namespace trax{
	using namespace dim;
	using namespace dim::ptreesupport;
	using namespace spat;
	using namespace spat::ptreesupport;

namespace ptreesupport{

static std::unique_ptr<Curve> CreateCurve( const boost::property_tree::ptree& pt )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "BasicLine" )
		{
			if( std::unique_ptr<Line> pLine = Line::Make(); pLine )
			{
				pair.second >> *pLine;
				return pLine;
			}
		}

		else if( pair.first == "Line" )
		{
			if( std::unique_ptr<LineP> pLineP = LineP::Make(); pLineP )
			{
				pair.second >> *pLineP;
				return pLineP;
			}
		}

		else if( pair.first == "BasicArc" )
		{
			if( std::unique_ptr<Arc> pArc = Arc::Make(); pArc )
			{
				pair.second >> *pArc;
				return pArc;
			}
		}

		else if( pair.first == "Arc" )
		{
			if( std::unique_ptr<ArcP> pArcP = ArcP::Make(); pArcP )
			{
				pair.second >> *pArcP;
				return pArcP;
			}
		}

		else if( pair.first == "BasicHelix" )
		{
			if( std::unique_ptr<Helix> pHelix = Helix::Make(); pHelix )
			{
				pair.second >> *pHelix;
				return pHelix;
			}
		}

		else if( pair.first == "Helix" )
		{
			if( std::unique_ptr<HelixP> pHelixP = HelixP::Make(); pHelixP )
			{
				pair.second >> *pHelixP;
				return pHelixP;
			}
		}

		else if( pair.first == "Cubic" )
		{
			if( std::unique_ptr<Cubic> pCubic = Cubic::Make(); pCubic )
			{
				pair.second >> *pCubic;
				return pCubic;
			}
		}

		else if( pair.first == "Spline" )
		{
			if( std::unique_ptr<Spline> pSpline = Spline::Make(); pSpline )
			{
				pair.second >> *pSpline;
				return pSpline;
			}
		}

		else if( pair.first == "Clothoid" )
		{
			if( std::unique_ptr<Clothoid> pClothoid = Clothoid::Make(); pClothoid )
			{
				pair.second >> *pClothoid;
				return pClothoid;
			}
		}

		else if( pair.first == "Rotator" )
		{
			Curve::CurveType type = (pt.get( "<xmlattr>.a0", 0_1Im ) == 0_1Im && pt.get( "<xmlattr>.b0", 0_1Im ) == 0_1Im) ? 
				Curve::CurveType::Rotator : Curve::CurveType::RotatorWithOffset;

			if( std::unique_ptr<Rotator> pRotator = Rotator::Make( type ); pRotator )
			{
				pair.second >> *pRotator;
				return pRotator;
			}
		}

		else if( pair.first == "RotatorChain" )
		{
			if( std::unique_ptr<RotatorChain> pRotatorChain = RotatorChain::Make(); pRotatorChain )
			{
				pair.second >> *pRotatorChain;
				return pRotatorChain;
			}
		}

		else if( pair.first == "PolygonalChain" )
		{
			if( std::unique_ptr<PolygonalChain> pPolygonalChain = PolygonalChain::Make(); pPolygonalChain )
			{
				pair.second >> *pPolygonalChain;
				return pPolygonalChain;
			}
		}

		else if( pair.first == "SampledCurve" )
		{
			if( std::unique_ptr<SampledCurve> pSampledCurve = SampledCurve::Make(); pSampledCurve )
			{
				pair.second >> *pSampledCurve;
				return pSampledCurve;
			}
		}
	}

	return nullptr;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Section& section )
{
	if( section.GetSectionType() == Section::SpecialSections::custom )
	{
		section.SetCntPoints( pt.get( "<xmlattr>.count", 0 ) );
		int idx = 0;
		for( const auto& pair : pt )
		{
			if( pair.first == "Position2D" ){
				spat::Position2D<Length> position;
				ReadPosition2D( pair.second, position );
				section.Set( idx++, position );
			}
		}
	}

	section.Shift( Vector2D<Length>{ get( pt, "<xmlattr>.shiftX", 0_m, _m ), get( pt, "<xmlattr>.shiftY", 0_m, _m ) } );
	section.Scale( get( pt, "<xmlattr>.scaleX", 1_1, _1 ), get( pt, "<xmlattr>.scaleY", 1_1, _1 ) );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, TrackBuilder& track )
{
	AttributesToReferences( pt, track );
	common::Interval<Length> range{ 0_m, get( pt, "<xmlattr>.length", 10_m, _m ) };

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			spat::Frame<Length,One> frame;
			ReadFrame( pair.second, frame );
			track.SetFrame( frame );
		}

		else if( pair.first == "Interval" )
		{
			common::ptreesupport::ReadInterval( pair.second, range );
		}

		else if( pair.first == "Curve" )
		{
			track.Attach( CreateCurve( pair.second ), range );
		}

		else if( pair.first == "Twist" )
		{
			track.Attach( CreateTwist( pair.second ) );
		}

		else if( pair.first == "Section" )
		{
			if( SectionTrack* pSectionTrack = dynamic_cast<SectionTrack*>(&track); pSectionTrack )
			{
				if( std::unique_ptr<Section> pSection = Section::Make( SpecialSection( pair.second.get( "<xmlattr>.type", "custom" ) ) ); pSection )
				{
					pair.second >> *pSection;
					pSectionTrack->Attach( std::move(pSection) );
				}
			}
		}
	}

	return pt;
}


void ReadConnection( const boost::property_tree::ptree& pt, Track::End& trackend ){
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackEnd" )
			ReadTrackEnd( pair.second, trackend );
	}
}

void ReadTrackEnd( const boost::property_tree::ptree& pt, Track::End& trackend ){
	trackend.id		= pt.get( "<xmlattr>.refid", IDType{0} );
	trackend.type	= ToEndType( pt.get( "<xmlattr>.type", "front" ) );
}

void ReadTrackLocation( const boost::property_tree::ptree& pt, TrackLocation& trackLocation ) noexcept{
	trackLocation.parameter		= get( pt, "<xmlattr>.parameter", 0_m, _m );
	trackLocation.orientation	= pt.get( "<xmlattr>.orientation", "para" ) == "anti" ? Orientation::Value::anti : Orientation::Value::para;
}

void ReadTrackLocationRef( const boost::property_tree::ptree& pt, TrackLocationRef& trackLocationRef ) noexcept{
	ReadTrackLocation( pt, trackLocationRef.location );
	trackLocationRef.refid = pt.get( "<xmlattr>.refid", IDType{0} );
}

void ReadTrackRange( const boost::property_tree::ptree& pt, TrackRange& trackRange ) noexcept{
	trackRange.range.Near( get( pt, "<xmlattr>.near", 0_m, _m ) );
	trackRange.range.Far( get( pt, "<xmlattr>.far", 0_m, _m ) );
	trackRange.refid = pt.get( "<xmlattr>.refid", IDType{0} );
}

void Read( const boost::property_tree::ptree& pt, Cubic::Data& data ){
	std::vector<spat::Vector<Length>> vectors;
	vectors.reserve( 3 );

	for( const auto& pair : pt )
	{
		if( pair.first == "Position" )
			ReadPosition( pair.second, data.a );

		else if( pair.first == "Vector" ){
			vectors.push_back( {} );
			ReadVector( pair.second, vectors.back() );
		}
	}

	if( vectors.size() > 0 )
		data.b = vectors.at(0);

	if( vectors.size() > 1 )
		data.c = vectors.at(1);

	if( vectors.size() > 2 )
		data.d = vectors.at(2);
}

void Read( const boost::property_tree::ptree& pt, CurveSample& sample ){
	sample.s = pt.get( "<xmlattr>.s", sample.s );
	sample.k = pt.get( "<xmlattr>.k", sample.k );
	sample.t = pt.get( "<xmlattr>.t", sample.t );

	for( const auto& pair : pt ){
		if( pair.first == "Frame" )
			ReadFrame( pair.second, sample.F );
	}
}

void Read( const boost::property_tree::ptree& pt, EEPCurve::Data& data ){
	data.gc_Kruemmung = pt.get( "<xmlattr>.Kruemmung", data.gc_Kruemmung );
	data.gc_Windung = pt.get( "<xmlattr>.Torsion", data.gc_Windung );
	data.gc_Verdrillung = pt.get( "<xmlattr>.Fuehrungsverdrehung", data.gc_Verdrillung );
	data.gc_Laenge = pt.get( "<xmlattr>.Laenge", data.gc_Laenge );
	data.gc_Kurve = pt.get( "<xmlattr>.Kurve", data.gc_Kurve );
	data.m_FuehrungsVerdrehung = pt.get( "<xmlattr>.Anfangsfuehrungsverdrehung", data.m_FuehrungsVerdrehung );

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" )
			ReadFrame( pair.second, data.m_AnfangsBein );
	}
}

} // namespace ptreesupport
} // namespace trax