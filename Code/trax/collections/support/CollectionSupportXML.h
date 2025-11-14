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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "common/support/CommonSupportConsole.h"
#include "trax/support/TraxSupportXML.h"
#include "trax/Curve.h"

#include <boost/property_tree/ptree.hpp>


/// \verbatim
///				ptree::front()									ptree::end()	
///					|													|
///					v													v
///				 ptree
///				/	  \						\				|			
///			key			ptree				 ptree			...			
///						/	 \				/	  |
///					key			ptree	   key     ...
///								/	 |
///							key			...
/// \endverbatim
namespace trax{

	struct CurveSample;
	struct Indicator;
	struct RoadwayTwist;
	struct SignalCollection;
	struct SocketRegistry;
	struct TrackSystem;

	namespace ptreesupport{

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

		dclspc void RegisterAllCurveReaders( class PTreeReader& toReader );

		dclspc std::unique_ptr<RoadwayTwist> CreateConstantTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateLinearTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseLinearTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseCircularTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateDirectionalTwist( const boost::property_tree::ptree& pt );

		dclspc void RegisterAllTwistReaders( class PTreeReader& toReader );

		///@}


		/// \brief Base class for XML readers based on ptree.
		///
		/// If curves or twists are not needed, they don't have to be registered with the reader; this
		/// prevents unnecessary coupling of all the elements the library provides and offers a method
		/// to extend the system with custom curves and twists.
		/// 
		/// Albeit this might seem to be better implemented in traxTrack, the reading can not be done
		/// for a single track. Plugs/Jacks and the track coupling as well as shared curves relate
		/// to whole systems of tracks. Hence we have it in traxTrackCollections.
		class PTreeReader{
		public:
			dclspc PTreeReader( const char* pLocale = nullptr );
			dclspc PTreeReader( SocketRegistry& socketRegistry, const char* pLocale = nullptr );
			PTreeReader( const PTreeReader& ) = delete;
			dclspc PTreeReader( PTreeReader&& ) noexcept;
			dclspc virtual ~PTreeReader();

			PTreeReader& operator=( const PTreeReader& ) = delete;
			PTreeReader& operator=( PTreeReader&& ) noexcept = delete;

			using CurveReaderFunction = std::unique_ptr<Curve> (*)( const boost::property_tree::ptree& pt );

			void dclspc RegisterCurveReader( const std::string& typeName, CurveReaderFunction readerFunc );

			void dclspc UnregisterCurveReader( const std::string& typeName );


			using TwistReaderFunction = std::unique_ptr<RoadwayTwist> (*)( const boost::property_tree::ptree& pt );

			void dclspc RegisterTwistReader( const std::string& typeName, TwistReaderFunction readerFunc );

			void dclspc UnregisterTwistReader( const std::string& typeName );

			inline const char* GetLocale() const{
				return m_pLocale;
			}

			virtual std::shared_ptr<trax::TrackSystem> ReadTrackSystem( const boost::property_tree::ptree& pt ) const = 0;
		protected:
			std::unique_ptr<Curve> CreateCurve( const boost::property_tree::ptree& pt ) const;

			std::unique_ptr<RoadwayTwist> CreateTwist( const boost::property_tree::ptree& pt ) const;

		private:
			std::unique_ptr<SocketRegistry> m_pSocketRegistry;
			class PTreeReaderBaseImpl* m_pImpl = nullptr;
			const char* m_pLocale = nullptr;

			std::unique_ptr<RoadwayTwist> CreateCombinedTwist( const boost::property_tree::ptree& pt ) const;
		
		protected:
			SocketRegistry& m_SocketRegistry;
		};


		template<class Interface>
		inline void AttributesToReferences( const boost::property_tree::ptree& pt, Interface& refInterface ){
			const auto iter = pt.find( "<xmlattr>" );
			if( iter != pt.not_found() )
				for( const auto& attribute : iter->second )
					refInterface.Reference( attribute.first, attribute.second.data() );
		}


		/// \name Explicit XML Reading
		///@{
		dclspc void Read( const boost::property_tree::ptree& pt, Cubic::Data& curve );
		dclspc void Read( const boost::property_tree::ptree& pt, CurveSample& sample );
		dclspc void Read( const boost::property_tree::ptree& pt, EEPCurve::Data& data );
		///@}

	} // namespace ptreesupport
} // namespace trax