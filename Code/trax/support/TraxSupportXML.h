//	trax track library
//	AD 2014 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "common/support/CommonSupportXML.h"
#include "dim/support/DimSupportXML.h"
#include "spat/support/SpatSupportXML.h"

#include "trax/Curve.h"
#include "trax/Track.h"

namespace trax{

	struct Section;
	struct TrackLocation;
	struct TrackLocationRef;
	struct TrackRange;

	namespace ptreesupport{

		using spat::ptreesupport::operator<<;


		/// \name Property Tree Streaming Support for Trax Classes
		///@{
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Line& line );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, LineP& lineP );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Arc& arc );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, ArcP& arcP );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Helix& helix );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, HelixP& helixP );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Cubic& cubic );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Spline& spline );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Clothoid& clothoid );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Rotator& rotator );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, RotatorChain& rotatorChain );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, PolygonalChain& polygonalChain );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, SampledCurve& sampledCurve );

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, ConstantTwist& constantTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, LinearTwist& linearTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, PiecewiseTwist& piecewiseTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, DirectionalTwist& directionalTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, PiecewiseDirectionalTwist& piecewiseDirectionalTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, CombinedTwist& combinedTwist );

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Section& section );

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, TrackBuilder& track );
		///@}

		/// \name Explicit XML Reading
		///@{	

		/// \name Explicit XML Reading
		///@{
		dclspc std::unique_ptr<Curve> CreateLine( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateLineP( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateArc( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateArcP( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateHelix( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateHelixP( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateCubic( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateSpline( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateClothoid( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateRotator( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateRotatorChain( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreatePolygonalChain( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateSampledCurve( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateEEPCurve( const boost::property_tree::ptree& pt );

		dclspc std::unique_ptr<RoadwayTwist> CreateTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateConstantTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateLinearTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseLinearTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseCircularTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateDirectionalTwist( const boost::property_tree::ptree& pt );
		dclspc void ReadConnection( const boost::property_tree::ptree& pt, Track::End& trackend );
		dclspc void ReadTrackEnd( const boost::property_tree::ptree& pt, Track::End& trackend );

		dclspc void ReadTrackLocation( const boost::property_tree::ptree& pt, TrackLocation& trackLocation ) noexcept;
		dclspc void ReadTrackLocationRef( const boost::property_tree::ptree& pt, TrackLocationRef& trackLocationRef ) noexcept;
		dclspc void ReadTrackRange( const boost::property_tree::ptree& pt, TrackRange& trackRange ) noexcept;

		dclspc void Read( const boost::property_tree::ptree& pt, Cubic::Data& curve );
		dclspc void Read( const boost::property_tree::ptree& pt, CurveSample& sample );
		dclspc void Read( const boost::property_tree::ptree& pt, EEPCurve::Data& data );
		///@}

		template<class Interface>
		inline void WriteLocalizedTag( 
			boost::property_tree::ptree& pt, 
			const std::string& tagName, 
			const Interface& refInterface )
		{
			auto localizedNames = refInterface.ReferenceNames( tagName + "_" );
			for( const auto& localizedName : localizedNames ){
				std::string localizedString{ localizedName };
				boost::property_tree::ptree ptLocalizedTag;
				std::string languageName = localizedString.substr( tagName.size() + 1 );
				if( languageName != "unknown" )
					ptLocalizedTag.add( "<xmlattr>.language", localizedString.substr( tagName.size() + 1 ) );

				ptLocalizedTag.put( "", refInterface.Reference(localizedString) );
				pt.add_child( tagName, ptLocalizedTag );
			}
		}

		template<class Interface>
		inline void ReadLocalizedTag( 
			const boost::property_tree::ptree& pt, 
			const char* tagName, 
			Interface& refInterface, 
			const char* localeString )
		{
			auto optional = pt.get_optional<std::string>( "<xmlattr>.language" );
			if( optional ){
				refInterface.Reference( std::string{ tagName } + "_" + *optional, pt.data() );

				if( refInterface.Reference( tagName ).empty() || localeString == *optional )
					refInterface.Reference( tagName, pt.data() );
			}
			else{
				refInterface.Reference( std::string{ tagName } + "_unknown", pt.data() );

				if( refInterface.Reference( tagName ).empty() )
					refInterface.Reference( tagName, pt.data() );
			}
		}

		template<class T>
		inline void ReferencesToAttributes( boost::property_tree::ptree& pt, const Identified<T>& refInterface ){
			const std::vector<char const*>& names = refInterface.ReferenceNames();
			for( char const* pName : names )
			{
				std::string name{ pName };
				pt.add( "<xmlattr>." + name, refInterface.Reference( name ) );
			}
		}

		template<class T>
		inline void AttributesToReferences( const boost::property_tree::ptree& pt, Identified<T>& refInterface ){
			const auto iter = pt.find( "<xmlattr>" );
			if( iter != pt.not_found() )
			{
				for( const auto& attribute : iter->second ){
					refInterface.Reference( attribute.first, attribute.second.data() );
				}
			}
		}

		/// \brief Move the node as child to another tree.
		///
		/// This will prevent the copying of the whole child node, but 
		/// leave the original child emty.
		inline void move_child( 
			boost::property_tree::ptree& toParent, 
			const boost::property_tree::ptree::path_type& path, 
			boost::property_tree::ptree& value )
		{
			toParent.add_child( path, boost::property_tree::ptree{} ).swap( value );
		}

		inline boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const spat::Vector<Velocity>& v ){
			boost::property_tree::ptree ptVector;
			ptVector.add( "<xmlattr>.dx", v.dx );
			ptVector.add( "<xmlattr>.dy", v.dy );
			ptVector.add( "<xmlattr>.dz", v.dz );
			opt.add_child( "Velocity", ptVector );
			return opt;
		}

		inline boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const spat::Vector<AngularVelocity>& v ){
			boost::property_tree::ptree ptVector;
			ptVector.add( "<xmlattr>.dx", v.dx );
			ptVector.add( "<xmlattr>.dy", v.dy );
			ptVector.add( "<xmlattr>.dz", v.dz );
			opt.add_child( "AngularVelocity", ptVector );
			return opt;
		}
	
	} // namespace ptreesupport
} // namespace trax