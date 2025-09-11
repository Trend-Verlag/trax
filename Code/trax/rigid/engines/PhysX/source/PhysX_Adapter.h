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

#include "PhysX_Imp.h"

#include "spat/Position.h"
#include "spat/Vector.h"
#include "spat/Frame.h"
#include "spat/Matrix.h"

#include "dim/DimensionedValues.h"


namespace trax{

	template<typename Valtype>
	physx::PxVec4 PxVec4From( const spat::Position<Valtype>& p ) noexcept;

	physx::PxVec4 PxVec4From( const spat::Position<Length>& p, Real engine_meters_per_unit ) noexcept;

	template<typename Valtype>
	physx::PxVec4 PxVec4From( const spat::Vector<Valtype>& v ) noexcept;

	template<typename Valtype>
	physx::PxVec3 From( const spat::Position<Valtype>& p ) noexcept;

	template<typename Valtype>
	physx::PxVec3 From( const spat::Vector<Valtype>& v ) noexcept;

	physx::PxVec3 From( const spat::Vector<Velocity>& v, Real engine_meters_per_unit ) noexcept;
	
	physx::PxVec3 From( const spat::Vector<Force>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;

	physx::PxVec3 From( const spat::Vector<Momentum>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;

	physx::PxVec3 From( const spat::Vector<Torque>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;

	physx::PxVec3 From( const spat::Vector<AngularVelocity>& v ) noexcept;

	physx::PxVec3 From( const spat::Vector<AngularMomentum>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;

	template<typename Valtype,typename ValtypeT=Valtype>
	physx::PxMat44 From( const spat::Frame<Valtype,ValtypeT>& frame ) noexcept;

	physx::PxMat44 From( const spat::Frame<Length,One>& frame, Real engine_meters_per_unit ) noexcept;

	template<typename Valtype,typename ValtypeT=Valtype>
	physx::PxTransform PoseFrom( const spat::Frame<Valtype,ValtypeT>& frame ) noexcept;

	physx::PxTransform PoseFrom( const spat::Frame<Length,One>& frame, Real engine_meters_per_unit ) noexcept;

	template<typename Valtype>
	physx::PxMat33 From( const spat::Rotation<Valtype>& rot ) noexcept;

	template<typename Valtype>
	physx::PxQuat QuatFrom( const spat::Rotation<Valtype>& rot ) noexcept;


	template<typename Valtype>
	spat::Position<Valtype> PosFrom( const physx::PxVec3& p ) noexcept;

	spat::Position<Length> PosFrom( const physx::PxVec3& p, Real engine_meters_per_unit ) noexcept;

	template<typename Valtype>
	spat::Position<Valtype> PosFrom( const physx::PxVec4& p ) noexcept;

	spat::Position<Length> PosFrom( const physx::PxVec4& p, Real engine_meters_per_unit ) noexcept;

	template<typename Valtype>
	spat::Position<Valtype> PosFrom( const physx::PxTransform& pxtrans ) noexcept;

	spat::Position<Length> PosFrom( const physx::PxTransform& pxtrans, Real engine_meters_per_unit ) noexcept;

	template<typename Valtype>
	spat::Vector<Valtype> VecFrom( const physx::PxVec3& v ) noexcept;

	template<typename Valtype>
	spat::Vector<Valtype> VecFrom( const physx::PxVec4& v ) noexcept;

	template<typename Valtype,typename ValtypeT=Valtype>
	spat::Frame<Valtype,ValtypeT> From( const physx::PxMat44& pxtrans ) noexcept;

	spat::Frame<Length,One> From( const physx::PxMat44& pxtrans, Real engine_meters_per_unit ) noexcept;

	template<typename Valtype,typename ValtypeT=Valtype>
	spat::Frame<Valtype,ValtypeT> From( const physx::PxTransform& pxtrans );

	spat::Frame<Length,One> From( const physx::PxTransform& pxtrans, Real engine_meters_per_unit );

	template<typename Valtype>
	spat::Transformation<Valtype> TransFrom( const physx::PxTransform& pxtrans ) noexcept;

	template<typename Valtype>
	spat::Vector<Valtype> From( const physx::PxVec3& pxv ) noexcept;

	spat::Vector<Velocity> VelocityFrom( const physx::PxVec3& pxv, Real engine_meters_per_unit ) noexcept;

///////////////////////////////////////
// inlines:
template<typename Valtype>
inline physx::PxVec4 PxVec4From( const spat::Position<Valtype>& p ) noexcept{
	return physx::PxVec4{ 
		static_cast<float>(p.x), 
		static_cast<float>(p.y), 
		static_cast<float>(p.z), 
		1.0f };
}

inline physx::PxVec4 PxVec4From( const spat::Position<Length>& p, Real engine_meters_per_unit ) noexcept{
	return physx::PxVec4{ 
		static_cast<float>(_m(p.x)/engine_meters_per_unit), 
		static_cast<float>(_m(p.y)/engine_meters_per_unit), 
		static_cast<float>(_m(p.z)/engine_meters_per_unit), 
		1.0f };
}

inline physx::PxVec4 PxVec4From( const spat::Position<float>& p ) noexcept{
	return physx::PxVec4{ p.x, p.y, p.z, 1.0f };
}

template<typename Valtype>
inline physx::PxVec4 PxVec4From( const spat::Vector<Valtype>& v ) noexcept{
	return physx::PxVec4( (float)v.dx, (float)v.dy, (float)v.dz, 0.0f );
}

template<typename Valtype>
inline physx::PxVec3 From( const spat::Position<Valtype>& p ) noexcept{
	return physx::PxVec3{ static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) };
}

inline physx::PxVec3 From( const spat::Position<float>& p ) noexcept{
	return physx::PxVec3{ p.x, p.y, p.z };
}

template<typename Valtype>
inline physx::PxVec3 From( const spat::Vector<Valtype>& v ) noexcept{
	return physx::PxVec3{ static_cast<float>(v.dx), static_cast<float>(v.dy), static_cast<float>(v.dz) };
}

inline physx::PxVec3 From( const spat::Vector<Velocity>& v, Real engine_meters_per_unit ) noexcept{
	return physx::PxVec3{ 
		static_cast<float>(_mIs(v.dx) / engine_meters_per_unit), 
		static_cast<float>(_mIs(v.dy) / engine_meters_per_unit), 
		static_cast<float>(_mIs(v.dz) / engine_meters_per_unit) };
}

inline physx::PxVec3 From( const spat::Vector<Force>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept{
	return physx::PxVec3{ 
		static_cast<float>(_N(v.dx) / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_N(v.dy) / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_N(v.dz) / engine_meters_per_unit / engine_kilograms_per_unit) };
}

inline physx::PxVec3 From( const spat::Vector<Momentum>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept{
	return physx::PxVec3{ 
		static_cast<float>(_kgmIs(v.dx) / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_kgmIs(v.dy) / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_kgmIs(v.dz) / engine_meters_per_unit / engine_kilograms_per_unit) };
}

inline physx::PxVec3 From( const spat::Vector<Torque>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept{
	return physx::PxVec3{ 
		static_cast<float>(_Nm(v.dx) / engine_meters_per_unit / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_Nm(v.dy) / engine_meters_per_unit / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_Nm(v.dz) / engine_meters_per_unit / engine_meters_per_unit / engine_kilograms_per_unit) };
}

inline physx::PxVec3 From( const spat::Vector<AngularVelocity>& v ) noexcept{
	return physx::PxVec3{ 
		static_cast<float>(_1Is(v.dx)), 
		static_cast<float>(_1Is(v.dy)), 
		static_cast<float>(_1Is(v.dz)) };
}

inline physx::PxVec3 From( const spat::Vector<AngularMomentum>& v, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept{
	return physx::PxVec3{ 
		static_cast<float>(_kgm2Is(v.dx) / engine_meters_per_unit / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_kgm2Is(v.dy) / engine_meters_per_unit / engine_meters_per_unit / engine_kilograms_per_unit), 
		static_cast<float>(_kgm2Is(v.dz) / engine_meters_per_unit / engine_meters_per_unit / engine_kilograms_per_unit) };
}

inline physx::PxVec3 From( const spat::Vector<Real>& v ) noexcept{
	return physx::PxVec3{ static_cast<float>(v.dx), static_cast<float>(v.dy), static_cast<float>(v.dz) };
}

#ifdef TRAX_DOUBLE
inline physx::PxVec3 From( const spat::Vector<physx::PxReal>& v ) noexcept{
	return physx::PxVec3{ v.dx, v.dy, v.dz };
}
#endif

template<typename Valtype,typename ValtypeT>
inline physx::PxMat44 From( const spat::Frame<Valtype,ValtypeT>& frame ) noexcept{
	return physx::PxMat44{	PxVec4From(frame.T), 
							PxVec4From(frame.N), 
							PxVec4From(frame.B), 
							PxVec4From(frame.P) };
}

inline physx::PxMat44 From( const spat::Frame<Length,One>& frame, Real engine_meters_per_unit ) noexcept{
	return physx::PxMat44{	PxVec4From(frame.T), 
							PxVec4From(frame.N), 
							PxVec4From(frame.B), 
							PxVec4From(frame.P,engine_meters_per_unit) };
}

template<typename Valtype,typename ValtypeT>
inline physx::PxTransform PoseFrom( const spat::Frame<Valtype,ValtypeT>& frame ) noexcept{
	return physx::PxTransform{ From<Valtype,ValtypeT>(frame) };
}

inline physx::PxTransform PoseFrom( const spat::Frame<Length,One>& frame, Real engine_meters_per_unit ) noexcept{
	return physx::PxTransform{ From(frame,engine_meters_per_unit) };
}

template<typename Valtype>
inline physx::PxMat33 From( const spat::Rotation<Valtype>& rot ) noexcept{
	return physx::PxMat33{
		From<Valtype>(Column(rot,0)), 
		From<Valtype>(Column(rot,1)), 
		From<Valtype>(Column(rot,2)) };
}

template<typename Valtype>
inline physx::PxQuat QuatFrom( const spat::Rotation<Valtype>& rot ) noexcept{
	return physx::PxQuat{ From(rot) };
}

template<typename Valtype>
inline spat::Position<Valtype> PosFrom( const physx::PxVec3& p ) noexcept{
	return { Valtype{p.x},
			 Valtype{p.y},
			 Valtype{p.z} };
}

inline spat::Position<Length> PosFrom( const physx::PxVec3& p, Real engine_meters_per_unit ) noexcept{
	return { engine_meters_per_unit * _m(p.x),
			 engine_meters_per_unit * _m(p.y),
			 engine_meters_per_unit * _m(p.z) };
}

template<typename Valtype>
inline spat::Position<Valtype> PosFrom( const physx::PxVec4& p ) noexcept{
	assert( p.w );
	return { Valtype{p.x/p.w},
			 Valtype{p.y/p.w},
			 Valtype{p.z/p.w} };
}

inline spat::Position<Length> PosFrom( const physx::PxVec4& p, Real engine_meters_per_unit ) noexcept{
	assert( p.w );
	return { _m(p.x/p.w * engine_meters_per_unit),
			 _m(p.y/p.w * engine_meters_per_unit),
			 _m(p.z/p.w * engine_meters_per_unit) };
}

template<typename Valtype>
inline spat::Position<Valtype> PosFrom( const physx::PxTransform& pxtrans ) noexcept{
	return PosFrom<Valtype>( pxtrans.p );
}

inline spat::Position<Length> PosFrom( const physx::PxTransform& pxtrans, Real engine_meters_per_unit ) noexcept{
	return PosFrom( pxtrans.p, engine_meters_per_unit );
}

template<typename Valtype>
inline spat::Vector<Valtype> VecFrom( const physx::PxVec3& v ) noexcept{
	return { Valtype{v.x},
			 Valtype{v.y},
			 Valtype{v.z} };
}

template<typename Valtype>
inline spat::Vector<Valtype> VecFrom( const physx::PxVec4& v ) noexcept{
	return { Valtype{v.x},
			 Valtype{v.y},
			 Valtype{v.z} };
}

template<typename Valtype,typename ValtypeT>
inline spat::Frame<Valtype,ValtypeT> From( const physx::PxMat44& pxtrans ) noexcept{
	return { 
		PosFrom<Valtype>(pxtrans.column3), 
		VecFrom<ValtypeT>(pxtrans.column0), 
		VecFrom<ValtypeT>(pxtrans.column1), 
		VecFrom<ValtypeT>(pxtrans.column2) };
}

inline spat::Frame<Length,One> From( const physx::PxMat44& pxtrans, Real engine_meters_per_unit ) noexcept{
	return { 
		PosFrom(pxtrans.column3,engine_meters_per_unit), 
		VecFrom<One>(pxtrans.column0), 
		VecFrom<One>(pxtrans.column1), 
		VecFrom<One>(pxtrans.column2) };
}

template<typename Valtype,typename ValtypeT>
inline spat::Frame<Valtype,ValtypeT> From( const physx::PxTransform& pxtrans ){
	return From<Valtype,ValtypeT>(physx::PxMat44{pxtrans});
}

inline spat::Frame<Length,One> From( const physx::PxTransform& pxtrans, Real engine_meters_per_unit ){
	return From(physx::PxMat44{pxtrans},engine_meters_per_unit);
}

template<typename Valtype>
inline spat::Transformation<Valtype> TransFrom( const physx::PxTransform& pxtrans ) noexcept{
	return spat::Transformation<Valtype>{ From<Valtype>(pxtrans) };
}

template<typename Valtype>
inline spat::Vector<Valtype> From( const physx::PxVec3& pxv ) noexcept{
	return { Valtype{pxv.x},
			 Valtype{pxv.y},
			 Valtype{pxv.z} };
}

inline spat::Vector<Velocity> VelocityFrom( const physx::PxVec3& pxv, Real engine_meters_per_unit ) noexcept{
	return { _mIs(engine_meters_per_unit*pxv.x),
			 _mIs(engine_meters_per_unit*pxv.y),
			 _mIs(engine_meters_per_unit*pxv.z) };
}

}