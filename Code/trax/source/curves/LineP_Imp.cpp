// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "LineP_Imp.h"
#include "trax/Units.h"

namespace trax{
///////////////////////////////////////
bool LineFunction::Mirror(const spat::VectorBundle<Length,One>& mirrorPlane){
	spat::Transformation<One> M;
	M.CreateMirror( mirrorPlane );

	m_Data.vb.P = M * m_Data.vb.P;
	m_Data.vb.T = M * m_Data.vb.T;
	return true;
}

common::Interval<Length> LineFunction::Create( const DataType& data ){
	if( !data.vb.T.IsNormal() )
		throw std::invalid_argument( "Tangent direction invalid!" );
	if( !data.up.IsNormal() )
		throw std::invalid_argument( "Up direction invalid!" );

	m_Data = data;
	return Range();
}
///////////////////////////////////////
std::unique_ptr<LineP> LineP::Make() noexcept{
	try{
		return std::make_unique<LineP_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	LineP_Imp::TypeName() const noexcept{
	return "Line";
}

Curve::CurveType LineP_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::LineP;
}

std::unique_ptr<Curve> LineP_Imp::Clone() const{
	return std::make_unique<LineP_Imp>( *this );
}

bool LineP_Imp::Equals( const Curve& toCurve, Length epsilon_length, Angle epsilon_angle ) const noexcept{
	if( this == &toCurve )
		return true;

	if( auto pLine = dynamic_cast<const trax::LineP*>(&toCurve) )
		return	GetData().vb.Equals( pLine->GetData().vb, epsilon_length, epsilon_angle ) &&
				GetData().up.Equals( pLine->GetData().up, epsilon_angle );

	return false;
}

spat::Vector<One> LineP_Imp::LocalUp() const noexcept{
	return f.GetData().up;
}

common::Interval<Length> LineP_Imp::Create( const Position<Length>& start, const Position<Length>& end, const Vector<One>& up ){
	DataType data;
	data.vb.P = start;
	const auto l = Normalize(end - start);
	data.vb.T = l.second;
	data.up = up;

	f.Create(data);

	return {0_m,l.first};
}

void LineP_Imp::Create( const VectorBundle<Length,One>& vb, const Vector<One>& up ){
	Create( vb.P, vb.T, up );
}

void LineP_Imp::Create( const spat::VectorBundle2<Length,One>& start ){
	Create( start.P, start.T, start.T % start.N );
}

void LineP_Imp::Create( const spat::Frame<Length,One>& start ){
	Create( start.P, start.T, start.B );
}

void LineP_Imp::Create( const Position<Length>& start, const Vector<One>& tan, const Vector<One>& up ){
	DataType data;
	data.vb.P = start;
	data.vb.T = tan;
	data.up = up;

	data.vb.T.Normalize();
	if( data.up.Normalize() == 0 )
		data.up = Up;

	f.Create( data );
}
///////////////////////////////////////
bool Equals( 
	const LineP::Data& A, 
	const LineP::Data& B, 
	common::Interval<Length> range,
	Length epsilon_length, 
	Angle epsilon_angle )
{
	LineFunction fA, fB;
	fA.Create( A );
	fB.Create( B );

	return	(fA(range.Near()) - fB(range.Near())).Length() <= epsilon_length &&
			(fA(range.Far()) - fB(range.Far())).Length() <= epsilon_length &&
			A.vb.T * B.vb.T >= cos(epsilon_angle) &&
			A.up * B.up >= cos(epsilon_angle);
}
///////////////////////////////////////
}