//	trax track library
//	AD 2013 
//
//  "If there is something they need
//   If there is something they want
//   They've got to raise their hand!"
//
//					Bruce Springsteen
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

#include "Configuration.h"
#include "Units.h"

#include <memory>

#include "common/Interval.h"
#include "spat/Frame.h"
#include "spat/Matrix.h"
#include "spat/VectorBundle.h"
#include "spat/VectorBundle2.h"
#include "spat/Vector2D.h"


namespace trax{
/// \page docu_curve Curves
/// \section curve_intro Introduction
/// Curves are separate objects in trax. They can be created as objects, using a trax::Factory; 
/// furthermore each Curve supplies several Create - methods to specify its characteristic. 
/// Each curve type might have its very special characteristic that is summarized in its 
/// CurveType::Data structure. After having properly created a curve, it can get attached to a 
/// track and such will make up the track's geometry. 
/// There also exist trax::CreateCurve methods that help to create the best fitting curve for 
/// solving a special geometrical problem. These also are used by the trax::Strech methods, which 
/// directly change a track and might be more convenient to use if you are writing a track editor.
///
/// \section curve_motivation Motivation
/// A Curve describes a path in space. That path alone determines a tangent vector T that is 
/// directed parallel to the curve at every point of it. A second vector N, the normal vector, 
/// is pointing in the direction of the mean curvature and it happens to be orthogonal to T. 
/// From that, the binormal vector B = T x N can get constructed. Now: two french guys, monsieur Frenet
/// and monsieur Serret, some 160 years ago figured out that this TNB Frame construct is moving 
/// with the parameter value along the curve in a predictable manner, maintaining its orthogonality 
/// and relationship to curvature and torsion of the curve. \see https://en.wikipedia.org/wiki/Frenet%E2%80%93Serret_formulas
///
/// \image html Curve.png
///
/// Since trax is build to work with ODE or PhysX style physics engines, and these engines happen to 
/// deal with velocities (linear and angular) rather than with positions and orientations, we can 
/// not ignore those facts about our TNB frame (well, we tried, but didn't work).
/// 
///
/// \section curve_detail Details
/// Unless differently stated, all curves are parameterized by their arc lengthes. To work with the 
/// physics engine, this is a requirement. Even if no physics engine in the strong sense is applied 
/// it is required to get physically meaningfull values for the track paramer. You might want to 
/// move a position along the track by ds = v*dt with v being a velocity; this can only work with 
/// Curves parametreized by arc length.
/// 
/// Just with the Curve alone there is no such thing defined as an upside or up direction of the Curve 
/// in every point. One might be tempted to mistake the binary vector B for that, but this is
/// plain wrong. Believe us, we tried: in general that vector, like the whole frame will happily 
/// rotate around T in all so fancy ways. Since this is often not acceptable for tracks, which typically
/// maintain strong ideas about what is supposed to be 'up', we provide a trax::RoadwayTwist for fixing 
/// that. Curve and twist define the inner geometry of a track.
/// \image html CurvesInterfaces.png
/// 
/// A single Curve can be shared with several tracks. So some standardized tracks could be created the 
/// same way like it is done in model railroading.
///
/// \section curve_special Some Curves:
///
/// <h3>Line:</h3>
/// 
/// A Line is the shortest curve that connects two arbitrary points in space. Albeit this is a very powerfull 
/// feature, it might turn out to be a little bit cumbersome on the edges as well as in the middle.
/// 
/// \image html CurveLine.jpg
///
/// <h3>Arc:</h3>
/// 
/// When Giotto was asked by the Pope for a sample of his artisanship, he drew a perfect Arc - freehand. 
/// The Arc is regarded by many people as the most beautiful of all curves; others worship it because of 
/// its capability to avoid obstacles by circumventing them. The Arc is very well able to connect two points, 
/// even while maintaining one of the tangents, but the second tangent then would be given - all beauty 
/// suffers from limited flexibility. Since the Arc is a plane curve, it performes very poorly when dealing 
/// with three dimensions. 
/// 
/// \image html CurveArc.jpg
///
/// <h3>Helix:</h3>
/// 
/// The Helix is the first choice when it comes to climbing from one level to another, or - when tilted - 
/// to build a looping. It can connect two nearly arbitrarily situated points and albeit it is a little bit 
/// restrictive with its tangents, a full loop has two parallel tangents at the ends - a feature that in many 
/// situations comes in handy.
///
/// \image html CurveHelix.jpg
///
/// <h3>Cubic:</h3>
/// 
/// The Cubic is a most powerful curve, when it comes to connecting two points and maintaining both tangents. 
/// With railways this is an important feature if it comes to close a gap or make a clean change in level, 
/// since it can work in all three dimensions. On top of this there exist two parameters that allow to further 
/// fine tune the exact path of the curve. 
/// On the other hand, since the Cubic maintains very wild ideas about its up direction it is essential to 
/// use it in conjunction with a directional twist.
///
/// \image html CurveCubic.jpg
///
/// <h3>Clothoid:</h3>
/// 
///	The Clothoid is a curve with linearly (with respect to its arc length) varying curvature. This is a most 
/// usefull feature in traffic systems, when it comes to smoothly transition between curves of different 
/// curvature. Since it happens to be a plane curve, it is somewhat inflexible if it has to deal with more then 
/// two dimensions.
///
/// \image html CurveClothoide.jpg
///
/// <h3>Rotator:</h3>
/// 
/// The Rotator is a curve that constantly rotates (with respect to its arc length) in the plane and 
/// perpendicular to it. Its strong side is the ability to determine the direction of the start and end tangents 
/// as well as its simplicity of concept. It works in a predictable manner especially if the rotation into the 
/// up direction stays reasonably small. On the other hand it can be a very cumbersome curve if used for tasks 
/// that it aint made for.
///
/// \image html CurveRotator.jpg
///
///
/// \image html CurveRotatorWithOffset.jpg
///
/// <h3>RotatorChain:</h3>
/// 
/// The Rotator raises the question, wether Rotators can be appended to each other, so that the total rotating 
/// angles add up in a straightforeward manner. Unfortunately in general the answere is: no. The RotatorChain 
/// cures that, since it defines a series of Rotators with respect to the same rotational axes.
///
/// \image html CurveRotatorChain.jpg
///
/// \section curve_examples Examples:
///
/// To create a quarter circle, starting from origin:
/// \code
///	auto pCurve = Factory::Instance().CreateArc();
/// spat::Interval<Length> range = pCurve->Create( {{0_m,0_m,0_m},{1,0,0}}, {10_m,10_m,0_m} );
/// assert( range.Equals( {0_m,trax::pi/2 * 10_m}, trax::epsilon__length ) );
/// \endcode
///
/// To use one of the helper functions, to create the same Arc:
/// \code
///	auto curve = CreateCurve( Factory::Instance(), {{0_m,0_m,0_m},{1,0,0}}, {{10_m,10_m,0_m},{0,1,0}}, spat::Ez<One> );
/// assert( curve.second.Equals( {0_m,trax::pi/2 * 10_m}, trax::epsilon__length ) );
/// assert( curve.first->TypeName() == "Arc" );
/// \endcode
///
/// To create a Cubic, connecting two points, but starting and ending with parallel tangents:
/// \code
///	auto pCurve = Factory::Instance().CreateCubic();
///	spat::VectorBundle<Length,One> start{{10_m, 10_m, 10_m},{250,0,0}};
///	spat::VectorBundle<Length,One> end{{10_m,70_m,10_m},{250,0,0}};
///	auto range = pCurve->Create( start, end );
/// \endcode
///
/// Do also look at the tests in the test suit for further examples.


	/// \brief Curves implement this interface that then can get 
	/// attached to a track to define the tracks geometry.
	///
	/// The curve has to be parametrized by its arc length to give
	/// correct results for train movement. Depending on the type of curve 
	/// generally the parameter s might run from infinite negative to infinite
	/// positive value.
	/// A concrete curve has to get initialized by a create method and will not 
	/// return usefull results until after calling the create method.
	struct Curve{

		/// \brief Curve type identification values.
		enum class CurveType{
			none	= 0,
			Line,				///< A straight line.
			Arc,				///< A circle circumferrence.
			Helix,				///< A helix curve.
			LineP,				///< A straight line with owned parameters.
			ArcP,				///< A circle circumferrence with owned parameters.
			HelixP,				///< A helix curve with owned parameters.
			Clothoid,			///< An Euler spiral.
			Cubic,				///< The solution for the curve given starting and ending points and tangent vectors.
			Spline,				///< A series of cubics, generated from control points and tangents, also called 'Cubic Hermite Spline'.
			Rotator,			///< A curve with linearly rotating tangent vector.
			RotatorWithOffset,	///< A Rotator curve that starts with certain offset angles.
			RotatorChain,		///< A series of connected Rotator curves.
			PolygonalChain,		///< An ordered list of vertices with tangents.
			SampledCurve,		///< An ordered list of F,k,t,s samples coming from a real curve that are used to numerically interpolate between them.
			Parallel,			///< A Curve that is parallel to a given track.
			EEPCurve,			///< The idiosyncratic curve type used by EEP. The parametrization of the curve is like in 
								/// EEP, but in general it is not the arc length. (Produces the same wrong results as in EEP).
			EEPResidual,		///< Arc length parametrized EEP curve, but only for u != 0, l!= 0 and k²+w² != 0.
			EEPAlternative,		///< Alternative curve shape, but directly parametreized by arc length. Only for u != 0, l!= 0 and k²+w² != 0.
								///  For w == 0 this curves becomes identical to EEPResidual.
			Unknown,			/// 
			UserDefined			///
		};


		/// \returns Name for the type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \returns Type that implements this interface. 
		virtual CurveType GetCurveType() const noexcept = 0;


		/// Tests wether the curve was properly created.
		/// \returns true if the curve is able to deliver valid geometry.
		virtual bool IsValid() const noexcept = 0;


		/// \param s Arc length parameter along the curve
		/// \returns The curvature of the curve at parameter s.
		virtual AnglePerLength Curvature( Length s ) const = 0;


		/// \param s Arc length parameter along the curve
		/// \returns The torsion of the curve at parameter s.
		virtual AnglePerLength Torsion( Length s ) const = 0;


		/// \returns true if it is guaranteed, that Torsion() will return 0_1Im for every s.
		virtual bool IsFlat() const noexcept = 0;


		///	\brief Copies the 3D Position at the specified location to pos.
		/// \param s Arc length parameter along the curve
		///	\param pos Position on curve at parameter s
		/// \throws std::out_of_range if s is not in the valid range of the curve.
		virtual void Transition( Length s, spat::Position<Length>& pos ) const = 0;


		///	\brief Copies the 3D tangential vector at the specified location to tan.
		/// \param s Arc length parameter along the curve
		///	\param tan Tangent on curve at parameter s
		/// \throws std::out_of_range if s is not in the valid range of the curve.
		virtual void Transition( Length s, spat::Vector<One>& tan ) const = 0;


		///	\brief Copies the 3D Position and tangential vector at the specified location to bundle.
		/// \param s Arc length parameter along the curve
		///	\param bundle VectorBundle on curve at parameter s
		/// \throws std::out_of_range if s is not in the valid range of the curve.
		virtual void Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const = 0;


		///	\brief Copies the 3D Position and tangential and normal vectors at the specified location to bundle.
		/// \param s Arc length parameter along the curve
		///	\param bundle VectorBundle on curve at parameter s
		/// \throws std::out_of_range if s is not in the valid range of the curve.
		virtual void Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const = 0;


		///	\brief Copies the 3D TBN-Frame at the specified location to frame.
		/// \param s Arc length parameter along the curve
		///	\param frame TBN-Frame on curve at parameter s
		/// \throws std::out_of_range if s is not in the valid range of the curve.
		virtual void Transition( Length s, spat::Frame<Length,One>& frame ) const = 0;


		/// \brief Returns a list of parameters at which the normal vector
		/// flips from one side to the other.
		///
		/// In general for a curve the curvature might become zero. If this results in a 
		/// rotation of the normal by pi, the parameter ist listed in the returned vector.
		/// No parameter is returned if the curvature gets zero without flipping the normal
		/// (e.g. for a straight line)
		/// The values are sorted by size.
		/// \throws std::runtime_error if the curve was not created yet.
		virtual std::vector<Length> ZeroSet() const = 0;


		/// Some curves can only get evaluated at a certain parameter range. This
		/// is returned by the function, or {-infinite,infinite} if there is no intrinsic 
		/// limit to the parameter value.
		/// \throws std::runtime_error if the curve was not created yet.
		virtual common::Interval<Length> Range() const = 0;


		/// \brief Gives the Curve's idiosyncratic up direction.
		/// Some curves maintain some idea about where they have their upside, either
		/// because of their form (e.g Helix) or because it is extra defined (e.g. for Line).
		/// Some curves maintain no such notion (e.g. many Cubics).
		/// \throws std::runtime_error if the Curve has no notion of up.
		/// \returns The Curve's up direction.
		virtual spat::Vector<One> LocalUp() const = 0;


		/// If there is a definition of a 'standard curve' of the given type, e.g. defined
		/// as starting (s==0) in the origin with a tangent parallel to Ex<One> and a normal
		/// pointing in Ey<One> direction, this function returns the transformation from that
		/// standard curve to the real values returned by this curve. 
		/// GetCurveLocalTransformation().FromParent() transforms this curve to the standard curve.
		/// The above mentioned standard curve is just an example (Transion( 0_m) would deliver the frame),
		/// the actual standard curve might be defined deifferently, e.g. for a helix it is 
		/// an angle 'slope' between Ex<One> and the tangent.
		virtual spat::Frame<Length,One> GetCurveLocalTransformation() const = 0;


		/// \brief make an exact copy of this curve.
		/// \throws std::bad_alloc on memory exhaustion.
		virtual std::unique_ptr<Curve> Clone() const = 0;


		/// \brief Make a Curve with mirrored geometry (but of course one thet returns right handed frames).
		/// \returns false if no mirrored curve could be created for the curve type.
		virtual bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) = 0;


		/// \brief Comparison
		///
		/// Two curves of different types are considered not equal.
		/// \returns true if the geometry and absolute pose of this and the given curve are equal up to the given thresholds.
		/// \param toCurve curve to compare with.
		/// \param range Parameter interval to compare at.
		/// \param epsilon_length maximum acceptable positional deviation.
		/// \param epsilon_angle maximum vector rotational deviation.
		virtual bool Equals( const Curve& toCurve, common::Interval<Length> range, 
			Length epsilon_length = epsilon__length, 
			Angle epsilon_angle = epsilon__angle ) const = 0;


		virtual ~Curve() = default;
		Curve( Curve&& ) = delete;
		Curve& operator=( const Curve& ) = delete;
		Curve& operator=( Curve&& ) = delete;
	protected:
		Curve() = default;
		Curve( const Curve& ) = default;
	};


	/// \brief Creates a clone of the given curve.
	/// \tparam CurveType The type of the curve to clone.
	/// \param curve The curve to clone.
	/// \returns A unique_ptr to the cloned curve with the 
	/// same type as the input curve.
	template<class CurveType> inline
	std::unique_ptr<CurveType> Clone( const CurveType& curve ) noexcept{
		return common::dynamic_unique_cast<CurveType>(curve.Clone());
	}


	/// \defgroup Group_CurveComparison Curve Comparison
	/// \brief Comparison functions for curves.
	/// Two curves of different types are considered not equal.
	/// \returns true if the geometry and absolute pose of this and the given curve are equal up to the given thresholds.
	///@{

	/// \param A curve to compare.
	/// \param B curve to compare with.
	/// \param epsilon_length maximum acceptable positional deviation.
	/// \param epsilon_angle maximum vector rotational deviation.
	bool dclspc Equals( 
		const std::pair<std::shared_ptr<const Curve>,common::Interval<Length>>& A, 
		const std::pair<std::shared_ptr<const Curve>,common::Interval<Length>>& B, 
		Length epsilon_length = epsilon__length, 
		Angle epsilon_angle = epsilon__angle );

	/// \param A curve to compare.
	/// \param B curve to compare with.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum acceptable positional deviation.
	bool dclspc Equals( 
		const Curve& A,
		const Curve& B,
		common::Interval<Length> range,
		Length epsilon_length = epsilon__length ) noexcept;
	///@}


	/// \brief Gets the type name of a curve from its type enumerator.
	dclspc const char* TypeToName( Curve::CurveType ct );


	/// \brief Gets the type enumerator of a curve from its type name.
	dclspc Curve::CurveType CurveNameToType( const std::string& name ) noexcept;


	/// \brief The dynamic data of a curve at one point.
	struct CurveSample{
		spat::Frame<Length,One> F;
		Length s;
		AnglePerLength k;
		AnglePerLength t;

		inline bool operator<( const CurveSample& sample ) const noexcept{
			return s < sample.s;
		}

		bool dclspc Equals( const CurveSample& sample, Length epsilon_length = epsilon__length, One epsilon_angle = epsilon__angle ) const noexcept;
	};

	inline bool operator<( Length s, const CurveSample& sample ) noexcept{
		return s < sample.s;
	}


	/// \brief Data definig a cubic curve.
	///
	/// This is used for persistence. Cubic::GetData() receives the data and
	/// there is a Create() method to create the curve from this data 
	/// set without any calculations made that might distort the values
	/// during many read/write cycles.
	struct CubicData{
		/// \name Construction
		///@{
		CubicData() noexcept
			:	a{spat::Origin3D<Length>},
				b{25_m * spat::Ex<One>},
				c{-15_m * spat::Ex<One> + 15_m * spat::Ey<One>},
				d{10_m * spat::Ex<One> - 10_m * spat::Ey<One>}
		{}

		CubicData( const spat::Position<Length>& a, const spat::Vector<Length>& b, const spat::Vector<Length>& c, const spat::Vector<Length>& d ) noexcept
			:	a{a},
				b{b},
				c{c},
				d{d}
		{}

		CubicData( const spat::VectorBundle<Length>& start, const spat::VectorBundle<Length>& end ) noexcept
			:	a{start.P},
				b{start.T},
				c{3_1*(end.P - start.P) - end.T - 2_1*start.T},
				d{2_1*(start.P - end.P) + start.T + end.T}
		{}
		///@}
		
		/// Data values in the cubic equation p(t) = a + b*t + c*t² + d*t³
		///@{
		spat::Position<Length> a;
		spat::Vector<Length> b;
		spat::Vector<Length> c;
		spat::Vector<Length> d;
		///@}

		inline bool operator!=( const CubicData& other ) const noexcept{
			return	a != other.a ||
					b != other.b ||
					c != other.c ||
					d != other.d;
		}

		inline bool operator==( const CubicData& other ) const noexcept{
			return !operator!=( other );
		}					

		inline spat::Position<Length> CubicPositionAtStart() const noexcept{
			return a;
		}
		inline spat::Vector<Length> CubicOvershootAtStart() const noexcept{
			return b;
		}
		inline spat::Position<Length> CubicPositionAtEnd() const noexcept{
			return a + b + c + d;
		}
		inline spat::Vector<Length> CubicOvershootAtEnd() const noexcept{
			return b + 2_1*c + 3_1*d;
		}
	};


	/// \brief A straight line.
	///
	/// This Line runs along the positive Ex<One>-axis, with normal 
	/// pointing in Ey<One> and binormal pointing in Ez<One> direction. 
	/// It does not support the Curve::Mirror() method.
	struct Line : Curve{

		/// \brief Makes a Line object.
		static dclspc std::unique_ptr<Line> Make() noexcept;
	};


	/// \brief  A straight line with owned parameters.
	///
	/// Try to use 'Line' instead. Only use this curve with redundant parameters
	/// if you really need it; for example to position a curve independently
	/// from the track's frame, or you want to use the create methods on a curve
	/// directly.
	struct LineP : Curve{

		/// \brief Makes a LineP object.
		static dclspc std::unique_ptr<LineP> Make() noexcept;


		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data{

			/// \name Construction
			///@{
			Data() noexcept{
				vb.Init();
			}
			Data( const spat::VectorBundle<Length,One>& vb, const spat::Vector<One>& up ) noexcept
				: vb{vb}, up{up}{}
			///@}
		
			spat::VectorBundle<Length,One> vb;	///< Position of parameter s=0 and tangent of the line.
			spat::Vector<One> up = Up;			///< Up direction.
		};


		/// \name Creation
		/// \brief Creates a straight line.
		/// \param start Defines the starting point of the line segment.
		/// \param end Defines the end point of the line segment.
		/// \param up Up direction (must be normalized).
		/// \param data Defines the starting point in data.vb.pos and the
		/// direction in data.vb.tan.
		/// \returns The total curve range between start and end
		/// \throws std::invalid_argument if the creation fails.
		///@{
		
		/// \brief Creates a straight line.
		virtual common::Interval<Length> Create( const spat::Position<Length>& start, const spat::Position<Length>& end, const spat::Vector<One>& up = Up ) = 0;

		/// \param start Defines the starting point in start.P and the direction in start.T.
		/// \param up Up direction.
		virtual void Create( const spat::VectorBundle<Length,One>& start, const spat::Vector<One>& up = Up ) = 0;

		virtual void Create( const spat::VectorBundle2<Length,One>& start ) = 0;

		virtual void Create( const spat::Frame<Length,One>& start ) = 0;

		/// \param start Defines the starting point.
		///	\param tan Defines the direction of the line.
		/// \param up Up direction.
		virtual void Create( const spat::Position<Length>& start, const spat::Vector<One>& tan, const spat::Vector<One>& up = Up ) = 0;

		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	/// \brief Comparison of two Lines.
	///@{
	
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const LineP::Data& A, const LineP::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle );
	///@}


	/// \brief A plane arc.
	///
	/// An Arc around the origin that starts at Origin<Length> + Radius() * Ex<One> and 
	/// runs in Ey<One> axis direction, bending towards -Ex<One>.
	/// Normal points to origin, binormal along Ez<One>.
	/// \verbatim
	/// 
	///              ( cos(ks) )
	/// P(s) = 1/k * ( sin(ks) )
	///              (    0    )
	/// 
	/// \endverbatim
	struct Arc : Curve{

		/// \brief Makes a Arc object.
		static dclspc std::unique_ptr<Arc> Make() noexcept;

		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data{
			AnglePerLength k; ///< Curvature of the arc.

			inline Length radius() const noexcept { return 1/k; } ///< Radius of Arc.
		};


		/// \brief Returns the partial derivatives of the position P to the curves radius r
		/// and parameter s in a matrix, customly called a 'Jacobian matrix'.
		///
		/// For calculation \see Helix::Jacobian.
		/// \param s parameter to evaluate the derivatives at.
		/// \returns dP/dr and dP/ds will make up the 0th and 1st column of the matrix.
		/// \throws std::bad_alloc on memory exhaustion.
		virtual spat::Matrix<One,2,3> Jacobian( Length s ) const = 0;


		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const Data& data ) = 0;


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{
	
	/// \brief Comparison of two Arcs.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param epsilon_length maximum positional deviation.
	bool dclspc Equals( const Arc::Data& A, const Arc::Data& B, Length epsilon_length = epsilon__length ) noexcept;
	///@}


	/// \brief A plane arc with owned parameters.
	///
	/// Try to use 'Arc' instead. Only use this curve with redundant parameters
	/// if you really need it; for example to position a curve independently
	/// from the track's frame, or you want to use the create methods on a curve
	/// directly.
	///
	/// \verbatim
	/// t: tangent
	/// n: normal
	/// k: curvature
	/// r: curve radius
	/// c: center point
	///
	/// P(s) = C + r * (sin(k*s) * t - cos(k*s) * n);
	/// \endverbatim
	struct ArcP : Curve{

		/// \brief Makes a ArcP object.
		static dclspc std::unique_ptr<ArcP> Make() noexcept;


		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data{
			/// \name Construction
			///@{
			Data() noexcept{
				vb2.Init();
				vb2.N *= _m(10_m);
			}
			Data( Length radius ) noexcept{
				vb2.Init();
				vb2.N *= _m(radius);
			}
			Data( const spat::VectorBundle2<Length,One>& vb2 ) noexcept
				: vb2{vb2}{}
			///@}
		
			spat::VectorBundle2<Length,One> vb2;	///< Center point, tangent and normal of the arc. The curve radius is the length of vb2.N in meters.
		};


		/// \returns the Arc's radius
		virtual Length Radius() const noexcept = 0;


		/// \brief Returns the partial derivatives of the position P-C to the curve radius r
		/// and parameter s in a matrix, customly called a 'Jacobian matrix'.
		///
		/// For calculation \see HelixP::Jacobian. The values are understood relative to the starting
		/// point at the curve (which technically would move, too).
		/// \param s parameter to evaluate the derivatives.
		/// \returns d(P-C)/dr and dP/ds will make up the 0th and 1st column of the matrix.
		/// \throws std::bad_alloc on memory exhaustion.
		virtual spat::Matrix<One,2,3> Jacobian( Length s ) const = 0;


		/// \name Creation
		///@{
		
		/// \brief Creates an Arc.
		/// 
		///	The center point of the arc lies in center.pos; center.tan is the tangent 
		/// at the starting point, center.nor the normal with the curve radius
		/// as its length.
		/// \param center Gives the geometry of the arc.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual void Create( const spat::VectorBundle2<Length,One>& center ) = 0;


		/// \brief Creates an Arc from its starting and ending points.
		/// \param start Starting point and tangent at starting point of the arc.
		/// \param end Ending point and tangent of the arc. I.e. the last point on the curve.
		/// \param e_angle the total angle of the arc that would be too small, so creation fails. 
		/// \returns The total curve range between start and end
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const spat::VectorBundle<Length,One>& start, const spat::Position<Length>& end, Angle e_angle = epsilon__angle ) = 0;


		/// \brief Creates an Arc from its starting and ending points.
		/// 
		/// \param start Starting point and tangent at starting point of the arc.
		/// \param end Ending point and tangent of the arc. I.e. the last point on the curve.
		/// \param e_angle the total angle of the arc that would be too small, so creation fails. 
		/// \returns The total curve range between start and end
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const spat::Position<Length>& start, const spat::VectorBundle<Length,One>& end, Angle e_angle = epsilon__angle ) = 0;


		/// \brief Creates an Arc from its curvature.
		///	
		/// \param start Starting point of the arc.
		/// \param tan Tangent at the arc at starting point.
		/// \param nor Normal direction at starting point.
		/// \param curvature Curvature of the curve i.e. 1 / Curve Radius.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual void Create( const spat::Position<Length>& start, const spat::Vector<One>& tan, const spat::Vector<One>& nor, AnglePerLength curvature ) = 0;


		/// \brief Creates an Arc from curvature.
		///	
		/// \param start Starting point, tangent and normal direction of the arc.
		/// \param curvature Curvature of the curve i.e. 1 / Curve Radius.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual void Create( const spat::VectorBundle2<Length,One>& start, AnglePerLength curvature ) = 0;


		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two Arcs.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const ArcP::Data& A, const ArcP::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle );
	///@}


	/// \brief A three dimensional spiral.
	///
	/// A Helix around the origin that starts at Origin<Length> + Radius() * Ex<One> and 
	/// runs in Ey<One> axis direction, bending towards -Ex and soring towards Ez<One> with 
	/// positive torsion.
	/// \verbatim
	/// k					: curvature
	/// t					: torsion
	/// a					: radius
	/// b/a	== tan(alpha)	: slope
	///
	///         { a*cos( s/sqrt(a²+b²) ) }
	/// P(s) =  { a*sin( s/sqrt(a²+b²) ) }
	///         { b*s/sqrt(a²+b²)		 }
	///
	/// k = a / (a² + b²)
	/// t = b / (a² + b²)
	/// a = k / (k² + t²)
	/// b = t / (k² + t²)
	/// \endverbatim
	struct Helix : Curve{

		/// \brief Makes a Helix object.
		static dclspc std::unique_ptr<Helix> Make() noexcept;

		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data{
			Data() noexcept
				: k{0_1Im}, t{0_1Im}
			{}
			Data( AnglePerLength k, AnglePerLength t ) noexcept
				: k{k}, t{t}
			{}
			Data( Length a, Length b ) noexcept
				: k{a / (a*a + b*b)}, t{b / (a*a + b*b)}
			{}

			AnglePerLength k;	///< Curvature of the helix.
			AnglePerLength t;	///< Torsion of the helix.

			constexpr Length a() const noexcept { return k / (k*k + t*t); }	///< radius
			constexpr Length b() const noexcept { return t / (k*k + t*t); }	///< b == a * slope
			constexpr One slope() const noexcept { return b()/a(); };			///< slope: b/a	== tan(alpha)
		};


		/// \brief Returns the partial derivatives of the position P to the parameters a, b and s
		/// in a matrix, customly called a 'Jacobian matrix'.
		///
		/// The derivatives with respect to k or t can be calculated by
		/// \verbatim
		/// dP/dk = J * { da/dk, db/dk, ds/dk };
		/// \endverbatim
		/// The reason we provide the derivatives for a and b and not k and t is the uniformity 
		/// of units in the matrix for the former.
		/// \param s parameter to evaluate the derivatives.
		/// \returns dP/da, dP/db and dP/ds will make up the 0th, 1st and 2nd column of the matrix.
		/// \throws std::bad_alloc on memory exhaustion.
		virtual spat::SquareMatrix<One,3> Jacobian( Length s ) const = 0;


		/// \brief Create the Helix from data set for wich it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const Data& data ) = 0;


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two Helix.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param epsilon_length maximum positional deviation.
	bool dclspc Equals( const Helix::Data& A, const Helix::Data& B, Length epsilon_length = epsilon__length ) noexcept;
	///@}


	/// \brief A three dimensional spiral with owned parameters.
	///
	/// Try to use 'Helix' instead. Only use this curve with redundant parameters
	/// if you really need it; for example to position a curve independently
	/// from the track's frame, or you want to use the create methods on a curve
	/// directly.
	/// 
	/// \verbatim
	/// k					: curvature
	/// t					: torsion
	/// a					: radius
	/// b/a	== tan(alpha)	: slope
	///
	///         { a*cos( s/sqrt(a²+b²) ) }
	/// P(s) =  { a*sin( s/sqrt(a²+b²) ) }
	///         { b*s/sqrt(a²+b²)		 }
	///
	/// k = a / (a² + b²)
	/// t = b / (a² + b²)
	/// a = k / (k² + t²)
	/// b = t / (k² + t²)
	/// \endverbatim
	struct HelixP : Curve{

		/// \brief Makes a HelixP object.
		static dclspc std::unique_ptr<HelixP> Make() noexcept;


		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data{
			/// \name Construction
			///@{
			Data() noexcept
				:	a{10_m},
					b{5_m/(2*pi)}
			{
				center.Init();
			}
			Data( Length radius, One slope ) noexcept
				:	a{radius},
					b{radius*slope}
			{
				center.Init();
			}
			constexpr Data( const spat::VectorBundle2<Length,One>& center, Length a, Length b ) noexcept
				:	center{center},
					a{a},
					b{b}
			{}
			///@}

		
			spat::VectorBundle2<Length,One> center;	///< The center around which the helix turns. The T vector points to the helix at parameter 0, 
													/// N together with T defines a plane, the helix crosses, N points toward the direction, the helix departs for positive s.
			Length a;								///< radius (not curve radius)
			Length b;								///< b/a == tan(alpha) : slope

			constexpr AnglePerLength Curvature() noexcept { return a / (a*a + b*b); }
			constexpr AnglePerLength Torsion() noexcept { return b / (a*a + b*b); }
			constexpr One Slope() const noexcept { return b/a; };	///< slope: b/a	== tan(alpha)
		};


		/// \returns The center around wich the spiral turns. 
		/// The T vector points to the spiral at parameter 0.
		virtual spat::VectorBundle2<Length,One> Center() const noexcept = 0;


		/// \returns Radius of helix, same as value a.
		virtual Length Radius() const noexcept = 0;


		/// \returns Tangens of inclination angle, being: b/a.
		virtual One Slope() const noexcept = 0;


		/// \brief Returns the partial derivatives of the position P to the parameters a, b and s
		/// in a matrix, customly called a 'Jacobian matrix'.
		///
		/// For calculation \see Helix::Jacobian.
		/// \param s parameter to evaluate the derivatives.
		/// \returns dP/da, dP/db and dP/ds will make up the 0th, 1st and 2nd column of the matrix.
		/// \throws std::bad_alloc on memory exhaustion.
		virtual spat::SquareMatrix<One,3> Jacobian( Length s ) const = 0;


		/// \name Creation
		///@{
		
		/// \brief Create the Helix.
		/// \param start Starting point with tangent, normal and binormal directions.
		/// \param curvature Constant curvature of the Helix. Keep in mind that the radius
		/// of the helix is r = k / (k² + t²), not 1/k.
		/// \param torsion Constant torsion (winding) of the helix.
		/// \throws std::invalid_argument if curvature <= 0.
		virtual void Create( const spat::Frame<Length,One>& start, AnglePerLength curvature, AnglePerLength torsion ) = 0;


		/// \brief Create an upright Helix from start to end.
		/// \param start Starting point with starting direction perpendicular to Up.
		/// \param end End point of helix.
		/// \param up Up direction.
		/// \param e_angle the total angle of the arc that would be too small, so creation fails. 
		/// \returns the range along the curve from start to end.
		/// \throws std::invalid_argument if a helix can not get calculated from input values,
		/// e.g. the two points are too near or the start.T points directly to end.
		virtual common::Interval<Length> Create( const spat::VectorBundle<Length,One>& start, const spat::Position<Length>& end, const spat::Vector<One>& up = Up, Angle e_angle = epsilon__angle ) = 0;


		/// \param start Starting point with starting direction perpendicular to Up.
		/// \param end End point of helix.
		/// \param up Up direction.
		/// \param e_angle the total angle of the arc that would be too small, so creation fails. 
		/// \returns the range along the curve from start to end.
		/// \throws std::invalid_argument if a helix can not get calculated from input values,
		/// e.g. the two points are too near or the start.T points directly to end.
		virtual common::Interval<Length> Create( const spat::Position<Length>& start, const spat::VectorBundle<Length,One>& end, const spat::Vector<One>& up = Up, Angle e_angle = epsilon__angle ) = 0;


		/// \brief Create the Helix.
		/// \param center Center point of the helix. center.T points to starting point of the helix at s=0.
		/// \param curvature Constant curvature of the Helix. Keep in mind that the radius
		/// of the helix is r = c / (c² + t²), not 1/c.
		/// \param torsion Constant torsion (winding) of the helix.
		/// \throws std::invalid_argument if curvature <= 0.
		virtual void Create( const spat::VectorBundle2<Length,One>& center, AnglePerLength curvature, AnglePerLength torsion ) = 0;


		/// \brief Create the Helix from data set for wich it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two HelixP.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const HelixP::Data& A, const HelixP::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle );
	///@}


	/// \brief Cubic polynom curve.
	///
	///	The cubic polynom: p(u) = a + b*u + c*u² + d*u³ in general has one solution
	/// for the boundary value problem that the starting and ending point and tangents
	/// are fixed. It goes like this:
	///
	/// \verbatim
	/// u: parameter running from 0 to 1
	/// p1,t1: starting point and tangent.
	/// p2,t2: ending point and tangent.
	/// p(u) = a + b*u + c*u² + d*u³: cubic equation.
	/// t(u) = b + 2*c*u + 3*d*u²: first derivative of cubic equation.
	///
	/// p1 = p(0) = a				(1)
	/// p2 = p(1) = a + b + c + d	(2)
	/// t1 = t(0) = b				(3)
	/// t2 = t(1) = b + 2c + 3d		(4)
	///
	/// (4)-3*(2)	=> t2-3p2 = -3a - 2b - c
	///             => c = 3p2 -t2 - 3a - 2b
	///             => c = 3p2 -t2 - 3p1 - 2t1 (1),(3)
	/// (2)         => d = p2 - p1 - t1 - (3p2 -t2 - 3p1 - 2t1)
	///             =>   = 2p1 + t1 - 2p2 + t2
	/// \endverbatim
	///
	/// The above curve will get parametrized by its arc length.
	struct Cubic : Curve{

		/// \brief Makes a Cubic object.
		static dclspc std::unique_ptr<Cubic> Make() noexcept;


		using Data = CubicData; ///< Data definig the curve.


		///\brief Gets the first derivative (with respect to the internal parameter u) 
		/// of the Cubic function at arc length s.
		/// Note that you can not use this vector to create a shortened Cubic. Use Shorten()
		/// and/or the Data::CubicOvershootAt() methods instead.
		virtual spat::Vector<Length> CubicOvershootAt( Length s ) const noexcept = 0;


		/// \name Creation
		///@{

		/// \brief Create the Cubic.
		/// \param start Position and tangent (including overshoot) at starting point.
		/// \param end Position and tangent (including overshoot) at end point.
		/// \returns The total curve range between start and end.
		/// \throws std::invalid_argument if the creation fails.
		virtual common::Interval<Length> Create( const spat::VectorBundle<Length,Length>& start, const spat::VectorBundle<Length,Length>& end ) = 0;


		/// \brief Cubic from Cubic Bezier control points.
		///
		/// These are 4 control points (P0,P1,P2,P3)
		/// p1 = P0 
		/// t1 = 3(P1-P0)
		/// p2 = P3
		/// t2 = 3(P3-P2)		
		virtual common::Interval<Length> CreateBezier( const spat::Position<Length>& P0, const spat::Position<Length>& P1, const spat::Position<Length>& P2, const spat::Position<Length>& P3 ) = 0;


		/// \brief Create the Cubic as approximation of an original curve.
		///
		/// The Cubic will be created in a way that it will run from the 
		/// starting point to the ending point of the range. It is tried
		/// to approximate the original curve as good as possible. The 
		/// maximum aberration is returned by the second std::pair value.
		/// \param originalCurve The curve to approximate.
		/// \param range The range on the original curve to approximate.
		/// \param maxDeviation The maximum distance between the original 
		/// curve and the Cubic.
		/// \returns The total curve range between start and end and the 
		/// maximum distance between the original curve and the Cubic.
		/// \throws std::invalid_argument if the creation fails due to malicious input.
		/// \throws std::runtime_error if the curve could not get created.
		virtual std::pair<common::Interval<Length>,Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation = epsilon__length ) = 0;


		/// \brief Create the Cubic from data set for wich it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \returns The total curve range between start and end.
		/// \throws std::invalid_argument if the creation fails.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Transforms the Cubic to one with the given parameter range.
		///
		/// Shortens the curve in a way that it will run from the starting point of
		/// toRange to the end along the same path as the original, but with parameters running 
		/// between {0_m,toRange.Length()}.
		/// \param toRange Parameter range of the original Cubic, to transform the Cubic to.
		/// If toRange is outside the original range it will get clipped.
		/// \returns {0_m,toRange.Length()}, the new parameter range. 
		/// \throws std::invalid_argument if the creation of the shorter cubic fails.
		virtual common::Interval<Length> Shorten( common::Interval<Length> toRange ) = 0;


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two Cubics.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const Cubic::Data& A, const Cubic::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) noexcept;
	///@}


	/// \ brief Cubic Hermite Spline.
	struct Spline : Curve{

		/// \brief Makes a Spline object.
		static dclspc std::unique_ptr<Spline> Make() noexcept;


		/// \brief Type of handling the ends:
		enum class WrapTypes{
			none = 0,
			nonperiodic, 
			periodic, 
			pinned 
		};

		using SegmentValueType = CubicData; ///< Type of the segments.
		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any calculations made that might distort the values
		/// during many read/write cycles.
		using Data = std::vector<SegmentValueType>;


		/// \returns the number of control points.
		virtual int CountControlPoints() const noexcept = 0;


		/// \brief Resets a control point.
		///
		/// The methods will change the controlpoint only. Any property
		/// achieved by using specially interpolated data sets might get lost.
		/// \param idx id of the control point to reset [0,CountControlPoints()[.
		/// \param controlPointAndTangent 3D position and tangent including cubic overshoot.
		/// \returns The total curve range between start and end.
		/// \throws std::logic_error if the spline wasn't created before.
		/// \throws std::out_of_range if idx is out of range.
		virtual common::Interval<Length> ResetControlPointAndTangent( int idx, const spat::VectorBundle<Length>& controlPointAndTangent ) = 0;


		/// \param idx id of the control point to retrieve [0,CountControlPoints()[.
		/// \returns Position and tangent (including cubic overshoot) at control point.
		/// \throws std::out_of_range if idx is out of range.
		/// \throws std::logic_error if the spline wasn't created before.
		virtual spat::VectorBundle<Length> GetControlPointAndTangent( int idx ) const = 0;


		/// \param idx id of the control point to retrieve [0,CountControlPoints()[.
		/// \returns the arc length parameter at a control point.
		/// \throws std::out_of_range if idx is out of range.
		virtual Length GetParameter( int idx ) const = 0;


		/// \brief The index of the control point that is the first one to have 
		/// greater parameter than s.
		/// 
		/// The index might become greater than CountControlPoints() - 1, in what case 
		/// s is beyond the curves range. 
		/// \returns the index of the first control point that has a parameter value greater 
		/// then s.
		virtual int UpperBound( Length s ) const = 0;


		/// \brief Check smoothness of the curve at the control points.
		virtual bool HasGaps( Length epsilon = epsilon__length ) const noexcept = 0;


		/// \brief Check smoothness of the tangets of the curve at the control points.
		virtual bool HasKinks( Angle epsilon = epsilon__angle ) const noexcept = 0;


		/// \name Creation
		///@{
		
		/// \brief Creates a spline curve from an existing curve.
		/// 
		/// \param originalCurve valid template curve.
		/// \param range range on the template curve
		/// \param maxDeviation the maximum positional deviation.
		/// \param sampleDistanceLimits The minimum and maximum distance between the sample points
		/// \throws std::invalid_argument if originalCurve is not a valid curve, or 
		/// the range of the curve does not intersect with range, or maxDeviation <= 0_m.
		/// \throws std::runtime_error if the curve could not get created.
		virtual common::Interval<Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation = epsilon__length, common::Interval<Length> sampleDistanceLimits = { 1_m, 1000_m } ) = 0;
		

		/// \param originalCurve valid template curve.
		/// \param maxDeviation the maximum positional deviation.
		/// \param sampleDistanceLimits The minimum and maximum distance between the sample points
		/// \throws std::invalid_argument if originalCurve is not a valid curve, or 
		/// the range of the curve does not intersect with range, or maxDeviation <= 0_m.
		/// \throws std::runtime_error if the curve could not get created.
		virtual common::Interval<Length> Create( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve, Length maxDeviation = epsilon__length, common::Interval<Length> sampleDistanceLimits = { 1_m, 1000_m } ) = 0;


		/// \brief Creates a Cubic Hermite Spline, guaranteed to be C1 (points and tangents match).
		///
		/// There are methods to yield these controlPoint-tangents by different interpolation methods.
		/// See below.
		/// \returns The total curve range between start and end.
		/// \throws std::invalid_argument if there are less then two control points.
		virtual common::Interval<Length> Create( const std::vector<spat::VectorBundle<Length>>& controlPointsAndTangents ) = 0;


		/// \brief Natural Cubic C2 - smooth Spline (points, tangent, normal and curvature of the segments match).
		///
		/// These are used in railroading often (or better be). The curvatures at start and end
		/// are zero.
		/// \param controlPoints A series of points to be connected by the spline.
		/// \returns The total curve range between start and end.
		/// \throws std::invalid_argument if there are less then two control points.
		virtual common::Interval<Length> Create( const std::vector<spat::Position<Length>>& controlPoints ) = 0;


		/// \brief Clamped Cubic C2 - smooth Spline (points, tangent, normal and curvature of the segments match).
		///
		/// These are used in railroading often (or better be). The curvatures at start and end
		/// are given by the supplied tangent vectors. If both end are set to be not clamped,
		/// a Natural Cubic C2 spline will be generated.
		/// \param controlPoints A series of points to be connected by the spline.
		/// \param bClampedAtStart If true the start tangent will be used.
		/// \param bClampedAtEnd If true the end tangent will be used.
		/// \param startTangent Tangent (and cubic overshoot) at start point.
		/// \param endTangent Tangent (and cubic overshoot) at end point.
		/// \returns The total curve range between start and end.
		/// \throws std::invalid_argument if there are less then two control points.
		virtual common::Interval<Length> Create( const std::vector<spat::Position<Length>>& controlPoints, bool bClampedAtStart, const spat::Vector<Length>& startTangent, bool bClampedAtEnd, const spat::Vector<Length>& endTangent ) = 0;


		/// \brief C1 Spline with Pi'th tangent being: tension * (Pi+1 - Pi-1). 
		///
		/// This might match well with paths on top of terrains. The tangents at start 
		/// and end are tension * (P1 - P0) and tension * (Pn - Pn-1) respectively.
		/// \param controlPoints A series of points to be connected by the spline.
		/// \param tension Streching value for the tangent's cubic overshoot. Must be > 0.
		/// \param wrap Type of wrapping the spline. If WrapTypes::periodic, the first and 
		/// last control point will be connected by an additional segment.
		/// \returns The total curve range between start and end
		/// \throws std::invalid_argument if there are less then two control points.
		/// \throws std::invalid_argument if tension <= 0.
		virtual common::Interval<Length> CreateCatmullRom( const std::vector<spat::Position<Length>>& controlPoints, One tension = 0.5f, WrapTypes wrap = WrapTypes::nonperiodic ) = 0;


		/// \brief C1 Spline with Pi'th tangent being: tension * (Pi+1 - Pi-1).Length() * Ti. 
		///
		/// Modified Catmull-Rom for known tangent directions.
		/// \param controlPoints A series of points and tangents to be connected by the spline.
		/// \param tension Streching value for the tangent's cubic overshoot. Must be > 0.
		/// \param wrap Type of wrapping the spline. If WrapTypes::periodic, the first and 
		/// last control point will be connected by an additional segment.
		/// \returns The total curve range between start and end
		/// \throws std::invalid_argument if there are less then two control points.
		/// \throws std::invalid_argument if tension <= 0.
		virtual common::Interval<Length> CreateCatmullRom( const std::vector<spat::VectorBundle<Length,One>>& controlPoints, One tension = 0.5f, WrapTypes wrap = WrapTypes::nonperiodic ) = 0;


		/// \brief C0 Spline from Cubic Bezier control points.
		///
		/// These are 4 control points (P0,P1,P2,P3) for the first segment and 3 control points (P4,P5,P6) for each 
		/// following segment.
		/// \verbatim
		/// p1 = P0 
		/// t1 = 3(P1-P0)
		/// p2 = P3
		/// t2 = 3(P3-P2)
		/// 
		/// p1 = P3
		/// t1 = 3(P4-P3)
		/// p2 = P6
		/// t2 = 3(P6-P5)
		/// \endverbatim
		/// 
		/// etc...
		/// 
		/// If the wrap type is periodic, the last segment will be closed with the first segment
		/// by an additional segment. For this only two additional control points are expected,
		/// since the first and last control point are the same.
		/// \param controlPoints A series of points to define the spline. For nonperiodic splines the 
		/// number of control points n is needed to define (n-4)/3 + 1 segments; for periodic
		/// it will be n/3 segments.
		/// \param wrap Type of wrapping the spline. 
		/// \returns The total curve range between start and end.
		/// \throws std::invalid_argument if the control points are not the right count.
		virtual common::Interval<Length> CreateBezier( const std::vector<spat::Position<Length>>& controlPoints, WrapTypes wrap = WrapTypes::nonperiodic ) = 0;


		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen, e.g. in write/read cycles.
		/// \returns The total curve range between start and end.
		/// \throws std::invalid_argument if the creation fails.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Transforms the Spline to one with the given parameter range.
		///
		/// Shortens the curve in a way that it will run from the starting point of
		/// toRange to the end along the same path as the original, but with parameters running 
		/// between {0_m,toRange.Length()}.
		/// \param toRange Parameter range of the original Spline, to transform the Spline to.
		/// If toRange is outside the original range it will get clipped.
		/// \returns {0_m,toRange.Length()}, the new parameter range. 
		/// \throws std::invalid_argument if the creation of the shorter spline fails.
		virtual common::Interval<Length> Shorten( common::Interval<Length> toRange ) = 0;


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two Splines.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const Spline::Data& A, const Spline::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) noexcept;
	///@}



	/// \brief Curve with linear increasing curvature. Also called 'Euler Spiral'.
	///
	/// The curve starts at the Origin with tangent {1,0,0} and curvature k = s/a²
	/// with some fixed a. 
	struct Clothoid : Curve{

		/// \brief Makes a Clothoid object.
		static dclspc std::unique_ptr<Clothoid> Make() noexcept;


		/// \brief Data defining the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data{
			Length a{ 100_m }; ///< Single parameter to specify a clothoid.
		};


		/// \brief Gets a limiting angle value.
		///
		/// Due to numerical complexity, the Clothoid can not get evaluated past this
		/// value. T = s*s/(2*a*a) <= MaximumAngle().
		/// \returns the maximum angle the tangent vector can rotate from the parameter 0_m to
		/// the end of the curve.
		static Angle dclspc MaxAngle() noexcept;


		/// \name Limits
		/// \brief Returns the limits on Clothoid creation from two curve radii and the 
		/// curve length between those two points.
		///
		/// One of the values can be changed inside the returned limits so that the curve will still create.
		/// \param curvature0 smaller of the two curvatures, curvature0 = 1 / radius0
		/// \param curvature1 bigger of the two curvatures, curvature1 = 1 / radius1
		/// \param radius0 bigger of the two radii.
		/// \param radius1 smaller of the two radii.
		/// \param length along the clothoid from the point of radius0 to radius1.
		/// \returns A tuple with the three limiting values (use std::get<i>() to extract).
		/// \throws std::invalid_argument if the input values are invalid.
		///@{
		
		static std::tuple<common::Interval<AnglePerLength>, common::Interval<AnglePerLength>, common::Interval<Length>> 
		dclspc Limits( AnglePerLength curvature0, AnglePerLength curvature1, Length length );

		static std::tuple<common::Interval<Length>, common::Interval<Length>, common::Interval<Length>>
		dclspc Limits( Length radius0, Length radius1, Length length );
		///@}


		/// \brief Returns the partial derivatives of the position P to the parameters a and s
		/// in a matrix, customly called a 'Jacobian matrix'.
		///
		/// For a general cuve C(x0(u),x1(u),...) where the parameters xi are functions of 
		/// another parameter u it holds that:
		/// \verbatim
		/// 
		/// dC/du = J * dx/du
		///
		/// \endverbatim
		/// \param s parameter to evaluate the derivatives.
		/// \returns dP/da and dP/ds will make up the 0th and 1st column of the matrix.
		/// \throws std::bad_alloc on memory exhaustion.
		virtual spat::SquareMatrix<One,2> Jacobian( Length s ) const = 0;


		/// \name Creation
		/// \returns The maximal reasonable range, the curve can be evaluated at.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		///@{
			
		/// \brief Create a clothoid with a*a.
		/// \param a2 a*a, the Clothoids paramter squared.
		/// \throws std::invalid_argument if a2 < 0.
		virtual common::Interval<Length> Create( Area a2 ) = 0;


		/// \brief Create a clothoid from a starting and ending curvature and a total length
		///
		/// a2 = l / (k1 - k0), [k0*a2,k1*a2]; l > 0_m, k1 > k0.
		/// \param curvature0 curvature at the interval's near position.
		/// \param curvature1 curvature at the interval's far position.
		/// \param length total length of the interval returned.
		/// \returns the interval on the clothoid that would the transition from curvature0 to curvature1.		
		virtual common::Interval<Length> Create( AnglePerLength curvature0, AnglePerLength curvature1, Length length ) = 0;


		/// \brief Create a clothoid from a starting and ending radius and a total length
		/// \param radius0 radius at the interval's near position.
		/// \param radius1 radius at the interval's far position.
		/// \param length total length of the interval returned.
		virtual common::Interval<Length> Create( Length radius0, Length radius1, Length length ) = 0;


		/// \brief Create the Clothoid from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \param data data set to create the curve from.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two Clothoids.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const Clothoid::Data& A, const Clothoid::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle );
	///@}


	/// \brief Curve with evenly (with respect to arc length) rotating tangent vector.
	///
	/// The curve starts at origin with tangent in Ex direction and will rotate its tangent
	/// around Ez by a*s and around T x Ez by b*s.
	/// 
	/// By definition it is:
	///\verbatim
	///	
	///              ( cos(a(s)) cos(b(s)) )
	/// dP/ds = D1 = ( sin(a(s)) cos(b(s)) )
	///              (           sin(b(s)) )
	///
	///\endverbatim
	/// 
	/// With some arbitray functions a(s), b(s). Since D1² = 1, the solution of the
	/// above differential equation, P(s), will be parameterized by arc length. So we
	/// have to solve the following integral:
	///
	///\verbatim
	///         s               s ( cos(a) cos(b) )
	/// P(s) =  I dP(t)/dt dt = I ( sin(a) cos(b) ) dt =
	///         0               0 (        sin(b) )
	///
	/// Bronstein 1991
	/// 2.5.2.1.3.
	///             s ( cos(a-b) + cos(a+b) )
	///      =  1/2 I ( sin(a-b) + sin(a+b) ) dt
	///             0 (          2 sin(b)   )
	///
	///\endverbatim
	/// 
	/// If we redefine a = a*s and b = b*s with now constant a,b, we get:
	/// 
	///\verbatim
	/// 
	///             s ( cos((a-b)t) + cos((a+b)t) )
	///      =  1/2 I ( sin((a-b)t) + sin((a+b)t) ) dt
	///             0 (             2 sin(bt)     )
	///
	///             (  sin((a-b)t)/(a-b) + sin((a+b)t)/(a+b) ) s
	///      =  1/2 ( -cos((a-b)t)/(a-b) - cos((a+b)t)/(a+b) ) |
	///             (                   -2 cos(bt)/b         ) 0
	///
	///             (  sin((a-b)s)/(a-b) + sin((a+b)s)/(a+b) - 0                 )
	///      =  1/2 ( -cos((a-b)s)/(a-b) - cos((a+b)s)/(a+b) + 1/(a-b) + 1/(a+b) )
	///             (                   -2 cos(bs)/b         + 2/b               )
	///
	///             (      sin((a-b)s)/(a-b) + sin((a+b)s)/(a+b)     )
	///      =  1/2 (  (1-cos((a-b)s))/(a-b) + (1-cos((a+b)s))/(a+b) )
	///             (                        2 (1-cos(bs))/b         )
	///
	///\endverbatim
	///
	/// The Rotator has non constant curvature: k = sqrt(D2*D2) = sqrt( pow<2>(a*cos(b*s)) + pow<2>(b) ), which
	/// only for a == 0 or b == 0 becomes constant, which cases would deliver an arc with curvature b or a respectively.
	/// Note that no other values for a and b would deliver an arc or a helix and that it in general can not get Normalize() 'd.
	struct Rotator : Curve{

		/// \brief Makes a Rotator object.
		static dclspc std::unique_ptr<Rotator> Make( CurveType type = CurveType::Rotator ) noexcept;


		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data{
			AnglePerLength a = 0_1Im;	///< Angular factor to rotate in the plain
			AnglePerLength b = 0_1Im;	///< Angular factor to rotate towards the up direction.
			Angle a0 = 0_deg;			///< Starting angle in the plain
			Angle b0 = 0_deg;			///< Starting angle 
		};


		/// \brief Returns the partial derivatives of the position P to the parameters a, b and s
		/// in a matrix, customly called a 'Jacobian matrix'.
		///
		/// \param s parameter to evaluate the derivatives.
		/// \returns dP/da, dP/db and dP/ds will make up the 0th, 1st, and 2nd column of the matrix.
		/// \throws std::bad_alloc on memory exhaustion.
		virtual spat::SquareMatrix<Real,3> Jacobian( Length s ) const = 0;


		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if for a non-offest rotator a0 or b0 ain't zero.
		virtual common::Interval<Length> Create( const Data& data ) = 0;


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two Rotators.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const Rotator::Data& A, const Rotator::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) noexcept;
	///@}


	/// \brief A series of Rotator curves that continue each other.
	///
	/// The Rotator raises the question, wether there could be a simple additive behaviour
	/// of those angular rotations. Unfortunately this is not given if Rotator curves are
	/// stiched together with their tracks. The reason for this is that in general the rotational
	/// axes would not be the same.
	/// But it is possible to do such addition based on the same frame of reference, which is what 
	/// RotatorChain does:
	struct RotatorChain : Curve{

		/// \brief Makes a RotatorChain object.
		static dclspc std::unique_ptr<RotatorChain> Make() noexcept;

		using SegmentValueType = std::tuple<Angle,Angle,Length>;
		/// \brief Data defining the curve.
		///
		/// The triples are the total anle a, the total angle b and the
		/// length of Rotator segments of the curve.
		using Data = std::vector<SegmentValueType>;


		/// \name Creation
		///@{
		
		/// \brief Create a RotatorChain that starts and ends with the same tangent
		/// and advances by the given vector.
		/// 
		/// Only the advance.dz is strictly respected, the other two might suffer 
		/// aberrivations.
		/// \param advance The vector to advance the curve by (end - start) == advance.
		virtual common::Interval<Length> Create( const spat::Vector<Length>& advance ) = 0;


		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}

		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two RotatorChains.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const RotatorChain::Data& A, const RotatorChain::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) noexcept;
	///@}


	struct Track;

	/// \brief A Curve parallel to a given track.
	///
	/// For a detailed explanation see <a href="../../Doc/Book/appendixC.html">http://../../Doc/Book/appendixC.html</a>.
	struct PointToPointParallel : Curve
	{
		struct Data{
			const Track* pTrack = nullptr;
			spat::Vector2D<Length> d = { 0_m, 0_m };
		};

		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		virtual common::Interval<Length> Create( const Data& data ) = 0;


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;

		virtual Length OriginalParameterFrom( Length sParallelParameter ) const noexcept = 0;

		virtual Length ParallelParameterFrom( Length sOriginalParameter ) const noexcept = 0;

		virtual bool IsTrackRelative() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two PointToPointParallels.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const PointToPointParallel::Data& A, const PointToPointParallel::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) noexcept;
	///@}


	/// \brief A series of samples of points and tangents that make up a curve.
	///
	/// The intermediate values for the TNB frame, curvature and torsion get 
	/// linearly interpolated. The curve by definition is parametrized by arc 
	/// length; the dynamic values in the sample points are calculated like 
	/// this (see Serret-Frenet):
	/// 
	///	\verbatim
	/// Pi, Ti;	
	/// Ni = Normalize( (Ti+1 - Ti) - (Ti+1 - Ti) * Ti * Ti ) = Normalize( Ti+1 - Ti+1*Ti * Ti )
	/// Bi = Ti % Ni
	/// dsi = |Pi+1 - Pi|
	/// ki = |Ti+1 - Ti| / dsi
	/// ti = (Bi - Bi+1) * Ni / dsi
	/// \endverbatim
	/// 
	/// This curve interpolates the dynamic values of a real curve and is meant 
	/// to be used if only some valid points (and maybe valid tangents) along a real curve
	/// are known. To sample a curve with known dynamic data use SampledCurve. 
	struct PolygonalChain : Curve{

		/// \brief Makes a PolygonalChain object.
		static dclspc std::unique_ptr<PolygonalChain> Make() noexcept;


		/// \brief Type of handling the ends:
		enum class WrapTypes{
			none = 0,
			nonperiodic, 
			periodic, 
		};

		using SegmentValueType = spat::VectorBundle<Length, One>; ///< Type of the segments.
		using Data = std::vector<SegmentValueType>;

		/// \returns the parameter value at vertex idx.
		/// \throws std::out_of_range if idx is out of range.
		virtual Length GetParameter( int idx ) const = 0;


		/// \brief The index of the polygonal vertex that is the first one to have 
		/// greater parameter than s.
		/// 
		/// The index might become greater than GetData().size() - 1, in what case 
		/// s is beyond the curves range. 
		/// \returns the index of the first vertex that has a parameter value greater 
		/// then s.
		virtual int UpperBound( Length s ) const = 0;


		/// \name Creation
		///@{

		/// \brief Creates a polygonal chain from an existing curve.
		/// 
		/// \param originalCurve valid template curve.
		/// \param range range on the curve
		/// \param maxDeviation the maximum spacial deviation, so that the polygonal 
		/// change is nowhere farther away from the original.
		/// \param minPointDistance the minimum distance of consecutive points. If 
		/// this is to broad it might be not possoble to maintain maxDeviation.
		/// \throws std::invalid_argument if originalCurve is not a valid curve, or 
		/// the range of the curve does not intersect with range or maxDeviation <= 0_m.
		virtual common::Interval<Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation, Length minPointDistance ) = 0;


		/// \param originalCurve valid template curve.
		/// \throws std::invalid_argument if originalCurve is not a valid curve, or 
		/// the range of the curve does not intersect with range or maxDeviation <= 0_m.
		/// \param maxDeviation the maximum spacial deviation, so that the polygonal 
		/// change is nowhere farther away from the original.
		/// \param minPointDistance the minimum distance of consecutive points. If 
		/// this is to broad it might be not possoble to maintain maxDeviation.
		virtual common::Interval<Length> Create( std::pair<std::shared_ptr<const Curve>, common::Interval<Length>> originalCurve, Length maxDeviation, Length minPointDistance ) = 0;


		/// \brief Creates a polygonal chain from a list of sample points.
		///
		/// The tangents get created by Ti = Normalize( Pi+1 - Pi ).
		/// \param samples List of sample points.
		/// \param wrap Type of wrapping the polygonal chain.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const std::vector<spat::Position<Length>>& samples, WrapTypes wrap = WrapTypes::nonperiodic ) = 0;


		/// \brief Creates a polygonal chain from a list of sample points.
		///
		/// The tangents get created by Ti = Normalize( Pi+1 - Pi-1 ).
		/// \param samples List of sample points.	
		/// \param wrap Type of wrapping the polygonal chain.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> CreateCatmullRom( const std::vector<spat::Position<Length>>& samples, WrapTypes wrap = WrapTypes::nonperiodic ) = 0;


		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if data.size() < 2 or data.size() == 2 and the two 
		/// points are the same.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two polygonal chains.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const PolygonalChain::Data& A, const PolygonalChain::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle );
	///@}


	/// \brief A curve given by Fi,ki,ti,si samples of a real curveT
	/// 
	/// The intermediate values k(s), t(s) are linearly interpolated and from
	/// them the F(s) are integrated following the Frenet differential equations.
	/// If the sample is not taken from a 'real' curve or are too far distant
	/// the segments will not be continuous.
	/// 
	/// If only some points (and maybe tangents) along a curve are known, use PolygonalChain
	/// or Spline.
	struct SampledCurve : Curve{

		/// \brief Makes a SampledCurve object.
		static dclspc std::unique_ptr<SampledCurve> Make() noexcept;


		using Data = std::vector<CurveSample>;


		/// \brief The index of the sample that is the first one to have 
		/// greater parameter than s.
		/// 
		/// The index might become greater than GetData().size() - 1, in what case 
		/// s is beyond the curves range. 
		/// \returns the index of the first vertex that has a parameter value greater 
		/// then s.
		virtual int UpperBound( Length s ) const = 0;


		/// \name Creation
		///@{
		
		/// \brief Creates a sampled curve from an existing curve.
		/// 
		/// \param originalCurve valid template curve.
		/// \param range range on the curve
		/// \param maxDeviationLength the maximum spacial deviation, so that the sample curve 
		/// is nowhere farther away from the original.
		/// \param maxDeviationAngle the maximum angular deviation.
		/// \param sampleDistanceLimits the minimum and maximum distance of consecutive points.
		/// \throws std::invalid_argument if originalCurve is not a valid curve, or 
		/// the range of the curve does not intersect with range or maxDeviation <= 0_m.
		virtual common::Interval<Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviationLength = epsilon__length, Angle maxDeviationAngle = epsilon__angle, common::Interval<Length> sampleDistanceLimits = { epsilon__length, 20_m } ) = 0;


		/// \param originalCurve valid template curve.
		/// \param maxDeviationLength the maximum spacial deviation, so that the sample curve 
		/// \param maxDeviationAngle the maximum angular deviation.
		/// \param sampleDistanceLimits the minimum and maximum distance of consecutive points.
		/// \throws std::invalid_argument if originalCurve is not a valid curve, or 
		/// the range of the curve does not intersect with range or maxDeviation <= 0_m.
		virtual common::Interval<Length> Create( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve, Length maxDeviationLength = epsilon__length, Angle maxDeviationAngle = epsilon__angle, common::Interval<Length> sampleDistanceLimits = { epsilon__length, 20_m } ) = 0;


		/// \brief Create the curve from data set for which it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if data.size() < 2 or data.size() == 2 and the two 
		/// points are the same.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create( const Data& data ) = 0;
		///@}


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two sampled curves.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals( const SampledCurve::Data& A, const SampledCurve::Data& B, common::Interval<Length> range, 
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) noexcept;
	///@}


	/// \brief Idiosyncratic curve used in EEP up to version 16.
	///
	/// This curve in general will not be parameterized by arc length.
	struct EEPCurve : Curve {

		/// \brief Makes a EEPCurve object.
		static dclspc std::unique_ptr<EEPCurve> Make(CurveType type = CurveType::EEPCurve) noexcept;


		/// \brief Data definig the curve.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the curve from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		struct Data {
			/// \name Construction
			//@{
			Data() noexcept
				: gc_Kruemmung{ 0 },
				gc_Windung{ 0 },
				gc_Verdrillung{ 0 },
				gc_Laenge{ _cm(10_m) }, //cm
				gc_Kurve{ 0 },
				m_FuehrungsVerdrehung{ 0 }
			{
				m_AnfangsBein.Init();
			}
			//@}

			Data(const Data&) noexcept = default;
			Data& operator=(const Data&) noexcept = default;

			bool operator==(const Data& data) const noexcept {
				return  gc_Kruemmung == data.gc_Kruemmung &&
					gc_Windung == data.gc_Windung &&
					gc_Verdrillung == data.gc_Verdrillung &&
					gc_Laenge == data.gc_Laenge &&
					gc_Kurve == data.gc_Kurve &&
					m_FuehrungsVerdrehung == data.m_FuehrungsVerdrehung &&
					m_AnfangsBein == data.m_AnfangsBein;
			}


			/// data for EEPCurve. Lengthes in cm.
			//@{
			Real gc_Kruemmung;
			Real gc_Windung;
			Real gc_Verdrillung;
			Real gc_Laenge;
			Real gc_Kurve;
			Real m_FuehrungsVerdrehung;
			spat::Frame<Real> m_AnfangsBein;
			//@}
		};


		/// \brief Create a curve from start and end point
		///
		/// If this method fails, it will leave the curve unchanged (strong guarantee).
		/// \returns the range of the curve.
		/// \throws std::invalid_argument if there is no valid curve btween start and end, e.g because the points are too close.
		//virtual common::Interval<Length> Create( const spat::Frame<Length,One>& start, const spat::Position<Length>& end ) = 0;


		/// \brief Create the Helix from data set for wich it is guaranteed, that 
		/// no calculational drift will happen e.g. in write/read cycles.
		/// \throws std::invalid_argument if the curve can not created from the input values.
		virtual common::Interval<Length> Create(const Data& data) = 0;


		/// \brief Retrieves the data to construct this curve type. A roundtrip 
		/// is guaranteed to be invariant.
		virtual const Data& GetData() const noexcept = 0;
	};


	/// \defgroup Group_CurveComparison Curve Comparison
	///@{

	/// \brief Comparison of two EEPCurves.
	/// \param A First data set.
	/// \param B Second data set.
	/// \param range Parameter interval to compare at.
	/// \param epsilon_length maximum positional deviation.
	/// \param epsilon_angle maximum directional deviation.
	bool dclspc Equals(const EEPCurve::Data& A, const EEPCurve::Data& B, common::Interval<Length> range,
		Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle) noexcept;
	///@}


	/// \brief Creates a matching Curve from the data that is not an EEPCurve.
	/// \returns Pointer to created curve or nullptr.
	/// \throws std::invalid_argument if curve creation is not possible due to invalid data.
	std::unique_ptr<Curve> dclspc CreateNonEEPCurve(const EEPCurve::Data& fromEEPCurveData);


	/// \name Total Angle
	///
	/// The total angle is the sum of all rotations of the tangent vector
	/// according to the curvature. It is the curve integral over the curvature.
	/// The total angle divided by the total length would be some kind
	/// of 'average curvature'. 
	/// Since k(s) always is >= 0, the total angle will always add up the rotations,
	/// they do not cancel out: e.g a curve that starts and ends with the same tangent
	/// but makes some curve inbetween would have a total angle > 0. Only a Line would
	/// have a total angle of 0. The method tends to be expensive.
	///@{
	
	/// \param curve reference to curve and range to evaluate.
	/// \param range the curve shall be evaluated at.
	/// \param accuracy the result shall divert no more from the exact solution than accuracy.
	/// \returns S k(s) ds over curve
	/// \throws std::invalid_argument if accuracynot > 0 or curve.first is nullptr or 
	/// curve is not valid. 
	/// \throws std::domain_error.
	Angle dclspc TotalAngle( const Curve& curve, common::Interval<Length> range, Angle accuracy = epsilon__angle );

	/// \param curve reference to curve and range to evaluate.
	/// \param accuracy the result shall divert no more from the exact solution than accuracy.
	/// \returns S k(s) ds over curve
	/// \returns S k(s) ds over curve
	/// \throws std::invalid_argument if accuracynot > 0 or curve.first is nullptr or 
	/// curve is not valid. 
	/// \throws std::domain_error.
	Angle dclspc TotalAngle( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> curve, Angle accuracy = epsilon__angle );
	///@}


	/// \defgroup Group_Curve Curve Creation
	/// \brief Curve creation by points to connect (Boundary Value Problem). 
	/// The methods try to create the simplest curve that will comply with the given parameters.
	/// If a tangent is missing for either the start or end position, this means that there is no restriction
	/// on it; the method will deliver the simplest curve with any tangent. If a tangent is supplied by a
	/// VectorBundle, it should carry a reasonable cubic overshoot as its length, in case a Cubic can get produced.
	/// The Arc will always be coplanar to the up - direction; the Helix always upright with respect to up.
	/// The error margins control what deviation in the start and end positions and tangents is acceptable. The 
	/// resulting curve is guaranteed to deliver values that deviate no more than these from the parameters used
	/// for creation.
	/// \param start starting point and tangent vector of the curve. If a tangent is supplied it should 
	/// carry the cubic overshoot as its length.
	/// \param end end point and tangent of the curve. If a tangent is supplied it should 
	/// carry the cubic overshoot as its length.
	/// \param up Vector pointing in up direction. Must be of unit length.
	/// \param e_length error margin for the positional values. 
	/// \param e_angle error margin for the directional values.
	/// \returns a pointer to the created curve and the range of 
	/// the curve from start to end.
	/// \throws std::invalid_argument if no curve could get created, e.g. because start and end are collocated.
	///@{
	
	/// \brief Will create a Line. Guarantees start and end.
	std::pair<std::unique_ptr<Curve>,common::Interval<Length>> dclspc CreateCurve( 
		const spat::Position<Length>& start,
		const spat::Position<Length>& end,
		const spat::Vector<One>& up );

	/// \brief Will create a Line, an Arc or a Helix. Guarantees start for Line; guarantees start.T to be 
	/// perpendicular to start.T % up for Arc and Helix. end.P will be in the margin of e_length.
	std::pair<std::unique_ptr<Curve>,common::Interval<Length>> dclspc CreateCurve( 
			  spat::VectorBundle<Length,One> start, 
		const spat::Position<Length>& end, 
		const spat::Vector<One>& up, Length e_length = epsilon__length );

	/// \brief Will create a Line, an Arc or a Helix. Guarantees end for Line. Guarantees end.T to be
	/// perpendicular to end.T % up for Arc and Helix. end.P will be in the margin of e_length.
	std::pair<std::unique_ptr<Curve>,common::Interval<Length>> dclspc CreateCurve( 
		const spat::Position<Length>& start, 
			  spat::VectorBundle<Length,One> end, 
		const spat::Vector<One>& up, Length e_length = epsilon__length );

	/// \brief Will create a Line, an Arc, a Helix or a Cubic.
	std::pair<std::unique_ptr<Curve>,common::Interval<Length>> dclspc CreateCurve( 
		const spat::VectorBundle<Length,One>& start, 
		const spat::VectorBundle<Length,One>& end, 
		const spat::Vector<One>& up, Length e_length = epsilon__length, Angle e_angle = epsilon__angle );
	///@}
	

	/// \defgroup Group_Curve Curve Creation
	/// \brief Curve creation from other curves. 
	/// Creator methods to create 'most similar' curves of a certain type from existing curves.
	/// In general it can not be guaranteed that the resulting curve will be the same as the template 
	/// curve, rather the opposite.
	/// \param type The type of the curve to create.
	/// \param fromCurve The curve to create the new curve from.
	/// \param range The range of the curve to create.
	/// \returns A unique pointer to the newly created curve and a valid range, similar to the given one.
	/// \throws std::invalid_argument if the curve can not created from the input values.
	///@{
	
	std::pair<std::unique_ptr<Curve>, common::Interval<Length>> dclspc CreateCurve( Curve::CurveType type, std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> fromCurve );

	std::pair<std::unique_ptr<Curve>,common::Interval<Length>> dclspc CreateCurve( Curve::CurveType type, const Curve& fromCurve, common::Interval<Length> range );
	
	std::pair<std::unique_ptr<Line>,common::Interval<Length>> dclspc CreateLine( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<LineP>,common::Interval<Length>> dclspc CreateLineP( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<Arc>,common::Interval<Length>> dclspc CreateArc( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<ArcP>,common::Interval<Length>> dclspc CreateArcP( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<Helix>,common::Interval<Length>> dclspc CreateHelix( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<HelixP>,common::Interval<Length>> dclspc CreateHelixP( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<Cubic>,common::Interval<Length>> dclspc CreateCubic( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<Spline>,common::Interval<Length>> dclspc CreateSpline( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<Clothoid>,common::Interval<Length>> dclspc CreateClothoid( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<Rotator>,common::Interval<Length>> dclspc CreateRotator( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<Rotator>,common::Interval<Length>> dclspc CreateRotatorWithOffset( const Curve& fromCurve, common::Interval<Length> range );
	
	std::pair<std::unique_ptr<RotatorChain>,common::Interval<Length>> dclspc CreateRotatorChain( const Curve& fromCurve, common::Interval<Length> range );

	std::pair<std::unique_ptr<PolygonalChain>,common::Interval<Length>> dclspc CreatePolygonalChain( const Curve& fromCurve, common::Interval<Length> range );
	///@}


	/// \brief Interface for streching the curvature of a curve, 
	/// by trying to reach an end point Z, while keeping the starting 
	/// point local, if possible.
	/// 
	/// Some curves that can get parametrized by a curvature parameter
	/// support this interface. Supporting curves are: Arc, ArcP, Helix, 
	/// HelixP, Clothoid.
	/// Streching is useful with graphical editors.
	struct CurvatureStrecher{

		/// \name Get CurvatureStrecher Interface
		/// \brief Retrieve interface for curvature streching.
		///@{
		static dclspc const CurvatureStrecher* InterfaceFrom( const Curve& curve ) noexcept;

		static dclspc CurvatureStrecher* InterfaceFrom( Curve& curve ) noexcept;
		///@}
		

		/// \returns The direction the point on curve at s will move on changing the curvature.
		virtual spat::Vector<One> Direction( Length s ) const = 0;

		virtual AnglePerLength Start( Length s, const spat::Position<Length>& Z, common::Interval<AnglePerLength> curvatureLimits = {epsilon__angle/maximum__length,180_deg/epsilon__length} ) = 0;

		virtual AnglePerLength Strech( Length s, const spat::Position<Length>& Z ) = 0;


		virtual ~CurvatureStrecher() = default;
		CurvatureStrecher( CurvatureStrecher&& ) = delete;
		CurvatureStrecher& operator=( const CurvatureStrecher& ) = delete;
		CurvatureStrecher& operator=( CurvatureStrecher&& ) = delete;
	protected:
		CurvatureStrecher() = default;
		CurvatureStrecher( const CurvatureStrecher& ) = default;
	};
}