// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "usd/SupportUSD_Reader.h"

#include "usd/SupportUSD_Tokens.h"
#include "usd/SupportUSD_Spat.h"

#include "trax/collections/TrackSystem.h"
#include "trax/collections/TrackCollection.h"
#include "trax/collections/TrackCollectionContainer.h"
#include "trax/collections/ConnectorCollection.h"
#include "trax/collections/MovableTrackAutoConnecting.h"
#include "trax/Curve.h"
#include "trax/GeomTrack.h"
#include "trax/Section.h"
#include "trax/Switch.h"

#include "common/support/CommonSupportConsole.h"
#include "dim/support/DimSupportStream.h"


namespace trax{
	using namespace common;
	using namespace dim;
	using namespace spat;


const Length USDReader::scm_EpsilonTrackGap = 0.1_m;
const Angle USDReader::scm_EpsilonTrackKink = 5_deg;


template<typename D>
D ReadAttribute( const pxr::UsdPrim& prim, const pxr::TfToken& attrName, D defaultValue = D{0} )
{		
	if( pxr::UsdAttribute attr = prim.GetAttribute( attrName ); attr ){
		std::string value;
		attr.Get( &value );
		return dim::sto<D>( value );
	}

    return defaultValue;
}


USDReader::USDReader( pxr::UsdStageRefPtr pStage )
    : m_pStage{ pStage }
{
}

std::vector<std::shared_ptr<TrackSystem>> USDReader::Read(){
    m_Curves.clear();

	if( m_pStage )
    {
        if( pxr::UsdPrim rootPrim = m_pStage->GetPseudoRoot(); rootPrim )
        {
            std::vector<std::shared_ptr<TrackSystem>> trackSystems;

            pxr::UsdPrim::SubtreeRange descendants = rootPrim.GetDescendants();
            for( const auto& descendant : descendants )
            {
                if( descendant.GetTypeName() == UsdTraxTokens->TrackSystem )
                    trackSystems.push_back( ReadTrackSystem( descendant.GetPrim() ) );

                if( descendant.GetTypeName() == UsdTraxTokens->Scope ) // deprecated format.
                    if( descendant.GetName() == "TrackSystem" )
                        trackSystems.push_back( ReadTrackSystem( descendant.GetPrim() ) );
           }

           return trackSystems;
        }
    }

    return {};
}

std::shared_ptr<TrackSystem> USDReader::ReadTrackSystem( const pxr::UsdPrim& trackSystem )
{
	if( trackSystem )
    {
        if( trackSystem.GetTypeName() != UsdTraxTokens->TrackSystem )
        {
			std::cerr << TextColor::yellow
				<< "trax::USDReader: " << trackSystem.GetPath().GetString() << ". Not a valid "
				<< UsdTraxTokens->TrackSystem << " prim. Tracks might not get read correctly. Please "
                << "think about updating older file versions." << rendl;
		}

        if( m_pTrackSystem = TrackSystem::Make() )
	    {
            m_pTrackSystem->Reference( "Name", trackSystem.GetPath().GetName() );
            m_pTrackSystem->Reference( "USDPath", trackSystem.GetPath().GetString() );

            double metersPerUnit = pxr::UsdGeomGetStageMetersPerUnit( GetStage() );
	        if( pxr::UsdAttribute attr = trackSystem.GetAttribute( pxr::UsdGeomTokens->metersPerUnit ); attr )
		        attr.Get( &metersPerUnit );

            bool bDefaultCollection = false;

            pxr::UsdPrim::SiblingRange siblings = trackSystem.GetChildren();
            for( const auto& sibling : siblings )
            {
                if( sibling.GetTypeName() == UsdTraxTokens->Track )
                // tracks outside of collections are added to the default collection.
                {
                    if( !bDefaultCollection )
				    {
					    IDType id = m_pTrackSystem->SetActiveCollection( m_pTrackSystem->CreateCollection() );
                        m_pTrackSystem->GetCollectionContainer()->Get( id )->Reference( 
                            "USDPath", 
                            trackSystem.GetPath().AppendPath( pxr::SdfPath{ "TrackCollection" + to_string( id ) } ).GetString() );
                        m_pTrackSystem->GetCollectionContainer()->Get( id )->Reference( 
                            "Name", 
                            "TrackCollection" + to_string( id ) );

					    bDefaultCollection = true;
				    }

                    m_pTrackSystem->Add( ReadTrack( sibling.GetPrim(), metersPerUnit ) );
                }

                else if( sibling.GetTypeName() == UsdTraxTokens->TrackCollection )
                    ReadTrackCollection( sibling.GetPrim(), metersPerUnit );
            }

            Connect( *m_pTrackSystem );

            ReadSwitches( trackSystem );

		    return m_pTrackSystem;
	    }
    }
    else
		std::cerr << TextColor::red 
        << "trax::USDReader: couldn't load track system: " 
        << trackSystem.GetPath().GetString() << ". Not a valid " 
        << UsdTraxTokens->TrackSystem << " prim." << rendl;

    return nullptr;
}

void USDReader::ReadTrackCollection( const pxr::UsdPrim& trackCollection, double metersPerUnit )
{
    if( m_pTrackSystem )
    {
        const IDType baseID = m_pTrackSystem->GetActiveCollection();
 
        m_pTrackSystem->SetCollectionFrame( 
            m_pTrackSystem->SetActiveCollection( m_pTrackSystem->CreateCollection() ), 
            ReadTransformationNormalized( trackCollection, metersPerUnit ) );
        m_pTrackSystem->GetCollectionContainer()->GetActive()->Reference( "Name", trackCollection.GetPath().GetName() );
        m_pTrackSystem->GetCollectionContainer()->GetActive()->Reference( "USDPath", trackCollection.GetPath().GetString() );

        pxr::UsdPrim::SiblingRange siblings = trackCollection.GetChildren();
        for( const auto& sibling : siblings )
        {
            if( sibling.GetTypeName() == UsdTraxTokens->Track )
                m_pTrackSystem->Add( ReadTrack( sibling.GetPrim(), metersPerUnit ) );
        }

        m_pTrackSystem->SetActiveCollection( baseID );
    }
}

static bool ReadSlot( 
    pxr::UsdPrim prim, 
    const pxr::TfToken& ConnectionToken, 
    const pxr::TfToken& ConnectionEnd, 
    std::function<bool(const pxr::SdfPath& connectionPath, const pxr::TfToken& connectionEnd)> callback )
{
    if( pxr::UsdRelationship connection = prim.GetRelationship( ConnectionToken ); connection )
    {
        pxr::SdfPathVector targets;
        connection.GetTargets( &targets );
        if( !targets.empty() )
        {
            if( pxr::UsdAttribute connectionEndTypeAttribute = prim.GetAttribute( ConnectionEnd ); connectionEndTypeAttribute )
            {
                pxr::TfToken connectionEndType;
                connectionEndTypeAttribute.Get( &connectionEndType );
                if( connectionEndType == UsdTraxTokens->FrontEnd || connectionEndType == UsdTraxTokens->EndEnd )
                {
                    return callback( targets.front(), connectionEndType );
                }
                else
                    std::cerr   << TextColor::red
                                << "trax::USDReader: a track connection from track:" << prim.GetPath() << "," << ConnectionToken
                                << " to track: " << targets.front() 
                                << " was found, but no valid connectionEndType. connectionEndType given is: " 
                                << connectionEndType << rendl;
            }
        }
    }

    return false;
}

static bool ReadSlot( 
    Track& track,
    pxr::UsdPrim prim, 
    const pxr::TfToken& ConnectionToken, 
    const pxr::TfToken& ConnectionEnd )
{
    return ReadSlot( prim, ConnectionToken, ConnectionEnd, [&track,&ConnectionToken,&ConnectionEnd]( const pxr::SdfPath& connectionPath, const pxr::TfToken& connectionEnd ) -> bool
    { 
        track.Reference( ConnectionToken.GetString(), connectionPath.GetString() );
        track.Reference( ConnectionEnd.GetString(), connectionEnd.GetString() );
        return true;
    } );
}

static bool ReadSlot(
    Connector& connector,
    int slotID, 
    const pxr::TfToken& ConnectionToken, 
    const pxr::TfToken& ConnectionEnd, 
    pxr::UsdPrim connectorPrim,
    const TrackSystem& trackSystem,
    bool bFlipEnd = false )
{
    if( !ReadSlot( connectorPrim, ConnectionToken, ConnectionEnd, 
        [&connector,slotID,&trackSystem,bFlipEnd]( const pxr::SdfPath& connectionPath, const pxr::TfToken& connectionEnd )
    {
        auto track = std::find_if( trackSystem.cbegin(), trackSystem.cend(), 
                [&connectionPath]( const TrackBuilder& otherTrack ){ return connectionPath.GetString() == otherTrack.Reference( "USDPath" ); } );
        if( track != trackSystem.end() ){
            connector.Slot( slotID, track->This(), bFlipEnd ? !ToEndType( connectionEnd ) : ToEndType( connectionEnd ) );
            return true;
        }
        std::cerr << TextColor::red << "trax::USDReader: connection path not found: " << connectionPath.GetString() << rendl;
        return false;
    } ) )
    {
        std::cerr   << TextColor::red << "trax::USDReader: couldn't populate switch slot. Switch: " 
                    << connectorPrim.GetPath() << ". Reason: Track was not found for: " 
                    << ConnectionToken << rendl;
        return false;
    }

    return true;
}

std::shared_ptr<TrackBuilder> USDReader::ReadTrack( const pxr::UsdPrim& track, double metersPerUnit ) 
{
    if( track )
    {
        if( std::shared_ptr<TrackBuilder> pTrack = MakeTrack( ReadTrackType( track ) ); pTrack )
	    {
            // identification:
	        if( pxr::UsdAttribute attr = track.GetAttribute( UsdTraxTokens->Id ); attr ){
                int id = 0;
		        attr.Get( &id );
                pTrack->ID( id );
	        }

            pTrack->Reference( "Name", track.GetPath().GetName() );
            pTrack->Reference( "USDPath", track.GetPath().GetString() );


            // frame:
            pTrack->SetFrame( ReadTransformationNormalized( track, metersPerUnit ) );


            // curve and twist:
            std::pair<std::shared_ptr<const Curve>,Interval<Length>> curve{ nullptr, ReadInterval( track ) };
            pxr::UsdPrim::SiblingRange siblings = track.GetChildren();
            for( const auto& sibling : siblings )
            {
                if( sibling.GetTypeName() == UsdTraxTokens->BasisCurves ){
	                std::pair<std::unique_ptr<Curve>,std::unique_ptr<RoadwayTwist>> pair = ReadBasisCurve( sibling.GetPrim(), true );
                    curve.first = std::move(pair.first);
                    pTrack->Attach( std::move(pair.second) );
                }

                else if( std::unique_ptr<Curve> pCurve = ReadCurve( sibling.GetPrim() ); pCurve )
				    curve.first = std::move(pCurve);

			    else if( sibling.GetTypeName() == UsdTraxTokens->Interval )
                    curve.second = ReadInterval( sibling.GetPrim() );

                else if( std::unique_ptr<RoadwayTwist> pTwist = ReadTwist( sibling.GetPrim() ); pTwist )
                    pTrack->Attach( std::move(pTwist) );

			    else if( sibling.GetTypeName() == UsdTraxTokens->Section ){
                    if( std::shared_ptr<GeomTrack> pGeomTrack = std::dynamic_pointer_cast<GeomTrack>(pTrack); pGeomTrack )
					    pGeomTrack->Attach( ReadSection( sibling.GetPrim() ) );
                }
            }
     
            if( pxr::UsdRelationship rel = track.GetRelationship( UsdTraxTokens->Curve ); rel )
            // curve reference has priority over curve child
            {
                pxr::SdfPathVector targets;
                rel.GetTargets( &targets );
                if( !targets.empty() )
                {        
                    if( m_Curves.find( targets.front() ) == m_Curves.end() ){
                        pxr::UsdPrim prim = track.GetPrimAtPath( targets.front() );

                        if( prim.GetTypeName() == UsdTraxTokens->BasisCurves )
	                        m_Curves[prim.GetPath()] = ReadBasisCurve( prim, false );
                        else
                            m_Curves[prim.GetPath()].first = ReadCurve( prim );    
                    }

                    if( m_Curves[targets.front()].first == nullptr )
                        m_Curves.erase( targets.front() );
                    else
                    {
                        curve.first = m_Curves[targets.front()].first;
                        if( m_Curves[targets.front()].second )
						    pTrack->Attach( m_Curves[targets.front()].second->Clone() );
                    }
                }
            }

            if( curve.first )
			    pTrack->Attach( curve );


            // connectors:
            ReadSlot( *pTrack, track, UsdTraxTokens->FrontConnection, UsdTraxTokens->FrontConnectionEnd );
            ReadSlot( *pTrack, track, UsdTraxTokens->EndConnection, UsdTraxTokens->EndConnectionEnd );

            if( pTrack->IsValid() )
                return pTrack;

            std::cerr << TextColor::red << "trax::USDReader: no curve or curve reference for track!" << rendl;
	    }
    }

    std::cerr << TextColor::red << "trax::USDReader: couldn't load track: " << track.GetPath().GetString() << rendl;
    return nullptr;
}

TrackBuilder::TrackType USDReader::ReadTrackType( const pxr::UsdPrim& track ) const noexcept
{
    Track::TrackType type = Track::TrackType::standard;
	if( pxr::UsdAttribute attr = track.GetAttribute( UsdTraxTokens->Type ); attr ){
		std::string value;
		attr.Get( &value );
		type = TrackType( value );
	}
    else if( pxr::UsdRelationship rel = track.GetRelationship( UsdTraxTokens->TrackBody ); rel )
    {
        pxr::SdfPathVector targets;
        rel.GetTargets( &targets );
        if( !targets.empty() )
        {
            type = Track::TrackType::movable_autoconnecting;
        }
    }

    return type;
}

std::shared_ptr<TrackBuilder> USDReader::MakeTrack( TrackBuilder::TrackType type ) const noexcept
{
    if( type == Track::TrackType::movable_autoconnecting )
        return MovableTrackAutoConnecting::Make();
    else
        return TrackBuilder::Make( type );
}

Interval<Length> USDReader::ReadInterval( const pxr::UsdPrim& interval )
{
    if( interval )
    {
        return { ReadAttribute( interval, UsdTraxTokens->Near, ReadAttribute( interval, UsdTraxTokens->IntervalNear, 0_m ) ),
                 ReadAttribute( interval, UsdTraxTokens->Far, ReadAttribute( interval, UsdTraxTokens->IntervalFar, +infinite__length ) ) };
    }

    return { 0_m, +infinite__length };
}

Position<Length> USDReader::ReadPosition( const pxr::UsdPrim& position )
{
    if( position )
	{
        return {    ReadAttribute( position, UsdTraxTokens->X, 0_m ),
                    ReadAttribute( position, UsdTraxTokens->Y, 0_m ),
                    ReadAttribute( position, UsdTraxTokens->Z, 0_m ) };
    }
    
    return {};
}

Vector<One> USDReader::ReadVector( const pxr::UsdPrim& vector )
{
    if( vector )
    {
		return {    ReadAttribute( vector, UsdTraxTokens->Dx, 0_1 ),
					ReadAttribute( vector, UsdTraxTokens->Dy, 0_1 ),
					ReadAttribute( vector, UsdTraxTokens->Dz, 0_1 ) };
	}

    return {};
}

spat::Vector<Length> USDReader::ReadDistance( const pxr::UsdPrim& distance )
{
    if( distance )
    {
		return {    ReadAttribute( distance, UsdTraxTokens->Dx, 0_m ),
					ReadAttribute( distance, UsdTraxTokens->Dy, 0_m ),
					ReadAttribute( distance, UsdTraxTokens->Dz, 0_m ) };
	}

    return {};
}

VectorBundle<Length,One> USDReader::ReadVectorBundle( const pxr::UsdPrim& vectorBundle )
{
    VectorBundle<Length,One> bundle;
    if( vectorBundle )
	{
        pxr::UsdPrim::SiblingRange siblings = vectorBundle.GetChildren();
        for( const auto& sibling : siblings )
        {
            if( sibling.GetTypeName() == UsdTraxTokens->Position )
                bundle.P = ReadPosition( sibling.GetPrim() );

            else if( sibling.GetTypeName() == UsdTraxTokens->Tangent )
                bundle.T = ReadVector( sibling.GetPrim() );
        }
    }

    return bundle;
}

VectorBundle2<Length,One> USDReader::ReadVectorBundle2( const pxr::UsdPrim& vectorBundle2 )
{
    VectorBundle2<Length,One> bundle2;
    if( vectorBundle2 )
    {
		pxr::UsdPrim::SiblingRange siblings = vectorBundle2.GetChildren();
		for( const auto& sibling : siblings )
		{
            if( sibling.GetTypeName() == UsdTraxTokens->Position )
                bundle2.P = ReadPosition( sibling.GetPrim() );

            else if( sibling.GetTypeName() == UsdTraxTokens->Tangent )
                bundle2.T = ReadVector( sibling.GetPrim() );

            else if( sibling.GetTypeName() == UsdTraxTokens->Normal )
                bundle2.N = ReadVector( sibling.GetPrim() );
		}
	}

    return bundle2;
}

spat::VectorBundle3<Length, One> USDReader::ReadVectorBundle3( const pxr::UsdPrim& vectorBundle3 )
{
     VectorBundle3<Length,One> bundle3;
     if( vectorBundle3 )
     {
		 pxr::UsdPrim::SiblingRange siblings = vectorBundle3.GetChildren();
		 for( const auto& sibling : siblings )
		 {
			 if( sibling.GetTypeName() == UsdTraxTokens->Position )
				 bundle3.P = ReadPosition( sibling.GetPrim() );

			 else if( sibling.GetTypeName() == UsdTraxTokens->Tangent )
				 bundle3.T = ReadVector( sibling.GetPrim() );

			 else if( sibling.GetTypeName() == UsdTraxTokens->Normal )
				 bundle3.N = ReadVector( sibling.GetPrim() );

			 else if( sibling.GetTypeName() == UsdTraxTokens->Binormal )
				 bundle3.B = ReadVector( sibling.GetPrim() );
		 }
	 }

     return bundle3;
}

CurveSample USDReader::ReadCurveSample( const pxr::UsdPrim& curveSample )
{
    if( curveSample )
	{
		return CurveSample{
            ReadChildFrame( curveSample ),
            ReadAttribute( curveSample, UsdTraxTokens->Length, 0_m ),
            ReadAttribute( curveSample, UsdTraxTokens->Curvature, 0.1_1Im ),
            ReadAttribute( curveSample, UsdTraxTokens->Torsion, 0.1_1Im ) };
	}

    return {};
}

CubicData USDReader::ReadCubicData( const pxr::UsdPrim& cubicData )
{
    CubicData data;
    if( cubicData )
    {
		pxr::UsdPrim::SiblingRange siblings = cubicData.GetChildren();
		for( const auto& sibling : siblings )
		{
            if( sibling.GetTypeName() == UsdTraxTokens->A )
                data.a = ReadPosition( sibling.GetPrim() );

            else if( sibling.GetTypeName() == UsdTraxTokens->B )
                data.b = ReadDistance( sibling.GetPrim() );

			else if( sibling.GetTypeName() == UsdTraxTokens->C )
				data.c = ReadDistance( sibling.GetPrim() );

			else if( sibling.GetTypeName() == UsdTraxTokens->D )
				data.d = ReadDistance( sibling.GetPrim() );
        }
    }

    return data;
}

std::tuple<Angle,Angle,Length> USDReader::ReadRotatorChainData( const pxr::UsdPrim& data )
{
    std::tuple<Angle,Angle,Length> rotatorChainData;
    if( data )
	{
        std::get<0>(rotatorChainData) = ReadAttribute( data, UsdTraxTokens->A, 0_deg );
		std::get<1>(rotatorChainData) = ReadAttribute( data, UsdTraxTokens->B, 0_deg );
		std::get<2>(rotatorChainData) = ReadAttribute( data, UsdTraxTokens->Length, 0_m );
	}

    return rotatorChainData;
}

std::pair<Length,Angle> USDReader::ReadLinearTwistData( const pxr::UsdPrim& data )
{
    std::pair<Length,Angle> linearTwistData;
    if( data )
    {
		linearTwistData.first = ReadAttribute( data, UsdTraxTokens->Length, 0_m );
        linearTwistData.second = ReadAttribute( data, UsdTraxTokens->A, 0_deg );
    }

    return linearTwistData;
}

const std::pair<Length,Vector<One>> USDReader::ReadDirectionalTwistData( const pxr::UsdPrim& data )
{
    if( data )
	{
        return std::pair<Length,Vector<One>>( ReadAttribute( data, UsdTraxTokens->Length, 0_m ), ReadVector( data ) );
    }

    return {};
}

std::unique_ptr<Curve> USDReader::ReadCurve( const pxr::UsdPrim& curve )
{
    if( curve.GetTypeName() == UsdTraxTokens->Line )
        return ReadLine( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->LineP )
        return ReadLineP( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->Arc )
	    return ReadArc( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->ArcP )
	    return ReadArcP( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->Helix )
	    return ReadHelix( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->HelixP )
	    return ReadHelixP( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->Cubic )
	    return ReadCubic( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->Spline )
	    return ReadSpline( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->Clothoid )
	    return ReadClothoid( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->Rotator )
	    return ReadRotator( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->RotatorChain )
	    return ReadRotatorChain( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->PolygonalChain )
	    return ReadPolygonalChain( curve );

    else if( curve.GetTypeName() == UsdTraxTokens->SampledCurve )
	    return ReadSampledCurve( curve );   

    return nullptr;
}

std::unique_ptr<Line> USDReader::ReadLine( const pxr::UsdPrim& line )
{
    if( line )
		return Line::Make();

    return nullptr;
}

std::unique_ptr<LineP> USDReader::ReadLineP( const pxr::UsdPrim& lineP )
{
    if( lineP )
    {
        if( std::unique_ptr<LineP> pLineP = LineP::Make(); pLineP )
		{
            pxr::UsdPrim::SiblingRange siblings = lineP.GetChildren();
            for( const auto& sibling : siblings )
            {
                if( sibling.GetTypeName() == UsdTraxTokens->VectorBundle )
                    pLineP->Create( ReadVectorBundle( sibling.GetPrim() ) );
            }

		    return pLineP;
        }
	}

    return nullptr;
}

std::unique_ptr<Arc> USDReader::ReadArc( const pxr::UsdPrim& arc )
{
    if( arc )
    {
		if( std::unique_ptr<Arc> pArc = Arc::Make(); pArc )
        {
            pArc->Create( { ReadAttribute( arc, UsdTraxTokens->Curvature, 0.1_1Im ) } );
            return pArc;
        }
    }

    return nullptr;
}

std::unique_ptr<ArcP> USDReader::ReadArcP( const pxr::UsdPrim& arcP )
{
    if( arcP )
	{
		if( std::unique_ptr<ArcP> pArcP = ArcP::Make(); pArcP )
		{
			pxr::UsdPrim::SiblingRange siblings = arcP.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->VectorBundle2 )
					pArcP->Create( ReadVectorBundle2( sibling.GetPrim() ) );
			}

			return pArcP;
		}
	}

    return nullptr;
}

std::unique_ptr<Helix> USDReader::ReadHelix( const pxr::UsdPrim& helix )
{
    if( helix )
	{
		if( std::unique_ptr<Helix> pHelix = Helix::Make(); pHelix )
		{
			pHelix->Create( { 
                ReadAttribute( helix, UsdTraxTokens->Curvature, 0.1_1Im ),
                ReadAttribute( helix, UsdTraxTokens->Torsion, 0.1_1Im ) } );

            return pHelix;
		}
	}

    return nullptr;
}

std::unique_ptr<HelixP> USDReader::ReadHelixP( const pxr::UsdPrim& helixP )
{
    if( helixP )
    {
        if( std::unique_ptr<HelixP> pHelixP = HelixP::Make(); pHelixP )
        {
			pxr::UsdPrim::SiblingRange siblings = helixP.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->VectorBundle2 )
					pHelixP->Create( {
                        ReadVectorBundle2( sibling.GetPrim() ),
                        ReadAttribute( helixP, UsdTraxTokens->A, 10_m ),
                        ReadAttribute( helixP, UsdTraxTokens->B, 5_m/(2*pi) ) } );
			}

			return pHelixP;
		}
	}

    return nullptr;
}

std::unique_ptr<Cubic> USDReader::ReadCubic( const pxr::UsdPrim& cubic )
{
    if( cubic )
    {
        if( std::unique_ptr<Cubic> pCubic = Cubic::Make(); pCubic )
		{
			pxr::UsdPrim::SiblingRange siblings = cubic.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->CubicData )
					pCubic->Create( ReadCubicData( sibling.GetPrim() ) );
			}

			return pCubic;
		}
	}

    return nullptr;
}

std::unique_ptr<Spline> USDReader::ReadSpline( const pxr::UsdPrim& spline )
{
    if( spline )
	{
		if( std::unique_ptr<Spline> pSpline = Spline::Make(); pSpline )
        {
            Spline::Data splineSamples;
            pxr::UsdPrim::SiblingRange siblings = spline.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->CubicData )
					splineSamples.push_back( ReadCubicData( sibling.GetPrim() ) );
			}

			pSpline->Create( splineSamples );
			return pSpline;
		}
    }

    return nullptr;
}

std::unique_ptr<Clothoid> USDReader::ReadClothoid( const pxr::UsdPrim& clothoid )
{
    if( clothoid )
    {
        if( std::unique_ptr<Clothoid> pClothoid = Clothoid::Make(); pClothoid )
		{
            pClothoid->Create( { ReadAttribute( clothoid, UsdTraxTokens->A, 100_m ) } );
            return pClothoid;
        }
	}

    return nullptr;
}

std::unique_ptr<Rotator> USDReader::ReadRotator( const pxr::UsdPrim& rotator )
{
    if( rotator )
    {
        if( std::unique_ptr<Rotator> pRotator = Rotator::Make(); pRotator )
		{
			pRotator->Create( {
                ReadAttribute( rotator, UsdTraxTokens->A, 0_1Im ),
                ReadAttribute( rotator, UsdTraxTokens->B, 0_1Im ),
                ReadAttribute( rotator, UsdTraxTokens->A0, 0_deg ),
                ReadAttribute( rotator, UsdTraxTokens->B0, 0_deg ) } );
            return pRotator;
		}
	}

    return nullptr;
}

std::unique_ptr<RotatorChain> USDReader::ReadRotatorChain( const pxr::UsdPrim& rotatorChain )
{
    if( rotatorChain )
	{
		if( std::unique_ptr<RotatorChain> pRotatorChain = RotatorChain::Make(); pRotatorChain )
        {
            RotatorChain::Data rotatorSamples;
            pxr::UsdPrim::SiblingRange siblings = rotatorChain.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->CurveSample )
					rotatorSamples.push_back( ReadRotatorChainData( sibling.GetPrim() ) );
			}

			pRotatorChain->Create( rotatorSamples );
            return pRotatorChain;
		}
    }

    return nullptr;
}

std::unique_ptr<PolygonalChain> USDReader::ReadPolygonalChain( const pxr::UsdPrim& polygonalChain )
{
    if( polygonalChain )
    {
        if( std::unique_ptr<PolygonalChain> pPolygonalChain = PolygonalChain::Make(); pPolygonalChain )
		{
			PolygonalChain::Data polygonalSamples;
			pxr::UsdPrim::SiblingRange siblings = polygonalChain.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->VectorBundle )
					polygonalSamples.push_back( ReadVectorBundle( sibling.GetPrim() ) );
			}

			pPolygonalChain->Create( polygonalSamples );
			return pPolygonalChain;
		}
	}

    return nullptr;
}

std::unique_ptr<SampledCurve> USDReader::ReadSampledCurve( const pxr::UsdPrim& sampledCurve )
{
    if( sampledCurve )
	{
		if( std::unique_ptr<SampledCurve> pSampledCurve = SampledCurve::Make(); pSampledCurve )
        {
            SampledCurve::Data sampledSamples;
			pxr::UsdPrim::SiblingRange siblings = sampledCurve.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->CurveSample )
					sampledSamples.push_back( ReadCurveSample( sibling.GetPrim() ) );
			}

			pSampledCurve->Create( sampledSamples );
			return pSampledCurve;
		}
    }

    return nullptr;
}

static PolygonalChain::WrapTypes ReadLinearWrap( const pxr::TfToken& wrap )
{
	if( wrap == pxr::UsdGeomTokens->periodic )
		return PolygonalChain::WrapTypes::periodic;
	else if( wrap == pxr::UsdGeomTokens->nonperiodic )
		return PolygonalChain::WrapTypes::nonperiodic;

	return PolygonalChain::WrapTypes::none;
}

static Spline::WrapTypes ReadCubicWrap( const pxr::TfToken& wrap )
{
	if( wrap == pxr::UsdGeomTokens->nonperiodic )
		return Spline::WrapTypes::nonperiodic;
	else if( wrap == pxr::UsdGeomTokens->periodic )
		return Spline::WrapTypes::periodic;
	else if( wrap == pxr::UsdGeomTokens->pinned )
		return Spline::WrapTypes::pinned;

	return Spline::WrapTypes::none;
}

std::pair<std::unique_ptr<Curve>,std::unique_ptr<RoadwayTwist>> USDReader::ReadBasisCurve( 
    const pxr::UsdPrim& basisCurve, bool bLocalTransformation )
{
    if( basisCurve )
    {
        pxr::TfToken basis;
        if( pxr::UsdAttribute basisAttribute = basisCurve.GetAttribute( pxr::UsdGeomTokens->basis ); basisAttribute )
            basisAttribute.Get( &basis );
        pxr::TfToken type;
        if( pxr::UsdAttribute typeAttribute = basisCurve.GetAttribute( pxr::UsdGeomTokens->type ); typeAttribute )
            typeAttribute.Get( &type );
        pxr::TfToken wrap;
        if( pxr::UsdAttribute wrapAttribute = basisCurve.GetAttribute( pxr::UsdGeomTokens->wrap ); wrapAttribute )
            wrapAttribute.Get( &wrap );

        if( pxr::UsdAttribute pointsAttribute = basisCurve.GetAttribute( pxr::UsdGeomTokens->points ); pointsAttribute )
        {
            const double MetersPerUnit = pxr::UsdGeomGetStageMetersPerUnit( basisCurve.GetStage() );

            pxr::VtArray<pxr::GfVec3f> points;
            pointsAttribute.Get( &points );
            std::vector<spat::Position<Length>> positions;
            positions.reserve( points.size() );
            for( const pxr::GfVec3f& point : points )
            {
                positions.push_back( {
                    _m(static_cast<Real>(MetersPerUnit * point[0])),
                    _m(static_cast<Real>(MetersPerUnit * point[1])),
                    _m(static_cast<Real>(MetersPerUnit * point[2])) } );
            }

            PiecewiseDirectionalTwist::Data attractors;
            if( pxr::UsdAttribute normalsAttribute = basisCurve.GetAttribute( pxr::UsdGeomTokens->normals ); normalsAttribute )
            {
                pxr::VtArray<pxr::GfVec3f> normals;
                normalsAttribute.Get( &normals );
                attractors.reserve( normals.size() );
                for( const pxr::GfVec3f& normal : normals )
                {
                    attractors.push_back( {
                        0_m,
                        { normal[0],
                          normal[1],
                          normal[2] } 
                        } );
                }
            }

            const spat::Frame<Length,One> frame = bLocalTransformation ? 
                ReadTransformation( basisCurve, MetersPerUnit ) : ComputeLocalToWorldTransform( basisCurve );

            if( frame != spat::Identity<Length,One> )
            {
                for( spat::Position<Length>& position : positions )
                    frame.ToParent( position );

                for( auto& pair : attractors )
                    frame.NormalToParent( pair.second ).Normalize();
            }

            // TODO: check strange interpolation behaviour of PolygonalChain
          /*  if( type == pxr::UsdGeomTokens->linear )
            {
                int nKnots;
				switch( ReadLinearWrap( wrap ) )
				{
				case PolygonalChain::WrapTypes::periodic:
					nKnots= common::narrow_cast<int>(positions.size()) + 1;
					break;
				case PolygonalChain::WrapTypes::nonperiodic:
					nKnots= common::narrow_cast<int>(positions.size());
					break;
				default:
					nKnots = -1;
					break;
				}

                if( std::unique_ptr<PolygonalChain> pPolyChain = PolygonalChain::Make(); pPolyChain )
                {
                    try{
                        pPolyChain->Create( positions, ReadLinearWrap( wrap ) );

                        if( attractors.size() >= nKnots )
                        {
                            for( int idx = 0; idx < nKnots; ++idx )
                                attractors.at( common::narrow_cast<size_t>(idx) ).first = pPolyChain->GetParameter( idx );

							if( std::unique_ptr<PiecewiseDirectionalTwist> pTwist = PiecewiseDirectionalTwist::Make(); pTwist )
							{
                                pTwist->Create( attractors );
                                return { std::move(pPolyChain), std::move(pTwist) };
                            }
                        }

                        return { std::move(pPolyChain), ReadUpTwist( *basisCurve.GetStage() ) };
                    }
					catch( const std::exception& e ){
                        std::cerr << TextColor::red << "trax::USDReader: failure on creating curve or loading normals for PolyChain: " << basisCurve.GetPath().GetString() 
                            << ". " << e.what() << rendl;
					}
                }
            }
            else */if( type == pxr::UsdGeomTokens->cubic )
            {
                int nKnots;
				switch( ReadCubicWrap( wrap ) )
				{
				case Spline::WrapTypes::periodic:
					nKnots= common::narrow_cast<int>(positions.size()) / 3;
					break;
				case Spline::WrapTypes::nonperiodic:
					nKnots= (common::narrow_cast<int>(positions.size()) - 4) / 3 + 2;
					break;
				case Spline::WrapTypes::pinned:
					nKnots= common::narrow_cast<int>(positions.size());
					break;
				default:
					nKnots = -1;
					break;
				}

                if( basis == pxr::UsdGeomTokens->bezier )
                {
                    if( std::unique_ptr<Spline> pBezier = Spline::Make(); pBezier )
                    {
                        try{
                            pBezier->CreateBezier( positions, ReadCubicWrap( wrap ) );
                            if( pBezier->HasGaps( scm_EpsilonTrackGap ) )
                                std::clog   << "trax::USDReader: BasisCurves has gaps that might lead to derailment: " 
                                            << basis << "Path: " << basisCurve.GetPath().GetString() << std::endl;
                            if( pBezier->HasKinks( scm_EpsilonTrackKink ) )
                                std::clog   << "trax::USDReader: BasisCurves has kinks that might lead to derailment: " 
                                            << basis << "Path: " << basisCurve.GetPath().GetString() << std::endl;

                            if( attractors.size() >= common::narrow_cast<size_t>(nKnots) )
                            {
                                for( int idx = 0; idx < nKnots; ++idx )
                                    attractors.at( common::narrow_cast<size_t>(idx) ).first = pBezier->GetParameter( idx );

								if( std::unique_ptr<PiecewiseDirectionalTwist> pTwist = PiecewiseDirectionalTwist::Make(); pTwist )
								{
									pTwist->Create( attractors );
									return { std::move( pBezier ), std::move( pTwist ) };
								}
                            }

                            return { std::move(pBezier), ReadUpTwist( *basisCurve.GetStage() ) };
                        }
                        catch( const std::exception& e ){
                            std::cerr << TextColor::red << "trax::USDReader: failure on creating curve loading normals for BasisCurve: " << basisCurve.GetPath().GetString() 
                                << ". " << e.what() << rendl;
                        }
                    }
                }
                else{
                    std::cerr   << TextColor::red
                                << "trax::USDReader: basis for BasisCurves not implemented yet: " 
                                << basis << ". Path: " << basisCurve.GetPath().GetString() << rendl;
                }
            }
            else{
                std::cerr   << TextColor::red
                            << "trax::USDReader: type for BasisCurves not implemented yet: " 
                            << type << ". Path: " << basisCurve.GetPath().GetString() << rendl;
            }
        }
    }
    else
        std::cerr   << TextColor::red
                    << "trax::USDReader: Curve reference invalid for track! Path: " 
                    << basisCurve.GetPath().GetString() << rendl;

    std::cerr   << TextColor::red
                << "trax::USDReader: Couldn't load curve track: " 
                << basisCurve.GetPath().GetString() << rendl;

    return {nullptr,nullptr};
}

std::unique_ptr<DirectionalTwist> USDReader::ReadUpTwist( const pxr::UsdStage& stage )
{
    if( std::unique_ptr<DirectionalTwist> pTwist = DirectionalTwist::Make(); pTwist )
    {
        pxr::TfToken upAxis;
        stage.GetMetadata( pxr::UsdGeomTokens->upAxis, &upAxis );
        if( upAxis == "Z" )
            pTwist->Attractor( Ez<One> );
        else if( upAxis == "Y" )
            pTwist->Attractor( Ey<One> );
        else if( upAxis == "X" )
            pTwist->Attractor( Ex<One> );

        return pTwist;
    }

    return nullptr;
}

std::unique_ptr<RoadwayTwist> USDReader::ReadTwist( const pxr::UsdPrim& twist )
{
    if( twist.GetTypeName() == UsdTraxTokens->ConstantTwist )
        return ReadConstantTwist( twist );

    else if( twist.GetTypeName() == UsdTraxTokens->LinearTwist )
        return ReadLinearTwist( twist );

    else if( twist.GetTypeName() == UsdTraxTokens->PiecewiseTwist )
	    return ReadPiecewiseTwist( twist );

    else if( twist.GetTypeName() == UsdTraxTokens->DirectionalTwist )
	    return ReadDirectionalTwist( twist );

    else if( twist.GetTypeName() == UsdTraxTokens->PiecewiseDirectionalTwist )
	    return ReadPiecewiseDirectionalTwist( twist );

    else if( twist.GetTypeName() == UsdTraxTokens->CombinedTwist )
	    return ReadCombinedTwist( twist );

    return nullptr;
}

std::unique_ptr<ConstantTwist> USDReader::ReadConstantTwist( const pxr::UsdPrim& constantTwist )
{
    if( constantTwist )
	{
		if( std::unique_ptr<ConstantTwist> pConstantTwist = ConstantTwist::Make(); pConstantTwist )
        {
			pConstantTwist->TwistValue( ReadAttribute( constantTwist, UsdTraxTokens->A, 0_deg ) );
			return pConstantTwist;
        }
    }

    return nullptr;
}

std::unique_ptr<LinearTwist> USDReader::ReadLinearTwist( const pxr::UsdPrim& linearTwist )
{
    if( linearTwist )
    {
        if( std::unique_ptr<LinearTwist> pLinearTwist = LinearTwist::Make(); pLinearTwist )
		{
			pLinearTwist->From( ReadAttribute( linearTwist, UsdTraxTokens->From, 0_deg ) );
            pLinearTwist->To( ReadAttribute( linearTwist, UsdTraxTokens->To, 0_deg ) );
			return pLinearTwist;
		}
	}

    return nullptr;
}

std::unique_ptr<PiecewiseTwist> USDReader::ReadPiecewiseTwist( const pxr::UsdPrim& piecewiseTwist )
{
    if( piecewiseTwist )
	{
		if( std::unique_ptr<PiecewiseTwist> pPiecewiseTwist = PiecewiseTwist::Make(); pPiecewiseTwist )
        {
            PiecewiseTwist::Data piecewiseTwistSamples;
            pxr::UsdPrim::SiblingRange siblings = piecewiseTwist.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->Pair )
                    piecewiseTwistSamples.push_back( ReadLinearTwistData( sibling.GetPrim() ) );
			}

		    pPiecewiseTwist->Create( piecewiseTwistSamples );
			return pPiecewiseTwist;
		}
    }

    return nullptr;
}

std::unique_ptr<DirectionalTwist> USDReader::ReadDirectionalTwist( const pxr::UsdPrim& directionalTwist )
{
    if( directionalTwist )
    {
        if( std::unique_ptr<DirectionalTwist> pDirectionalTwist = DirectionalTwist::Make(); pDirectionalTwist )
		{
            pxr::UsdPrim::SiblingRange siblings = directionalTwist.GetChildren();
			for( const auto& sibling : siblings )
			{
			    if( sibling.GetTypeName() == UsdTraxTokens->Attractor )
                    pDirectionalTwist->Attractor( ReadVector( sibling.GetPrim() ) );
            }

			return pDirectionalTwist;
		}
	}

    return nullptr;
}

std::unique_ptr<PiecewiseDirectionalTwist> USDReader::ReadPiecewiseDirectionalTwist( const pxr::UsdPrim& piecewiseDirectionalTwist )
{
    if( piecewiseDirectionalTwist )
    {
        if( std::unique_ptr<PiecewiseDirectionalTwist> pPiecewiseDirectionalTwist = PiecewiseDirectionalTwist::Make(); pPiecewiseDirectionalTwist )
		{
			PiecewiseDirectionalTwist::Data piecewiseDirectionalTwistSamples;
			pxr::UsdPrim::SiblingRange siblings = piecewiseDirectionalTwist.GetChildren();
			for( const auto& sibling : siblings )
			{
				if( sibling.GetTypeName() == UsdTraxTokens->DirectionalTwist )
					piecewiseDirectionalTwistSamples.push_back( ReadDirectionalTwistData( sibling.GetPrim() ) );
			}

			pPiecewiseDirectionalTwist->Create( piecewiseDirectionalTwistSamples );
			return pPiecewiseDirectionalTwist;
		}
	}

    return nullptr;
}

std::unique_ptr<CombinedTwist> USDReader::ReadCombinedTwist( const pxr::UsdPrim& combinedTwist )
{
    if( combinedTwist )
    {
        if( std::unique_ptr<CombinedTwist> pCombinedTwist = CombinedTwist::Make(); pCombinedTwist )
		{
            bool bFirstTwist = true;
			pxr::UsdPrim::SiblingRange siblings = combinedTwist.GetChildren();
			for( const auto& sibling : siblings )
			{
                if( std::unique_ptr<RoadwayTwist> pTwist = ReadTwist( sibling.GetPrim() ); pTwist )
                {
					if( bFirstTwist )
						pCombinedTwist->AttachTwist1( std::move(pTwist) );
					else
						pCombinedTwist->AttachTwist2( std::move(pTwist) );

                    bFirstTwist = !bFirstTwist;
				}
			}

			return pCombinedTwist;
		}
	}

    return nullptr;
}

std::unique_ptr<Section> USDReader::ReadSection( const pxr::UsdPrim& section )
{
    if( section )
    {
        Section::SpecialSections type = Section::SpecialSections::unknown;
	    if( pxr::UsdAttribute attr = section.GetAttribute( UsdTraxTokens->Type ); attr ){
		    std::string value;
		    attr.Get( &value );
		    type = SpecialSection( value );
	    }

        return Section::Make( type );
	}

    return nullptr;
}

void USDReader::ReadSwitches( pxr::UsdPrim scopePrim )
{
    if( scopePrim )
    {
        pxr::UsdPrim::SiblingRange siblings = scopePrim.GetChildren();
        for( const auto& sibling : siblings )
        {
            if( sibling.GetTypeName() == UsdTraxTokens->Scope )
                ReadSwitches( sibling.GetPrim() );

            if( sibling.GetTypeName() == UsdTraxTokens->ConnectorCollection )
                ReadSwitches( sibling.GetPrim() );

            else if( sibling.GetTypeName() == UsdTraxTokens->TwoWaySwitch )
                m_pTrackSystem->GetConnectorCollection()->Add( ReadTwoWaySwitch( sibling.GetPrim() ) );
            
            else if( sibling.GetTypeName() == UsdTraxTokens->ThreeWaySwitch )
                m_pTrackSystem->GetConnectorCollection()->Add( ReadThreeWaySwitch( sibling.GetPrim() ) );

            else if( sibling.GetTypeName() == UsdTraxTokens->SingleSlipSwitch )
                m_pTrackSystem->GetConnectorCollection()->Add( ReadSingleSlipSwitch( sibling.GetPrim() ) );

            else if( sibling.GetTypeName() == UsdTraxTokens->DoubleSlipSwitch )
                m_pTrackSystem->GetConnectorCollection()->Add( ReadDoubleSlipSwitch( sibling.GetPrim() ) );
        }
    }
}

std::unique_ptr<Connector> USDReader::ReadTwoWaySwitch( pxr::UsdPrim switchPrim )
{
    if( switchPrim )
    {
        std::unique_ptr<Switch> pSwitch = Switch::Make();
        pSwitch->Reference( "Name", switchPrim.GetPath().GetName() );
        pSwitch->Reference( "USDPath", switchPrim.GetPath().GetString() );

        ReadSlot( *pSwitch, NarrowSwitch::SlotNames::slot_0, UsdTraxTokens->NarrowTrack, UsdTraxTokens->NarrowTrackEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pSwitch, NarrowSwitch::SlotNames::slot_1, UsdTraxTokens->StraightTrack, UsdTraxTokens->StraightTrackEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pSwitch, NarrowSwitch::SlotNames::slot_2, UsdTraxTokens->DivergingTrack, UsdTraxTokens->DivergingTrackEnd, switchPrim, *m_pTrackSystem );

        if( pxr::UsdAttribute idAttribute = switchPrim.GetAttribute( UsdTraxTokens->Id ); idAttribute )
        {
            int id;
            idAttribute.Get( &id );
            pSwitch->ID( id );
        }

        if( pxr::UsdAttribute stateAttribute = switchPrim.GetAttribute( UsdTraxTokens->SwitchState ); stateAttribute )
        {
            pxr::TfToken state;
            stateAttribute.Get( &state );
            pSwitch->Set( ToNarrowSwitchStatus( state ), false );
        }

        if( !pSwitch->IsComplete() )
            std::cerr << TextColor::red << "trax::USDReader: a two way switch's slots are not fully populated: " << switchPrim.GetPath() << rendl;

        return pSwitch;
    }

    std::cerr   << TextColor::red
                << "trax::USDReader: Couldn't load switch: " 
                << switchPrim.GetPath().GetString() << rendl;
    return nullptr;
}

std::unique_ptr<Connector> USDReader::ReadThreeWaySwitch( pxr::UsdPrim switchPrim )
{
    if( switchPrim )
    {
        std::unique_ptr<ThreeWaySwitch> pThreeWaySwitch = ThreeWaySwitch::Make();
        pThreeWaySwitch->Reference( "Name", switchPrim.GetPath().GetName() );
        pThreeWaySwitch->Reference( "USDPath", switchPrim.GetPath().GetString() );

        ReadSlot( *pThreeWaySwitch, NarrowSwitch::SlotNames::slot_0, UsdTraxTokens->NarrowTrack, UsdTraxTokens->NarrowTrackEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pThreeWaySwitch, NarrowSwitch::SlotNames::slot_1, UsdTraxTokens->StraightTrack, UsdTraxTokens->StraightTrackEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pThreeWaySwitch, NarrowSwitch::SlotNames::slot_2, UsdTraxTokens->DivergingTrack1, UsdTraxTokens->DivergingTrack1End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pThreeWaySwitch, NarrowSwitch::SlotNames::slot_3, UsdTraxTokens->DivergingTrack2, UsdTraxTokens->DivergingTrack2End, switchPrim, *m_pTrackSystem );

        if( pxr::UsdAttribute idAttribute = switchPrim.GetAttribute( UsdTraxTokens->Id ); idAttribute )
        {
            int id;
            idAttribute.Get( &id );
            pThreeWaySwitch->ID( id );
        }

        if( pxr::UsdAttribute stateAttribute = switchPrim.GetAttribute( UsdTraxTokens->SwitchState ); stateAttribute )
        {
            pxr::TfToken state;
            stateAttribute.Get( &state );
            pThreeWaySwitch->Set( ToNarrowSwitchStatus( state ), false );
        }

        if( !pThreeWaySwitch->IsComplete() )
            std::cerr << TextColor::red << "trax::USDReader: a three way switch's slots are not fully populated: " << switchPrim.GetPath() << rendl;

        return pThreeWaySwitch;
    }

    std::cerr   << TextColor::red
                << "trax::USDReader: Couldn't load switch: " 
                << switchPrim.GetPath().GetString() << rendl;
    return nullptr;
}

std::unique_ptr<Connector> USDReader::ReadSingleSlipSwitch( pxr::UsdPrim switchPrim )
{
    if( switchPrim )
    {
        std::unique_ptr<SingleSlipSwitch> pSingleSlipSwitch = SingleSlipSwitch::Make();
        pSingleSlipSwitch->Reference( "Name", switchPrim.GetPath().GetName() );
        pSingleSlipSwitch->Reference( "USDPath", switchPrim.GetPath().GetString() );

        ReadSlot( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_0, UsdTraxTokens->IncomingTrack1, UsdTraxTokens->IncomingTrack1End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_1, UsdTraxTokens->IncomingTrack2, UsdTraxTokens->IncomingTrack2End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_2, UsdTraxTokens->OutgoingTrack1, UsdTraxTokens->OutgoingTrack1End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_3, UsdTraxTokens->OutgoingTrack2, UsdTraxTokens->OutgoingTrack2End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_4, UsdTraxTokens->Track1Diverge, UsdTraxTokens->Track1DivergeEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_5, UsdTraxTokens->Track1Diverge, UsdTraxTokens->Track1DivergeEnd, switchPrim, *m_pTrackSystem, true );

        if( pxr::UsdAttribute idAttribute = switchPrim.GetAttribute( UsdTraxTokens->Id ); idAttribute )
        {
            int id;
            idAttribute.Get( &id );
            pSingleSlipSwitch->ID( id );
        }

        if( pxr::UsdAttribute stateAttribute = switchPrim.GetAttribute( UsdTraxTokens->SwitchState ); stateAttribute )
        {
            pxr::TfToken state;
            stateAttribute.Get( &state );
            pSingleSlipSwitch->Set( ToSingleSlipSwitchStatus( state ), false );
        }

        std::cerr << TextColor::red;
        if( !pSingleSlipSwitch->Check( std::cerr, 10*epsilon__length ) )
            std::cerr << "trax::USDReader: a single slip switch's slots are not fully populated or show gaps or kinks or twists: " << switchPrim.GetPath() << std::endl;
        std::cerr << TextColor::reset;
 
        return pSingleSlipSwitch;
    }

    std::cerr   << TextColor::red
                << "trax::USDReader: Couldn't load switch: " 
                << switchPrim.GetPath().GetString() << rendl;
    return nullptr;
}

std::unique_ptr<Connector> USDReader::ReadDoubleSlipSwitch( pxr::UsdPrim switchPrim )
{
    if( switchPrim )
    {
        std::unique_ptr<DoubleSlipSwitch> pDoubleSlipSwitch = DoubleSlipSwitch::Make();
        pDoubleSlipSwitch->Reference( "Name", switchPrim.GetPath().GetName() );
        pDoubleSlipSwitch->Reference( "USDPath", switchPrim.GetPath().GetString() );

        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_0, UsdTraxTokens->IncomingTrack1, UsdTraxTokens->IncomingTrack1End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_1, UsdTraxTokens->IncomingTrack2, UsdTraxTokens->IncomingTrack2End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_2, UsdTraxTokens->OutgoingTrack1, UsdTraxTokens->OutgoingTrack1End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_3, UsdTraxTokens->OutgoingTrack2, UsdTraxTokens->OutgoingTrack2End, switchPrim, *m_pTrackSystem );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_4, UsdTraxTokens->Track1Straight, UsdTraxTokens->Track1StraightEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_5, UsdTraxTokens->Track2Straight, UsdTraxTokens->Track2StraightEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_6, UsdTraxTokens->Track1Straight, UsdTraxTokens->Track1StraightEnd, switchPrim, *m_pTrackSystem, true );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_7, UsdTraxTokens->Track2Straight, UsdTraxTokens->Track2StraightEnd, switchPrim, *m_pTrackSystem, true );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_8, UsdTraxTokens->Track1Diverge, UsdTraxTokens->Track1DivergeEnd, switchPrim, *m_pTrackSystem );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_9, UsdTraxTokens->Track1Diverge, UsdTraxTokens->Track1DivergeEnd, switchPrim, *m_pTrackSystem, true );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_10, UsdTraxTokens->Track2Diverge, UsdTraxTokens->Track2DivergeEnd, switchPrim, *m_pTrackSystem, true );
        ReadSlot( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_11, UsdTraxTokens->Track2Diverge, UsdTraxTokens->Track2DivergeEnd, switchPrim, *m_pTrackSystem );

        if( pxr::UsdAttribute idAttribute = switchPrim.GetAttribute( UsdTraxTokens->Id ); idAttribute )
        {
            int id;
            idAttribute.Get( &id );
            pDoubleSlipSwitch->ID( id );
        }

        if( pxr::UsdAttribute stateAttribute = switchPrim.GetAttribute( UsdTraxTokens->SwitchState ); stateAttribute )
        {
            pxr::TfToken state;
            stateAttribute.Get( &state );
            pDoubleSlipSwitch->Set( ToDoubleSlipSwitchStatus( state ), false );
        }

        std::cerr << TextColor::red;
        if( !pDoubleSlipSwitch->Check( std::cerr, 10*epsilon__length ) )
            std::cerr << "trax::USDReader: a double slip switch's slots are not fully populated or show gaps or kinks or twists: " << switchPrim.GetPath() << std::endl;
        std::cerr << TextColor::reset;

        return pDoubleSlipSwitch;
    }

    std::cerr   << TextColor::red 
                << "trax::USDReader: Couldn't load switch: " 
                << switchPrim.GetPath().GetString() << rendl;
    return nullptr;
}

Frame<Length,One> USDReader::ReadTransformationNormalized( const pxr::UsdPrim& prim, double metersPerUnit )
{
    Frame<Length,One> frame = ReadTransformation( prim, metersPerUnit );
    if( !frame.IsOrthoNormal( 10*std::numeric_limits<One>::epsilon(), 10*std::numeric_limits<One>::epsilon() ) )
    {
        std::clog << "trax::USDReader: Track does not support scaling or projection. These transformations are ignored by trax library objects." << std::endl;
        frame.OrthoNormalize();
    }
 
    return frame;
}

Frame<Length,One> USDReader::ReadTransformation( const pxr::UsdPrim& prim, double metersPerUnit )
{
    if( prim )
        return ReadTransformation( prim.GetAttribute( pxr::UsdGeomTokens->xformOpOrder ), metersPerUnit );

    return Identity<Length,One>;
}

Frame<Length,One> USDReader::ReadTransformation( const pxr::UsdAttribute& xformOpOrderAttribute, double metersPerUnit )
{
    Frame<Length,One> frame = Identity<Length,One>;

    if( xformOpOrderAttribute )
    {
        pxr::VtArray<pxr::TfToken> opOrder;
        xformOpOrderAttribute.Get( &opOrder );
        for( const pxr::TfToken& opToken : opOrder )
        {
            if( pxr::UsdAttribute opAttribute = xformOpOrderAttribute.GetPrim().GetAttribute( opToken ); opAttribute )
                frame *= ReadFrame( opAttribute, metersPerUnit );
        }
    }

    return frame;
}

Frame<Length,One> USDReader::ReadChildFrame( const pxr::UsdPrim& prim )
{
    if( prim )
    {
        if( pxr::UsdPrim framePrim = prim.GetChild( UsdTraxTokens->Frame ); framePrim )
        {
            return ReadVectorBundle3( framePrim );
        }
    }

    return Identity<Length,One>;
}

template<typename ValueType,typename Vec3Type,typename QuatType>
Frame<Length,One> ReadFrame( 
    const pxr::UsdAttribute& opAttribute, double metersPerUnit )
{
    Frame<Length,One> frame = Identity<Length,One>;

    switch( pxr::UsdGeomXformOp::GetOpTypeEnum( opAttribute.GetBaseName() ) ){
    case pxr::UsdGeomXformOp::TypeTranslate:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.P.x = _m(static_cast<Real>(metersPerUnit*value[0]));
            frame.P.y = _m(static_cast<Real>(metersPerUnit*value[1]));
            frame.P.z = _m(static_cast<Real>(metersPerUnit*value[2]));
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeScale:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.T *= value[0];
            frame.N *= value[1];
            frame.B *= value[2];
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateX:
    {
        ValueType value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateTan( _deg(static_cast<Real>(value)) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateY:
    {
        ValueType value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateNor( _deg(static_cast<Real>(value)) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateZ:
    {
        ValueType value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateBin( _deg(static_cast<Real>(value)) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateXYZ:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateTan( _deg(static_cast<Real>(value[0])) );
            frame.RotateNor( _deg(static_cast<Real>(value[1])) );
            frame.RotateBin( _deg(static_cast<Real>(value[2])) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateXZY:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateTan( _deg(static_cast<Real>(value[0])) );
            frame.RotateBin( _deg(static_cast<Real>(value[2])) );
            frame.RotateNor( _deg(static_cast<Real>(value[1])) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateYXZ:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateNor( _deg(static_cast<Real>(value[1])) );
            frame.RotateTan( _deg(static_cast<Real>(value[0])) );
            frame.RotateBin( _deg(static_cast<Real>(value[2])) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateYZX:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateNor( _deg(static_cast<Real>(value[1])) );
            frame.RotateBin( _deg(static_cast<Real>(value[2])) );
            frame.RotateTan( _deg(static_cast<Real>(value[0])) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateZXY:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateBin( _deg(static_cast<Real>(value[2])) );
            frame.RotateTan( _deg(static_cast<Real>(value[0])) );
            frame.RotateNor( _deg(static_cast<Real>(value[1])) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeRotateZYX:
    {
        Vec3Type value;
        if( opAttribute.Get( &value ) )
        {
            frame.RotateBin( _deg(static_cast<Real>(value[2])) );
            frame.RotateNor( _deg(static_cast<Real>(value[1])) );
            frame.RotateTan( _deg(static_cast<Real>(value[0])) );
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeOrient:
    {
        QuatType value;
        if( opAttribute.Get( &value ) )
        {
            Rotation<One> rotation{ 
                static_cast<One>(value.GetReal()),
                static_cast<One>(value.GetImaginary()[0]),
                static_cast<One>(value.GetImaginary()[1]),
                static_cast<One>(value.GetImaginary()[2]) };
            frame = rotation;
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeTransform:
    {
        pxr::GfMatrix4d value;
        if( opAttribute.Get( &value ) )
        {
            Transformation<ValueType> transform;
            copy_row_major( value.data(), transform );
            frame = transform;
        }
        return frame;
    }
    case pxr::UsdGeomXformOp::TypeInvalid:
    default:
        return frame;
    }
}

Frame<Length,One> USDReader::ReadFrame( 
    const pxr::UsdAttribute& opAttribute, double metersPerUnit )
{
    switch( pxr::UsdGeomXformOp::GetPrecisionFromValueTypeName( opAttribute.GetTypeName() ) ){
    case pxr::UsdGeomXformOp::Precision::PrecisionDouble:
        return trax::ReadFrame<double,pxr::GfVec3d,pxr::GfQuatd>( opAttribute, metersPerUnit );
    case pxr::UsdGeomXformOp::Precision::PrecisionFloat:
        return trax::ReadFrame<float,pxr::GfVec3f,pxr::GfQuatf>( opAttribute, metersPerUnit );
    case pxr::UsdGeomXformOp::Precision::PrecisionHalf:
        //  return trax::ReadFrame<pxr::GfHalf,pxr::GfVec3h,pxr::GfQuath>( opAttribute, metersPerUnit );
    default:
        return Identity<Length,One>;
    }
    
    return Identity<Length,One>;
}

spat::Frame<Length,One> USDReader::ComputeLocalToWorldTransform( pxr::UsdPrim forPrim )
{
    const pxr::UsdGeomXformable xform{ forPrim };
    const pxr::GfMatrix4d matrix = xform.ComputeLocalToWorldTransform( pxr::UsdTimeCode::Default() );  
    Transformation<One> transformation;
    copy_column_major( matrix.data(), transformation );

    //spat::Transformation<One> scaling;
    //transformation.GetScaling( scaling );
    //if( !scaling.IsIdentity() )
    //    std::clog << "trax::USDReader: scaling not supported yet." << std::endl;

    const Frame<One> frame{ transformation };
    const double MetersPerUnit = pxr::UsdGeomGetStageMetersPerUnit( forPrim.GetStage() );
    return Frame<Length,One>{
        {   _m(static_cast<Real>(MetersPerUnit * frame.P.x)),
            _m(static_cast<Real>(MetersPerUnit * frame.P.y)),
            _m(static_cast<Real>(MetersPerUnit * frame.P.z)) },
        frame.T,
        frame.N,
        frame.B }; 
}

void USDReader::Connect( TrackSystem& trackSystem )
{
    for( TrackBuilder& trackA : trackSystem )
    {
        if( !trackA.Reference( "frontConnection" ).empty() )
        {
            auto coupledTrack = std::find_if( trackSystem.begin(), trackSystem.end(), 
                [&trackA]( TrackBuilder& trackB ){ return trackA.Reference( UsdTraxTokens->FrontConnection.GetString() ) == trackB.Reference( "USDPath" ); } );
            if( coupledTrack != trackSystem.end() )
            {
                trackA.Couple( 
                    std::make_pair( 
                        trackA.This(), 
                        Track::EndType::front ), 
                    std::make_pair( 
                        coupledTrack->This(),
                        ToEndType( trackA.Reference( UsdTraxTokens->FrontConnectionEnd.GetString() ) ) ) );

                Length distance = DistanceToCoupled( trackA, Track::EndType::front );
                if( distance > scm_EpsilonTrackGap )
                    std::clog   << "trax::USDReader: the distance between two coupled track ends is greater then: " 
                                << scm_EpsilonTrackGap << ": " << distance << ". Track at front: " 
                                << trackA.Reference( "USDPath" ) << " coupled with: " << coupledTrack->Reference( "USDPath" ) 
                                << " at: " << trackA.Reference( UsdTraxTokens->FrontConnectionEnd.GetString() ) << std::endl;
            }
            else
                std::cerr   << TextColor::red
                            << "trax::USDReader: Can not find track for track connection! " 
                            << UsdTraxTokens->Track << " " << UsdTraxTokens->FrontConnection 
                            << ": " << trackA.Reference( UsdTraxTokens->FrontConnection.GetString() ) << rendl;
        }

        if( !trackA.Reference( "endConnection" ).empty() )
        {
            auto coupledTrack = std::find_if( trackSystem.begin(), trackSystem.end(), 
                [&trackA]( TrackBuilder& trackB ){ return trackA.Reference( UsdTraxTokens->EndConnection.GetString() ) == trackB.Reference( "USDPath" ); } );
            if( coupledTrack != trackSystem.end() )
            {
                trackA.Couple( 
                    std::make_pair( 
                        trackA.This(), 
                        Track::EndType::end ),
                    std::make_pair( 
                        coupledTrack->This(), 
                        ToEndType( trackA.Reference( UsdTraxTokens->EndConnectionEnd.GetString() ) ) ) );

                Length distance = DistanceToCoupled( trackA, Track::EndType::end );
                if( distance > scm_EpsilonTrackGap )
                    std::clog   << "trax::USDReader: the distance between two coupled track ends is greater then: "
                                << scm_EpsilonTrackGap << ": " << distance << ". Track at end: " 
                                << trackA.Reference( "USDPath" ) << " coupled with: " << coupledTrack->Reference( "USDPath" ) 
                                << " at: " << trackA.Reference( UsdTraxTokens->EndConnectionEnd.GetString() ) << std::endl;
            }
            else
                std::cerr   << TextColor::red
                            << "trax::USDReader: Can not find track for track connection! " 
                            << UsdTraxTokens->Track << " " << UsdTraxTokens->EndConnection 
                            << ": " << trackA.Reference( UsdTraxTokens->EndConnection.GetString() ) << rendl;
        }
    }
}

}

