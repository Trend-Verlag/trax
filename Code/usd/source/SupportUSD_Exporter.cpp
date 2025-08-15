// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "usd/SupportUSD_Exporter.h"

#include "usd/Support_USDTrackPainter.h"

#include "trax/Curve.h"
#include "trax/GeomTrack.h"
#include "trax/Switch.h"
#include "trax/collections/TrackSystem.h"
#include "trax/collections/TrackCollection.h"
#include "trax/collections/TrackCollectionContainer.h"
#include "trax/collections/ConnectorCollection.h"

#include "common/support/CommonSupportConsole.h"

#if defined(_MSC_VER)
#	pragma warning( push )
#	pragma warning( disable: 4003 ) // not enough arguments for function-like macro invocation
#endif
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xformable.h>
#if defined(_MSC_VER)
#	pragma warning( pop )
#endif


namespace trax{
	using namespace dim;
	using namespace spat;

USDExporter::USDExporter( pxr::UsdStageRefPtr stage, const pxr::SdfPath& atPath, bool bWriteTrackMesh )
	: m_Stage			{ stage }
	, m_bWriteTrackMesh	{ bWriteTrackMesh }
{
	if( !m_Stage )
		throw std::runtime_error{ "railOmniverse: Invalid stage" };

	m_PathStack.push( atPath );
}

USDExporter::~USDExporter()
{
	m_PathStack.pop();
	if( m_Stage )
		m_Stage->GetRootLayer()->Save();
}

void USDExporter::Prepare( TrackSystem& tracksystem )
{
	m_PathStack.push( m_PathStack.top().AppendPath( Name( tracksystem, "TrackSystem" ) ) );
	{
		tracksystem.Reference( "USDPath", m_PathStack.top().GetString() );

		for( auto& trackCollection : *tracksystem.GetCollectionContainer() )
			Prepare( trackCollection );

		if( ConnectorCollection* pConnectorCollection = tracksystem.GetConnectorCollection(); pConnectorCollection )
			Prepare( *pConnectorCollection );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const TrackSystem& tracksystem )
{
	m_PathStack.push( m_PathStack.top().AppendPath( Name( tracksystem, "TrackSystem" ) ) );
	if( pxr::UsdPrim primTrackSystem = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->TrackSystem ); primTrackSystem )
	{
		if( pxr::UsdAttribute metersPerUnit = primTrackSystem.CreateAttribute( pxr::UsdGeomTokens->metersPerUnit, pxr::SdfValueTypeNames->Double ); metersPerUnit )
			metersPerUnit.Set( pxr::UsdGeomGetStageMetersPerUnit( m_Stage ) );

		for( const auto& trackCollection : *tracksystem.GetCollectionContainer() )
			Write( trackCollection );

		if( ConnectorCollection* pConnectorCollection = tracksystem.GetConnectorCollection(); pConnectorCollection )
			Write( *pConnectorCollection );
	}
	m_PathStack.pop();
}

void USDExporter::Prepare( TrackCollection& collection )
{
	m_PathStack.push( m_PathStack.top().AppendPath( Name( collection, "TrackCollection" ) ) );
	{	
		collection.Reference( "USDPath", m_PathStack.top().GetString() );

		for( auto& track : collection )
			Prepare( track );
	}
	m_PathStack.pop();
}

void USDExporter::Prepare( ConnectorCollection& collection )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ "ConnectorCollection" } ) );
	{
		for( auto& connector : collection )
			Prepare( connector );
	}
	m_PathStack.pop();
}

void USDExporter::Prepare( TrackBuilder& track )
{
	m_PathStack.push( m_PathStack.top().AppendPath( Name( track, "Track" ) ) );
	if( pxr::UsdPrim primTrack = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Track ); primTrack )
	{
		track.Reference( "USDPath", m_PathStack.top().GetString() );
	}
	m_PathStack.pop();
}

void USDExporter::Prepare( Connector& connector )
{
	m_PathStack.push( m_PathStack.top().AppendPath( Name( connector, "Switch" ) ) );
	{
		connector.Reference( "USDPath", m_PathStack.top().GetString() );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const TrackCollection& collection )
{
	m_PathStack.push( m_PathStack.top().AppendPath( Name( collection, "TrackCollection" ) ) );
	if( pxr::UsdPrim primTrackCollection = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->TrackCollection ); primTrackCollection )
	{	
		Write( primTrackCollection, collection.GetFrame() );

		for( const auto& track : collection )
			Write( track );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const ConnectorCollection& collection )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ "ConnectorCollection" } ) );
	if( pxr::UsdPrim primTrackCollection = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->ConnectorCollection ); primTrackCollection )
	{
		for( const auto& connector : collection )
			Write( connector );
	}
	m_PathStack.pop();
}

template<class TraxType>
void WriteSlot(
    const TraxType& traxType,
    Track::TrackEnd end, 
    const pxr::TfToken& ConnectionToken, 
    const pxr::TfToken& ConnectionEnd, 
    pxr::UsdPrim prim )
{
	if( end.first )
	{
		if( pxr::UsdRelationship rel = prim.CreateRelationship( ConnectionToken ); rel )
		{
			rel.AddTarget( pxr::SdfPath{ end.first->Reference( "USDPath" ) } );
			if( pxr::UsdAttribute attribute = prim.CreateAttribute( ConnectionEnd, pxr::SdfValueTypeNames->Token ); attribute )
				attribute.Set( end.second == Track::EndType::front ? UsdTraxTokens->FrontEnd : UsdTraxTokens->EndEnd );
		}
	}
}

void USDExporter::Write( const TrackBuilder& track )
{
	if( track.IsValid() )
	{
		m_PathStack.push( m_PathStack.top().AppendPath( Name( track, "Track" ) ) );
		if( pxr::UsdPrim primTrack = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Track ); primTrack )
		{
			if( pxr::UsdAttribute attribute = primTrack.CreateAttribute( UsdTraxTokens->Type, pxr::SdfValueTypeNames->String ); attribute )
				attribute.Set( ToString( track.GetTrackType() ) );
			if( pxr::UsdAttribute attribute = primTrack.CreateAttribute( UsdTraxTokens->Id, pxr::SdfValueTypeNames->Int ); attribute )
				attribute.Set( static_cast<int>(track.ID()) );
		 
            WriteSlot( track, track.TransitionEnd( Track::EndType::front ), UsdTraxTokens->FrontConnection, UsdTraxTokens->FrontConnectionEnd, primTrack );
			WriteSlot( track, track.TransitionEnd( Track::EndType::end ), UsdTraxTokens->EndConnection, UsdTraxTokens->EndConnectionEnd, primTrack );

			Frame<Length,One> trackFrame = track.GetFrame();
			Write( primTrack, trackFrame );

			Write( *track.GetCurve().first );
			Write( track.GetCurve().second );
			Write( track.GetTwist() );

			if( m_bWriteTrackMesh ){
				if( const GeomTrack* pGeomTrack = dynamic_cast<const GeomTrack*>(&track); pGeomTrack )
				{
					Write( *pGeomTrack->GetSection() );

					// Define a mesh
					pxr::UsdGeomMesh mesh = pxr::UsdGeomMesh::Define( m_Stage, m_PathStack.top().AppendPath( pxr::SdfPath{ "TrackGeometry" } ) );

					// Define the vertices of the mesh
					pxr::VtVec3fArray points;
					// Add your vertex data here

					// Define the face vertex indices
					pxr::VtArray<int> faceVertexCounts;
					pxr::VtArray<int> faceVertexIndices;
					// Add your face vertex data here

					BufferedUSDTrackPainter painter{ 
						points, 
						faceVertexIndices, 
						faceVertexCounts, 
						static_cast<Real>(pxr::UsdGeomGetStageMetersPerUnit( m_Stage )) };
				
					const_cast<GeomTrack*>(pGeomTrack)->SetFrame( Identity<Length,One> );
					painter( *pGeomTrack );
					const_cast<GeomTrack*>(pGeomTrack)->SetFrame( trackFrame );
								
					// Set the mesh points
					mesh.GetPointsAttr().Set(points);

					// Set the face vertex counts and indices
					mesh.GetFaceVertexCountsAttr().Set(faceVertexCounts);
					mesh.GetFaceVertexIndicesAttr().Set(faceVertexIndices);

					// Set the extent
					pxr::VtVec3fArray extent;
					if( pxr::UsdGeomMesh::ComputeExtent( points, &extent ) )
						mesh.GetExtentAttr().Set( extent );
				}
			}
		}
		m_PathStack.pop();
	}
}

void USDExporter::Write( const Connector& connector )
{
	m_PathStack.push( m_PathStack.top().AppendPath( Name( connector, "Switch" ) ) );
	switch( connector.GetConnectorType() )
	{
		case ConnectorType::TwoWaySwitch:
			Write( dynamic_cast<const Switch&>(connector) );
			break;
		case ConnectorType::ThreeWaySwitch:
			Write( dynamic_cast<const ThreeWaySwitch&>(connector) );
			break;
		case ConnectorType::SingleSlipSwitch:
			Write( dynamic_cast<const SingleSlipSwitch&>(connector) );
			break;
		case ConnectorType::DoubleSlipSwitch:
			Write( dynamic_cast<const DoubleSlipSwitch&>(connector) );
			break;
		default:
			std::cerr << common::TextColor::red << "railOmniverse: Unsupported connector type: " << connector.TypeName() << common::rendl;
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Switch& connector )
{
	if( pxr::UsdPrim primSwitch = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->TwoWaySwitch ); primSwitch )
	{
		if( pxr::UsdAttribute attribute = primSwitch.CreateAttribute( UsdTraxTokens->SwitchState, pxr::SdfValueTypeNames->Token ); attribute )
			attribute.Set( pxr::TfToken{ ToString(connector.Get()) } );

		WriteSlot( connector, connector.Slot( Switch::SlotNames::slot_0 ), UsdTraxTokens->NarrowTrack, UsdTraxTokens->NarrowTrackEnd, primSwitch );
		WriteSlot( connector, connector.Slot( Switch::SlotNames::slot_1 ), UsdTraxTokens->StraightTrack, UsdTraxTokens->StraightTrackEnd, primSwitch );
		WriteSlot( connector, connector.Slot( Switch::SlotNames::slot_2 ), UsdTraxTokens->DivergingTrack, UsdTraxTokens->DivergingTrackEnd, primSwitch );
	}
}

void USDExporter::Write( const ThreeWaySwitch& connector )
{
	if( pxr::UsdPrim primSwitch = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->ThreeWaySwitch ); primSwitch )
	{
		if( pxr::UsdAttribute attribute = primSwitch.CreateAttribute( UsdTraxTokens->SwitchState, pxr::SdfValueTypeNames->Token ); attribute )
			attribute.Set( pxr::TfToken{ ToString(connector.Get()) } );

		WriteSlot( connector, connector.Slot( ThreeWaySwitch::SlotNames::slot_0 ), UsdTraxTokens->NarrowTrack, UsdTraxTokens->NarrowTrackEnd, primSwitch );
		WriteSlot( connector, connector.Slot( ThreeWaySwitch::SlotNames::slot_1 ), UsdTraxTokens->StraightTrack, UsdTraxTokens->StraightTrackEnd, primSwitch );
		WriteSlot( connector, connector.Slot( ThreeWaySwitch::SlotNames::slot_2 ), UsdTraxTokens->DivergingTrack1, UsdTraxTokens->DivergingTrack1End, primSwitch );
        WriteSlot( connector, connector.Slot( ThreeWaySwitch::SlotNames::slot_3 ), UsdTraxTokens->DivergingTrack2, UsdTraxTokens->DivergingTrack2End, primSwitch );
	}
}

void USDExporter::Write( const SingleSlipSwitch& connector )
{
	if( pxr::UsdPrim primSwitch = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->SingleSlipSwitch ); primSwitch )
	{
		if( pxr::UsdAttribute attribute = primSwitch.CreateAttribute( UsdTraxTokens->SwitchState, pxr::SdfValueTypeNames->Token ); attribute )
			attribute.Set( pxr::TfToken{ ToString(connector.Get()) } );

		WriteSlot( connector, connector.Slot( SingleSlipSwitch::SlotNames::slot_0 ), UsdTraxTokens->IncomingTrack1, UsdTraxTokens->IncomingTrack1End, primSwitch );
		WriteSlot( connector, connector.Slot( SingleSlipSwitch::SlotNames::slot_1 ), UsdTraxTokens->IncomingTrack2, UsdTraxTokens->IncomingTrack2End, primSwitch );
		WriteSlot( connector, connector.Slot( SingleSlipSwitch::SlotNames::slot_2 ), UsdTraxTokens->OutgoingTrack1, UsdTraxTokens->OutgoingTrack1End, primSwitch );
		WriteSlot( connector, connector.Slot( SingleSlipSwitch::SlotNames::slot_3 ), UsdTraxTokens->OutgoingTrack2, UsdTraxTokens->OutgoingTrack2End, primSwitch );
		WriteSlot( connector, connector.Slot( SingleSlipSwitch::SlotNames::slot_4 ), UsdTraxTokens->Track1Diverge, UsdTraxTokens->Track1DivergeEnd, primSwitch );
	}
}

void USDExporter::Write( const DoubleSlipSwitch& connector )
{
	if( pxr::UsdPrim primSwitch = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->DoubleSlipSwitch ); primSwitch )
	{
		if( pxr::UsdAttribute attribute = primSwitch.CreateAttribute( UsdTraxTokens->SwitchState, pxr::SdfValueTypeNames->Token ); attribute )
			attribute.Set( pxr::TfToken{ ToString(connector.Get()) } );

		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_0 ), UsdTraxTokens->IncomingTrack1, UsdTraxTokens->IncomingTrack1End, primSwitch );
		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_1 ), UsdTraxTokens->IncomingTrack2, UsdTraxTokens->IncomingTrack2End, primSwitch );
		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_2 ), UsdTraxTokens->OutgoingTrack1, UsdTraxTokens->OutgoingTrack1End, primSwitch );
		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_3 ), UsdTraxTokens->OutgoingTrack2, UsdTraxTokens->OutgoingTrack2End, primSwitch );
		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_4 ), UsdTraxTokens->Track1Straight, UsdTraxTokens->Track1StraightEnd, primSwitch );
		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_5 ), UsdTraxTokens->Track2Straight, UsdTraxTokens->Track2StraightEnd, primSwitch );
		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_8 ), UsdTraxTokens->Track1Diverge, UsdTraxTokens->Track1DivergeEnd, primSwitch );
		WriteSlot( connector, connector.Slot( DoubleSlipSwitch::SlotNames::slot_11 ), UsdTraxTokens->Track2Diverge, UsdTraxTokens->Track2DivergeEnd, primSwitch );
	}
}

void USDExporter::Write( const Curve& curve, const std::string& pathName )
{
	switch( curve.GetCurveType() )
	{
		case Curve::CurveType::Line:
			Write( dynamic_cast<const Line&>(curve), pathName );
			break;
		case Curve::CurveType::LineP:
			Write( dynamic_cast<const LineP&>(curve), pathName );
			break;
		case Curve::CurveType::Arc:
			Write( dynamic_cast<const Arc&>(curve), pathName );
			break;
		case Curve::CurveType::ArcP:
			Write( dynamic_cast<const ArcP&>(curve), pathName );
			break;
		case Curve::CurveType::Helix:
			Write( dynamic_cast<const Helix&>(curve), pathName );
			break;
		case Curve::CurveType::HelixP:
			Write( dynamic_cast<const HelixP&>(curve), pathName );
			break;
		case Curve::CurveType::Cubic:
			Write( dynamic_cast<const Cubic&>(curve), pathName );
			break;
		case Curve::CurveType::Spline:
			Write( dynamic_cast<const Spline&>(curve), pathName );
			break;
		case Curve::CurveType::Clothoid:
			Write( dynamic_cast<const Clothoid&>(curve), pathName );
			break;
		case Curve::CurveType::Rotator:
			Write( dynamic_cast<const Rotator&>(curve), pathName );
			break;
		case Curve::CurveType::RotatorChain:
			Write( dynamic_cast<const RotatorChain&>(curve), pathName );
			break;
		case Curve::CurveType::PolygonalChain:
			Write( dynamic_cast<const PolygonalChain&>(curve), pathName );
			break;
		case Curve::CurveType::SampledCurve:
			Write( dynamic_cast<const SampledCurve&>(curve), pathName );
			break;
		default:
			std::cerr << common::TextColor::red << "railOmniverse: Unsupported curve type: " << curve.TypeName() << common::rendl;
	}
}

void USDExporter::Write( const Line& line, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Line ); primCurve )
	{

	}
	m_PathStack.pop();
}

void USDExporter::Write( const LineP& line, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->LineP ); primCurve )
	{
		Write( line.GetData().vb, UsdTraxTokens->VectorBundle, "PointAndDirection" );
		Write( line.GetData().up, UsdTraxTokens->Vector, "Up" );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Arc& arc, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Arc ); primCurve )
	{
		CreateAttribute( primCurve, UsdTraxTokens->Curvature, arc.GetData().k );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const ArcP& arc, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->ArcP ); primCurve )
	{
		Write( arc.GetData().vb2, UsdTraxTokens->VectorBundle2, "Center" );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Helix& helix, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Helix ); primCurve )
	{
		CreateAttribute( primCurve, UsdTraxTokens->Curvature, helix.GetData().k );
		CreateAttribute( primCurve, UsdTraxTokens->Torsion, helix.GetData().t );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const HelixP& helix, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->HelixP ); primCurve )
	{
		Write( helix.GetData().center, UsdTraxTokens->VectorBundle2, "Center" );
		CreateAttribute( primCurve, UsdTraxTokens->A, helix.GetData().a );
		CreateAttribute( primCurve, UsdTraxTokens->B, helix.GetData().b );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Cubic& cubic, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Cubic ); primCurve )
	{
		Write( cubic.GetData(), UsdTraxTokens->CubicData, "Data" );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Spline& spline, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Spline ); primCurve )
	{
		int idx = 0;
		for( const auto& point : spline.GetData() )
			Write( point, UsdTraxTokens->CubicData, "Data" + std::to_string( idx++ ) );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Clothoid& clothoid, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Clothoid ); primCurve )
	{
		CreateAttribute( primCurve, UsdTraxTokens->A, clothoid.GetData().a );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Rotator& rotator, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Rotator ); primCurve )
	{
		CreateAttribute( primCurve, UsdTraxTokens->A, rotator.GetData().a );
		CreateAttribute( primCurve, UsdTraxTokens->B, rotator.GetData().b );
		CreateAttribute( primCurve, UsdTraxTokens->A0, rotator.GetData().a0 );
		CreateAttribute( primCurve, UsdTraxTokens->B0, rotator.GetData().b0 );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const RotatorChain& rotatorChain, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->RotatorChain ); primCurve )
	{
		int idx = 0;
		for( const auto& tuple : rotatorChain.GetData() )
		{
			m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ "Data" + std::to_string( idx++ ) } ) );
			if( pxr::UsdPrim primTuple = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Tuple ); primTuple )
			{
				CreateAttribute( primTuple, UsdTraxTokens->A0, std::get<0>(tuple) );
				CreateAttribute( primTuple, UsdTraxTokens->B0, std::get<1>(tuple) );
				CreateAttribute( primTuple, UsdTraxTokens->Length, std::get<2>(tuple) );
			}
			m_PathStack.pop();
		}
	}
	m_PathStack.pop();
}

void USDExporter::Write( const PolygonalChain& polygonalChain, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->PolygonalChain ); primCurve )
	{
		int idx = 0;
		for( const auto& data : polygonalChain.GetData() )
			Write( data, UsdTraxTokens->VectorBundle, std::string{"Sample"} + std::to_string( idx++ ) );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const SampledCurve& sampledCurve, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurve = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->SampledCurve ); primCurve )
	{
		int idx = 0;
		for( const auto& data : sampledCurve.GetData() )
			Write( data, UsdTraxTokens->CurveSample, std::string{"Sample"} + std::to_string( idx++ ) );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const RoadwayTwist& twist, const std::string& pathName ){
	switch( twist.GetTwistType() )
	{
		case RoadwayTwist::TwistType::Constant:
			Write( dynamic_cast<const ConstantTwist&>(twist), pathName );
			break;
		case RoadwayTwist::TwistType::Linear:
			Write( dynamic_cast<const LinearTwist&>(twist), pathName );
			break;
		case RoadwayTwist::TwistType::Piecewise:
		case RoadwayTwist::TwistType::PiecewiseLinear:
		case RoadwayTwist::TwistType::PiecewiseCircular:
			Write( dynamic_cast<const PiecewiseTwist&>(twist), pathName );
			break;
		case RoadwayTwist::TwistType::Directional:
			Write( dynamic_cast<const DirectionalTwist&>(twist), pathName );
			break;
		case RoadwayTwist::TwistType::PiecewiseDirectional:
			Write( dynamic_cast<const PiecewiseDirectionalTwist&>(twist), pathName );
			break;
		case RoadwayTwist::TwistType::Combined:
			Write( dynamic_cast<const CombinedTwist&>(twist), pathName );
			break;
		default:
			std::cerr << common::TextColor::red << "railOmniverse: Unsupported twist type: " << twist.TypeName() << common::rendl;
	}
}

void USDExporter::Write( const ConstantTwist& twist, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primTwist = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->ConstantTwist ); primTwist )
	{
		CreateAttribute( primTwist, UsdTraxTokens->A, twist.TwistValue() );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const LinearTwist& twist, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primTwist = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->LinearTwist ); primTwist )
	{
		CreateAttribute( primTwist, UsdTraxTokens->From, twist.From() );
		CreateAttribute( primTwist, UsdTraxTokens->To, twist.To() );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const PiecewiseTwist& twist, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primTwist = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->PiecewiseTwist ); primTwist )
	{
		int idx = 0;
		for( const auto& data : twist.GetData() )
		{
			m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ "Sample" + std::to_string(idx++) } ) );
			if( pxr::UsdPrim primPair = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Pair ); primPair )
			{
				CreateAttribute( primPair, UsdTraxTokens->Length, std::get<0>(data) );
				CreateAttribute( primPair, UsdTraxTokens->A0, std::get<1>(data) );
			}
			m_PathStack.pop();
		}
	}
	m_PathStack.pop();
}

void USDExporter::Write( const DirectionalTwist& twist, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primTwist = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->DirectionalTwist ); primTwist )
	{
		Write( twist.Attractor(), UsdTraxTokens->Attractor, "Attractor" );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const PiecewiseDirectionalTwist& twist, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primTwist = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->PiecewiseDirectionalTwist ); primTwist )
	{
		int idx = 0;
		for( const auto& data : twist.GetData() )
		{
			m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ "Sample" + std::to_string(idx++) } ) );
			if( pxr::UsdPrim primPair = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Pair ); primPair )
			{
				CreateAttribute( primPair, UsdTraxTokens->Length, std::get<0>(data) );
				Write( std::get<1>(data), UsdTraxTokens->Vector, "Vector" );
			}
			m_PathStack.pop();
		}
	}
	m_PathStack.pop();
}

void USDExporter::Write( const CombinedTwist& twist, const std::string& pathName ){
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primTwist = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->CombinedTwist ); primTwist )
	{
		Write( twist.Twist1(), "Twist1" );
		Write( twist.Twist2(), "Twist2" );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const Section& section )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ "Section" } ) );
	if( pxr::UsdPrim primSection = m_Stage->DefinePrim( m_PathStack.top(), UsdTraxTokens->Section ); primSection )
	{
		if( pxr::UsdAttribute attribute = primSection.CreateAttribute( UsdTraxTokens->Type, pxr::SdfValueTypeNames->String ); attribute )
			attribute.Set( ToString( section.GetSectionType() ) );
	}
	m_PathStack.pop();
}

void USDExporter::Write( pxr::UsdPrim& toPrim, const Frame<Length,One>& frame )
{
	pxr::UsdGeomXformable xformable( toPrim );

	pxr::UsdAttribute translateAttr = xformable.AddTranslateOp();
	pxr::UsdAttribute rotateZYXAttr = xformable.AddRotateZYXOp();
				
	translateAttr.Set( From<pxr::GfVec3d>( frame.P, static_cast<Real>(pxr::UsdGeomGetStageMetersPerUnit( m_Stage ))) );

	Position<One> angles = frame.AircraftPrincipalAxes();
	std::swap( angles.x, angles.z );
	rotateZYXAttr.Set( From( angles ) );

	std::vector<pxr::UsdGeomXformOp> xformOpOrder;
	xformOpOrder.push_back( pxr::UsdGeomXformOp{ translateAttr } );
	xformOpOrder.push_back( pxr::UsdGeomXformOp{ rotateZYXAttr } );
	xformable.SetXformOpOrder( xformOpOrder );
}

void USDExporter::Write( const spat::VectorBundle<Length,One>& bundle, const pxr::TfToken& typeName, const std::string& pathName )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim prim = m_Stage->DefinePrim( m_PathStack.top(), typeName ); prim )
	{
		Write( bundle.P, UsdTraxTokens->Position, "P" );
		Write( bundle.T, UsdTraxTokens->Tangent, "T" );
	}
	m_PathStack.pop();
}
		
void USDExporter::Write( const spat::VectorBundle2<Length,One>& bundle2, const pxr::TfToken& typeName, const std::string& pathName )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim prim = m_Stage->DefinePrim( m_PathStack.top(), typeName ); prim )
	{
		Write( bundle2.P, UsdTraxTokens->Position, "P" );
		Write( bundle2.T, UsdTraxTokens->Tangent, "T" );
		Write( bundle2.N, UsdTraxTokens->Normal, "N" );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const spat::Frame<Length,One>& frame, const pxr::TfToken& typeName, const std::string& pathName )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim prim = m_Stage->DefinePrim( m_PathStack.top(), typeName ); prim )
	{
		Write( frame.P, UsdTraxTokens->Position, "P" );
		Write( frame.T, UsdTraxTokens->Tangent, "T" );
		Write( frame.N, UsdTraxTokens->Normal, "N" );
		Write( frame.B, UsdTraxTokens->Binormal, "B" );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const CurveSample& curveSample, const pxr::TfToken& typeName, const std::string& pathName )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ pathName } ) );
	if( pxr::UsdPrim primCurveSample = m_Stage->DefinePrim( m_PathStack.top(), typeName ); primCurveSample )
	{
		Write( curveSample.F, UsdTraxTokens->Frame, "Frame" );
		CreateAttribute( primCurveSample, UsdTraxTokens->Length, curveSample.s );
		CreateAttribute( primCurveSample, UsdTraxTokens->Curvature, curveSample.k );
		CreateAttribute( primCurveSample, UsdTraxTokens->Torsion, curveSample.t );
	}
	m_PathStack.pop();
}

void USDExporter::Write( const CubicData& cubicData, const pxr::TfToken& typeName, const std::string& path )
{
	m_PathStack.push( m_PathStack.top().AppendPath( pxr::SdfPath{ path } ) );
	if( pxr::UsdPrim prim = m_Stage->DefinePrim( m_PathStack.top(), typeName ); prim )
	{
		Write( cubicData.a, UsdTraxTokens->A, "A" );
		Write( cubicData.b, UsdTraxTokens->B, "B" );
		Write( cubicData.c, UsdTraxTokens->C, "C" );
		Write( cubicData.d, UsdTraxTokens->D, "D" );
	}
	m_PathStack.pop();
}

}