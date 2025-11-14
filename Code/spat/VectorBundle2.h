//	trax track library
//	AD 2013 
//
//  "we'll meet again some sunny day"
//
//								Vera Lynn
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

#include "Position.h"
#include "Vector.h"

namespace spat{

	template<typename,typename> struct VectorBundle;
	template<typename,typename> struct Frame;
	template<typename> class Rotation;

	/// \brief Implements a tangential space bundle
	///
	/// Specifies the coordinates of a point in 3D space and two additional Vector. 
	template<typename Valtype,typename ValtypeT = Valtype>
	struct VectorBundle2
	{
		Position<Valtype>	P; ///< Base space postion
		Vector<ValtypeT>	T; ///< Tangent vector or x-axis.
		Vector<ValtypeT>	N; ///< Normal axis or y-axis.


		/// \name Construction
		/// \param x
		/// \param y
		/// \param z
		/// \param dx
		/// \param dy
		/// \param dz
		/// \param pos
		/// \param tan
		/// \param nor
		/// \param bundle
		/// \param rot
		/// \param frame
		///@{
		constexpr			VectorBundle2() noexcept = default;						///< Does not initialize the members.
		constexpr			VectorBundle2( Valtype x, Valtype y, Valtype z,
										   ValtypeT tx, ValtypeT ty, ValtypeT tz,
										   ValtypeT nx, ValtypeT ny, ValtypeT nz ) noexcept;
		constexpr			VectorBundle2( const Position<Valtype>& pos,
										   const Vector<ValtypeT>& tan,
										   const Vector<ValtypeT>& nor ) noexcept;
		constexpr			VectorBundle2( const VectorBundle<Valtype,ValtypeT>& bundle,
										   const Vector<ValtypeT>& nor ) noexcept;
							VectorBundle2( const Position<Valtype>& pos,
										   const Rotation<ValtypeT>& rot ) noexcept;
		explicit constexpr	VectorBundle2( const Frame<Valtype,ValtypeT>& frame ) noexcept;
		explicit			VectorBundle2( const Rotation<ValtypeT>& rot ) noexcept;
		template<typename Valtype2>
		explicit		   VectorBundle2( const Valtype2* pVal ) noexcept;
		///@}


		/// \name Assignment
		/// \param frame
		/// \param rot
		///@{
		VectorBundle2& operator=( const Frame<Valtype,ValtypeT>& frame ) noexcept;

		VectorBundle2& operator=( const Rotation<ValtypeT>& rot ) noexcept;

		template<typename Valtype2>
		VectorBundle2& operator=( const Valtype2* pVal ) noexcept;
		///@}


		/// \brief Implicit conversion
		/// \returns A Vector bundle with the P and T  values.
		operator VectorBundle<Valtype,ValtypeT>() const noexcept;


		/// \brief Pointer accessor for direct copying
		Valtype* ptr() noexcept;


		/// \brief Pointer accessor for direct copying
		const Valtype* ptr() const noexcept;


		/// \brief Initializes the VectorBundle2 structure to position zero and the vectors to
		/// standard right-handed unit vector system.
		VectorBundle2& Init() noexcept;


		/// \brief Initializes the VectorBundle2 structure to vb and calculates nor as to be
		/// orthogonal to up.
		/// \param bundle Values for pos and tan
		/// \param up Vector pointing in up direction
		VectorBundle2& Init( const VectorBundle<Valtype,ValtypeT>& bundle, const Vector<ValtypeT>& up ) noexcept;


		/// \brief Makes the vectors unit vectors.
		VectorBundle2& Normalize() noexcept;


		/// \brief Normalizes the vectors preserving the direction of T as is. It then
		/// adjusts Nor and Bin in a way to form a orthogonal frame.
		VectorBundle2& OrthoNormalize() noexcept;


		/// \param epsilon_length Margin for deviation.
		/// \param epsilon_angle Margin for deviation.
		/// \returns true if the Frame forms an orthonormal frameset.
		bool IsOrthoNormal( ValtypeT epsilon_length = 2*std::numeric_limits<ValtypeT>::epsilon(), ValtypeT epsilon_angle = 2*std::numeric_limits<ValtypeT>::epsilon() ) const noexcept;


		/// \brief Moves the Frame by d in direction T.
		/// \param d Distance to move along T.
		VectorBundle2& TransportTo( Valtype d ) noexcept;


		/// \brief Transport this Frame to p. 
		/// \param p a point in space formulated with respect to this Frame.
		VectorBundle2& TransportTo( const Position2D<Valtype>& p ) noexcept;


		/// \brief Moves the Frame by d in direction T.
		/// \param d Distance to move along T.
		VectorBundle2& TransportTan( Valtype d ) noexcept;


		/// \brief Moves the Frame by d in direction N.
		/// \param d Distance to move along N.
		VectorBundle2& TransportNor( Valtype d ) noexcept;


		///	\brief Rotate the vectors around nr with angle in radiants.
		VectorBundle2& Rotate( const Vector<ValtypeT>& nr, ValtypeT angle ) noexcept;


		///	\brief Rotate the vectors around r with angle r.Length() in radiants.
		VectorBundle2& Rotate( const Vector<ValtypeT>& r ) noexcept;


		/// \brief Rotates the vectors around the tan vector
		VectorBundle2& RotateTan( ValtypeT angle ) noexcept;


		/// \brief Rotates the vectors around the nor vector
		VectorBundle2& RotateNor( ValtypeT angle ) noexcept;


		/// \brief Comparison within some ranges.
		///
		///	The area within two positions regarded as equal is a sphere
		/// with radius epsilon_length; the absolute difference of two vectors will 
		/// at most be epsilon_lengthT. If the two vectors are both unit vectors, the actual
		/// difference between them will be sin(a) with a being the rotational angle. 
		/// This makes a small enough epsilon_lengthT a limit in rotational angle.
		/// \param bundle VectorBundle2 to compare with.
		/// \param epsilon_length epsilon distance <= which two positions are 
		/// regarded as equal. 
		/// \param epsilon_lengthT epsilon for the Vectors of the VectorBundle2s.
		constexpr bool Equals( const VectorBundle2& bundle, 
			Valtype epsilon_length = std::numeric_limits<Valtype>::epsilon(), 
			ValtypeT epsilon_lengthT = std::numeric_limits<ValtypeT>::epsilon() ) const noexcept;
	};

	/// \brief Cast to VectorBundle2 with different underlying value types.
	template<typename Valtype,typename ValtypeT=Valtype,typename Valtype2=Valtype,typename ValtypeT2=Valtype>
	VectorBundle2<Valtype,ValtypeT>& spatial_cast( VectorBundle2<Valtype2,ValtypeT2>& from ) noexcept;

	/// \brief Cast to VectorBundle2 with different underlying value types.
	template<typename Valtype,typename ValtypeT=Valtype,typename Valtype2=Valtype,typename ValtypeT2=Valtype>
	const VectorBundle2<Valtype,ValtypeT>& spatial_cast( const VectorBundle2<Valtype2,ValtypeT2>& from ) noexcept;


	/// \name Mathematical Operators for VectorBundle2
	///@{
	
	/// \brief VectorBundle2 operator.
	template<typename Valtype,typename ValtypeT> constexpr
	bool operator==( const VectorBundle2<Valtype,ValtypeT>& b1, const VectorBundle2<Valtype,ValtypeT>& b2 ) noexcept;
	template<typename Valtype,typename ValtypeT> constexpr
	bool operator!=( const VectorBundle2<Valtype,ValtypeT>& b1, const VectorBundle2<Valtype,ValtypeT>& b2 ) noexcept;
	///@}

///////////////////////////////////////
template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle2<Valtype,ValtypeT>::VectorBundle2(
	Valtype x, Valtype y, Valtype z,
	ValtypeT tx, ValtypeT ty, ValtypeT tz,
	ValtypeT nx, ValtypeT ny, ValtypeT nz ) noexcept
	:	P{x,y,z},
		T{tx,ty,tz},
		N{nx,ny,nz}
{}

template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle2<Valtype,ValtypeT>::VectorBundle2( 
	const Position<Valtype>& pos,
	const Vector<ValtypeT>& tan,
	const Vector<ValtypeT>& nor ) noexcept
	:	P{pos},
		T{tan},
		N{nor}
{}

template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle2<Valtype,ValtypeT>::VectorBundle2(	
	const VectorBundle<Valtype,ValtypeT>& bundle,
	const Vector<ValtypeT>& nor ) noexcept
	:	P{bundle.P},
		T{bundle.T},
		N{nor}
{}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>::VectorBundle2(	
	const Position<Valtype>& pos,
	const Rotation<ValtypeT>& rot ) noexcept
	:	P{pos}
{
	operator=( rot );
}

template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle2<Valtype,ValtypeT>::VectorBundle2( 
	const Frame<Valtype,ValtypeT>& frame ) noexcept
	:	P{frame.P},
		T{frame.T},
		N{frame.N}
{}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>::VectorBundle2( 
	const Rotation<ValtypeT>& rot ) noexcept
{
	operator=( rot );
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::operator=( const Frame<Valtype,ValtypeT>& frame ) noexcept{
	P = frame.P;
	T = frame.T;
	N = frame.N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::operator=( const Rotation<ValtypeT>& rot ) noexcept{
	T.dx = rot(0,0);
	T.dy = rot(0,1);
	T.dz = rot(0,2);

	N.dx = rot(1,0);
	N.dy = rot(1,1);
	N.dz = rot(1,2);

	return *this;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline VectorBundle2<Valtype,ValtypeT>::VectorBundle2( const Valtype2* pVal ) noexcept
	:	P{pVal},
		T{pVal+3},
		N{pVal+6}
{
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline VectorBundle2<Valtype,ValtypeT> & VectorBundle2<Valtype,ValtypeT>::operator=( const Valtype2* pVal ) noexcept{
	P = pVal;
	T = pVal + 3;
	N = pVal + 6;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>::operator VectorBundle<Valtype,ValtypeT>() const noexcept{
	return VectorBundle<Valtype,ValtypeT>{ *this };
}

template<typename Valtype,typename ValtypeT>
inline Valtype* VectorBundle2<Valtype,ValtypeT>::ptr() noexcept{
	static_assert( sizeof(Valtype) == sizeof(ValtypeT), "Can not access a VectorBundle via pointer if the two types have different memory sizes.");
	return P.ptr();
}

template<typename Valtype,typename ValtypeT>
inline const Valtype * VectorBundle2<Valtype,ValtypeT>::ptr() const noexcept{
	static_assert( sizeof(Valtype) == sizeof(ValtypeT), "Can not access a VectorBundle via pointer if the two types have different memory sizes.");
	return P.ptr();
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::Init() noexcept{
	P.Init();
	T.Init( Vector<ValtypeT>::specials::ex );
	N.Init( Vector<ValtypeT>::specials::ey );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::Init( const VectorBundle<Valtype,ValtypeT>& vb, const Vector<ValtypeT>& up ) noexcept{
	P = vb.P;
	T = vb.T;

	N = up % T;
	if( N.Normalize() == 0 )
	// up and tan are parallel
	{
		N.Init( Vector<ValtypeT>::specials::ey );
		N -= (N*up)*up;
		if( N.Normalize() == 0 )
		// up and ey are parallel
		{
			N.Init( Vector<ValtypeT>::specials::ex );
			N -= (N*up)*up;
			N.Normalize();
			assert(N.Length() != 0); // should not be possible...
		}
	}
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::Normalize() noexcept{
	T.Normalize();
	N.Normalize();
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::OrthoNormalize() noexcept{
	if ( T.Is(Vector<ValtypeT>::specials::null) )
	{
		T.dx = ValtypeT{1};
		N = Ey<ValtypeT>;
	}
	T.Normalize();
	N -= (N * T) * T;

	if ( N.Is(Vector<ValtypeT>::specials::null) )
	{
		if ( !(T % Ey<ValtypeT>).Is(Vector<ValtypeT>::specials::null) )
			N = Ey<ValtypeT> - (Ey<ValtypeT> * T) * T;
		else
			N = Ex<ValtypeT> - (Ex<ValtypeT> * T) * T;
	}

	N.Normalize();
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline bool VectorBundle2<Valtype,ValtypeT>::IsOrthoNormal( ValtypeT _epsilon_length, ValtypeT _epsilon_angle ) const noexcept{
	return	T.IsNormal( _epsilon_length ) &&
			N.IsNormal( _epsilon_length ) &&
			std::abs( T*N ) <= _epsilon_angle;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::TransportTo( Valtype d ) noexcept{
	P += d * T;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::TransportTo( const Position2D<Valtype>& p ) noexcept{
	P += p.x * T + p.y * N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::TransportTan( Valtype dist ) noexcept{
	P += dist * T;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::TransportNor( Valtype dist ) noexcept{
	P += dist * N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::Rotate( const Vector<ValtypeT>& nr, ValtypeT angle ) noexcept{
	T.Rotate( nr, angle );
	N.Rotate( nr, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::Rotate( const Vector<ValtypeT>& rot ) noexcept{
	Vector<ValtypeT> r(rot);
	ValtypeT angle = r.Normalize();
	T.Rotate( r, angle );
	N.Rotate( r, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::RotateTan( ValtypeT angle ) noexcept{
	N.Rotate( T, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle2<Valtype,ValtypeT>& VectorBundle2<Valtype,ValtypeT>::RotateNor( ValtypeT angle ) noexcept{
	T.Rotate( N, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT> constexpr
inline bool VectorBundle2<Valtype,ValtypeT>::Equals( const VectorBundle2& bundle, Valtype _epsilon_length, ValtypeT epsilon_lengthT ) const noexcept{
	return	P.Equals( bundle.P, _epsilon_length ) &&
			T.Equals( bundle.T, epsilon_lengthT ) &&
			N.Equals( bundle.N, epsilon_lengthT );
}

template<typename Valtype,typename ValtypeT,typename Valtype2,typename ValtypeT2>
inline VectorBundle2<Valtype,ValtypeT>& spatial_cast( VectorBundle2<Valtype2,ValtypeT2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	static_assert( sizeof(ValtypeT) == sizeof(ValtypeT2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<VectorBundle2<Valtype,ValtypeT>&>(from);
}


template<typename Valtype,typename ValtypeT,typename Valtype2,typename ValtypeT2>
inline const VectorBundle2<Valtype,ValtypeT>& spatial_cast( const VectorBundle2<Valtype2,ValtypeT2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	static_assert( sizeof(ValtypeT) == sizeof(ValtypeT2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<const VectorBundle2<Valtype,ValtypeT>&>(from);
}

template<typename Valtype,typename ValtypeT> constexpr
inline bool operator ==( const VectorBundle2<Valtype,ValtypeT>& b1, const VectorBundle2<Valtype,ValtypeT>& b2 ) noexcept{
	return !(b1 != b2);
}

template<typename Valtype,typename ValtypeT> constexpr
inline bool operator !=( const VectorBundle2<Valtype,ValtypeT>& b1, const VectorBundle2<Valtype,ValtypeT>& b2 ) noexcept{
	return	b1.P != b2.P || 
			b1.T != b2.T || 
			b1.N != b2.N;
}

} // namespace spat
