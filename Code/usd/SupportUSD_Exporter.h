// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "SupportUSD.h"
#include "SupportUSD_Tokens.h"

#include "dim/support/DimSupportStream.h"

#include "spat/VectorBundle.h"
#include "spat/VectorBundle2.h"
#include "spat/Frame.h"

#include <stack>

namespace trax{

	/// \brief Trax writer for USD prims.
	///
	/// The USDExporter writes trax objects to an USD stage.
	/// Do not combine the USDExporter with the USDReader in 
	/// a read/write cycle, since the values might migrate over
	/// many cycles; use the XML read/write support for that purpose.
	class USDExporter{
	public:
		USDExporter( pxr::UsdStageRefPtr stage, const pxr::SdfPath& atPath, bool bWriteTrackMesh = false );
		~USDExporter();

		/// \brief Prepare the stage for writing the track system.
		/// \param tracksystem TrackSystem to prepare.
		/// 
		/// The method writes the USDPath property to the objects 
		/// references, so they can be used to establish connections 
		/// in the USD file. If the TrackSystem was read by USDReader 
		/// there is no need to call the method prior to writing.
		void Prepare( TrackSystem& tracksystem );


		/// \brief Write the track system to the stage.
		void Write( const TrackSystem& tracksystem );


        inline pxr::UsdStageRefPtr GetStage() const noexcept{ 
            return m_Stage;
        }
	private:
		void Prepare( TrackCollection& collection );
		void Prepare( ConnectorCollection& collection );
		void Prepare( TrackBuilder& track );
		void Prepare( Connector& connector );

		void Write( const TrackCollection& collection );
		void Write( const ConnectorCollection& collection );
		void Write( const TrackBuilder& track );
		void Write( const Connector& connector );
		void Write( const Switch& connector );
		void Write( const ThreeWaySwitch& connector );
		void Write( const SingleSlipSwitch& connector );
		void Write( const DoubleSlipSwitch& connector );
		void Write( const Curve& curve, const std::string& pathName = "Curve" );
		void Write( const Line& line, const std::string& pathName = "Curve" );
		void Write( const LineP& line, const std::string& pathName = "Curve" );
		void Write( const Arc& arc, const std::string& pathName = "Curve" );
		void Write( const ArcP& arcP, const std::string& pathName = "Curve" );
		void Write( const Helix& helix, const std::string& pathName = "Curve" );
		void Write( const HelixP& helixP, const std::string& pathName = "Curve" );
		void Write( const Cubic& cubic, const std::string& pathName = "Curve" );
		void Write( const Spline& spline, const std::string& pathName = "Curve" );
		void Write( const Clothoid&, const std::string& pathName = "Curve"  );
		void Write( const Rotator&, const std::string& pathName = "Curve"  );
		void Write( const RotatorChain&, const std::string& pathName = "Curve"  );
		void Write( const PolygonalChain&, const std::string& pathName = "Curve"  );
		void Write( const SampledCurve& curve, const std::string& pathName = "Curve" );
		void Write( const RoadwayTwist& twist, const std::string& pathName = "Twist" );
		void Write( const ConstantTwist& twist, const std::string& pathName = "Twist" );
		void Write( const LinearTwist& twist, const std::string& pathName = "Twist" );
		void Write( const PiecewiseTwist& twist, const std::string& pathName = "Twist" );
		void Write( const DirectionalTwist& twist, const std::string& pathName = "Twist" );
		void Write( const PiecewiseDirectionalTwist& twist, const std::string& pathName = "Twist" );
		void Write( const CombinedTwist& twist, const std::string& pathName = "Twist" );
		void Write( const Section& section );

		pxr::UsdStageRefPtr m_Stage;
		std::stack<pxr::SdfPath> m_PathStack;
		bool m_bWriteTrackMesh;

		void Write( pxr::UsdPrim& toPrim, const spat::Frame<dim::Length,dim::One>& frame );

		template<typename D>
		void Write( const common::Interval<D>& interval, const pxr::TfToken& typeName = UsdTraxTokens->Interval, const std::string& pathName = "Interval" ){
			m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
			if( pxr::UsdPrim primInterval = m_Stage->DefinePrim( m_PathStack.top(), typeName ); primInterval )
			{
				CreateAttribute( primInterval, UsdTraxTokens->Near, interval.Near() );
				CreateAttribute( primInterval, UsdTraxTokens->Far, interval.Far() );
			}
			m_PathStack.pop();
		}

		template<typename D>
		void Write( const spat::Position<D>& pos, const pxr::TfToken& typeName, const std::string& pathName )
		{
			m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
			if( pxr::UsdPrim prim = m_Stage->DefinePrim( m_PathStack.top(), typeName ); prim )
			{
				CreateAttribute( prim, UsdTraxTokens->X, pos.x );
				CreateAttribute( prim, UsdTraxTokens->Y, pos.y );
				CreateAttribute( prim, UsdTraxTokens->Z, pos.z );
			}
			m_PathStack.pop();
		}

		template<typename D>
		void Write( const spat::Vector<D>& T, const pxr::TfToken& typeName, const std::string& pathName )
		{
			m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
			if( pxr::UsdPrim prim = m_Stage->DefinePrim( m_PathStack.top(), typeName ); prim )
			{
				CreateAttribute( prim, UsdTraxTokens->Dx, T.dx );
				CreateAttribute( prim, UsdTraxTokens->Dy, T.dy );
				CreateAttribute( prim, UsdTraxTokens->Dz, T.dz );
			}
			m_PathStack.pop();
		}

		void Write( const spat::VectorBundle<dim::Length,dim::One>& bundle, const pxr::TfToken& typeName, const std::string& pathName );		
		void Write( const spat::VectorBundle2<dim::Length,dim::One>& bundle2, const pxr::TfToken& typeName, const std::string& pathName );
		void Write( const spat::Frame<dim::Length,dim::One>& frame, const pxr::TfToken& typeName, const std::string& pathName );
		void Write( const CurveSample& curveSample, const pxr::TfToken& typeName, const std::string& pathName );
		void Write( const CubicData& cubicData, const pxr::TfToken& typeName, const std::string& pathName );

		template<typename D> inline
		void CreateAttribute( pxr::UsdPrim& toPrim, const pxr::TfToken& typeName, D d ){		
			if( pxr::UsdAttribute attribute = toPrim.CreateAttribute( typeName, pxr::SdfValueTypeNames->String ); attribute )
				attribute.Set( to_string( d ) );
		}
	
        template<class TraxType>
        static pxr::SdfPath Name( const TraxType& object, const char* pDefault )
		{
	        if( std::string Name = object.Reference( "Name" ); Name.empty() )
		        return pxr::SdfPath{ pDefault + std::to_string( object.ID() ) };
	        else
				return pxr::SdfPath{ pxr::SdfPath{ Name }.GetName() };
		}
	};

}
