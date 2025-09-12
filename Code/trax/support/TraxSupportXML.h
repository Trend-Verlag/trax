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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "common/support/CommonSupportXML.h"
#include "dim/support/DimSupportXML.h"
#include "spat/support/SpatSupportXML.h"

#include "trax/Track.h"

namespace trax{

	struct Material;
	struct TrackJointLimits;
	struct TrackLocation;
	struct TrackLocationRef;
	struct TrackRange;
	struct TractionForceCharacteristic;
	struct Wheelset;

	namespace ptreesupport{

		using spat::ptreesupport::operator<<;

		/// \name Explicit XML Reading
		///@{
		dclspc void ReadConnection( const boost::property_tree::ptree& pt, Track::End& trackend ) noexcept;
	//	dclspc void ReadMaterial( const boost::property_tree::ptree& pt, Material& material ) noexcept;
		dclspc void ReadTrackEnd( const boost::property_tree::ptree& pt, Track::End& trackend ) noexcept;
	//	dclspc void ReadTrackJointLimits( const boost::property_tree::ptree& pt, TrackJointLimits& wfl ) noexcept;
		dclspc void ReadTrackLocation( const boost::property_tree::ptree& pt, TrackLocation& trackLocation ) noexcept;
		dclspc void ReadTrackLocationRef( const boost::property_tree::ptree& pt, TrackLocationRef& trackLocationRef ) noexcept;
		dclspc void ReadTrackRange( const boost::property_tree::ptree& pt, TrackRange& trackRange ) noexcept;
		dclspc void ReadWheelset( const boost::property_tree::ptree& pt, Wheelset& wheelset ) noexcept;
	//	dclspc void ReadTractionForceCharacteristic( const boost::property_tree::ptree& pt, TractionForceCharacteristic& tractionForceCharacteristic ) noexcept;
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
	
	} // namespace ptreesupport
} // namespace trax