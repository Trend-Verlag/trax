// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "SupportUSD.h"

#include "common/Interval.h"
#include "spat/Frame.h"
#include "trax/Track.h"

#include <filesystem>

namespace trax{

    /// \brief Trax reader for a USD stage.
    ///
    /// The USD reader reads trax objects from an USD stage.
    class USDReader{
    public:
        USDReader( pxr::UsdStageRefPtr pStage );

        // todo: return Module ...
        std::vector<std::shared_ptr<TrackSystem>> Read();


        /// \brief Called to reads a track system from and USD definition.
        /// \returns Pointer to a newly created track system.
        std::shared_ptr<TrackSystem> ReadTrackSystem( const pxr::UsdPrim& trackSystem );

        inline pxr::UsdStageRefPtr GetStage() const noexcept{ 
            return m_pStage;
        }

    protected:
        virtual std::shared_ptr<TrackBuilder> ReadTrack( const pxr::UsdPrim& track, double metersPerUnit );
        virtual TrackBuilder::TrackType ReadTrackType( const pxr::UsdPrim& track ) const noexcept;
        virtual std::shared_ptr<TrackBuilder> MakeTrack( TrackBuilder::TrackType type ) const noexcept;
        
        std::shared_ptr<TrackSystem> m_pTrackSystem;
    private:
        void ReadTrackCollection( const pxr::UsdPrim& trackCollection, double metersPerUnit );

        static common::Interval<dim::Length> ReadInterval( const pxr::UsdPrim& interval );
        static spat::Position<dim::Length> ReadPosition( const pxr::UsdPrim& position );
        static spat::Vector<dim::One> ReadVector( const pxr::UsdPrim& vector );
        static spat::Vector<dim::Length> ReadDistance( const pxr::UsdPrim& distance );
        static spat::VectorBundle<dim::Length,dim::One> ReadVectorBundle( const pxr::UsdPrim& vectorBundle );
        static spat::VectorBundle2<dim::Length,dim::One> ReadVectorBundle2( const pxr::UsdPrim& vectorBundle2 );
        static spat::VectorBundle3<dim::Length,dim::One> ReadVectorBundle3( const pxr::UsdPrim& vectorBundle3 );

		static CurveSample ReadCurveSample( const pxr::UsdPrim& curveSample );
		static CubicData ReadCubicData( const pxr::UsdPrim& cubicData );
		static std::tuple<dim::Angle,dim::Angle,dim::Length> ReadRotatorChainData( const pxr::UsdPrim& data );
		static std::pair<dim::Length,dim::Angle> ReadLinearTwistData( const pxr::UsdPrim& data );
		static const std::pair<dim::Length,spat::Vector<dim::One>> ReadDirectionalTwistData( const pxr::UsdPrim& data );

        static std::unique_ptr<Curve> ReadCurve( const pxr::UsdPrim& curve );
        static std::unique_ptr<Line> ReadLine( const pxr::UsdPrim& line );
        static std::unique_ptr<LineP> ReadLineP( const pxr::UsdPrim& lineP );
        static std::unique_ptr<Arc> ReadArc( const pxr::UsdPrim& arc );
        static std::unique_ptr<ArcP> ReadArcP( const pxr::UsdPrim& arcP );
        static std::unique_ptr<Helix> ReadHelix( const pxr::UsdPrim& helix );
        static std::unique_ptr<HelixP> ReadHelixP( const pxr::UsdPrim& helixP );
        static std::unique_ptr<Cubic> ReadCubic( const pxr::UsdPrim& cubic );
        static std::unique_ptr<Spline> ReadSpline( const pxr::UsdPrim& spline );
        static std::unique_ptr<Clothoid> ReadClothoid( const pxr::UsdPrim& clothoid );
        static std::unique_ptr<Rotator> ReadRotator( const pxr::UsdPrim& rotator );
        static std::unique_ptr<RotatorChain> ReadRotatorChain( const pxr::UsdPrim& rotatorChain );
        static std::unique_ptr<PolygonalChain> ReadPolygonalChain( const pxr::UsdPrim& polygonalChain );
        static std::unique_ptr<SampledCurve> ReadSampledCurve( const pxr::UsdPrim& sampledCurve );

        static std::pair<std::unique_ptr<Curve>,std::unique_ptr<RoadwayTwist>> ReadBasisCurve( const pxr::UsdPrim& basisCurve, bool bLocalTransformation );
        static std::unique_ptr<DirectionalTwist> ReadUpTwist( const pxr::UsdStage& stage );

        static std::unique_ptr<RoadwayTwist> ReadTwist( const pxr::UsdPrim& twist );
        static std::unique_ptr<ConstantTwist> ReadConstantTwist( const pxr::UsdPrim& constantTwist );
        static std::unique_ptr<LinearTwist> ReadLinearTwist( const pxr::UsdPrim& linearTwist );
        static std::unique_ptr<PiecewiseTwist> ReadPiecewiseTwist( const pxr::UsdPrim& piecewiseTwist );
        static std::unique_ptr<DirectionalTwist> ReadDirectionalTwist( const pxr::UsdPrim& directionalTwist );
        static std::unique_ptr<PiecewiseDirectionalTwist> ReadPiecewiseDirectionalTwist( const pxr::UsdPrim& piecewiseDirectionalTwist );
        static std::unique_ptr<CombinedTwist> ReadCombinedTwist( const pxr::UsdPrim& combinedTwist );

        static std::unique_ptr<Section> ReadSection( const pxr::UsdPrim& section );

        void ReadSwitches( pxr::UsdPrim scopePrim );
        std::unique_ptr<Connector> ReadTwoWaySwitch( pxr::UsdPrim switchPrim );
        std::unique_ptr<Connector> ReadThreeWaySwitch( pxr::UsdPrim switchPrim );
        std::unique_ptr<Connector> ReadSingleSlipSwitch( pxr::UsdPrim switchPrim );
        std::unique_ptr<Connector> ReadDoubleSlipSwitch( pxr::UsdPrim switchPrim );


		pxr::UsdStageRefPtr m_pStage;
        std::map<pxr::SdfPath,std::pair<std::shared_ptr<Curve>,std::unique_ptr<RoadwayTwist>>> m_Curves;

    public:
        static spat::Frame<dim::Length,dim::One> ReadTransformationNormalized( const pxr::UsdPrim& prim, double metersPerUnit );
        static spat::Frame<dim::Length,dim::One> ReadTransformation( const pxr::UsdPrim& prim, double metersPerUnit );
        static spat::Frame<dim::Length,dim::One> ReadTransformation( const pxr::UsdAttribute& xformOpOrderAttribute, double metersPerUnit );
        static spat::Frame<dim::Length,dim::One> ReadChildFrame( const pxr::UsdPrim& prim );
        static spat::Frame<dim::Length,dim::One> ReadFrame( const pxr::UsdAttribute& opAttribute, double metersPerUnit );
    private:
        static spat::Frame<dim::Length,dim::One> ComputeLocalToWorldTransform( pxr::UsdPrim forPrim );

        static void Connect( trax::TrackSystem& trackSystem );

        static const dim::Length scm_EpsilonTrackGap;
        static const dim::Angle scm_EpsilonTrackKink;
    };

}