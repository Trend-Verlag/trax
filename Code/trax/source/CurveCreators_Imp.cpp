//	trax track library
//	AD 2022 
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

#include "trax/Curve.h"

namespace trax
{

std::unique_ptr<Curve> CreateNonEEPCurve(
	const EEPCurve::Data& curveData)
{
	spat::Frame<Length, One> frame{ spat::spatial_cast<Length,One>(curveData.m_AnfangsBein) };
	frame.P /= 100;
	frame.RotateTan(-curveData.m_FuehrungsVerdrehung);	// The starting Frame includes the twist at the starting point. 
	// For trax this is not the case, so we calculate this out here:

	const One curvatureSign = common::Sign(curveData.gc_Kruemmung);

	if (common::Equals(curveData.gc_Kruemmung, Real{ 0 }, epsilon) &&
		common::Equals(curveData.gc_Windung, Real{ 0 }, epsilon) &&
		common::Equals(curveData.gc_Kurve, Real{ 0 }, epsilon))
	{
		if (auto pLine = LineP::Make())
		{
			pLine->Create(spat::VectorBundle<Length, One>{ frame }, spat::Ez<One>);

			return pLine;
		}
	}
	else if (common::Equals(curveData.gc_Windung, Real{ 0 }, epsilon) &&
		common::Equals(curveData.gc_Kurve, Real{ 0 }, epsilon)) {
		if (auto pArc = ArcP::Make()) {
			frame.N *= curvatureSign;

			pArc->Create(spat::VectorBundle2<Length, One>{ frame },
				_1Icm(std::fabs(curveData.gc_Kruemmung)));

			return pArc;
		}
	}
	else if (common::Equals(curveData.gc_Kurve, Real{ 0 }, epsilon)) {
		if (auto pHelix = HelixP::Make()) {
			frame.N *= curvatureSign;
			frame.B *= curvatureSign;

			pHelix->Create(frame,
				_1Icm(std::fabs(curveData.gc_Kruemmung)),
				_1Icm(curveData.gc_Windung * curvatureSign));

			return pHelix;
		}
	}
	else if (common::Equals(curveData.gc_Windung, Real{ 0 }, epsilon)) {
		if (auto pRotator = Rotator::Make()) {
			pRotator->Create({
				_1Icm(curveData.gc_Kruemmung),
				_1Icm(curveData.gc_Kurve / curveData.gc_Laenge) });

			return pRotator;
		}
	}

	return nullptr;
}

std::pair<std::unique_ptr<Curve>,common::Interval<Length>> CreateCurve(
	const spat::Position<Length>& start,
	const spat::Position<Length>& end,
	const spat::Vector<One>& up )
{
	if( auto pLine = LineP::Make() ){
		common::Interval<Length> length = pLine->Create( start, end, up );
		if( pLine->IsValid() )
			return std::make_pair( std::move(pLine), length );
	}

	return {};
}

std::pair<std::unique_ptr<Curve>,common::Interval<Length>> CreateCurve(
		  spat::VectorBundle<Length,One> start, 
	const spat::Position<Length>& end, 
	const spat::Vector<One>& up, const Length e_length )
{
	assert( std::abs(up.Length() - 1.f) <= epsilon );

	start.T.Normalize();
	const auto D = end - start.P;
	const auto Dp = Normalize( D - (D*up)*up );
	const auto Tp = Normalize( start.T - (start.T*up)*up );
	const One cosa = Dp.second * Tp.second;

	spat::VectorBundle<Length,One> target{ start.P, Tp.second };	// target towards end but 
	target.TransportTo( Dp.first*cosa );							// keep the error all in end
	target.P += (D*up)*up;											// to prevent wanderlust of
	if( target.P.Equals( end, e_length ) ){							// starting point or tangent.
		if( auto pLine = LineP::Make() ){
			const auto Dn = Normalize( target.P - start.P );
			pLine->Create( { start.P, Dn.second }, up );
			return std::make_pair( std::move(pLine), common::Interval<Length>{ 0_m, Dn.first } );
		}
	}

	else{
		const Length e = D * up; 
		if( abs(e) <= e_length ){
			if( auto pArc = ArcP::Make() ){
				start.T -= (start.T * up)*up;
				start.T.Normalize();
				common::Interval<Length> length = pArc->Create( start, end - e*up, One{0} );
				return std::make_pair( std::move(pArc), length );
			}
		}

		else if( auto pHelix = HelixP::Make() ){
			common::Interval<Length> length = pHelix->Create( start, end, up, One{0} );
			return std::make_pair( std::move(pHelix), length );
		}
	}

	return {};
}

std::pair<std::unique_ptr<Curve>,common::Interval<Length>> CreateCurve( 
	const spat::Position<Length>& start, 
		  spat::VectorBundle<Length,One> end, 
	const spat::Vector<One>& up, Length e_length )
{
	assert( std::abs(up.Length() - 1.f) <= epsilon );

	end.T.Normalize();
	const auto D = end.P - start;
	const auto Dp = Normalize( D - (D*up)*up );
	const auto Tp = Normalize( end.T - (end.T*up)*up );
	const One cosa = Dp.second * Tp.second;

	spat::VectorBundle<Length,One> target{ end.P, Tp.second };	// target towards start but 
	target.TransportTo( -Dp.first*cosa );						// keep the error all in start
	target.P -= (D*up)*up;										// to prevent wanderlust of
	if( target.P.Equals( start, e_length ) ){					// ending point or tangent.
		if( auto pLine = LineP::Make() ){
			const auto Dn = Normalize( end.P - target.P );
			pLine->Create( { target.P, Dn.second }, up );
			return std::make_pair( std::move(pLine), common::Interval<Length>{ 0_m, Dn.first } );
		}
	}
	
	else{
		const Length e = (end.P - start) * up; 
		if( abs(e) <= e_length ){
			if( auto pArc = ArcP::Make() ){
				end.T -= (end.T * up)*up;
				end.T.Normalize();
				common::Interval<Length> length = pArc->Create( start + e*up, end, One{0} );
				return std::make_pair( std::move(pArc), length );
			}
		}

		else if( auto pHelix = HelixP::Make() ){
			common::Interval<Length> length = pHelix->Create( start, end, up, One{0} );
			return std::make_pair( std::move(pHelix), length );
		}
	}

	return {};
}

inline bool Check( 
	const Curve& curve, 
	const spat::VectorBundle<Length,One>& start, 
	const spat::VectorBundle<Length,One>& end, 
	common::Interval<Length> length,
	Length e_length, 
	Angle e_angle )
{
	spat::VectorBundle<Length,One> cStart, cEnd;
	curve.Transition( length.Near(), cStart );
	curve.Transition( length.Far(), cEnd );

	spat::VectorBundle<Length,One> Start{start}, End{end};
	Start.T.Normalize();
	End.T.Normalize();

	return  cStart.Equals( Start, e_length, e_angle ) &&
			cEnd.Equals( End, e_length, e_angle );
}

std::pair<std::unique_ptr<Curve>,common::Interval<Length>> CreateCurve( 
	const spat::VectorBundle<Length,One>& start, 
	const spat::VectorBundle<Length,One>& end, 
	const spat::Vector<One>& up, Length e_length, Angle e_angle )
{
	assert( std::abs(up.Length() - 1.f) <= epsilon );

	if( auto pLine = LineP::Make() ){
		common::Interval<Length> length = pLine->Create( start.P, end.P, up );
		if( Check( *pLine, start, end, length, e_length, e_angle ) )
			return std::make_pair( std::move(pLine), length );
	}

	if( auto pArc = ArcP::Make() ){
		try{
			common::Interval<Length> length = pArc->Create( start, end.P, e_angle );
			if( Check( *pArc, start, end, length, e_length, e_angle ) )
				return std::make_pair( std::move(pArc), length );
		}
		catch( const std::invalid_argument& ){}
	}

	if( auto pHelix = HelixP::Make() ){
		try{
			common::Interval<Length> length = pHelix->Create( start, end.P, up, e_angle );
			if( Check( *pHelix, start, end, length, e_length, e_angle ) )
				return std::make_pair( std::move(pHelix), length );
		}
		catch( const std::invalid_argument& ){}
	}

	if( auto pCubic = Cubic::Make() ){
		common::Interval<Length> length = pCubic->Create( spat::spatial_cast<Length,Length>(start), spat::spatial_cast<Length,Length>(end) );
		return std::make_pair( std::move(pCubic), length );
	}

	return {};
}

std::pair<std::unique_ptr<Curve>,common::Interval<Length>> CreateCurve( 
	Curve::CurveType type, 
	std::pair<std::shared_ptr<const Curve>, common::Interval<Length>> fromCurve )
{
	return CreateCurve( type, *fromCurve.first, fromCurve.second );
}

std::pair<std::unique_ptr<Curve>,common::Interval<Length>> CreateCurve(
	Curve::CurveType type, 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	switch( type )
	{
		case Curve::CurveType::Line:
			return CreateLine( fromCurve, range );
		case Curve::CurveType::LineP:
			return CreateLineP( fromCurve, range );
		case Curve::CurveType::Arc:
			return CreateArc( fromCurve, range );
		case Curve::CurveType::ArcP:
			return CreateArcP( fromCurve, range );
		case Curve::CurveType::Helix:
			return CreateHelix( fromCurve, range );
		case Curve::CurveType::HelixP:
			return CreateHelixP( fromCurve, range );
		case Curve::CurveType::Clothoid:
			return CreateClothoid( fromCurve, range );
		case Curve::CurveType::Cubic:
			return CreateCubic( fromCurve, range );
		case Curve::CurveType::Spline:
			return CreateSpline( fromCurve, range );
		case Curve::CurveType::Rotator:
			return CreateRotator( fromCurve, range );
		case Curve::CurveType::RotatorWithOffset:
			return CreateRotatorWithOffset( fromCurve, range );
		case Curve::CurveType::RotatorChain:
			return CreateRotatorChain( fromCurve, range );
		case Curve::CurveType::PolygonalChain:
			return CreatePolygonalChain( fromCurve, range );
		default:
			return { nullptr, range };
	}
}

std::pair<std::unique_ptr<Line>,common::Interval<Length>> CreateLine(
	const Curve& /*fromCurve*/, 
	common::Interval<Length> range )
{
	if( auto pLine = Line::Make() )
	{
		return std::make_pair( std::move(pLine), range );
	}

	return std::make_pair( nullptr, range );
}

std::pair<std::unique_ptr<LineP>,common::Interval<Length>> CreateLineP( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pLine = LineP::Make() )
	{
		spat::VectorBundle<Length,One> start;
		fromCurve.Transition( range.Near(), start );
		if( !range.Normal() )
			start.T *= -1;

		pLine->Create( start );
		return std::make_pair( std::move(pLine), range );
	}

	return std::make_pair( nullptr, range );
}

void CreateArcFromRotatorData( Arc& arc, const Rotator::Data& data ){
	AnglePerLength min_k = 1_1 / plausible_maximum_length;

	if( data.a < min_k ){
		if( data.b < min_k )
			arc.Create( { min_k } );
		else
			arc.Create( { abs(data.b) } );
	}
	else
		arc.Create( { abs(data.a) } );
}

void CreateArcPFromRotatorData( ArcP& arc, const Rotator::Data& data ){
	AnglePerLength min_k = 1_1 / plausible_maximum_length;

	if( data.a < min_k ){
		if( data.b < min_k )
			arc.Create( {	spat::Origin3D<Length>, 
							spat::Ex<One>, 
							spat::Ey<One> }, 
							min_k );
		else
			arc.Create( {spat::Origin3D<Length>, 
							spat::Ex<One>, 
							data.b > 0_1Im ? +spat::Ez<One> : -spat::Ez<One> }, 
							abs(data.b) );
	}
	else
		arc.Create( {spat::Origin3D<Length>, 
						spat::Ex<One>, 
						data.a > 0_1Im ? +spat::Ey<One> : -spat::Ey<One> },
						abs(data.a) );
}

std::pair<std::unique_ptr<Arc>,common::Interval<Length>> CreateArc( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pArc = Arc::Make() )
	{
	//	AnglePerLength min_k = 1_1 / plausible_maximum_length;

		if( fromCurve.GetCurveType() == Curve::CurveType::Line ||
			fromCurve.GetCurveType() == Curve::CurveType::LineP )
		{
			pArc->Create( {1_1 / 100_m} );
		}
		else if( auto pFromArc = dynamic_cast<const Arc*>(&fromCurve) )
			pArc->Create( pFromArc->GetData() );
		else if( auto pFromArcP = dynamic_cast<const ArcP*>(&fromCurve) )
			pArc->Create( {1_1 / pFromArcP->Radius()} );
		else if( auto pFromHelix = dynamic_cast<const HelixP*>(&fromCurve) ){
			const HelixP::Data data{ pFromHelix->GetData() };
			if( data.a > 0_m )
				pArc->Create( {	1_1 / data.a } );
			else
				pArc->Create( { 1_1 / 100_m } );
		}
		else if( auto pFromRotator = dynamic_cast<const Rotator*>(&fromCurve) )
			CreateArcFromRotatorData( *pArc, pFromRotator->GetData() );
		else if( auto pCurve = dynamic_cast<const RotatorChain*>(&fromCurve) ){
			Angle a=0_deg, b=0_deg;
			Length l= 0_m;

			for( const auto& link : pCurve->GetData() ){
				a += std::get<0>(link);
				b += std::get<1>(link);
				l += std::get<2>(link);
			}

			if( l )
				CreateArcFromRotatorData( *pArc, { a/l, b/l, 0_deg, 0_deg } );
		}
		else{		
			pArc->Create( { 1_1 / 100_m } );
		}

		return std::make_pair(std::move(pArc),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<ArcP>,common::Interval<Length>> CreateArcP( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pArcP = ArcP::Make() )
	{
		if( fromCurve.GetCurveType() == Curve::CurveType::Line ||
			fromCurve.GetCurveType() == Curve::CurveType::LineP )
		{
			spat::VectorBundle2<Length,One> start;
			fromCurve.Transition( range.Near(), start );
			if( !range.Normal() )
				start.T *= -1;

			pArcP->Create( start, 1_1 / 100_m );
		}
		else if( auto pFromArc = dynamic_cast<const Arc*>(&fromCurve) )
		{
			spat::VectorBundle2<Length,One> start;
			fromCurve.Transition( range.Near(), start );
			if( !range.Normal() )
				start.T *= -1;

			pArcP->Create( start, pFromArc->GetData().k );
		}
		else if( auto pFromArcP = dynamic_cast<const ArcP*>(&fromCurve) )
			pArcP->Create( pFromArcP->GetData() );
		else if( auto pFromHelix = dynamic_cast<const HelixP*>(&fromCurve) ){
			const HelixP::Data data{ pFromHelix->GetData() };
			assert( data.a > 0_m );
			pArcP->Create( {	data.center.P + data.a * data.center.T,
							data.center.N,
							-data.center.T }, 
							1 / data.a );
		}
		else if( auto pFromRotator = dynamic_cast<const Rotator*>(&fromCurve) )
			CreateArcPFromRotatorData( *pArcP, pFromRotator->GetData() );
		else if( auto pCurve = dynamic_cast<const RotatorChain*>(&fromCurve) ){
			Angle a=0_deg, b=0_deg;
			Length l= 0_m;

			for( const auto& link : pCurve->GetData() ){
				a += std::get<0>(link);
				b += std::get<1>(link);
				l += std::get<2>(link);
			}

			if( l )
				CreateArcPFromRotatorData( *pArcP, { a/l, b/l, 0_deg, 0_deg } );
		}
		else{
			spat::VectorBundle<Length,One> start;
			fromCurve.Transition( range.Near(), start );
			spat::Position<Length> end;
			fromCurve.Transition( range.Far(), end );
			if( !range.Normal() )
				start.T *= -1;

			if( Parallel( Normalize(end - start.P).second, start.T, epsilon__angle ) )
				pArcP->Create( start.P, start.T, Up % start.T, { 1_1 / 100_m } );
			else
				pArcP->Create( start, end, epsilon__angle );
		}

		return std::make_pair(std::move(pArcP),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<Helix>,common::Interval<Length>> CreateHelix( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pHelix = Helix::Make() )
	{
		if( fromCurve.GetCurveType() == Curve::CurveType::Line || 
			fromCurve.GetCurveType() == Curve::CurveType::LineP ){
			pHelix->Create( { 100_km, 0_m } );
		}
		else if( auto pArc = dynamic_cast<const ArcP*>(&fromCurve) ){
			pHelix->Create( { pArc->Radius(), 0_m } );
		}
		else if( auto pHel = dynamic_cast<const Helix*>(&fromCurve) ){
			pHelix->Create( pHel->GetData() );
		}
		else if( auto pCurve = dynamic_cast<const HelixP*>(&fromCurve)  ){
			pHelix->Create( { pCurve->GetData().a, pCurve->GetData().b } );
		}
		else{
			pHelix->Create( { 100_m, 0_m } );
		}

		return std::make_pair(std::move(pHelix),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<HelixP>,common::Interval<Length>> CreateHelixP( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pHelixP = HelixP::Make() )
	{
		if( fromCurve.GetCurveType() == Curve::CurveType::Line ){
			spat::Frame<Length,One> start;
			fromCurve.Transition( range.Near(), start );
			if( !range.Normal() ){
				start.T *= -1;
				start.B *= -1;
			}

			pHelixP->Create( start, 1 / 100_m, 0_1Im );
		}
		else if( auto pArc = dynamic_cast<const ArcP*>(&fromCurve) ){
			spat::Frame<Length,One> start;
			fromCurve.Transition( range.Near(), start );
			if( !range.Normal() ){
				start.T *= -1;
				start.B *= -1;
			}

			pHelixP->Create( start, 1 / pArc->Radius(), 0_1Im );
		}
		else if( auto pCurve = dynamic_cast<const HelixP*>(&fromCurve)  )
			pHelixP->Create( pCurve->GetData() );
		else{
			spat::Frame<Length,One> start;
			fromCurve.Transition( range.Near(), start );
			if( !range.Normal() ){
				start.T *= -1;
				start.N *= -1;
			}

			pHelixP->Create( start, 1 / 100_m, 0_1Im );
		}

		return std::make_pair(std::move(pHelixP),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<Cubic>,common::Interval<Length>> CreateCubic( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pCubic = Cubic::Make() ){
		range = pCubic->Create( fromCurve, range ).first;
		return std::make_pair(std::move(pCubic),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<Spline>,common::Interval<Length>> CreateSpline( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pSpline = Spline::Make() ){
		range = pSpline->Create( fromCurve, range );
		return std::make_pair(std::move(pSpline),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<Clothoid>,common::Interval<Length>> CreateClothoid( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pClothoid = Clothoid::Make() )
	{
		if( auto pCurve = dynamic_cast<const Clothoid*>(&fromCurve)  )
			pClothoid->Create( pCurve->GetData() );
		else if( const AnglePerLength k = fromCurve.Curvature( range.Center() ) )
		{
			range = common::Interval<Length>{ 0_m, range.Length() };
			range.Intersection( pClothoid->Create( range.Far() / k ) );
		}
		else 
		{
			range = common::Interval<Length>{ 0_m, range.Length() };
			range.Intersection( pClothoid->Create( range.Far() * plausible_maximum_length ) );
		}

		return std::make_pair(std::move(pClothoid),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<Rotator>,common::Interval<Length>> CreateRotator( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pRotator = Rotator::Make() )
	{
		if( auto pCurve = dynamic_cast<const Rotator*>(&fromCurve)  ){
			Rotator::Data data = pCurve->GetData();
			data.a0 = 0_deg;
			data.b0 = 0_deg;
			pRotator->Create( data );
		}
		else if( auto pRotatorChain = dynamic_cast<const RotatorChain*>(&fromCurve)  )
			pRotator->Create( { 
				std::get<0>(pRotatorChain->GetData().front()) / std::get<2>(pRotatorChain->GetData().front()),
				std::get<1>(pRotatorChain->GetData().front()) / std::get<2>(pRotatorChain->GetData().front()),
				0_deg, 0_deg } );
		else if( auto pArcP = dynamic_cast<const ArcP*>(&fromCurve) )
			pRotator->Create( { pArcP->Curvature( 0_m ), 0_1Im, 0_deg, 0_deg } );
		else if( auto pArc = dynamic_cast<const Arc*>(&fromCurve) )
			pRotator->Create( { pArc->Curvature( 0_m ), 0_1Im, 0_deg, 0_deg } );
		else if( auto pHelixP = dynamic_cast<const HelixP*>(&fromCurve)  )
			pRotator->Create( { pHelixP->Curvature( 0_m ), pHelixP->Torsion( 0_m ), 0_deg, 0_deg } );
		else if( auto pHelix = dynamic_cast<const Helix*>(&fromCurve)  )
			pRotator->Create( { pHelix->Curvature( 0_m ), pHelix->Torsion( 0_m ), 0_deg, 0_deg } );
		else
		{
			//TotalAngle(
			range = common::Interval<Length>{ 0_m, range.Length() };
			range.Intersection( pRotator->Create( { fromCurve.Curvature( range.Center() ), fromCurve.Torsion( range.Center() ), 0_deg, 0_deg } ) );
		}

		return std::make_pair(std::move(pRotator),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<Rotator>,common::Interval<Length>> CreateRotatorWithOffset(
	const Curve& fromCurve,
	common::Interval<Length> range )
{
	if( auto pRotator = Rotator::Make( Curve::CurveType::RotatorWithOffset ) )
	{
		if( auto pCurve = dynamic_cast<const Rotator*>(&fromCurve)  )
			pRotator->Create( pCurve->GetData() );
		else if( auto pRotatorChain = dynamic_cast<const RotatorChain*>(&fromCurve)  )
			pRotator->Create( { 
				std::get<0>(pRotatorChain->GetData().front()) / std::get<2>(pRotatorChain->GetData().front()),
				std::get<1>(pRotatorChain->GetData().front()) / std::get<2>(pRotatorChain->GetData().front()),
				0_deg, 0_deg } );
		else
		{
			range = common::Interval<Length>{ 0_m, range.Length() };
			range.Intersection( pRotator->Create( { fromCurve.Curvature( range.Center() ), 0_1Im, 0_deg, 0_deg } ) );
		}

		return std::make_pair(std::move(pRotator),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<RotatorChain>,common::Interval<Length>> CreateRotatorChain( 
	const Curve& fromCurve, 
	common::Interval<Length> range )
{
	if( auto pRotatorChain = RotatorChain::Make() )
	{
		if( auto pCurve = dynamic_cast<const RotatorChain*>(&fromCurve)  )
			pRotatorChain->Create( pCurve->GetData() );
		else if( auto pRotator = dynamic_cast<const Rotator*>(&fromCurve)  )
			pRotatorChain->Create( RotatorChain::Data{ 1, { 
				pRotator->GetData().a * range.Length(), 
				pRotator->GetData().b * range.Length(), 
				range.Length() } } );
		else
		{
			range = common::Interval<Length>{ 0_m, range.Length() };
			range.Intersection( pRotatorChain->Create( RotatorChain::Data{ 1, { fromCurve.Curvature( range.Center() ) * range.Length(), 0_deg, range.Length() } } ) );
		}

		return std::make_pair(std::move(pRotatorChain),range);
	}

	return std::make_pair(nullptr,range);
}

std::pair<std::unique_ptr<PolygonalChain>,common::Interval<Length>> CreatePolygonalChain( 
	const Curve& fromCurve,
	common::Interval<Length> range )
{
	if( auto pPolygonalChain = PolygonalChain::Make() ){
		range = pPolygonalChain->Create( fromCurve, range, epsilon__length, epsilon__length );
		return std::make_pair(std::move(pPolygonalChain),range);
	}

	return std::make_pair(nullptr,range);
}

}