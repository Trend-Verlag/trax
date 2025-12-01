//	trax track library
//	AD 2014 
//
//  "Twilight driving gotta watch out for the roos"
//
//								Methyl Ethel
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
#include "PositionH.h"

//#include <boost/numeric/ublas/matrix.hpp>
//#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/lu.hpp>

// TODO: watch MPL2 license.
#ifdef TRAX_OPEN_SOURCE
#	if defined(_MSC_VER)
#		pragma warning( disable: 4127 )
#	endif
#	include <Eigen/Dense>
#endif

#include <cassert>
#include <algorithm>
#include <cmath>


namespace spat{

	template<typename> struct Vector;
	template<typename> struct Vector2D;
	template<typename,typename> struct VectorBundle;
	template<typename,typename> struct Frame;

	///	\brief Matrix template for arbitrary dimensions 
	///	and value type.
	///
	/// For special matrix types like square matrices,
	///	special derived matrix types are provided.
	template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
	class Matrix
	{
	public:
		typedef Valtype value_type;

		/// \name Construction
		/// \param matrix Create from matrix
		/// \param elements Create from initializer list
		///@{
		Matrix();
		Matrix( const Matrix& matrix );
		Matrix( Matrix&& matrix ) noexcept;
		explicit Matrix( const std::initializer_list<Valtype>& elements );
		~Matrix() noexcept = default;
		///@}


		/// \name Assignment
		/// \param matrix Assign from matrix
		/// \param elements Assign from initializer list
		///@{
		Matrix&	operator=( const Matrix& matrix ) noexcept;

		Matrix&	operator=( Matrix&& matrix ) noexcept;

		Matrix&	operator=( const std::initializer_list<Valtype>& elements ) noexcept;
		///@}


		/// \name Subscripting.
		/// \returns Reference to value.
		///@{
		const Valtype& operator()( unsigned short col, unsigned short row ) const noexcept;

		Valtype& operator()( unsigned short col, unsigned short row ) noexcept;
		///@}


		///	\brief Sets all elements to 0.
		void SetNull() noexcept;


		///	\brief Number of columns.
		constexpr unsigned short Cols() const noexcept;


		///	\brief Number of rows.
		constexpr unsigned short Rows() const noexcept;


		///	\brief Test elements for equality up to delta distance.
		bool IsEqual( const Matrix& matrix, Valtype delta = 0 ) const noexcept;


		/// \brief Comparison for equality.
		bool operator==( const Matrix& matrix ) const noexcept;


		/// \brief Comparison for inequality.
		bool operator!=( const Matrix& matrix ) const noexcept;


		/// \brief Matrix * Skalar multiplication.
		void operator*=( Valtype skalar ) noexcept;


		/// \brief Matrix multiplication.
		void operator*=( const Matrix<Valtype,nCols,nCols>& matrix );


		/// \brief Matrix / Skalar division.
		void operator/=( Valtype skalar ) noexcept;


		/// \brief Matrix addition.
		void operator+=( const Matrix& matrix ) noexcept;


		/// \brief Matrix subtraction.
		void operator-=( const Matrix& matrix ) noexcept;


		/// \brief Pointer accessor for direct copying.
		/// The memory layout is column-major order.
		const Valtype* ptr() const noexcept;


		///	\brief Extracts the submatrix of mat wich results from
		///	omitting the row r and  collumn c.
		Matrix<Valtype,nCols-1,nRows-1> SubMatrix( unsigned short c, unsigned short r ) const;
	private:
		std::unique_ptr<Valtype[]> m; ///< Pointer to array.
	};


	/// \returns The idx column of matrix r as a Vector.
	template<typename Valtype,const unsigned short nCols> inline
	constexpr Vector<Valtype> Column( const Matrix<Valtype,nCols,3>& m, unsigned short idx ) noexcept;


	/// \returns The idx row of matrix r as a Vector.
	template<typename Valtype,const unsigned short nRows> inline
	constexpr Vector<Valtype> Row( const Matrix<Valtype,3,nRows>& m, unsigned short idx ) noexcept;


	/// \returns The transposed matrix (i.e. rows become columns and vice versa).
	template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
	constexpr Matrix<Valtype,nRows,nCols> Transposed( const Matrix<Valtype,nCols,nRows>& m ) noexcept;


	/// \name Mathematical Operators for Matrix
	///@{
	template<	typename Valtype, 
				typename Valtype2, 
				const unsigned short nCols, 
				const unsigned short nRows > inline
	auto operator*( Valtype skalar, const Matrix<Valtype2,nCols,nRows>& matrix ) noexcept -> Matrix<decltype(Valtype{}*Valtype2{}),nCols,nRows>;


	//template<	typename Valtype,
	//			typename Valtype2, 
	//			const unsigned short nCols, 
	//			const unsigned short nRows,
	//			typename = std::enable_if<std::is_pod<Valtype2>::value, void>::type> inline
	//auto operator*( const Matrix<Valtype,nCols,nRows>& matrix, Valtype2 skalar ) -> Matrix<decltype(Valtype{}*Valtype2{}),nCols,nRows>;
	//// Should only be considered if Valtype2 is POD, SFINAEd otherwise. But doesn't work for unknown reason...


	template<	typename Valtype,
				typename Valtype2,
				unsigned short  nColsFirst_RowsSecond,
				unsigned short  nRowsFirst,
				unsigned short  nColsSecond > inline
	auto operator*( const Matrix<Valtype,nColsFirst_RowsSecond,nRowsFirst>& first, 
					const Matrix<Valtype2,nColsSecond,nColsFirst_RowsSecond>& second ) noexcept -> Matrix<decltype(Valtype{}*Valtype2{}),nColsSecond,nRowsFirst>;


	template<	typename Valtype,
				typename Valtype2,
				unsigned short  nCols,
				unsigned short  nRows > inline
	Matrix<Valtype,nCols,nRows>& operator*=( Matrix<Valtype,nCols,nRows>& first, Valtype2 skalar ) noexcept;


	template<	typename Valtype,
				typename Valtype2,
				unsigned short  nCols,
				unsigned short  nRows > inline
	Matrix<Valtype,nCols,nRows>& operator/=( Matrix<Valtype,nCols,nRows>& first, Valtype2 skalar ) noexcept;


	template<	typename Valtype,
				const unsigned short nCols, 
				const unsigned short nRows > inline
	Matrix<Valtype,nCols,nRows> operator+( 
							const Matrix<Valtype,nCols,nRows>& first, 
							const Matrix<Valtype,nCols,nRows>& second ) noexcept;


	template<	typename Valtype, 
				const unsigned short nCols, 
				const unsigned short nRows > inline
	Matrix<Valtype,nCols,nRows> operator-( 
							const Matrix<Valtype,nCols,nRows>& first, 
							const Matrix<Valtype,nCols,nRows>& second ) noexcept;


	template<	typename Valtype,
				const unsigned short nCols, 
				const unsigned short nRows > inline
	constexpr Matrix<Valtype,nCols,nRows>& operator+( Matrix<Valtype,nCols,nRows>& matrix ) noexcept;


	template<	typename Valtype, 
				const unsigned short nCols, 
				const unsigned short nRows > inline
	Matrix<Valtype,nCols,nRows>& operator-( Matrix<Valtype,nCols,nRows>& matrix ) noexcept;
	///@}


	/// \name Copy to and from matrix data array
	/// 
	/// The major column or row is assumed to fill the matrix as contiguous memory.
	///@{
	template<	typename Valtype,
				typename Valtype2,
				const unsigned short nCols, 
				const unsigned short nRows > inline
	void copy_column_major( const Matrix<Valtype,nCols,nRows>& source, Valtype2* target ) noexcept;

	template<	typename Valtype,
				typename Valtype2,
				const unsigned short nCols, 
				const unsigned short nRows > inline
	void copy_column_major( const Valtype2* source, Matrix<Valtype,nCols,nRows>& target ) noexcept;

	template<	typename Valtype,
				typename Valtype2,
				const unsigned short nCols, 
				const unsigned short nRows > inline
	void copy_row_major( const Matrix<Valtype,nCols,nRows>& source, Valtype2* target ) noexcept;

	template<	typename Valtype,
				typename Valtype2,
				const unsigned short nCols, 
				const unsigned short nRows > inline
	void copy_row_major( const Valtype2* source, Matrix<Valtype,nCols,nRows>& target ) noexcept;
	///@}


	///	\brief Square matrix with nColumns == nRows.
	///
	///	Some special operations like determinant calculation
	///	are provided.
	template<typename Valtype, const unsigned short nColsAndRows>
	class SquareMatrix : public Matrix<Valtype,nColsAndRows,nColsAndRows>
	{
	public:
		using Basetype = Matrix<Valtype,nColsAndRows,nColsAndRows>; ///< Utmost base type of this SquareMatrix

		/// \name Construction
		/// \param matrix Construct from matrix
		/// \param elements Create from initializer list
		///@{
		// using Matrix<Valtype,nColsAndRows,nColsAndRows>::Matrix; //does not work ...
		SquareMatrix() noexcept(false) = default;
		SquareMatrix( const SquareMatrix& ) = default;
		SquareMatrix( const Basetype& matrix );
		SquareMatrix( Basetype&& matrix ) noexcept;
		explicit SquareMatrix( const std::initializer_list<Valtype>& elements );
		///@}

		/// \name Assignment
		/// \param matrix Assign from matrix
		///@{
		using Basetype::operator=;
		///@}


		using Basetype::operator();


		/// \brief Sets the matrix to the identity matrix 
		/// (i.e. all 1 in diagonal, 0 for everything else)
		void SetIdentity() noexcept;


		/// \returns true if the matrix is the identity matrix.
		bool IsIdentity( Valtype epsilon = 0 ) const noexcept;


		/// \returns true if the matrix is diagonal.
		bool IsDiagonal() const noexcept;


		/// \returns true if the matrix is symmetric, i.e m(i,j) == m(j,i)
		bool IsSymmetric( Valtype epsilon = 0 ) const noexcept;


		///	\brief Change colums and rows.
		SquareMatrix& Transpose() noexcept;


		///	\brief Invert matrix so that M*I == Identity
		SquareMatrix& Invert();


		///	\brief Trace of the matrix, wich is the sum of all element
		///	with same indices.
		///	\returns the trace of the matrix
		Valtype Trace() const noexcept;
	};


	// IdentityMatrix not existing due to initializing problematic. Use SquareMatrix::SetIdentity() instead.


	/// \brief Transformation of a Vector by an 3x3 Matrix.
	template<typename Valtype,typename Valtype2> constexpr
	auto operator*( const SquareMatrix<Valtype,3>& m, const Vector<Valtype2>& v ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>;


	/// \returns The inverse matrix so that M*I == Identity
	template<typename Valtype, const unsigned short nColsAndRows > constexpr 
	SquareMatrix<Valtype,nColsAndRows> Inverted( const SquareMatrix<Valtype,nColsAndRows>& m );


	/// \returns The matrix with changed colums and rows.
	template<typename Valtype, const unsigned short nColsAndRows > constexpr 
	SquareMatrix<Valtype,nColsAndRows> Transposed( const SquareMatrix<Valtype,nColsAndRows>& m );


	///	\brief Determinant of the matrix.
	///	\returns Returns the determinant of the matrix
	template<typename Valtype, const unsigned short nColsAndRows > constexpr 
	auto Determinant( const SquareMatrix<Valtype,nColsAndRows>& m ) -> decltype(pow<nColsAndRows>(Valtype{}));


	/// \name Adjungated
	///	\brief Adjungate value for element
	///@{
	template<typename Valtype, const unsigned short nColsAndRows > constexpr 
	auto Adjungated( const SquareMatrix<Valtype,nColsAndRows>& m, unsigned short c, unsigned short r ) -> decltype(pow<nColsAndRows-1>(Valtype{}));


	template<typename Valtype > constexpr 
	auto Adjungated( const SquareMatrix<Valtype,1>& m, unsigned short c, unsigned short r ) noexcept -> decltype(pow<0>(Valtype{}));
	///@}


	///	\brief Matrix of adjungated values
	template<typename Valtype, const unsigned short nColsAndRows >
	SquareMatrix<Valtype,nColsAndRows> AdjungatedMatrix( const SquareMatrix<Valtype,nColsAndRows>& m );


	template<typename Valtype1,typename Valtype2>
	auto operator*( const SquareMatrix<Valtype1,2>& m, const Vector2D<Valtype2>& v ) noexcept -> Vector2D<decltype(Valtype1{}*Valtype2{})>;


	template<typename Valtype> class Rotation;


	///	\brief Homogenous transformation matrix.
	template<typename Valtype>
	class Transformation : public SquareMatrix<Valtype,4>
	{
	public:
		using Basetype = typename SquareMatrix<Valtype,4>::Basetype;

		/// \name Construction
		/// \param rot Construct pure rotational transformation
		/// \param matrix Construct from matrix
		/// \param frame Construct from frame so that this transformation 
		/// transforms coordinates, local to the frame into coordinates
		/// in the system the frame is defined.
		///@{
					Transformation() = default;
					Transformation( const SquareMatrix<Valtype,4>& matrix );
					Transformation( SquareMatrix<Valtype,4>&& matrix ) noexcept;
					Transformation( const Basetype& matrix );
					Transformation( Basetype&& matrix ) noexcept;

		template<typename Valtype2>
					Transformation( const Rotation<Valtype2>& rot );
		template<typename Valtype2,typename ValtypeT2>
		explicit	Transformation( const Frame<Valtype2,ValtypeT2>& frame );
		explicit	Transformation( const std::initializer_list<Valtype>& elements );
 		///@}


		/// \name Assignment
		/// \param rot Assign pure rotational transformation
		/// \param frame Assign frame local to parent transformation
		///@{
		using SquareMatrix<Valtype,4>::operator=;

		template<typename Valtype2>
		Transformation& operator=( const Rotation<Valtype2>& rot ) noexcept;
		template<typename Valtype2,typename ValtypeT2>
		Transformation& operator=( const Frame<Valtype2,ValtypeT2>& frame ) noexcept;
		///@}


		using SquareMatrix<Valtype,4>::operator();


		/// \name Transformation
		///@{
		template<typename Valtype2>
		PositionH<Valtype2> operator*( const PositionH<Valtype2>& p ) const noexcept;

		template<typename Valtype2>
		Position<Valtype2> operator*( const Position<Valtype2>& pos ) const noexcept;

		template<typename Valtype2>
		Vector<Valtype2> operator*( const Vector<Valtype2>& vec ) const noexcept;
		///@}



		/// \name CreateTranslation
		/// \brief Creates a matrix that has the effect of a translation.
		/// \param translation Translation vector
		/// \param tx x - translation.
		/// \param ty y - translation.
		/// \param tz z - translation.
		///@{
		void CreateTranslation( const Vector<Valtype>& translation ) noexcept;

		void CreateTranslation( Valtype tx, Valtype ty, Valtype tz ) noexcept;
		///@}


		/// \name CreateRotation
		/// \brief Creates a matrix that has the effect of a rotation.
		/// \param rotation Rotation vector
		/// \param rx x - component of rotation vector.
		/// \param ry y - component of rotation vector.
		/// \param rz z - component of rotation vector.
		///@{		
		void CreateRotation( const Vector<Valtype>& rotation );
		
		void CreateRotation( Valtype rx, Valtype ry, Valtype rz );
		///@}


		/// \name CreateScaling
		/// \brief Creates a matrix that has the effect of a scaling.
		/// \param scaling Scaling vector
		/// \param rx x - component of scaling vector.
		/// \param ry y - component of scaling vector.
		/// \param rz z - component of scaling vector.
		///@{		
		void CreateScaling( const Vector<Valtype>& scaling ) noexcept;

		void CreateScaling( Valtype sx, Valtype sy, Valtype sz ) noexcept;
		///@}


		/// \name CreateMirror
		/// \brief Creates a matrix that has the effect of mirroring on
		/// a plane, described by mirror.
		template<typename Valtype2>
		void CreateMirror( const VectorBundle<Valtype2,Valtype>& mirror );


		///	\brief Create a view matrix that transforms to camera space.
		void LookAt(	const Position<Valtype>& at, 
						const Position<Valtype>& eye, 
						const Vector<Valtype>& up ) noexcept;

		///	\brief Creates a viewport projection
		void CreateViewport(	Valtype Width, 
								Valtype Height, 
								Valtype MinZ, 
								Valtype MaxZ,
								Valtype LeftMargin = 0.0f, 
								Valtype TopMargin = 0.0f  ) noexcept;


		///	\brief Creates camera projection transformation
		///	\param fovy Field of view angle.
		///	\param Aspect Width to height aspect of opening angles.
		///	\param zn Distance to the near plane of the camera frustum.
		///	\param zf Distance to the far plane of the camera frustum.
		bool CreateCameraProjection		( Valtype fovy, Valtype Aspect, Valtype zn, Valtype zf ) noexcept;


		///	\brief Creates orthographic projection transformation
		bool CreateOrthographicProjection( Valtype width, Valtype height, Valtype znear, Valtype zfar ) noexcept;


		///	\brief Vector transformation. Use it for difference vectors
		/// between positions or dynamic vectors like velocities and accellerations. 
		/// 
		/// If you want to transform normal vectors be aware that due to the nature of this kind of vector
		/// beeing orthogonal to a plane you have to use the inverse transposed matrix.
		/// 
		/// M: Transformation matrix for coordinates.
		/// Q: Transformation matrix for normals.
		/// D = P2 - P1: Vector between two arbitrary points in a plane.
		/// D' = P2' - P1' =  M * P2 - M * P1 = M * D: Transformed vector D.
		/// N: Normal on that plane.
		/// 
		/// Now its N*D = 0 for all D since N is orthogonal to the plane. But with
		/// the transformed vectors the same condition must hold: N'*D' = 0.
		/// -> Q*N*M*D = 0
		/// -> N*Qt*M*D = 0   (Qt is the transposed of Q)
		/// since this must hold for all D (or lets put it the other way: to be on the safe side) 
		/// Qt*M = IdentityMatrix must hold.
		/// hence Q = Transposed(Inverse(M)) = Inverse(Transposed(M))
		Vector<Valtype> TransformVector	( const Vector<Valtype>& vec ) const noexcept;


		///	\brief Kovariant vector use this for face normals or vectors denoting a
		/// rotation orientation.
		//Vector<Valtype> TransformNormal	( const Vector<Valtype>& vec ) const;


		///	\brief Calculates a translational part of the transformation
		///	so that T * R * S == *this.
		void GetTranslation( Transformation& T ) const noexcept;


		///	\brief Calculates a rotational part of the transformation
		///	so that T * R * S == *this.
		bool GeRotation( Transformation& R ) const noexcept;


		///	\brief Calculates a scaling part of the transformation
		///	so that T * R * S == *this.
		bool GetScaling( Transformation& S ) const noexcept;


		///	\brief Calculates translational, rotational and scaling part of the transformation
		///	so that T * R * S == *this.
		bool Dismantle( Transformation& T, Transformation& R, Transformation& S ) const noexcept;


		///	\brief Tests wether the matrix is a valid homogenous transformation.
		bool IsValid() const noexcept;
	};

	/// \brief Transformation that leaves the transformed object unchanged.
	template<typename Valtype>
	const Transformation<Valtype> IdentityTransformation{	
													Valtype{1}, Valtype{0}, Valtype{0}, Valtype{0},
													Valtype{0}, Valtype{1}, Valtype{0}, Valtype{0},
													Valtype{0}, Valtype{0}, Valtype{1}, Valtype{0},
													Valtype{0}, Valtype{0}, Valtype{0}, Valtype{1} };


	template<	typename Valtype,
				typename Valtype2 > inline
	auto operator*( const Transformation<Valtype>& first, 
					const Transformation<Valtype2>& second ) noexcept -> Transformation<decltype(Valtype{}*Valtype2{})>;


	///	\brief Spherical interpolates two matrices weighting them by t.
	///	
	///	\param out Result
	///	\param inA The matrix
	///	\param inB The other matrix
	///	\param t Interpolation value 
	///	between 0 and 1. t is the weight of inB, so
	///	for t=1 we get the full matrix B for t=0 A.
	template<typename Valtype>
	bool  Slerp(	Transformation<Valtype>& out, 
					const Transformation<Valtype>& inA, 
					const Transformation<Valtype>& inB,
					Valtype t );


	/// \brief Inverts the frame, so that FI * F == Identity
	/// @tparam Valtype Value type of the frame's Pos part. 
	/// @tparam ValtypeT Value type of the frame's T,N,B part. 
	/// @param frame Frame to invert
	/// \returns the inverted frame.
	template<typename Valtype, typename ValtypeT>
	Frame<Valtype,ValtypeT> Invert( const Frame<Valtype,ValtypeT> frame );


	///	\brief Rotation matrix.
	template<typename Valtype>
	class Rotation : public SquareMatrix<Valtype,3>
	{
	public:
		using Basetype = typename SquareMatrix<Valtype,3>::Basetype; ///< Utmost base type of this Rotation

		/// \name Construction
		/// \param matrix Construct from transformation matrix.
		/// \param axis Create from a rotational axis.
		/// \param frame Create a rotation that transforms frame local 
		/// coordinates with respect to their rotation only.
		///@{
					Rotation() noexcept(false) = default;
					Rotation( const SquareMatrix<Valtype,3>& matrix );
					Rotation( SquareMatrix<Valtype,3>&& matrix ) noexcept;
					Rotation( const Basetype& matrix );
					Rotation( Basetype&& matrix ) noexcept;

		explicit	Rotation( const Vector<Valtype>& axis );
					Rotation( Valtype q0, Valtype q1, Valtype q2, Valtype q3 ) noexcept; // from quarternion
		template<typename ValtypeP>
		explicit	Rotation( const Frame<ValtypeP,Valtype>& frame );
		template<typename Valtype2>					
					Rotation( const Transformation<Valtype2>& matrix );
		explicit	Rotation(	Valtype r00, Valtype r10, Valtype r20, 
								Valtype r01, Valtype r11, Valtype r21, 
								Valtype r02, Valtype r12, Valtype r22 );					
		///@}

		/// \name Assignment
		/// \param frame Assign from frame with respect to its rotation only
		///@{
		using SquareMatrix<Valtype,3>::operator=;

		template<typename ValtypeP>
		Rotation& operator=( const Frame<ValtypeP,Valtype>& frame ) noexcept;
		///@}


		using SquareMatrix<Valtype,3>::operator();


		///	\brief This constructs a rotation with an axis according to the right-hand-rule. (Thumb
		/// points in vector direction, fingers show the rotation)
		void CreateFromAxis( const Vector<Valtype>& axis );


		///	\brief This constructs a rotation with an axis according to the right-hand-rule. (Thumb
		/// points in vector direction, fingers show the rotation)
		void CreateFromAxis( Valtype rx, Valtype ry, Valtype rz ) noexcept;


		/// \name Rotate
		/// \brief Transform an element by this Rotation
		///@{
		void Rotate( Vector<Valtype>& v ) const noexcept;

		template<typename ValtypeP>
		void Rotate( Frame<ValtypeP,Valtype>& frame ) const noexcept;
		///@}


		/// \name Transformation
		///@{
		Position<Valtype> operator*( const Position<Valtype>& p ) const noexcept;

		Vector<Valtype> operator*( const Vector<Valtype>& v ) const noexcept;
		///@}


		///	\brief Axis of the rotation described by this matrix.
		///	\returns the normalised axis of rotation.
		Vector<Valtype>	RotationAxis() const noexcept;


		///	\brief Angle of the rotation described by this matrix.
		///	\returns the angle of rotation. The 
		///	angle is given in radiants.
		Valtype RotationAngle() const noexcept;
	};


	/// \brief Rotation that leaves the transformed object unchanged.
	template<typename Valtype>
	const Rotation<Valtype> IdentityRotation{	1,0,0,
												0,1,0,
												0,0,1 };

	template<	typename Valtype,
				typename Valtype2 > inline
	auto operator*( const Rotation<Valtype>& first, 
					const Rotation<Valtype2>& second ) noexcept -> Rotation<decltype(Valtype{}*Valtype2{})>;

///////////////////////////////////////
template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline Matrix<Valtype,nCols,nRows>::Matrix()
	: m{ std::make_unique<Valtype[]>(nCols*nRows) }
{
	static_assert( nCols, "Matrix: Number columes can not be zero" );
	static_assert( nRows, "Matrix: Number of rows can not be zero" );
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
Matrix<Valtype,nCols,nRows>::Matrix( const Matrix& matrix )
	: m{ std::make_unique<Valtype[]>(nCols*nRows) }
{
	for( size_t Col = 0; Col < nCols; ++Col )
		for( size_t Row = 0; Row < nRows; ++Row )
			m[nRows * Col + Row] = matrix.m[nRows * Col + Row];
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline Matrix<Valtype,nCols,nRows>::Matrix( Matrix&& matrix ) noexcept
{
	m = std::move(matrix.m);
}

template<typename Valtype,unsigned short nCols,unsigned short nRows>
inline Matrix<Valtype,nCols,nRows>::Matrix( const std::initializer_list<Valtype>& elements )
	: m{ std::make_unique<Valtype[]>(nCols*nRows) }
{
	static_assert( nCols, "Matrix: Number columes can not be zero" );
	static_assert( nRows, "Matrix: Number of rows can not be zero" );

	operator=( elements );
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
Matrix<Valtype,nCols,nRows>& Matrix<Valtype,nCols,nRows>::operator=( 
	const Matrix& matrix ) noexcept
{
	for( size_t Col = 0; Col < nCols; ++Col )
		for( size_t Row = 0; Row < nRows; ++Row )
			m[nRows * Col + Row] = matrix.m[nRows * Col + Row];

	return (*this);
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline Matrix<Valtype,nCols,nRows>& Matrix<Valtype,nCols,nRows>::operator=( 
	Matrix&& matrix ) noexcept
{
	m = std::move(matrix.m);
	return *this;
}

template<typename Valtype,unsigned short nCols,unsigned short nRows>
inline Matrix<Valtype,nCols,nRows>& Matrix<Valtype,nCols,nRows>::operator=(
	const std::initializer_list<Valtype>& elements ) noexcept
{
	auto it  = elements.begin();
	const auto end = elements.end();

	// Fill in row-major order into column-major storage
	for( size_t Row = 0; Row < nRows; ++Row ){
		for( size_t Col = 0; Col < nCols; ++Col ){
			m[nRows * Col + Row] = (it != end) ? *it++ : Valtype{0};
		}
	}
	return *this;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline const Valtype& Matrix<Valtype,nCols,nRows>::operator()( unsigned short col, unsigned short row ) const noexcept{
//	assert( col < nCols && row < nRows );
	return m[nRows * static_cast<size_t>(col) + row];
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline Valtype& Matrix<Valtype,nCols,nRows>::operator()( unsigned short col, unsigned short row ) noexcept{
//	assert( col < nCols && row < nRows );
	return m[nRows * static_cast<size_t>(col) + row];
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
void Matrix<Valtype,nCols,nRows>::SetNull() noexcept{
	for( size_t Col = 0; Col < nCols; ++Col )
		for( size_t Row = 0; Row < nRows; ++Row )
			m[nRows * Col + Row] = 0;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline constexpr unsigned short Matrix<Valtype,nCols,nRows>::Cols() const noexcept{
	return nCols;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline constexpr unsigned short Matrix<Valtype,nCols,nRows>::Rows() const noexcept{
	return nRows;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
bool Matrix<Valtype,nCols,nRows>::IsEqual( const Matrix& matrix, Valtype delta ) const noexcept{
	for( unsigned short x = 0; x < nCols; ++x )
		for( unsigned short y = 0; y < nRows; ++y )
			if( !common::Equals( (*this)( x, y ), matrix( x, y ), delta ) )
				return false;

	return true;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline bool Matrix<Valtype,nCols,nRows>::operator==( const Matrix& matrix ) const noexcept{
	return IsEqual( matrix, std::numeric_limits<Valtype>::epsilon() );
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline bool Matrix<Valtype,nCols,nRows>::operator!=( const Matrix& matrix ) const noexcept{
	return !operator==( matrix );
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline void Matrix<Valtype,nCols,nRows>::operator*=( Valtype skalar ) noexcept{
	for( size_t Col = 0; Col < nCols; ++Col )
		for( size_t Row = 0; Row < nRows; ++Row )
			m[nRows * Col + Row] *= skalar;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline void Matrix<Valtype,nCols,nRows>::operator*=( const Matrix<Valtype,nCols,nCols>& matrix ){
	Matrix<Valtype,nCols,nRows> TempVal;
	MatrixMultiply( *this, matrix, TempVal );
	*this = TempVal;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline void Matrix<Valtype,nCols,nRows>::operator/=( Valtype skalar ) noexcept{
	for( size_t Col = 0; Col < nCols; ++Col )
		for( size_t Row = 0; Row < nRows; ++Row )
			m[nRows * Col + Row] /= skalar;
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline void Matrix<Valtype,nCols,nRows>::operator+=( const Matrix& matrix ) noexcept{
	for( size_t Col = 0; Col < nCols; ++Col )
		for( size_t Row = 0; Row < nRows; ++Row )
			m[nRows * Col + Row] += matrix.m[nRows * Col + Row];
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline void Matrix<Valtype,nCols,nRows>::operator-=( const Matrix& matrix ) noexcept{
	for( size_t Col = 0; Col < nCols; ++Col )
		for( size_t Row = 0; Row < nRows; ++Row )
			m[nRows * Col + Row] -= matrix.m[nRows * Col + Row];
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
inline const Valtype* Matrix<Valtype,nCols,nRows>::ptr() const noexcept{
	return m.get();
}

template<typename Valtype, const unsigned short nCols, const unsigned short nRows >
Matrix<Valtype,nCols-1,nRows-1> Matrix<Valtype,nCols,nRows>::SubMatrix( unsigned short c, unsigned short r ) const{
	Matrix<Valtype,nCols-1,nRows-1> Result;
	unsigned short x;
	for( x = 0; x < c; ++x ){
		unsigned short y;
		for( y = 0; y < r; ++y )
			Result( x, y ) = (*this)( x, y );
		for( y++; y < nRows; ++y )
			Result( x, y - 1u ) = (*this)( x, y );
	}
	for( x++; x < nCols; ++x ){
		unsigned short y;
		for( y = 0; y < r; ++y )
			Result( x - 1u, y ) = (*this)( x, y );
		for( y++; y < nRows; ++y )
			Result( x - 1u, y - 1u ) = (*this)( x, y );
	}
	return Result;
}

template<typename Valtype,const unsigned short nCols>
inline constexpr Vector<Valtype> Column( const Matrix<Valtype,nCols,3>& m, unsigned short idx ) noexcept{
	return Vector<Valtype>{ m(idx,0), m(idx,1), m(idx,2) };
}

template<typename Valtype,const unsigned short nRows>
inline constexpr Vector<Valtype> Row( const Matrix<Valtype,3,nRows>& m, unsigned short idx ) noexcept{
	return Vector<Valtype>{ m(0,idx), m(1,idx), m(2,idx) };
}

template<typename Valtype, unsigned short nCols, unsigned short nRows>
constexpr Matrix<Valtype,nRows,nCols> Transposed( const Matrix<Valtype,nCols,nRows>& m ) noexcept{
	Matrix<Valtype,nRows,nCols> Result;
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			Result(r,c) = m(c,r);
 
	return Result;
}
///////////////////////////////////////
template<typename Valtype, typename Valtype2, const unsigned short nCols, const unsigned short nRows >
inline auto operator*( Valtype scalar, const Matrix<Valtype2,nCols,nRows>& matrix ) noexcept -> Matrix<decltype(Valtype{}*Valtype2{}),nCols,nRows>{
//	static_assert( std::is_scalar<Valtype>::value, "Scalar multiplication only defined for scalar types!" );

	Matrix<decltype(Valtype{}*Valtype2{}),nCols,nRows> Retval;
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			Retval(c,r) = scalar * matrix(c,r);

	return Retval;
}

//
//template<typename Valtype, typename Valtype2, const unsigned short nCols, const unsigned short nRows, typename >
//inline auto operator*( const Matrix<Valtype,nCols,nRows>& matrix, Valtype2 skalar ) -> Matrix<decltype(Valtype{}*Valtype2{}),nCols,nRows>{
//	return skalar * matrix;
//}

template<
	typename Valtype,
	typename Valtype2,
	unsigned short nColsFirst_RowsSecond,
	unsigned short nRowsFirst,
	unsigned short nColsSecond>
inline auto operator*(
	const Matrix<Valtype,nColsFirst_RowsSecond,nRowsFirst>& first,
	const Matrix<Valtype2,nColsSecond,nColsFirst_RowsSecond>& second ) noexcept -> Matrix<decltype(Valtype{}*Valtype2{}),nColsSecond,nRowsFirst>
{
	using ResultType = decltype(Valtype{}*Valtype2{});

	Matrix<ResultType,nColsSecond,nRowsFirst> result;
	for( unsigned short x = 0; x < nColsSecond; ++x )
		for( unsigned short y = 0; y < nRowsFirst; ++y )
		{
			result(x,y) = ResultType{0};
			for( unsigned short z = 0; z < nColsFirst_RowsSecond; ++z )
				result(x,y) += first(z,y) * second(x,z);
		}

	return result;
}

template<	typename Valtype,
			typename Valtype2,
			unsigned short  nCols,
			unsigned short  nRows > inline
Matrix<Valtype,nCols,nRows>& operator*=( Matrix<Valtype,nCols,nRows>& matrix, Valtype2 skalar ) noexcept{
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			matrix(c,r) *= skalar;
	return matrix;
}

template<	typename Valtype,
			typename Valtype2,
			unsigned short  nCols,
			unsigned short  nRows > inline
Matrix<Valtype,nCols,nRows>& operator/=( Matrix<Valtype,nCols,nRows>& matrix, Valtype2 skalar ) noexcept{
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			matrix(c,r) /= skalar;
	return matrix;
}

template<typename Valtype,unsigned short nCols,unsigned short nRows>
inline Matrix<Valtype,nCols,nRows> operator+(const Matrix<Valtype,nCols,nRows>& first,const Matrix<Valtype,nCols,nRows>& second) noexcept{
	Matrix<Valtype,nCols,nRows> result;
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			result(c,r) = first(c,r) + second(c,r);
	return result;
}

template<typename Valtype,unsigned short nCols,unsigned short nRows>
inline Matrix<Valtype,nCols,nRows> operator-(const Matrix<Valtype,nCols,nRows>& first,const Matrix<Valtype,nCols,nRows>& second) noexcept{
	Matrix<Valtype,nCols,nRows> result;
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			result(c,r) = first(c,r) - second(c,r);
	return result;
}

template<typename Valtype,unsigned short nCols,unsigned short nRows>
inline constexpr Matrix<Valtype,nCols,nRows>& operator+( Matrix<Valtype,nCols,nRows>& matrix ) noexcept{
	return matrix;
}

template<typename Valtype,unsigned short nCols,unsigned short nRows>
inline Matrix<Valtype,nCols,nRows>& operator-( Matrix<Valtype,nCols,nRows>& matrix ) noexcept{
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			matrix(c,r) *= -1;
	return matrix;
}

template<	typename Valtype,
			typename Valtype2,
			const unsigned short nCols, 
			const unsigned short nRows > inline
void copy_column_major( const Matrix<Valtype,nCols,nRows>& source, Valtype2* target ) noexcept{
	assert( target );
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			target[c*nRows+r] = static_cast<Valtype2>(source(c,r));
}

template<	typename Valtype,
			typename Valtype2,
			const unsigned short nCols, 
			const unsigned short nRows > inline
void copy_column_major( const Valtype2* source, Matrix<Valtype,nCols,nRows>& target ) noexcept{
	assert( source );
	for( unsigned short c = 0; c < nCols; ++c )
		for( unsigned short r = 0; r < nRows; ++r )
			target(c,r) = static_cast<Valtype>(source[c*nRows+r]);
}

template<	typename Valtype,
			typename Valtype2,
			const unsigned short nCols, 
			const unsigned short nRows > inline
void copy_row_major( const Matrix<Valtype,nCols,nRows>& source, Valtype2* target ) noexcept{
	assert( target );
	for( unsigned short r = 0; r < nRows; ++r )
		for( unsigned short c = 0; c < nCols; ++c )
			target[r*nCols+c] = static_cast<Valtype2>(source(c,r));
}

template<	typename Valtype,
			typename Valtype2,
			const unsigned short nCols, 
			const unsigned short nRows > inline
void copy_row_major( const Valtype2* source, Matrix<Valtype,nCols,nRows>& target ) noexcept{
	assert( source );
	for( unsigned short r = 0; r < nRows; ++r )
		for( unsigned short c = 0; c < nCols; ++c )
			target(c,r) = static_cast<Valtype>(source[r*nCols+c]);
}
///////////////////////////////////////
template<typename Valtype,unsigned short nColsAndRows>
inline SquareMatrix<Valtype,nColsAndRows>::SquareMatrix( const Basetype& matrix )
	: Basetype{matrix}
{
}

template<typename Valtype,unsigned short nColsAndRows>
inline SquareMatrix<Valtype,nColsAndRows>::SquareMatrix( Basetype&& matrix ) noexcept
	: Basetype{std::move(matrix)}
{
}

template<typename Valtype,unsigned short nColsAndRows>
inline SquareMatrix<Valtype,nColsAndRows>::SquareMatrix( const std::initializer_list<Valtype>& elements )
	: Basetype{elements}
{
}

template<typename Valtype, const unsigned short nColsAndRows>
void SquareMatrix<Valtype,nColsAndRows>::SetIdentity() noexcept{
	for( unsigned short x = 0; x < nColsAndRows; ++x )
		for( unsigned short y = 0; y < nColsAndRows; ++y )
			(*this)(x,y) = (Valtype)((x == y) ? 1 : 0);
}

template<typename Valtype, const unsigned short nColsAndRows>
bool SquareMatrix<Valtype,nColsAndRows>::IsIdentity( Valtype epsilon_ ) const noexcept{
	for( unsigned short x = 0; x < nColsAndRows; ++x )
		for( unsigned short y = 0; y < nColsAndRows; ++y )
			if( !common::Equals( (*this)(x,y), (Valtype)((x == y) ? 1 : 0), epsilon_ ) )
				return false;
	return true;
}

template<typename Valtype, const unsigned short nColsAndRows>
bool SquareMatrix<Valtype,nColsAndRows>::IsDiagonal() const noexcept{
	for( unsigned short x = 0; x < nColsAndRows; ++x )
		for( unsigned short y = 0; y < nColsAndRows; ++y )
			if( x != y && (*this)(x,y) != Valtype{0} )
				return false;

	return true;
}

template<typename Valtype,unsigned short nColsAndRows>
inline bool SquareMatrix<Valtype,nColsAndRows>::IsSymmetric( Valtype epsilon_ ) const noexcept{
	for( unsigned short i = 0; i < nColsAndRows; ++i )
		for( unsigned short j = 0; j < nColsAndRows; ++j ){
			if( i == j ) 
				continue;
			if( !common::Equals( (*this)(i,j), (*this)(j,i), epsilon_ ) )
				return false;
		}

	return true;
}

template<typename Valtype, const unsigned short nColsAndRows>
SquareMatrix<Valtype,nColsAndRows>& SquareMatrix<Valtype,nColsAndRows>::Transpose() noexcept{
	for( unsigned short x = 0; x < nColsAndRows; ++x )
		for( unsigned short y = 0; y < x; ++y )
			std::swap( (*this)( x, y ), (*this)( y, x ) );

	return *this;
}

template<typename Valtype, const unsigned short nColsAndRows>
SquareMatrix<Valtype,nColsAndRows>& SquareMatrix<Valtype,nColsAndRows>::Invert(){
	Valtype d = Determinant(*this);
	if( d == Valtype{0} )
		throw std::logic_error( "no inverse matrix" );

	*this = 1 / d * AdjungatedMatrix(*this).Transpose();
	return *this;
}

template<typename Valtype, const unsigned short nColsAndRows>
Valtype SquareMatrix<Valtype,nColsAndRows>::Trace() const noexcept{
	Valtype T = 0;
	for( unsigned short z = 0; z < nColsAndRows; ++z )
		T += (*this)( z, z );

	return T;
}

template<typename Valtype,typename Valtype2> constexpr
inline auto operator*( const SquareMatrix<Valtype,3>& m, const Vector<Valtype2>& v ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>{
	return {
		m(0,0) * v.dx + m(1,0) * v.dy + m(2,0) * v.dz,
		m(0,1) * v.dx + m(1,1) * v.dy + m(2,1) * v.dz,
		m(0,2) * v.dx + m(1,2) * v.dy + m(2,2) * v.dz };
}

template<typename Valtype,unsigned short nColsAndRows>
inline constexpr SquareMatrix<Valtype,nColsAndRows> Inverted( const SquareMatrix<Valtype,nColsAndRows>& m ){
	SquareMatrix<Valtype,nColsAndRows> result{ m };
	result.Invert();
	return result;
}

template<typename Valtype, const unsigned short nColsAndRows > constexpr 
inline SquareMatrix<Valtype,nColsAndRows> Transposed( const SquareMatrix<Valtype,nColsAndRows>& m ){
	SquareMatrix<Valtype,nColsAndRows> Result;
	for( unsigned short c = 0; c < nColsAndRows; ++c )
		for( unsigned short r = 0; r < nColsAndRows; ++r )
			Result(r,c) = m(c,r);
 
	return Result;
}

template<typename Valtype, const unsigned short nColsAndRows > constexpr
auto Determinant( const SquareMatrix<Valtype,nColsAndRows>& m ) -> decltype(pow<nColsAndRows>(Valtype{})){
	decltype(pow<nColsAndRows>(Valtype{})) D{0};
	for( unsigned short z = 0; z < nColsAndRows; ++z )
		D += m( z, 0 ) * Adjungated( m, z, 0 );

	return D;
}

template<typename Valtype, const unsigned short nColsAndRows > constexpr
auto Adjungated( const SquareMatrix<Valtype,nColsAndRows>& m, unsigned short c, unsigned short r ) -> decltype(pow<nColsAndRows-1>(Valtype{})){
	SquareMatrix<Valtype,nColsAndRows-1> sub{ m.SubMatrix( c, r ) };
	return (((r + c) % 2) ? -1 : 1 ) * Determinant( sub );
}

template<typename Valtype > constexpr 
auto Adjungated( const SquareMatrix<Valtype,1>&, unsigned short, unsigned short ) noexcept -> decltype(pow<0>(Valtype{})){
	return 1;
}

template<typename Valtype, const unsigned short nColsAndRows >
SquareMatrix<Valtype,nColsAndRows> AdjungatedMatrix( const SquareMatrix<Valtype,nColsAndRows>& m ){
	SquareMatrix<Valtype,nColsAndRows> Retval;
	for( unsigned short x = 0; x < nColsAndRows; ++x )
		for( unsigned short y = 0; y < nColsAndRows; ++y )
			Retval( x, y ) = Adjungated( m, x, y );

	return Retval;
}

template<typename Valtype1,typename Valtype2>
inline auto operator*( const SquareMatrix<Valtype1,2>& m, const Vector2D<Valtype2>& v ) noexcept -> Vector2D<decltype(Valtype1{}*Valtype2{})>{
	return { m(0,0) * v.dx + m(1,0) * v.dy, m(0,1) * v.dx + m(1,1) * v.dy };
}
///////////////////////////////////////
template<typename Valtype>
inline Transformation<Valtype>::Transformation( const SquareMatrix<Valtype,4>& matrix )
	: SquareMatrix<Valtype,4>{matrix}
{
}

template<typename Valtype>
inline Transformation<Valtype>::Transformation( SquareMatrix<Valtype,4>&& matrix ) noexcept
	: SquareMatrix<Valtype,4>{std::move(matrix)}
{
}

template<typename Valtype>
inline Transformation<Valtype>::Transformation( const Basetype& matrix )
	: SquareMatrix<Valtype,4>{matrix}
{
}

template<typename Valtype>
inline Transformation<Valtype>::Transformation( Basetype&& matrix ) noexcept
	: SquareMatrix<Valtype,4>{std::move(matrix)}
{
}

template<typename Valtype>
template<typename Valtype2>
inline Transformation<Valtype>::Transformation( const Rotation<Valtype2>& rot )
	: SquareMatrix<Valtype,4>{}
{
	operator=(rot);
}

template<typename Valtype>
template<typename Valtype2,typename ValtypeT2>
inline Transformation<Valtype>::Transformation( const Frame<Valtype2,ValtypeT2>& frame )
	: SquareMatrix<Valtype,4>{}
{
	operator=( frame );
}

template<typename Valtype>
inline Transformation<Valtype>::Transformation( const std::initializer_list<Valtype>& elements )
	: SquareMatrix<Valtype,4>{elements}
{
}

template<typename Valtype>
template<typename Valtype2>
Transformation<Valtype>& Transformation<Valtype>::operator=(
	const Rotation<Valtype2>& rot ) noexcept
{
	operator()(0,0) = static_cast<Valtype>(rot(0,0));
	operator()(1,0) = static_cast<Valtype>(rot(1,0));
	operator()(2,0) = static_cast<Valtype>(rot(2,0));
	operator()(3,0) = 0;

	operator()(0,1) = static_cast<Valtype>(rot(0,1));
	operator()(1,1) = static_cast<Valtype>(rot(1,1));
	operator()(2,1) = static_cast<Valtype>(rot(2,1));
	operator()(3,1) = 0;

	operator()(0,2) = static_cast<Valtype>(rot(0,2));
	operator()(1,2) = static_cast<Valtype>(rot(1,2));
	operator()(2,2) = static_cast<Valtype>(rot(2,2));
	operator()(3,2) = 0;

	operator()(0,3) = 0;
	operator()(1,3) = 0;
	operator()(2,3) = 0;
	operator()(3,3) = 1;

	return *this;
}

template<typename Valtype>
template<typename Valtype2,typename ValtypeT2>
Transformation<Valtype>& Transformation<Valtype>::operator=( 
	const Frame<Valtype2,ValtypeT2>& frame ) noexcept
{
	operator()( 0, 0 ) = static_cast<Valtype>(frame.T.dx/ValtypeT2{1});
	operator()( 0, 1 ) = static_cast<Valtype>(frame.T.dy/ValtypeT2{1});
	operator()( 0, 2 ) = static_cast<Valtype>(frame.T.dz/ValtypeT2{1});
	operator()( 0, 3 ) = 0;
																		 
	operator()( 1, 0 ) = static_cast<Valtype>(frame.N.dx/ValtypeT2{1});
	operator()( 1, 1 ) = static_cast<Valtype>(frame.N.dy/ValtypeT2{1});
	operator()( 1, 2 ) = static_cast<Valtype>(frame.N.dz/ValtypeT2{1});
	operator()( 1, 3 ) = 0;
																		 
	operator()( 2, 0 ) = static_cast<Valtype>(frame.B.dx/ValtypeT2{1});
	operator()( 2, 1 ) = static_cast<Valtype>(frame.B.dy/ValtypeT2{1});
	operator()( 2, 2 ) = static_cast<Valtype>(frame.B.dz/ValtypeT2{1});
	operator()( 2, 3 ) = 0;

	operator()( 3, 0 ) = static_cast<Valtype>(frame.P.x/Valtype2{1});
	operator()( 3, 1 ) = static_cast<Valtype>(frame.P.y/Valtype2{1});
	operator()( 3, 2 ) = static_cast<Valtype>(frame.P.z/Valtype2{1});
	operator()( 3, 3 ) = 1;

	return *this;
}

template<typename Valtype>
template<typename Valtype2>
inline PositionH<Valtype2> Transformation<Valtype>::operator*( 
	const PositionH<Valtype2>& pos ) const noexcept
{
	return { 
		(*this)(0,0) * pos.x + (*this)(1,0) * pos.y + (*this)(2,0) * pos.z + (*this)(3,0) * pos.w,
		(*this)(0,1) * pos.x + (*this)(1,1) * pos.y + (*this)(2,1) * pos.z + (*this)(3,1) * pos.w,
		(*this)(0,2) * pos.x + (*this)(1,2) * pos.y + (*this)(2,2) * pos.z + (*this)(3,2) * pos.w,
		(*this)(0,3) * pos.x + (*this)(1,3) * pos.y + (*this)(2,3) * pos.z + (*this)(3,3) * pos.w 
	};
}

template<typename Valtype>
template<typename Valtype2>
inline Position<Valtype2> Transformation<Valtype>::operator*( 
	const Position<Valtype2>& pos ) const noexcept
{
	PositionH<Valtype> HPos{ pos };
	HPos = operator*( HPos );
	HPos.Homogenize();
	return { Valtype2{HPos.x}, Valtype2{HPos.y}, Valtype2{HPos.z} };
}

template<typename Valtype>
template<typename Valtype2>
inline Vector<Valtype2> Transformation<Valtype>::operator*( 
	const Vector<Valtype2>& dir ) const noexcept
{
	PositionH<Valtype> HPos{ dir };
	HPos = operator*( HPos );
	return { Valtype2{HPos.x}, Valtype2{HPos.y}, Valtype2{HPos.z} };
}

template<typename Valtype>
inline void Transformation<Valtype>::CreateTranslation( 
	const Vector<Valtype>& translation ) noexcept
{
	CreateTranslation( translation.dx, translation.dy, translation.dz );
}

template<typename Valtype>
void Transformation<Valtype>::CreateTranslation( 
	Valtype tx, Valtype ty, Valtype tz ) noexcept
{
	operator()(0,0) = 1;
	operator()(1,0) = 0;
	operator()(2,0) = 0;
	operator()(3,0) = tx;

	operator()(0,1) = 0;
	operator()(1,1) = 1;
	operator()(2,1) = 0;
	operator()(3,1) = ty;

	operator()(0,2) = 0;
	operator()(1,2) = 0;
	operator()(2,2) = 1;
	operator()(3,2) = tz;

	operator()(0,3) = 0;
	operator()(1,3) = 0;
	operator()(2,3) = 0;
	operator()(3,3) = 1;
}

template<typename Valtype>
void Transformation<Valtype>::CreateRotation( const Vector<Valtype>& rotation )
	//	Source D. Paull. Charles River Media 2002
	//	This at least is proven to work with the
	//	rotations around the coordinate axes.
{
	Rotation<Valtype> R;
	R.CreateFromAxis(rotation);

	operator()(0,0) = R(0,0);
	operator()(1,0) = R(1,0);
	operator()(2,0) = R(2,0);
	operator()(3,0) = 0;

	operator()(0,1) = R(0,1);
	operator()(1,1) = R(1,1);
	operator()(2,1) = R(2,1);
	operator()(3,1) = 0;

	operator()(0,2) = R(0,2);
	operator()(1,2) = R(1,2);
	operator()(2,2) = R(2,2);
	operator()(3,2) = 0;

	operator()(0,3) = 0;
	operator()(1,3) = 0;
	operator()(2,3) = 0;
	operator()(3,3) = 1;
}

template<typename Valtype>
inline void Transformation<Valtype>::CreateRotation( Valtype rx, Valtype ry, Valtype rz )
{
	CreateRotation( Vector<Valtype>( rx, ry, rz ) );
}

template<typename Valtype>
inline void Transformation<Valtype>::CreateScaling( const Vector<Valtype>& scaling ) noexcept
{
	CreateScaling( scaling.dx, scaling.dy, scaling.dz );
}

template<typename Valtype>
void Transformation<Valtype>::CreateScaling( Valtype sx, Valtype sy, Valtype sz ) noexcept
{
	operator()(0,0) = sx;
	operator()(1,0) = 0;
	operator()(2,0) = 0;
	operator()(3,0) = 0;

	operator()(0,1) = 0;
	operator()(1,1) = sy;
	operator()(2,1) = 0;
	operator()(3,1) = 0;

	operator()(0,2) = 0;
	operator()(1,2) = 0;
	operator()(2,2) = sz;
	operator()(3,2) = 0;

	operator()(0,3) = 0;
	operator()(1,3) = 0;
	operator()(2,3) = 0;
	operator()(3,3) = 1;
}

template<typename Valtype>
template<typename Valtype2>
inline void Transformation<Valtype>::CreateMirror( const VectorBundle<Valtype2,Valtype>& mirror ){
	assert( mirror.T.IsNormal() );
	Matrix<Valtype,1,4> N{ mirror.T.dx, mirror.T.dy, mirror.T.dz, (Origin3D<Valtype2> - mirror.P) * mirror.T / Valtype2{1} };
	*this = IdentityTransformation<Valtype> - 2*N*Transposed(N);
	operator()(0,3) = Valtype{0};
	operator()(1,3) = Valtype{0};
	operator()(2,3) = Valtype{0};
	operator()(3,3) = Valtype{1};
}

template<typename Valtype>
void Transformation<Valtype>::LookAt(	const Position<Valtype>& at, 
										const Position<Valtype>& eye, 
										const Vector<Valtype>& up ) noexcept
{
	//	Vector directed at at;
	Vector<Valtype> d = at - eye;
	d.Normalize();

	//	Vector that really shows up, nearest to up
	//	but othogonal to d;
	Vector<Valtype> u = up - (up*d)*d;
	u.Normalize();

	Vector<Valtype> n = d % u;
	Vector<Valtype> e = eye - Position<Valtype>{0,0,0};

	//	To transform a vector x into new projected coords X,Y,Z
	//	with respect to a right handed coord system (negative Z)
	//	calculations are;
	//	X =  n(x - e)
	//	Y =  u(x - e)
	//	Z = -d(x - e)
	//	This makes:

	*this = { 	n.dx,	n.dy,	n.dz,	-n*e,
				u.dx,	u.dy,	u.dz,	-u*e,
			   -d.dx,  -d.dy,  -d.dz,	 d*e,
				   0,	   0,	   0,	   1 };
}

template<typename Valtype>
void Transformation<Valtype>::CreateViewport( 
	Valtype Width, 
	Valtype Height, 
	Valtype MinZ, 
	Valtype MaxZ,	
	Valtype LeftMargin, 
	Valtype TopMargin  ) noexcept
	// This assumes incomming data to be in a -1 to +1 cube in x/y
	// directions an 0 to -1 in z.
{
	*this = {	Width / 2,			   0,			  0,     LeftMargin + Width / 2,
						0,	 -Height / 2,			  0,     TopMargin + Height / 2,
						0,			   0,	MinZ - MaxZ,					   MinZ,
						0,			   0,			  0,						  1 };
}

template<typename Valtype>
bool Transformation<Valtype>::CreateCameraProjection( 
	Valtype Fovy, Valtype Aspect, Valtype zn, Valtype zf ) noexcept
{
	if( zf <= zn	||
		zn <= 0.0f	||
		Aspect == 0.0f)
		return false;

	Valtype n = zn;
	Valtype f = zf;
	Valtype w2 = n * tan( Fovy / 2 );
	Valtype h2 = w2 / Aspect;

	if( w2 == 0.0f || 
		h2 == 0.0f )
		return false;

	*this = {	n/w2,			0,			0,				0,
				   0,		  n/h2,			0,				0,
				   0,			0,	  f/(f-n),		-f*n/(f-n),
				   0,			0,			1,				0 };

	return true;
}

template<typename Valtype>
bool Transformation<Valtype>::CreateOrthographicProjection( 
	Valtype width, Valtype height, Valtype znear, Valtype zfar ) noexcept
{
	if( zfar	<= znear	||
		znear	<= 0.0f	||
		width	<= 0.0f	||
		height	<= 0.0f	)
		return false;

	Valtype n = znear;
	Valtype f = zfar;
	Valtype w = width;
	Valtype h = height;

	*this = {	2/w,			0,			0,				0,
				  0,		  2/h,			0,				0,
			      0,			0,	  -1/(f-n),		 -n/(f-n),
				  0,			0,			0,				1 };

	return true;
}

template<typename Valtype>
Vector<Valtype> Transformation<Valtype>::TransformVector( const Vector<Valtype>& v ) const noexcept
{
	//If this assertion is not true a vector will not get transformed
	//as expected. The resulting vector would have a 4 - komponent. As
	//vectors are usually differences between two positions the resulting
	//vector would only with a 4 komponent be the difference vector between
	//the transformed positions. Without it, it is not usefull for most purposes.
	//Additionally be aware that normal vectors are covariant and have to
	//be transformed with the inverse transposed matrix.
	assert( (*this)(0,3) == 0 && (*this)(1,3) == 0 && (*this)(2,3) == 0 );

	return Vector<Valtype>{
		operator()(0,0) * v.dx + operator()(1,0) * v.dy  + operator()(2,0) * v.dz,
		operator()(0,1) * v.dx + operator()(1,1) * v.dy  + operator()(2,1) * v.dz,
		operator()(0,2) * v.dx + operator()(1,2) * v.dy  + operator()(2,2) * v.dz };
}

//template<typename Valtype>
//Vector<Valtype> Transformation<Valtype>::TransformNormal( const Vector<Valtype>& v ) const
//{
//	//If this assertion is not true a vector will not get transformed
//	//as expected. Use the inverse transposed of the matrix used for position
//	//transformation matrix to transform normals. Instead you may want to transform
//	//the vector with TransformVector but be aware that this will not work correctly 
//	//for normals if scalings are involved.
//	assert( (*this)(3,0) == 0 && (*this)(3,1) == 0 && (*this)(3,2) == 0 );
//
//	Vector<Valtype> Retval(
//		operator()(0,0) * v.dx + operator()(1,0) * v.dy  + operator()(2,0) * v.dz,
//		operator()(0,1) * v.dx + operator()(1,1) * v.dy  + operator()(2,1) * v.dz,
//		operator()(0,2) * v.dx + operator()(1,2) * v.dy  + operator()(2,2) * v.dz );
//
//	return Retval;
//}

template<typename Valtype>
void Transformation<Valtype>::GetTranslation( 
	Transformation& T ) const noexcept
{
	assert( IsValid() );

	T(0,0) = 1;
	T(1,0) = 0;
	T(2,0) = 0;

	T(0,1) = 0;
	T(1,1) = 1;
	T(2,1) = 0;

	T(0,2) = 0;
	T(1,2) = 0;
	T(2,2) = 1;

	T(3,0) = operator()(3,0);
	T(3,1) = operator()(3,1);
	T(3,2) = operator()(3,2);
	T(3,3) = 1;

	T(0,3) = operator()(0,3);
	T(1,3) = operator()(1,3);
	T(2,3) = operator()(2,3);
}

template<typename Valtype>
bool Transformation<Valtype>::GeRotation( 
	Transformation& R ) const noexcept
{
	assert( IsValid() );

	//	M*ex = R*S*ex = R*sx -> sx = |M*ex| since |R*sx| = |sx|
	Valtype sx	= std::sqrt( std::pow( operator()(0,0), Valtype{2} ) + std::pow( operator()(0,1), Valtype{2} ) + std::pow( operator()(0,2), Valtype{2} ) );
	Valtype sy	= std::sqrt( std::pow( operator()(1,0), Valtype{2} ) + std::pow( operator()(1,1), Valtype{2} ) + std::pow( operator()(1,2), Valtype{2} ) );
	Valtype sz	= std::sqrt( std::pow( operator()(2,0), Valtype{2} ) + std::pow( operator()(2,1), Valtype{2} ) + std::pow( operator()(2,2), Valtype{2} ) );

	if( sx == 0 || sy == 0 || sz == 0 )
	// degenerated matrix
		return false;

	//	Invert Matrix:
	Valtype isx = 1/sx;
	Valtype isy = 1/sy;
	Valtype isz = 1/sz;

	//	R = M * Inv(S)
	R(0,0) = operator()(0,0) * isx;
	R(0,1) = operator()(0,1) * isx;
	R(0,2) = operator()(0,2) * isx;
	R(0,3) = 0;

	R(1,0) = operator()(1,0) * isy;
	R(1,1) = operator()(1,1) * isy;
	R(1,2) = operator()(1,2) * isy;
	R(1,3) = 0;

	R(2,0) = operator()(2,0) * isz;
	R(2,1) = operator()(2,1) * isz;
	R(2,2) = operator()(2,2) * isz;
	R(2,3) = 0;

	R(3,0) = 0;
	R(3,1) = 0;
	R(3,2) = 0;
	R(3,3) = 1;

	return true;
}

template<typename Valtype>
bool Transformation<Valtype>::GetScaling( 
	Transformation& S ) const noexcept
	//	M*ex = R*S*ex = R*sx -> sx = |M*ex| since |R*sx| = |sx|
{
	assert( IsValid() );

	S(0,0) = std::sqrt( std::pow( operator()(0,0), Valtype{2} ) + std::pow( operator()(0,1), Valtype{2} ) + std::pow( operator()(0,2), Valtype{2} ) );
	S(0,1) = 0;
	S(0,2) = 0;
	S(0,3) = 0;

	S(1,0) = 0;
	S(1,1) = std::sqrt( std::pow( operator()(1,0), Valtype{2} ) + std::pow( operator()(1,1), Valtype{2} ) + std::pow( operator()(1,2), Valtype{2} ) );
	S(1,2) = 0;
	S(1,3) = 0;

	S(2,0) = 0;
	S(2,1) = 0;
	S(2,2) = std::sqrt( std::pow( operator()(2,0), Valtype{2} ) + std::pow( operator()(2,1), Valtype{2} ) + std::pow( operator()(2,2), Valtype{2} ) );
	S(2,3) = 0;

	S(3,0) = 0;
	S(3,1) = 0;
	S(3,2) = 0;
	S(3,3) = 1;

	if( S(0,0) == 0 || S(1,1) == 0 || S(2,2) == 0 )
		return false;

	return true;
}

template<typename Valtype>
bool Transformation<Valtype>::Dismantle( Transformation& T, 
										   Transformation& R, 
										   Transformation& S ) const noexcept
{
	if( !GetScaling( S ) )
		return false;

	GetTranslation( T );

	//	Invert Matrix:
	Valtype isx = 1/S(0,0);
	Valtype isy = 1/S(1,1);
	Valtype isz = 1/S(2,2);

	//	R = T * Inv(S)
	R(0,0) = operator()(0,0) * isx;
	R(0,1) = operator()(0,1) * isx;
	R(0,2) = operator()(0,2) * isx;
	R(0,3) = 0;

	R(1,0) = operator()(1,0) * isy;
	R(1,1) = operator()(1,1) * isy;
	R(1,2) = operator()(1,2) * isy;
	R(1,3) = 0;

	R(2,0) = operator()(2,0) * isz;
	R(2,1) = operator()(2,1) * isz;
	R(2,2) = operator()(2,2) * isz;
	R(2,3) = 0;

	R(3,0) = 0;
	R(3,1) = 0;
	R(3,2) = 0;
	R(3,3) = 1;

	return true;
}

template<typename Valtype>
inline bool Transformation<Valtype>::IsValid() const noexcept
{
	if( !common::Equals(operator()(3,3), Valtype{1}, Valtype{0.001} ) )
		return false;

	return true;
}
///////////////////////////////////////
template<	typename Valtype,
			typename Valtype2 > inline
auto operator*( const Transformation<Valtype>& first, 
				const Transformation<Valtype2>& second ) noexcept -> Transformation<decltype(Valtype{}*Valtype2{})>
{
	return operator*( static_cast<const Matrix<Valtype,4,4>&>( first ), static_cast<const Matrix<Valtype2,4,4>&>( second ) );
}

template<typename Valtype>
bool Slerp(	Transformation<Valtype>& out, 
			const Transformation<Valtype>& A,
			const Transformation<Valtype>& B,
			Valtype t )
{
	assert( A.IsValid() );
	assert( B.IsValid() );

	//	B = M * A; means M transforms A to B;
	Transformation<Valtype> iA{ A };
	iA.Invert();
	Transformation<Valtype> M = B * iA;

	Transformation<Valtype> T,R,S;

	if( !M.Dismantle( T, R, S ) )
		return false;

	//	Interpolate translation;
	T(3,0) *= t;
	T(3,1) *= t;
	T(3,2) *= t;

	T(0,3) *= t;
	T(1,3) *= t;
	T(2,3) *= t;

	//	Interpolate rotation spherical linearly (slerp);
	Rotation<Valtype> Rot{ R };

	const Vector<Valtype>	Axis  = Rot.RotationAxis();
	Valtype					Angle = Rot.RotationAngle();

	Angle *= t;
	Rot.CreateFromAxis( Angle * Axis );

	R = Rot;

	//	Interpolate the scalation;
	S(0,0) = 1 + (S(0,0) - 1) * t;
	S(1,1) = 1 + (S(1,1) - 1) * t;
	S(2,2) = 1 + (S(2,2) - 1) * t;

	//	put all together again;
	out = T * R * S * A;

	assert( out.IsValid() );
	return true;
}

template<typename Valtype, typename ValtypeT>
inline Frame<Valtype,ValtypeT> Invert( Frame<Valtype,ValtypeT> frame )
{
	Transformation<ValtypeT> T{ frame };
	T.Invert();
	frame = T;
	return frame;
}
///////////////////////////////////////
template<typename Valtype>
inline Rotation<Valtype>::Rotation( const SquareMatrix<Valtype,3>& matrix )
	:	SquareMatrix<Valtype,3>{matrix}
{
}

template<typename Valtype>
inline Rotation<Valtype>::Rotation( SquareMatrix<Valtype,3>&& matrix ) noexcept
	:	SquareMatrix<Valtype,3>{std::move(matrix)}
{
}

template<typename Valtype>
inline Rotation<Valtype>::Rotation( const Basetype& matrix )
	:	SquareMatrix<Valtype,3>{matrix}
{
}

template<typename Valtype>
inline Rotation<Valtype>::Rotation( Basetype&& matrix ) noexcept
	:	SquareMatrix<Valtype,3>{std::move(matrix)}
{
}

template<typename Valtype>
inline Rotation<Valtype>::Rotation( const Vector<Valtype>& axis )
	:	SquareMatrix<Valtype,3>{}
{
	CreateFromAxis( axis );
}

template<typename Valtype>
inline Rotation<Valtype>::Rotation( Valtype q0, Valtype q1, Valtype q2, Valtype q3 ) noexcept
	:	SquareMatrix<Valtype,3>
{	2*(q0*q0+q1*q1)-1,	2*(q1*q2-q0*q3),	2*(q1*q3+q0*q2),
	2*(q1*q2+q0*q3),	2*(q0*q0+q2*q2)-1,	2*(q2*q3-q0*q1),
	2*(q1*q3-q0*q2),	2*(q2*q3+q0*q1),	2*(q0*q0+q3*q3)-1 }
{
}

template<typename Valtype>
template<typename ValtypeP>
Rotation<Valtype>::Rotation( const Frame<ValtypeP,Valtype>& frame )
	:	SquareMatrix<Valtype,3>{}
{
	operator=( frame );
}

template<typename Valtype>
template<typename Valtype2>
Rotation<Valtype>::Rotation( const Transformation<Valtype2>& tr )
	:	SquareMatrix<Valtype,3>{}
{
	Transformation<Valtype2> R;
	tr.GeRotation( R );

	operator()(0,0) = static_cast<Valtype>(R(0,0));
	operator()(1,0) = static_cast<Valtype>(R(1,0));
	operator()(2,0) = static_cast<Valtype>(R(2,0));

	operator()(0,1) = static_cast<Valtype>(R(0,1));
	operator()(1,1) = static_cast<Valtype>(R(1,1));
	operator()(2,1) = static_cast<Valtype>(R(2,1));

	operator()(0,2) = static_cast<Valtype>(R(0,2));
	operator()(1,2) = static_cast<Valtype>(R(1,2));
	operator()(2,2) = static_cast<Valtype>(R(2,2));
}

template<typename Valtype>
inline Rotation<Valtype>::Rotation( 
	Valtype r00, Valtype r10, Valtype r20, 
	Valtype r01, Valtype r11, Valtype r21, 
	Valtype r02, Valtype r12, Valtype r22 )
	:  SquareMatrix<Valtype,3>{ r00, r10, r20, 
								r01, r11, r21, 
								r02, r12, r22 }
{
}

template<typename Valtype>
template<typename ValtypeP>
inline Rotation<Valtype>& Rotation<Valtype>::operator=( const Frame<ValtypeP,Valtype>& frame ) noexcept
{
	operator()(0,0) = frame.T.dx;
	operator()(0,1) = frame.T.dy;
	operator()(0,2) = frame.T.dz;

	operator()(1,0) = frame.N.dx;
	operator()(1,1) = frame.N.dy;
	operator()(1,2) = frame.N.dz;

	operator()(2,0) = frame.B.dx;
	operator()(2,1) = frame.B.dy;
	operator()(2,2) = frame.B.dz;

	return *this;
}

template<typename Valtype>
void Rotation<Valtype>::CreateFromAxis( const Vector<Valtype>& a )
//	Once and for all: this is a rotation with an axis according to the right-hand-rule. (Thumb
//  points in vector direction, fingers show the rotation)
//
//	a:		rotation vector
//	theta:  rotating angle theta = |a|
//  u:		normalised rotation vector
//  v:		vector to be rotated 
//	v':		rotated vector (v'=R*v;)
//  vp:		parallel part of v with respect to u
//  vo:		orthogonal part of v with respect to u
//	I :		Identity matrix
//  
//  it holds:  
//	v	= vp + vo		(1)
//  vo	= -u % (u % v)  (2)
//
//	if we construct an orthogonal base for the rotation with:
//
//	rx = vo/|vo|
//	ry = u % vo/|vo| = u % v/|vo|
//	rz = u
//
//	we can write vo' as
//
//	vo' = |vo|*cos(theta) * rx + |vo|*sin(theta) * ry 
//		= cos(theta) * vo + sin(theta) * (u % v)		(3)
//
//  v'	= vp'+ vo'
//		= vp + vo'
//	   (3)
//		= vp + cos(theta) * vo + sin(theta) * (u % v)
//		= vp + vo - vo + cos(theta) * vo + sin(theta) * (u % v)
//		= v - (1 - cos(theta)) * vo + sin(theta) * (u % v)
//	   (2)
//		= v + (1 - cos(theta)) * u % (u % v) + sin(theta) * (u % v)  (4)
//
//	We can write 
//
//			(u2v3 - u3v2)	(  0 -u3  u2)   (v1)
//	u % v = (u3v1 - u1v3) =	( u3   0 -u1) * (v2) = 	S * v
//			(u1v2 - u2v1)	(-u2  u1   0)   (v3)
//	with
//
//		  0	 -u3	u2
//	S =	 u3	   0	-u1
//		-u2	  u1	 0
//
//	So (4) gives:
//
//	R = I + sin(theta) * S + (1 - cos(theta)) * S * S
{
	Vector<Valtype> u{ a };
	Valtype theta = u.Normalize();

	SquareMatrix<Valtype,3> S{		0,	-u.dz,	 u.dy,
								 u.dz,      0,	-u.dx,
								-u.dy,	 u.dx,	    0 };

	SquareMatrix<Valtype,3> I{		1,		0,		0,
									0,		1,		0,
									0,		0,		1 };
		
	*this = I + sin(theta) * S + (1 - cos(theta)) * S * S;
}

template<typename Valtype>
inline void Rotation<Valtype>::CreateFromAxis( Valtype rx, Valtype ry, Valtype rz ) noexcept
{
	CreateFromAxis( Vector<Valtype>(rx,ry,rz) );
}

template<typename Valtype>
inline void Rotation<Valtype>::Rotate( Vector<Valtype>& v ) const noexcept{
	v = operator*( v );
}

template<typename Valtype>
template<typename ValtypeP>
inline void Rotation<Valtype>::Rotate( Frame<ValtypeP,Valtype>& frame ) const noexcept{
	frame.T = operator*( frame.T );
	frame.N = operator*( frame.N );
	frame.B = operator*( frame.B );
}

template<typename Valtype>
inline Position<Valtype> Rotation<Valtype>::operator*( 
	const Position<Valtype>& p ) const noexcept
{
	return Position<Valtype>{
		(*this)(0,0) * p.x + (*this)(1,0) * p.y + (*this)(2,0) * p.z,
		(*this)(0,1) * p.x + (*this)(1,1) * p.y + (*this)(2,1) * p.z,
		(*this)(0,2) * p.x + (*this)(1,2) * p.y + (*this)(2,2) * p.z };
}

template<typename Valtype>
inline Vector<Valtype> Rotation<Valtype>::operator*(
	const Vector<Valtype>& v ) const noexcept
{
	return Vector<Valtype>{
		(*this)(0,0) * v.dx + (*this)(1,0) * v.dy + (*this)(2,0) * v.dz,
		(*this)(0,1) * v.dx + (*this)(1,1) * v.dy + (*this)(2,1) * v.dz,
		(*this)(0,2) * v.dx + (*this)(1,2) * v.dy + (*this)(2,2) * v.dz };
}

//template<typename Valtype>
//inline Vector<Valtype> Rotation<Valtype>::RotationAxis() const noexcept
//	//	Source D. Paull. Charles River Media 2002
//	//	This at least is proven to work with the
//	//	rotations around the coordinate axes.
////	Why this?????
//{
//	//Vector<Valtype>	Axis{	(*this)(1,2) - (*this)(2,1),
//	//						(*this)(2,0) - (*this)(0,2),
//	//						(*this)(0,1) - (*this)(1,0) };
//
//	//Axis.Normalize();
//	//return Axis;
//
//	namespace ublas = boost::numeric::ublas;
//
//    // Define the rotation matrix R
//	ublas::matrix<Valtype> R{3,3};
//    
//    // Assign values to the elements of R
//    // Example:
//    R(0,0) = (*this)(0,0); R(0,1) = (*this)(0,1); R(0,2) = (*this)(0,2);
//    R(1,0) = (*this)(1,0); R(1,1) = (*this)(1,1); R(1,2) = (*this)(1,2);
//    R(2,0) = (*this)(2,0); R(2,1) = (*this)(2,1); R(2,2) = (*this)(2,2);
//
//    // Subtract identity matrix from R
//    ublas::matrix<Valtype> A = R - ublas::identity_matrix<Valtype>(3);
//
//    // Define the right-hand side vector (zero vector)
//    ublas::vector<Valtype> b( 3, 0.0 );
//
//    // Solve the system of linear equations Ax = b
//    ublas::permutation_matrix<std::size_t> pm(A.size1());
//    ublas::lu_factorize(A, pm);
//    ublas::lu_substitute(A, pm, b);
//
//	Vector<Valtype>	axis{ b(0), b(1), b(2) };
//	axis.Normalize();
//	return axis;
//}

template<typename Valtype>
inline Vector<Valtype> Rotation<Valtype>::RotationAxis() const noexcept
{
#ifdef TRAX_OPEN_SOURCE
    // Define the rotation matrix R
    Eigen::Matrix3d R;

    // Assign values to the elements of R
    // Example:
    R(0,0) = (*this)(0,0); R(0,1) = (*this)(0,1); R(0,2) = (*this)(0,2);
	R(1,0) = (*this)(1,0); R(1,1) = (*this)(1,1); R(1,2) = (*this)(1,2);
	R(2,0) = (*this)(2,0); R(2,1) = (*this)(2,1); R(2,2) = (*this)(2,2);

    // Subtract identity matrix from R
    Eigen::Matrix3d A = R - Eigen::Matrix3d::Identity();

    // Define the right-hand side vector (zero vector)
	Eigen::Vector3d b{ 0.0, 0.0, 0.0 };

    // Solve the system of linear equations Ax = b
	Eigen::JacobiSVD<Eigen::Matrix3d> svd{ A, Eigen::ComputeFullU | Eigen::ComputeFullV };
    Eigen::Vector3d x = svd.matrixV().col(2); // Extract the last column of the right singular vectors matrix

	Vector<Valtype>	axis{ static_cast<Valtype>(x(0)), static_cast<Valtype>(x(1)), static_cast<Valtype>(x(2)) };
	axis.Normalize();
	return axis;
#else
	return Null<Valtype>;
#endif
}

template<typename Valtype>
Valtype Rotation<Valtype>::RotationAngle() const noexcept
{
	Valtype trace = SquareMatrix<Valtype,3>::Trace();

	Valtype cosangle = common::Clamp( (trace-Valtype{1}) / 2, static_cast<Valtype>(-1), static_cast<Valtype>(+1) );
	return acos( cosangle );
}
///////////////////////////////////////
template<	typename Valtype,
			typename Valtype2 > inline
auto operator*( const Rotation<Valtype>& first, 
				const Rotation<Valtype2>& second ) noexcept -> Rotation<decltype(Valtype{}*Valtype2{})>
{
	return operator*( static_cast<const Matrix<Valtype,3,3>&>( first ), static_cast<const Matrix<Valtype2,3,3>&>( second ) );
}
///////////////////////////////////////
} // namespace spat
