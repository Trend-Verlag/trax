//	trax track library
//	AD 2013 
//
//	"Hojotoh! Hojotoh! Hoooo! Hojohoooo!"
//
//								Richard Wagner
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
#include "Box.h"
#include "Sphere.h"

#include <cmath>

namespace spat{

	template<typename,typename> struct VectorBundle;
	template<typename,typename> struct VectorBundle2;

	template<typename> class Rotation;
	template<typename,const unsigned short,const unsigned short> class Matrix;
	
	///	\brief A Frame ("TNBFrame") describes a location in 3d space
	///	and an orientation using a right handed coordinate cross of vectors.
	///	
	///	These axes normaly are used right handed. If tan points in the
	///	direction of the thump of the right hand and the index finger points along
	///	the nor vector, then bin points along the middle finger.
	template<typename Valtype,typename ValtypeT = Valtype>
	struct Frame
	{
		Position<Valtype>	P;	///< Point in 3D space.
		Vector<ValtypeT>	T;	///< Tangential axis or x-axis.
		Vector<ValtypeT>	N;	///< Normal axis or y-axis.
		Vector<ValtypeT>	B;	///< Binormal axis or z-axis.


		/// \name Construction
		/// \param vb2 VectorBundle2 object giving the P,T,N part; B gets computed.
		/// \param vb VectorBundle object giving the P,T part; N and B get computed.
		/// \param up Up direction. N gets calculated so that it is perpendicular to this.
		/// \param pos Position object giving the P part.
		/// \param tan Vector object giving the T part.
		/// \param nor Vector object giving the N part.
		/// \param bin Vector object giving the B part.
		/// \param rot Rotation object giving the T,N and B part.
		/// \param trans Transformation object. Compute the Frame so that it is the 
		/// frame of reference corresponding to the transformation.
		/// \param pHTrans Pointer to homogenuous transformation matrix. The first three 
		/// columns of the transformation matrix are the transformed global axis 
		/// vectors. The position is the transformed homogen origin (0,0,0,1).
		///@{
				 constexpr Frame() noexcept = default;						///< Does not initialize the members.
				 constexpr Frame( Valtype px, Valtype py, Valtype pz,
								  ValtypeT tx, ValtypeT ty, ValtypeT tz, 
								  ValtypeT nx, ValtypeT ny, ValtypeT nz, 
								  ValtypeT bx, ValtypeT by, ValtypeT bz ) noexcept;
		explicit constexpr Frame( const VectorBundle2<Valtype,ValtypeT>& vb2 ) noexcept;
						   Frame( const VectorBundle<Valtype,ValtypeT>& vb, const Vector<ValtypeT>& up ) noexcept;
		explicit constexpr Frame( const Position<Valtype>& pos ) noexcept;
				 constexpr Frame( const Position<Valtype>& pos,
								  const Vector<ValtypeT>& tan,
								  const Vector<ValtypeT>& nor,
								  const Vector<ValtypeT>& bin ) noexcept;
						   Frame( const Position<Valtype>& pos,
								  const Rotation<ValtypeT>& rot ) noexcept;
		explicit		   Frame( const Rotation<ValtypeT>& rot ) noexcept;
		template<typename Valtype2>		
		explicit		   Frame( const Matrix<Valtype2,4,4>& trans ) noexcept;
		template<typename Valtype2>
		explicit		   Frame( const Valtype2* pVal ) noexcept;
		///@}


		/// \name Assignment
		/// \param vb2 VectorBundle2 object giving the P,T,N part; B gets computed.
		/// \param rot Rotation object giving the T,N and B part.
		/// \param trans Transformation object. Compute the Frame so that it is the 
		/// frame of reference corresponding to the transformation.
		///@{
		// no assignment for VectorBundle because its behaviour would not be obvious...
		Frame& operator=( const VectorBundle2<Valtype,ValtypeT>& vb2 ) noexcept;

		Frame& operator=( const Rotation<ValtypeT>& rot ) noexcept;

		template<typename Valtype2>
		Frame& operator=( const Matrix<Valtype2,4,4>& trans ) noexcept;

		template<typename Valtype2>
		Frame& operator=( const Valtype2* pVal ) noexcept;
		///@}


		/// \name Implicit conversion
		/// \returns A Vector2 bundle with the P, T and N values.
		///@{
		operator VectorBundle2<Valtype,ValtypeT>() const noexcept;

		operator VectorBundle<Valtype,ValtypeT>() const noexcept;
		///@}


		/// \brief Pointer accessor for direct copying
		Valtype* ptr() noexcept;


		/// \brief Pointer accessor for direct copying
		const Valtype* ptr() const noexcept;


		/// \brief Initializes the Frame structure to position (or zero) and the vectors to
		/// standard right-handed orthogonal unit vector system.
		///@{
		Frame& Init() noexcept;

		Frame& Init( const Position<Valtype>& pos ) noexcept;
		///@}

		/// \brief Initializes the Frame structure to vb and calculates nor as to be
		/// orthogonal to up.
		/// \param vb Values for pos and tan.
		/// \param up Vector pointing in up direction.
		Frame& Init( const VectorBundle<Valtype,ValtypeT>& vb, const Vector<ValtypeT>& up ) noexcept;


		/// \brief Initializes the Frame structure.
		/// \param vb2 VectorBundle2 object giving the P,T,N part; B gets computed.
		Frame& Init( const VectorBundle2<Valtype,ValtypeT>& vb2 ) noexcept;


		// use Matrix.h Invert( Frame<Valtype,ValtypeT>& frame )
		/////	\brief Invert this Frame so that F*Fi == Init().
		///// \returns A reference to this object.
		///Frame& Invert();


		/// \name LookAt
		/// \brief Rotate the Frame so that a reference vector points along dir.
		///
		/// \param dir Direction to point to.
		/// \param reference The vector formulated in the frame reference,
		/// that is supposed to point in the direction of dir
		/// \param up Direction to bring B to by a final rotation.
		///@{
		Frame& LookAt( const Vector<ValtypeT>& reference, const Vector<ValtypeT>& dir ) noexcept;

		Frame& LookTAt( const Vector<ValtypeT>& dir ) noexcept;

		Frame& LookTAt( const Vector<ValtypeT>& dir, const Vector<ValtypeT>& up ) noexcept;

		Frame& LookNAt( const Vector<ValtypeT>& dir ) noexcept;

		Frame& LookBAt( const Vector<ValtypeT>& dir ) noexcept;
		///@}


		/// \name AircraftPrincipalAxes
		/// \brief Calculate the vectors as orthonormal vectors from Yaw, Pitch and Roll.
		/// 
		/// The angles used in aviation refer to a right handed coordinate system and rotate
		/// Yaw, Pitch and Roll in the order of Z, Y, and X following a right handed orientation 
		/// rule. Note that in order to keep the Pitch positive on upward rotation, aeroplanes
		/// are typically oriented with the Z-axis down in the frame.
		/// 
		/// \param yaw The Yaw angle (rotation around Z).
		/// \param pitch The Pitch angle (rotation around rotated Y).
		/// \param roll The Roll angle (rotation around rotated X).
		/// \param epsilon If T is epsilon-close to Ez, the yaw will be zero and Pitch +- pi/2.
		/// \returns Yaw, Pitch, Roll in a Position struct.
		///@{
		Frame& AircraftPrincipalAxes( ValtypeT yaw, ValtypeT pitch, ValtypeT roll );

		Frame& AircraftPrincipalAxes( const Position<ValtypeT>& angles );

		Position<ValtypeT> AircraftPrincipalAxes( ValtypeT epsilon = std::numeric_limits<ValtypeT>::epsilon() ) const;
		///@}


		/// \brief Makes the vectors unit vectors.
		Frame& Normalize() noexcept;


		/// \brief Normalizes the vectors preserving the direction of T as is. It then
		/// adjusts Nor and Bin in a way to form an orthogonal frame.
		Frame& OrthoNormalize() noexcept;


		/// \param epsilon_length Margin for deviation.
		/// \param epsilon_angle Margin for deviation.
		/// \returns true if the Frame forms an orthonormal frameset.
		bool IsOrthoNormal( ValtypeT epsilon_length = 2*std::numeric_limits<ValtypeT>::epsilon(), ValtypeT epsilon_angle = 2*std::numeric_limits<ValtypeT>::epsilon() ) const noexcept;


		/// \name Transformations to parent frame.
		/// \brief Transformation to the coordinate system in wich this Frame
		/// is formulated. 
		///
		/// The coordinates are understood to be formulated relative to
		/// this Frame and get calculated to be formulated to the base Frame.
		///@{
		template<typename Valtype2> 
		Position<Valtype2>& ToParent( Position<Valtype2>& p ) const noexcept;

		template<typename Valtype2> 
		Vector<Valtype2>& ToParent( Vector<Valtype2>& v ) const noexcept;

		template<typename Valtype2> 
		Vector<Valtype2>& NormalToParent( Vector<Valtype2>& normal ) const noexcept;

		template<typename Valtype2,typename ValtypeT2> 
		VectorBundle<Valtype2,ValtypeT2>& ToParent( VectorBundle<Valtype2,ValtypeT2>& v ) const noexcept;

		template<typename Valtype2,typename ValtypeT2> 
		VectorBundle2<Valtype2,ValtypeT2>& ToParent( VectorBundle2<Valtype2,ValtypeT2>& v ) const noexcept;

		template<typename Valtype2,typename ValtypeT2> 
		Frame<Valtype2,ValtypeT2>& ToParent( Frame<Valtype2,ValtypeT2>& frame ) const noexcept;

		template<typename Valtype2> 
		Box<Valtype2>& ToParent( Box<Valtype2>& box ) const noexcept;

		template<typename Valtype2> 
		Sphere<Valtype2>& ToParent( Sphere<Valtype2>& sphere ) const noexcept;
		///@}


		/// \name Transformations from parent frame.
		/// \brief Transformation from the coordinate system in wich this Frame
		/// is formulated. 
		///
		/// The coordinates are understood to be formulated relative to
		/// the parent Frame and get calculated to be formulated
		/// relative to this Frame.
		///@{
		template<typename Valtype2> 
		Position<Valtype2>& FromParent( Position<Valtype2>& p ) const noexcept;

		template<typename Valtype2> 
		Vector<Valtype2>& FromParent( Vector<Valtype2>& v ) const noexcept;

		template<typename Valtype2,typename ValtypeT2> 
		VectorBundle<Valtype2,ValtypeT2>& FromParent( VectorBundle<Valtype2,ValtypeT2>& b ) const noexcept;

		template<typename Valtype2,typename ValtypeT2> 
		VectorBundle2<Valtype2,ValtypeT2>& FromParent( VectorBundle2<Valtype2,ValtypeT2>& b ) const noexcept;

		template<typename Valtype2,typename ValtypeT2> 
		Frame<Valtype2,ValtypeT2>& FromParent( Frame<Valtype2,ValtypeT2>& frame ) const noexcept;
		///@}


		/// \brief Moves the Frame by d in direction T.
		/// \param d Distance to move along T.
		Frame& TransportTo( Valtype d ) noexcept;


		/// \brief Moves the Frame by v.
		/// \param v Vector formulated in parent Frame, giving the distance to
		/// move P
		Frame& TransportBy( const Vector<Valtype>& v ) noexcept;


		/// \brief Transport this Frame to p. 
		/// \param p a point in space formulated with respect to this Frame.
		Frame& TransportTo( const Position<Valtype>& p ) noexcept;


		/// \brief Moves the Frame by d in direction T.
		/// \param d Distance to move along T.
		Frame& TransportTan( Valtype d ) noexcept;


		/// \brief Moves the Frame by d in direction N.
		/// \param d Distance to move along N.
		Frame& TransportNor( Valtype d ) noexcept;


		/// \brief Moves the Frame by d in direction B.
		/// \param d Distance to move along B.
		Frame& TransportBin( Valtype d ) noexcept;


		/// \brief Rotates the Frame by pi around T.
		Frame& FlipTan() noexcept;


		/// \brief Rotates the Frame by pi around N.
		Frame& FlipNor() noexcept;


		/// \brief Rotates the Frame by pi around B.
		Frame& FlipBin() noexcept;


		///	\brief Rotate the vectors around nr with angle in radiants.
		Frame& Rotate( const Vector<ValtypeT>& nr, ValtypeT angle ) noexcept;


		///	\brief Rotate the vectors around r with angle r.Length() in radiants.
		Frame& Rotate( const Vector<ValtypeT>& r ) noexcept;
		

		/// \brief Rotates the frame by axis (length of axis.T make angle in radiants)
		/// \tparam Valtype2 template parameter of axis.T
		/// \param axis rotation axis
		template<typename Valtype2>
		Frame& Rotate( const VectorBundle<Valtype,Valtype2>& axis ) noexcept;


		/// \brief Rotates the frame by axis and angle in radiants.
		/// \tparam Valtype2 template parameter of axis.T and angle
		/// \param axis rotation axis
		/// \param angle rotation angle in radiants
		template<typename Valtype2>
		Frame& Rotate( const VectorBundle<Valtype,Valtype2>& axis, Valtype2 angle ) noexcept;


		/// \brief Rotates the vectors around the tan vector (roll).
		/// \param angle rotation angle in radiants
		Frame& RotateTan( ValtypeT angle ) noexcept;
		

		/// \brief Brings B as close as possible to up by rotating around T.
		Frame& RotateTan( const Vector<ValtypeT>& up ) noexcept;
		

		/// \brief Rotates the vectors around the nor vector (-pitch)
		/// \param angle rotation angle in radiants
		Frame& RotateNor( ValtypeT angle ) noexcept;
		

		/// \brief Rotates the vectors around the bin vector (-yaw)
		/// \param angle rotation angle in radiants
		Frame& RotateBin( ValtypeT angle ) noexcept;


		/// \brief Comparison within some ranges.
		///
		///	The area within two positions regarded as equal is a sphere
		/// with radius epsilon_length; the absolute difference of two vectors will 
		/// at most be epsilon_lengthT. If the two vectors are both unit vectors, the actual
		/// difference between them will be sin(a) with a being the rotational angle. 
		/// This makes a small enough epsilon_lengthT a limit in rotational angle.
		/// \param frame Frame to compare with.
		/// \param epsilon_length epsilon distance <= which two positions are 
		/// regarded as equal. 
		/// \param epsilon_lengthT epsilon for the Vectors of the Frames.
		constexpr bool Equals( const Frame& frame, 
			Valtype epsilon_length = std::numeric_limits<Valtype>::epsilon(), 
			ValtypeT epsilon_lengthT = std::numeric_limits<ValtypeT>::epsilon() ) const noexcept;


		/// \brief Rounds the frames position and vectors to a certain digit after the point.
		Frame& Round( int toDigit ) noexcept;
	};

	/// \brief Different name for Frame.
	template<typename Valtype,typename ValtypeT = Valtype>
	using VectorBundle3 = Frame<Valtype,ValtypeT>;


	/// \brief Identity frame.
	template<typename Valtype,typename ValtypeT=Valtype>
	constexpr Frame<Valtype,ValtypeT> Identity{ Origin3D<Valtype>, Ex<ValtypeT>, Ey<ValtypeT>, Ez<ValtypeT> };

	/// \brief Cast to Frame with different underlying value types.
	template<typename Valtype,typename ValtypeT=Valtype,typename Valtype2=Valtype,typename ValtypeT2=Valtype>
	Frame<Valtype,ValtypeT>& spatial_cast( Frame<Valtype2,ValtypeT2>& from ) noexcept;

	/// \brief Cast to Frame with different underlying value types.
	template<typename Valtype,typename ValtypeT=Valtype,typename Valtype2=Valtype,typename ValtypeT2=Valtype>
	const Frame<Valtype,ValtypeT>& spatial_cast( const Frame<Valtype2,ValtypeT2>& from ) noexcept;

	/// \brief Comparison
	/// \returns true if the two frames are equal.
	template<typename Valtype,typename ValtypeT=Valtype> constexpr
	bool operator==( const Frame<Valtype,ValtypeT>& frame1, const Frame<Valtype,ValtypeT>& frame2 ) noexcept;


	/// \brief Comparison
	/// \returns true if the two frames are inequal.
	template<typename Valtype,typename ValtypeT=Valtype> constexpr
	bool operator!=( const Frame<Valtype,ValtypeT>& frame1, const Frame<Valtype,ValtypeT>& frame2 ) noexcept;


	/// \name Transformation
	/// \brief Transformation of an element to a parent coordinate system.
	/// \param frame Frame to transform element to.
	/// \param p Position to get the transformed for.
	/// \param v Vector to get the transformed for.
	/// \param vb VectorBundle to get the transformed for.
	/// \param vb2 VectorBundle2 to get the transformed for.
	/// \param f Frame to get the transformed for.
	/// \returns Transformed element.
	///@{
	template<typename Valtype1,typename ValtypeT1,typename Valtype2>
	Position<Valtype2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const Position<Valtype2>& p ) noexcept;

	template<typename Valtype1,typename ValtypeT1,typename ValtypeT2>
	Vector<ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const Vector<ValtypeT2>& v ) noexcept;

	template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2>
	VectorBundle<Valtype2,ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const VectorBundle<Valtype2,ValtypeT2>& vb ) noexcept;

	template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2>
	VectorBundle2<Valtype2,ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const VectorBundle2<Valtype2,ValtypeT2>& vb2 ) noexcept;

	template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2>
	Frame<Valtype2,ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const Frame<Valtype2,ValtypeT2>& f ) noexcept;
	
	template<typename Valtype1,typename ValtypeT1,typename Valtype2>
	Sphere<Valtype2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const Sphere<Valtype2>& sp ) noexcept;

	template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2>
	Frame<Valtype2,ValtypeT2>& operator*=( Frame<Valtype1,ValtypeT1>& frame, const Frame<Valtype2,ValtypeT2>& f ) noexcept; ///< right side multiplication of f to frame.
	///@}

	/// \brief Scale the vectors of Frame by a factor.
	///
	/// The positional part remains unchanged.
	template<typename Valtype,typename ValtypeT> 
	Frame<Valtype,ValtypeT>& operator*=( Frame<Valtype,ValtypeT>& frame, ValtypeT param ) noexcept;


	/// \brief Outer normalizing
	/// \returns The length of the Vectors as a dimensionated value in a Vector and a Frame with vectors of unit length with dimension One.
	template<typename Valtype,typename ValtypeT> 
	auto Normalize( const Frame<Valtype,ValtypeT>& f ) noexcept -> std::pair<Vector<ValtypeT>,Frame<Valtype,decltype(ValtypeT{}/ValtypeT{})>>;


template<typename Valtype,typename ValtypeT> constexpr
inline Frame<Valtype,ValtypeT>::Frame(
	Valtype px, Valtype py, Valtype pz,
	ValtypeT tx, ValtypeT ty, ValtypeT tz, 
	ValtypeT nx, ValtypeT ny, ValtypeT nz, 
	ValtypeT bx, ValtypeT by, ValtypeT bz ) noexcept
	:	P{px, py, pz},
		T{tx, ty, tz},
		N{nx, ny, nz},
		B{bx, by, bz}
{}

template<typename Valtype,typename ValtypeT> constexpr
inline Frame<Valtype,ValtypeT>::Frame( const VectorBundle2<Valtype,ValtypeT>& vb2 ) noexcept
	:	P{vb2.P},
		T{vb2.T},
		N{vb2.N},
		B{vb2.T % vb2.N}
{}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>::Frame( const VectorBundle<Valtype,ValtypeT>& vb, const Vector<ValtypeT>& up ) noexcept
{
	Init( vb, up );
}


template<typename Valtype,typename ValtypeT> constexpr
inline Frame<Valtype,ValtypeT>::Frame( const Position<Valtype>& pos ) noexcept
	:	P{pos},
		T{Vector<ValtypeT>::specials::ex},
		N{Vector<ValtypeT>::specials::ey},
		B{Vector<ValtypeT>::specials::ez}
{}

template<typename Valtype,typename ValtypeT> constexpr
inline Frame<Valtype,ValtypeT>::Frame(	const Position<Valtype>& pos,
										const Vector<ValtypeT>& tan,
										const Vector<ValtypeT>& nor,
										const Vector<ValtypeT>& bin ) noexcept
	:	P{pos},
		T{tan},
		N{nor},
		B{bin}
{}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>::Frame( 
		const Position<Valtype>& pos,
		const Rotation<ValtypeT>& rot ) noexcept
	: P{pos}
{
	operator=( rot );
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>::Frame( const Rotation<ValtypeT>& rot ) noexcept{
	P.Init();
	operator=( rot );
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline Frame<Valtype,ValtypeT>::Frame( const Matrix<Valtype2,4,4>& trans ) noexcept{
	operator=( trans );
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline Frame<Valtype,ValtypeT>::Frame( const Valtype2* pVal ) noexcept
	:	P{pVal},
		T{pVal+3},
		N{pVal+6},
		B{pVal+9}
{
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::operator=( const VectorBundle2<Valtype,ValtypeT>& vb2 ) noexcept{
	P = vb2.P;
	T = vb2.T;
	N = vb2.N;
	B = T % N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::operator=( const Rotation<ValtypeT>& rot ) noexcept{
	T.dx = rot(0,0);
	T.dy = rot(0,1);
	T.dz = rot(0,2);

	N.dx = rot(1,0);
	N.dy = rot(1,1);
	N.dz = rot(1,2);

	B.dx = rot(2,0);
	B.dy = rot(2,1);
	B.dz = rot(2,2);
	return *this;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::operator=( const Matrix<Valtype2,4,4>& trans ) noexcept{
	T.dx = ValtypeT{trans(0,0)};
	T.dy = ValtypeT{trans(0,1)};
	T.dz = ValtypeT{trans(0,2)};
																		 
	N.dx = ValtypeT{trans(1,0)};
	N.dy = ValtypeT{trans(1,1)};
	N.dz = ValtypeT{trans(1,2)};
																		 
	B.dx = ValtypeT{trans(2,0)};
	B.dy = ValtypeT{trans(2,1)};
	B.dz = ValtypeT{trans(2,2)};

	P.x = Valtype{trans(3,0)};
	P.y = Valtype{trans(3,1)};
	P.z = Valtype{trans(3,2)};

	return *this;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::operator=( const Valtype2* pVal ) noexcept{
	P = pVal;
	T = pVal + 3;
	N = pVal + 6;
	B = pVal + 9;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>::operator VectorBundle2<Valtype,ValtypeT>() const noexcept{
	return VectorBundle2<Valtype,ValtypeT>{ *this };
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>::operator VectorBundle<Valtype,ValtypeT>() const noexcept{
	return VectorBundle<Valtype,ValtypeT>{ *this };
}

template<typename Valtype,typename ValtypeT>
inline Valtype* Frame<Valtype,ValtypeT>::ptr() noexcept{
	static_assert( sizeof(Valtype) == sizeof(ValtypeT), "Can not access a VectorBundle via pointer if the two types have different memory sizes.");
	return P.ptr();
}

template<typename Valtype,typename ValtypeT>
inline const Valtype* Frame<Valtype,ValtypeT>::ptr() const noexcept{
	static_assert( sizeof(Valtype) == sizeof(ValtypeT), "Can not access a VectorBundle via pointer if the two types have different memory sizes.");
	return P.ptr();
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Init() noexcept{
	P.Init();
	T.Init( Vector<ValtypeT>::specials::ex );
	N.Init( Vector<ValtypeT>::specials::ey );
	B.Init( Vector<ValtypeT>::specials::ez );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Init( const Position<Valtype>& pos ) noexcept{
	P = pos;
	T.Init( Vector<ValtypeT>::specials::ex );
	N.Init( Vector<ValtypeT>::specials::ey );
	B.Init( Vector<ValtypeT>::specials::ez );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Init( const VectorBundle<Valtype,ValtypeT>& vb, const Vector<ValtypeT>& up ) noexcept{
	P = vb.P;
	T = vb.T;
	N = up % T;
	N.Normalize();
	B = T % N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Init( const VectorBundle2<Valtype,ValtypeT>& vb ) noexcept{
	P = vb.P;
	T = vb.T;
	N = vb.N;
	B = T % N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::LookAt( const Vector<ValtypeT>& reference, const Vector<ValtypeT>& dir ) noexcept{
//#ifdef _DEBUG
//	if( !IsOrthoNormal() ){
//		OrthogonalityDump( std::cout, *this );
//		assert( !"Frame misses to be orthogonal!" );
//	}
//#endif

	const ValtypeT pi__{3.141592654f};
	Vector<ValtypeT> D = dir;
	D.Normalize();
	Vector<ValtypeT> R = reference;
	R.Normalize();
	
	Vector<ValtypeT> rotation = R % D;
	if( ValtypeT sin = rotation.Normalize() ){
		if( R*D < decltype(ValtypeT{}*ValtypeT{}){0} )
			rotation *= pi__ - std::asin( sin );
		else
			rotation *= std::asin( sin );
	}
	else{
		if( R == -D ){
			// try to find a vector orthogonal on R
			rotation = B.OrthogonalProjection( R );
			if( !rotation.Normalize() ){
				rotation = N.OrthogonalProjection( R );
				if( !rotation.Normalize() ){
					assert( !"impossible" );
					return *this;
				}
			}

			rotation *= pi__;
		}
		else
			return *this; // R already points to D
	}
	
	Rotate( rotation );
	assert( IsOrthoNormal() );
	return *this;
}

template<typename Valtype,typename ValtypeT>
Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::LookTAt( const Vector<ValtypeT>& dir ) noexcept{
	return LookAt( T, dir );
}

template<typename Valtype,typename ValtypeT>
Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::LookTAt( const Vector<ValtypeT>& dir, const Vector<ValtypeT>& up ) noexcept{
	return LookAt( T, dir ).RotateTan( up );
}

template<typename Valtype,typename ValtypeT>
Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::LookNAt( const Vector<ValtypeT>& dir ) noexcept{
	return LookAt( N, dir );
}

template<typename Valtype,typename ValtypeT>
Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::LookBAt( const Vector<ValtypeT>& dir ) noexcept{
	return LookAt( B, dir );
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::AircraftPrincipalAxes( ValtypeT yaw, ValtypeT pitch, ValtypeT roll )
{
	T = Ex<ValtypeT>;
	N = Ey<ValtypeT>;
	B = Ez<ValtypeT>;

	return RotateBin( yaw ).RotateNor( pitch ).RotateTan( roll );
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::AircraftPrincipalAxes( const Position<ValtypeT>& angles )
{
	return AircraftPrincipalAxes( angles.x, angles.y, angles.z );
}

template<typename Valtype,typename ValtypeT>
inline Position<ValtypeT> Frame<Valtype,ValtypeT>::AircraftPrincipalAxes( ValtypeT _epsilon ) const
{
	const ValtypeT pi__{3.141592654f};

	// todo: optimize!
	if( Parallel( T, Ez<ValtypeT>, _epsilon ) ){
		if( T * Ez<ValtypeT> > ValtypeT{0} )
			return { ValtypeT{0}, -pi__/2, atan2( N * -Ex<ValtypeT>, N * Ey<ValtypeT> ) };
		else
			return { ValtypeT{0}, pi__/2, atan2( N * Ex<ValtypeT>, N * Ey<ValtypeT> ) };
	}

	Position<ValtypeT> retval{
		atan2( T.dy, T.dx ),
		atan2( -T.dz, sqrt( T.dx*T.dx + T.dy*T.dy ) ),
		ValtypeT{0} };

	Frame<Valtype,ValtypeT> ref;
	ref.Init();
	ref.RotateBin( retval.x ).RotateNor( retval.y );

	retval.z = atan2( N * ref.B, N * ref.N );
	return retval;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Normalize() noexcept{
	T.Normalize();
	N.Normalize();
	B.Normalize();
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::OrthoNormalize() noexcept{
	if ( T.Is(Vector<ValtypeT>::specials::null) )
	{
		T.dx = ValtypeT{1};
		N = Ey<ValtypeT>;
		B = Ez<ValtypeT>;
	}
	T.Normalize();
	N -= (N * T) * T;

	if ( N.Is(Vector<ValtypeT>::specials::null) )
	{
		B -= (B * T) * T;
		if ( B.Is(Vector<ValtypeT>::specials::null) )
		{
			if ( !(T % Ey<ValtypeT>).Is(Vector<ValtypeT>::specials::null) )
				N = Ey<ValtypeT> - (Ey<ValtypeT> * T) * T;
			else
				N = Ex<ValtypeT> - (Ex<ValtypeT> * T) * T;
		}
		else
		{
			B.Normalize();
			N = B % T;
			return *this;
		}
	}

	N.Normalize();
	B = T % N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline bool Frame<Valtype,ValtypeT>::IsOrthoNormal( ValtypeT _epsilon_length, ValtypeT _epsilon_angle ) const noexcept{
	return	T.IsNormal( _epsilon_length ) &&
			N.IsNormal( _epsilon_length ) &&
			B.IsNormal( _epsilon_length ) &&
			abs(T*N) <= ValtypeT{1} * _epsilon_angle &&
			abs(T*B) <= ValtypeT{1} * _epsilon_angle &&
			abs(N*B) <= ValtypeT{1} * _epsilon_angle;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2> 
inline Position<Valtype2>& Frame<Valtype,ValtypeT>::ToParent( Position<Valtype2>& p ) const noexcept{
	p = spatial_cast<Valtype2>(P) + spatial_cast<Valtype2>(p.x * T + p.y * N + p.z * B);
	return p;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2> 
inline Vector<Valtype2>& Frame<Valtype,ValtypeT>::ToParent( Vector<Valtype2>& v ) const noexcept{
	v = spatial_cast<Valtype2>(v.dx * T + v.dy * N + v.dz * B);
	return v;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline Vector<Valtype2>& Frame<Valtype,ValtypeT>::NormalToParent( Vector<Valtype2>& n ) const noexcept
{
	n = spatial_cast<Valtype2>(n.dx * T / (T*T) + n.dy * N / (N*N) + n.dz * B / (B*B));
	return n;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2,typename ValtypeT2> 
inline VectorBundle<Valtype2,ValtypeT2>& Frame<Valtype,ValtypeT>::ToParent( VectorBundle<Valtype2,ValtypeT2>& b ) const noexcept{
	ToParent( b.P );
	ToParent( b.T );
	return b;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2,typename ValtypeT2> 
inline VectorBundle2<Valtype2,ValtypeT2>& Frame<Valtype,ValtypeT>::ToParent( VectorBundle2<Valtype2,ValtypeT2>& b ) const noexcept{
	ToParent( b.P );
	ToParent( b.T );
	ToParent( b.N );
	return b;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2,typename ValtypeT2> 
inline Frame<Valtype2,ValtypeT2>& Frame<Valtype,ValtypeT>::ToParent( Frame<Valtype2,ValtypeT2>& frame ) const noexcept{
	ToParent( frame.P );
	ToParent( frame.T );
	ToParent( frame.N );	
	ToParent( frame.B );
	return frame;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2> 
inline Box<Valtype2>& Frame<Valtype,ValtypeT>::ToParent( Box<Valtype2>& box ) const noexcept{

	//todo
	return box;
}

template<typename Valtype, typename ValtypeT>
template<typename Valtype2>
inline Sphere<Valtype2>& spat::Frame<Valtype,ValtypeT>::ToParent( Sphere<Valtype2>& sphere ) const noexcept
{
	ToParent( sphere.c );
	return sphere;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2> 
inline Position<Valtype2>& Frame<Valtype,ValtypeT>::FromParent( Position<Valtype2>& p ) const noexcept{
	Vector<Valtype2> v{ p - spatial_cast<Valtype2>(P) };
	FromParent( v );
	p.x = v.dx;
	p.y = v.dy;
	p.z = v.dz;
	return p;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2> 
inline Vector<Valtype2>& Frame<Valtype,ValtypeT>::FromParent( Vector<Valtype2>& v ) const noexcept{
	const Vector<Valtype2> vtemp{ v };
	v.dx = vtemp*T;
	v.dy = vtemp*N;
	v.dz = vtemp*B;
	return v;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2,typename ValtypeT2> 
inline VectorBundle<Valtype2,ValtypeT2>& Frame<Valtype,ValtypeT>::FromParent( VectorBundle<Valtype2,ValtypeT2>& b ) const noexcept{
	FromParent( b.P );
	FromParent( b.T );
	return b;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2,typename ValtypeT2> 
inline VectorBundle2<Valtype2,ValtypeT2>& Frame<Valtype,ValtypeT>::FromParent( VectorBundle2<Valtype2,ValtypeT2>& b ) const noexcept{
	FromParent( b.P );
	FromParent( b.T );
	FromParent( b.N );
	return b;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2,typename ValtypeT2> 
inline Frame<Valtype2,ValtypeT2>& Frame<Valtype,ValtypeT>::FromParent( Frame<Valtype2,ValtypeT2>& frame ) const noexcept{
	FromParent( frame.P );
	FromParent( frame.T );
	FromParent( frame.N );
	FromParent( frame.B );
	return frame;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::TransportTo( Valtype d ) noexcept{
	P += d * T;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::TransportTo( const Position<Valtype>& p ) noexcept{
	P += p.x * T + p.y * N + p.z * B;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::TransportBy( const Vector<Valtype>& v ) noexcept{
	P.Transport( v );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::TransportTan( Valtype dist ) noexcept{
	P += dist * T;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::TransportNor( Valtype dist ) noexcept{
	P += dist * N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::TransportBin( Valtype dist ) noexcept{
	P += dist * B;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::FlipTan() noexcept{
	N = -N;
	B = -B;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::FlipNor() noexcept{
	T = -T;
	B = -B;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::FlipBin() noexcept{
	T = -T;
	N = -N;
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Rotate( const Vector<ValtypeT>& nr, ValtypeT angle ) noexcept{
	T.Rotate( nr, angle );
	N.Rotate( nr, angle );
	B.Rotate( nr, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Rotate( const Vector<ValtypeT>& rot ) noexcept{
	Vector<ValtypeT> r(rot);
	ValtypeT angle = r.Normalize();

	T.Rotate( r, angle );
	N.Rotate( r, angle );
	B.Rotate( r, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Rotate( const VectorBundle<Valtype,Valtype2>& axis ) noexcept
{
	VectorBundle<Valtype,Valtype2> r(axis);
	Valtype2 angle = r.T.Normalize();

	return Rotate( r, angle );
}

template<typename Valtype,typename ValtypeT>
template<typename Valtype2>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Rotate( const VectorBundle<Valtype,Valtype2>& axis, Valtype2 angle ) noexcept
{
	P.Rotate( axis, angle );
	T.Rotate( axis.T, angle );
	N.Rotate( axis.T, angle );
	B.Rotate( axis.T, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::RotateTan( ValtypeT angle ) noexcept{
	N.Rotate( T, angle );
	B.Rotate( T, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::RotateTan( const Vector<ValtypeT>& up ) noexcept{
	return RotateTan( -std::atan2(up*N,up*B) );
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::RotateNor( ValtypeT angle ) noexcept{
	T.Rotate( N, angle );
	B.Rotate( N, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::RotateBin( ValtypeT angle ) noexcept{
	T.Rotate( B, angle );
	N.Rotate( B, angle );
	return *this;
}

template<typename Valtype,typename ValtypeT> constexpr
inline bool Frame<Valtype,ValtypeT>::Equals( const Frame& frame, Valtype _epsilon_length, ValtypeT epsilon_lengthT ) const noexcept{
	return	P.Equals( frame.P, _epsilon_length ) &&
			T.Equals( frame.T, epsilon_lengthT ) &&
			N.Equals( frame.N, epsilon_lengthT ) &&
			B.Equals( frame.B, epsilon_lengthT );
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& Frame<Valtype,ValtypeT>::Round( int toDigit ) noexcept{
	P.Round(toDigit);
	T.Round(toDigit);
	N.Round(toDigit);
	B.Round(toDigit);
	return *this;
}

template<typename Valtype,typename ValtypeT,typename Valtype2,typename ValtypeT2>
inline Frame<Valtype,ValtypeT>& spatial_cast( Frame<Valtype2,ValtypeT2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	static_assert( sizeof(ValtypeT) == sizeof(ValtypeT2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<Frame<Valtype,ValtypeT>&>(from);
}

template<typename Valtype,typename ValtypeT,typename Valtype2,typename ValtypeT2>
inline const Frame<Valtype,ValtypeT>& spatial_cast( const Frame<Valtype2,ValtypeT2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	static_assert( sizeof(ValtypeT) == sizeof(ValtypeT2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<const Frame<Valtype,ValtypeT>&>(from);
}

template<typename Valtype,typename ValtypeT> constexpr
inline bool operator==( const Frame<Valtype,ValtypeT>& frame1, const Frame<Valtype,ValtypeT>& frame2 ) noexcept{
	return !(frame1 != frame2);
}

template<typename Valtype,typename ValtypeT> constexpr
inline bool operator!=( const Frame<Valtype,ValtypeT>& frame1, const Frame<Valtype,ValtypeT>& frame2 ) noexcept{
	return	frame1.P != frame2.P || 
			frame1.T != frame2.T || 
			frame1.N != frame2.N || 
			frame1.B != frame2.B;
}

template<typename Valtype1,typename ValtypeT1,typename Valtype2> 
inline Position<Valtype2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const Position<Valtype2>& v ) noexcept{
	Position<Valtype2> result{ v };
	frame.ToParent( result );
	return result;
}

template<typename Valtype1,typename ValtypeT1,typename ValtypeT2> 
inline Vector<ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const Vector<ValtypeT2>& v ) noexcept{
	Vector<ValtypeT2> result{ v };
	frame.ToParent( result );
	return result;
}

template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2> 
inline VectorBundle<Valtype2,ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const VectorBundle<Valtype2,ValtypeT2>& vb ) noexcept{
	VectorBundle<Valtype2,ValtypeT2> result{ vb };
	frame.ToParent( result );
	return result;
}

template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2> 
inline VectorBundle2<Valtype2,ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const VectorBundle2<Valtype2,ValtypeT2>& vb2 ) noexcept{
	VectorBundle2<Valtype2,ValtypeT2> result{ vb2 };
	frame.ToParent( result );
	return result;
}

template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2> 
inline Frame<Valtype2,ValtypeT2> operator*( const Frame<Valtype1,ValtypeT1>& frame, const Frame<Valtype2,ValtypeT2>& f ) noexcept{
	Frame<Valtype2,ValtypeT2> result{ f };
	frame.ToParent( result );
	return result;
}

template<typename Valtype1,typename ValtypeT1,typename Valtype2> 
Sphere<Valtype2> operator*( const Frame<Valtype1,ValtypeT1>&frame, const Sphere<Valtype2>&sp ) noexcept{
	Sphere<Valtype2> result{ sp };
	frame.ToParent( result );
	return result;
}

template<typename Valtype1,typename ValtypeT1,typename Valtype2,typename ValtypeT2> 
inline Frame<Valtype2,ValtypeT2>& operator*=( Frame<Valtype1,ValtypeT1>& frame, const Frame<Valtype2,ValtypeT2>& f ) noexcept{
	return frame = frame * f;
}

template<typename Valtype,typename ValtypeT>
inline Frame<Valtype,ValtypeT>& operator*=( Frame<Valtype,ValtypeT>& frame, ValtypeT param ) noexcept{
	frame.T *= param;
	frame.N *= param;
	frame.B *= param;
	return frame;
}

template<typename Valtype,typename ValtypeT> 
auto Normalize( const Frame<Valtype,ValtypeT>& f ) noexcept -> std::pair<Vector<ValtypeT>,Frame<Valtype,decltype(ValtypeT{}/ValtypeT{})>>{
	auto normT = Normalize( f.T );
	auto normN = Normalize( f.N );
	auto normB = Normalize( f.B );

	return std::make_pair( 
		Vector<ValtypeT>{
			normT.first,
			normN.first,
			normB.first },
		Frame<Valtype,decltype(ValtypeT{}/ValtypeT{})>{
			f.P,
			normT.second,
			normN.second,
			normB.second } );
}

} // namespace spat
