// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

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
		std::unique_ptr<Curve> dclspc CreateLine( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateLineP( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateArc( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateArcP( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateHelix( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateHelixP( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateCubic( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateSpline( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateClothoid( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateRotator( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateRotatorChain( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreatePolygonalChain( const boost::property_tree::ptree& pt );
		std::unique_ptr<Curve> dclspc CreateSampledCurve( const boost::property_tree::ptree& pt );
#ifdef TRAX_SUPPORT_EEPCURVE
		std::unique_ptr<Curve> dclspc CreateEEPCurve( const boost::property_tree::ptree& pt );
#endif

		void dclspc RegisterAllCurveReaders( class PTreeReader& toReader );

		std::unique_ptr<RoadwayTwist> dclspc CreateConstantTwist( const boost::property_tree::ptree& pt );
		std::unique_ptr<RoadwayTwist> dclspc CreateLinearTwist( const boost::property_tree::ptree& pt );
		std::unique_ptr<RoadwayTwist> dclspc CreatePiecewiseTwist( const boost::property_tree::ptree& pt );
		std::unique_ptr<RoadwayTwist> dclspc CreatePiecewiseLinearTwist( const boost::property_tree::ptree& pt );
		std::unique_ptr<RoadwayTwist> dclspc CreatePiecewiseCircularTwist( const boost::property_tree::ptree& pt );
		std::unique_ptr<RoadwayTwist> dclspc CreateDirectionalTwist( const boost::property_tree::ptree& pt );

		void dclspc RegisterAllTwistReaders( class PTreeReader& toReader );

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
		void dclspc Read( const boost::property_tree::ptree& pt, Cubic::Data& curve );
		void dclspc Read( const boost::property_tree::ptree& pt, CurveSample& sample );
		void dclspc Read( const boost::property_tree::ptree& pt, EEPCurve::Data& data );
		///@}

	} // namespace ptreesupport
} // namespace trax