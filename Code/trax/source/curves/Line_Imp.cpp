// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "Line_Imp.h"

#include "spat/Position.h"
#include "spat/Vector.h"

namespace trax{
	using namespace spat;

///////////////////////////////////////
std::unique_ptr<Line> Line::Make() noexcept{
	try{
		return std::make_unique<Line_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	Line_Imp::TypeName() const noexcept{
	return "BasicLine";
}

Curve::CurveType Line_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Line;
}

std::unique_ptr<Curve> Line_Imp::Clone() const{
	return std::make_unique<Line_Imp>( *this );
}

bool Line_Imp::Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept
// Can not be mirrored.
{
	return false;
}

bool Line_Imp::IsValid() const noexcept{
	return true;
}

AnglePerLength Line_Imp::Curvature( Length /*s*/ ) const noexcept{
	return 0_1Im;
}

AnglePerLength Line_Imp::Torsion( Length /*s*/ ) const noexcept{
	return 0_1Im;
}

bool Line_Imp::IsFlat() const noexcept{
	return true;
}

void Line_Imp::Transition( Length s, spat::Position<Length>& pos ) const noexcept{
	pos = Origin3D<Length> + s * Ex<One>;
}

void Line_Imp::Transition( Length /*s*/, spat::Vector<One>& tan ) const noexcept{
	tan = Ex<One>;
}

void Line_Imp::Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const noexcept{
	bundle.P = Origin3D<Length> + s * Ex<One>;
	bundle.T = Ex<One>;
}

void Line_Imp::Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const noexcept{
	bundle.P = Origin3D<Length> + s * Ex<One>;
	bundle.T = Ex<One>;
	bundle.N = Ey<One>;
}

void Line_Imp::Transition( Length s, spat::Frame<Length,One>& frame ) const noexcept{
	frame.P = Origin3D<Length> + s * Ex<One>;
	frame.T = Ex<One>;
	frame.N = Ey<One>;
	frame.B = Ez<One>;
}

std::vector<Length> Line_Imp::ZeroSet() const noexcept{
	return {};
}

common::Interval<Length> Line_Imp::Range() const noexcept{
	return { -infinite__length, +infinite__length };
}

spat::Vector<One> Line_Imp::LocalUp() const noexcept{
	return Ez<One>;
}

spat::Frame<Length,One> Line_Imp::GetCurveLocalTransformation() const noexcept{
	spat::Frame<Length,One> retval;
	Transition( 0_m, retval );
	return retval;
}

bool Line_Imp::Equals( const Curve& toCurve, common::Interval<Length> /*range*/, Length /*epsilon_length*/, Angle /*epsilon_angle*/ ) const noexcept{
	return GetCurveType() == toCurve.GetCurveType();
}
///////////////////////////////////////
}
