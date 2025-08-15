// Spatial Coordinates And Vectors Library (spat)
// 
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2013 
//
//  "Die Stadt muss brennen"
//
//					Casper

#pragma once

#include "Position.h"
#include "Vector.h"

namespace spat{

	template<typename,typename> struct VectorBundle2;
	template<typename,typename> struct Frame;

	/// \brief Implements a Vector bundle
	///
	/// Specifies the coordinates of a point in 3D space and an additional Vector. 
	template<typename Valtype,typename ValtypeT = Valtype>
	struct VectorBundle
	{
		Position<Valtype>	P; ///< Base space postion
		Vector<ValtypeT>	T; ///< Tangent vector or x-axis.


		/// \name Construction
		/// \param pos
		/// \param tan
		/// \param x
		/// \param y
		/// \param z
		/// \param dx
		/// \param dy
		/// \param dz
		/// \param rot
		/// \param frame
		///@{
		constexpr			VectorBundle() noexcept = default;						///< Does not initialize the members.	
		explicit constexpr  VectorBundle( const Position<Valtype>& pos ) noexcept;
		constexpr			VectorBundle( const Position<Valtype>& pos,
										  const Vector<ValtypeT>& tan ) noexcept;
		constexpr			VectorBundle( Valtype x, Valtype y, Valtype z,
										  ValtypeT dx, ValtypeT dy, ValtypeT dz ) noexcept;
		explicit constexpr	VectorBundle( const VectorBundle2<Valtype,ValtypeT>& bundle ) noexcept;
		explicit constexpr	VectorBundle( const Frame<Valtype,ValtypeT>& frame ) noexcept;
		template<typename Valtype2>
		explicit		    VectorBundle( const Valtype2* const pVal ) noexcept;
		///@}


		/// \name Assignment
		/// \param frame
		///@{
		VectorBundle& operator= ( const Frame<Valtype,ValtypeT>& frame ) noexcept;

		template<typename Valtype2>
		VectorBundle& operator=( const Valtype2* pVal ) noexcept;
		///@}


		/// \brief Pointer accessor for direct copying
		Valtype* ptr() noexcept;


		/// \brief Pointer accessor for direct copying
		const Valtype* ptr() const noexcept;


		/// \brief Initializes the VectorBundle structure to position zero and the ex Vector
		VectorBundle& Init() noexcept;


		/// \brief Initializes the VectorBundle structure to vb
		/// \param vb Values for pos and tan
		VectorBundle& Init( const VectorBundle& vb ) noexcept;


		/// \brief Moves the Frame by d in direction T.
		/// \param d Distance to move along T.
		VectorBundle& TransportTo( Valtype d ) noexcept;


		/// \brief Transport this Frame by v.
		/// \param v a vector to translate this frames pos value by.
		VectorBundle& TransportBy( const Vector<Valtype>& v ) noexcept;


		///	\brief Rotate the tan around nr with angle in radiants.
		VectorBundle& Rotate( const Vector<Valtype>& nr, Valtype angle ) noexcept;


		///	\brief Rotate the tan around r with angle r.Length() in radiants.
		VectorBundle& Rotate( const Vector<Valtype>& r ) noexcept;


		/// \brief Comparison within some ranges.
		///
		///	The area within two positions regarded as equal is a sphere
		/// with radius epsilon_length; the absolute difference of two vectors will 
		/// at most be epsilon_lengthT. If the two vectors are both unit vectors, the actual
		/// difference between them will be sin(a) with a being the rotational angle. 
		/// This makes a small enough epsilon_lengthT a limit in rotational angle.
		/// \param bundle VectorBundle to compare with.
		/// \param epsilon_length epsilon distance <= which two positions are 
		/// regarded as equal. 
		/// \param epsilon_lengthT epsilon for the Vectors of the VectorBundles.
		constexpr bool Equals( const VectorBundle& bundle, 
			Valtype epsilon_length = std::numeric_limits<Valtype>::epsilon(), 
			ValtypeT epsilon_lengthT = std::numeric_limits<ValtypeT>::epsilon() ) const noexcept;
	};

	/// \brief Different name for VectorBundle.
	template<typename Valtype,typename ValtypeT = Valtype>
	using VectorBundle1 = VectorBundle<Valtype,ValtypeT>;

	/// \brief Cast to VectorBundle with different underlying value types.
	template<typename Valtype,typename ValtypeT=Valtype,typename Valtype2=Valtype,typename ValtypeT2=Valtype>
	VectorBundle<Valtype,ValtypeT>& spatial_cast( VectorBundle<Valtype2,ValtypeT2>& from ) noexcept;

	/// \brief Cast to VectorBundle with different underlying value types.
	template<typename Valtype,typename ValtypeT=Valtype,typename Valtype2=Valtype,typename ValtypeT2=Valtype>
	const VectorBundle<Valtype,ValtypeT>& spatial_cast( const VectorBundle<Valtype2,ValtypeT2>& from ) noexcept;


	/// \name Mathematical Operators for VectorBundle
	///@{
	template<typename Valtype,typename ValtypeT> constexpr
	bool operator ==( const VectorBundle<Valtype,ValtypeT>& b1, const VectorBundle<Valtype,ValtypeT>& b2 ) noexcept;
	template<typename Valtype,typename ValtypeT> constexpr 
	bool operator !=( const VectorBundle<Valtype,ValtypeT>& b1, const VectorBundle<Valtype,ValtypeT>& b2 ) noexcept;
	///@}


	/// \returns the minimal distance of the two lines defined by the two VectorBundles and the point on
	/// the first line that is closest to the second line.
	template<typename Valtype,typename ValtypeT = Valtype>
	VectorBundle1<Valtype,Valtype> Distance( const VectorBundle1<Valtype,ValtypeT>& b1, const VectorBundle1<Valtype,ValtypeT>& b2 ) noexcept;


template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle<Valtype,ValtypeT>::VectorBundle( const Position<Valtype>& pos ) noexcept
	:	P{pos},
		T{Vector<ValtypeT>::specials::ex}
{}

template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle<Valtype,ValtypeT>::VectorBundle( 
	const Position<Valtype>& pos,
	const Vector<ValtypeT>& tan ) noexcept
	:	P{pos},
		T{tan}
{}

template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle<Valtype,ValtypeT>::VectorBundle(	
				Valtype x, Valtype y, Valtype z,
				ValtypeT dx, ValtypeT dy, ValtypeT dz ) noexcept
	:	P{x,y,z},
		T{dx,dy,dz}
{}

template<typename Valtype,typename ValtypeT>
inline constexpr VectorBundle<Valtype,ValtypeT>::VectorBundle( const VectorBundle2<Valtype,ValtypeT>& bundle ) noexcept
	:	P{bundle.P},
		T{bundle.T}
{}

template<typename Valtype,typename ValtypeT> constexpr
inline VectorBundle<Valtype,ValtypeT>::VectorBundle( const Frame<Valtype,ValtypeT>& frame ) noexcept
	:	P{frame.P},
		T{frame.T}
{}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline VectorBundle<Valtype,ValtypeT>::VectorBundle( const Valtype2* const pVal ) noexcept
	:	P{pVal},
		T{pVal+3}
{
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::operator=( const Valtype2* pVal ) noexcept{
	P = pVal;
	T = pVal + 3;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::operator=( const Frame<Valtype,ValtypeT>& frame ) noexcept{
	P = frame.P;
	T = frame.T;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Valtype* VectorBundle<Valtype,ValtypeT>::ptr() noexcept{
	static_assert( sizeof(Valtype) == sizeof(ValtypeT), "Can not access a VectorBundle via pointer if the two types have different memory sizes." );
	return P.ptr();
}

template<typename Valtype,typename ValtypeT>
inline const Valtype* VectorBundle<Valtype,ValtypeT>::ptr() const noexcept{
	static_assert( sizeof(Valtype) == sizeof(ValtypeT), "Can not access a VectorBundle via pointer if the two types have different memory sizes.");
	return P.ptr();
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::Init() noexcept{
	P.Init();
	T.Init();
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::Init( const VectorBundle& bundle ) noexcept{
	P = bundle.P;
	T = bundle.T;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::TransportTo( Valtype d ) noexcept{
	P += d * T;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::TransportBy( const Vector<Valtype>& v ) noexcept{
	P.Transport( v );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::Rotate( const Vector<Valtype>& nr, Valtype angle ) noexcept{
	T.Rotate( nr, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline VectorBundle<Valtype,ValtypeT>& VectorBundle<Valtype,ValtypeT>::Rotate( const Vector<Valtype>& r ) noexcept{
	T.Rotate( r );
	return *this;
}

template<typename Valtype,typename ValtypeT> constexpr
bool VectorBundle<Valtype,ValtypeT>::Equals( const VectorBundle& bundle, Valtype epsilon_length_, ValtypeT epsilon_lengthT ) const noexcept{
	return	P.Equals( bundle.P, epsilon_length_ ) && 
			T.Equals( bundle.T, epsilon_lengthT );
}

template<typename Valtype,typename ValtypeT,typename Valtype2,typename ValtypeT2>
inline VectorBundle<Valtype,ValtypeT>& spatial_cast( VectorBundle<Valtype2,ValtypeT2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	static_assert( sizeof(ValtypeT) == sizeof(ValtypeT2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<VectorBundle<Valtype,ValtypeT>&>(from);
}

template<typename Valtype,typename ValtypeT,typename Valtype2,typename ValtypeT2>
inline const VectorBundle<Valtype,ValtypeT>& spatial_cast( const VectorBundle<Valtype2,ValtypeT2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	static_assert( sizeof(ValtypeT) == sizeof(ValtypeT2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<const VectorBundle<Valtype,ValtypeT>&>(from);
}

template<typename Valtype,typename ValtypeT> constexpr
bool operator ==( const VectorBundle<Valtype,ValtypeT>& b1, const VectorBundle<Valtype,ValtypeT>& b2 ) noexcept{
	return !(b1 != b2);
}

template<typename Valtype,typename ValtypeT> constexpr
bool operator !=( const VectorBundle<Valtype,ValtypeT>& b1, const VectorBundle<Valtype,ValtypeT>& b2 ) noexcept{
	if( b1.P != b2.P ||
		b1.T != b2.T )
		return true;

	return false;
}

template<typename Valtype, typename ValtypeT>
VectorBundle1<Valtype,Valtype> Distance( const VectorBundle1<Valtype,ValtypeT>& b1, const VectorBundle1<Valtype,ValtypeT>& b2 ) noexcept
// d : result
// 
// d*n1 == 0
// d*n2 == 0
// d = D + s2n2 - s1n1
// 
// -> s1 = D*(n1 - n2*n1n2) / (1 - n1n2*n1n2)
//    s2 = D*(n1*n1n2 - n2) / (1 - n1n2*n1n2)
{
	Vector<Valtype> D = b2.P - b1.P;
	Vector<ValtypeT> n1 = Normalize(b1.T).second;
	Vector<ValtypeT> n2 = Normalize(b2.T).second;
	ValtypeT n1n2 = n1 * n2;

	if( n1n2 == ValtypeT{1} )
		return { b1.P, D - D*n1*n1 };
	else{
		Valtype s1 = D*(n1 - n2*n1n2) / (1 - n1n2*n1n2);
		Valtype s2 = D*(n1*n1n2 - n2) / (1 - n1n2*n1n2);

		return { b1.P + s1*n1, D + s2*n2 - s1*n1 };
	}
}

//{
//	Vector<ValtypeT> n = b1.T % b2.T;
//	if( n == Null<ValtypeT> )
//		return ((b2.P - b1.P) % b2.T).Length() / b2.T.Length();
//
//	return abs((b2.P - b1.P) * n) / n.Length();
//}

} // namespace spat
