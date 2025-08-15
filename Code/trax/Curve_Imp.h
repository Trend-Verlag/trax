// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2013 
//
//  "where is my mind"
//
//			Pixies


#pragma once

#include "Curve.h"
#include "UnitsHelper.h"
#include "Numerics.h" 

#include "common/NarrowCast.h"
#include "dim/support/DimSupportStream.h"
#include "spat/Matrix.h"

#include <cmath>
#include <iostream>

namespace trax{

	using namespace common;
	using namespace spat;


	/// \brief Implements a Curve that is parametrized by its arc length.
	///
	/// The Function type is supposed to implement an arbitrarily parameterized 
	/// curve p(t) with parameter range from 0 to 1 and its derivatives 
	///	dp(t)/dt, d²p(t)/dt² and d³p(t)/dt³. There is a fictive parameter transformation t(s),
	/// which makes the function p(t(s)) be parametrized by its arc length s. It happens to be:
	///
	/// (1) dp/ds = dp/dt * dt/ds = D1 * dt/ds
	///
	/// (2) d²p/ds² = d²p/dt² * (dt/ds)² + dp/dt*d²t/ds² = D2 * (dt/ds)² + D1 * d²t/ds²
	/// 
	/// Since ds = sqrt(D1²) * dt (because (dp/ds)² = 1),  we get:
	/// dt/ds = 1/sqrt(D1²) and the second derivative:
	///	d²t/ds² = -1/(2*sqrt(D1²)³) * 2 * D1*dD1/dt*dt/ds = -D1D2/(D1²D1²)
	/// from that it results:
	/// 
	/// (1) -> dp/ds = D1 / sqrt(D1²)
	///
	/// (2) -> d²p/ds² = (D2 - D1D2/D1² * D1) / D1²
	/// 
	/// d³p/ds³ = 
	///
	/// For this p(t(s)) and its derivatives to s the Frenet-Formulas hold:
	///
	/// T = dp/ds  (being a unit vector)
	/// dT/ds = kN -> k = sqrt((d²p/ds²)²) = sqrt(D2² -(D1D2)²/D1²)/D1²
	/// N = d²p/ds² / sqrt((d²p/ds²)²)
	/// B = T x N
	///
	/// The reparametrization of an arbitrary parametrized curve is computational
	/// demanding. So it is recommended to use this only where unavoidable.
	template<class Function, class Base>
	class Curve_Imp : public Base{
	protected:
		Function f;
	public:
		using DataType = typename Base::Data;

		Curve_Imp() = default;
		Curve_Imp( Real length ) noexcept
			:	f{ length }
		{}

		// Curve:
		bool IsValid() const noexcept override{
			return f.IsValid() && L >= 0_m;
		}

		AnglePerLength Curvature( Length s ) const override{
			const Real ts = t(s);
			const Vector<Length> D1 = f.D1( ts );
			const Vector<Length> D2 = f.D2( ts );
			const auto D1D1 = D1*D1;

			return sqrt(k2(D1,D2,D1D1));
		}

		AnglePerLength Torsion( Length s ) const override
		// Bronstein 1991 4.3.2.2.
		{
			const Real ts = t(s);
			Vector<Length> D1 = f.D1( ts );
			Vector<Length> D2 = f.D2( ts );
			Vector<Length> D3 = f.D3( ts );

			const auto D1D1 = D1*D1;
			const auto k2_ = k2(D1,D2,D1D1);
			if( k2_.Units() ){
				SquareMatrix<Length,3> m;
				m(0,0) = D1.dx;
				m(1,0) = D1.dy;
				m(2,0) = D1.dz;

				m(0,1) = D2.dx;
				m(1,1) = D2.dy;
				m(2,1) = D2.dz;

				m(0,2) = D3.dx;
				m(1,2) = D3.dy;
				m(2,2) = D3.dz;

				return Determinant( m ) / (k2_ * pow<3>(D1D1)); //Why is that?
			}

			return 0_1Im;
		}

		void Transition( Length s, Position<Length>& pos ) const override{
			pos = f( t(s) );
		}

		void Transition( Length s, Vector<One>& tan ) const override{
			tan = f.D1( t(s) ) / Length{1};
			if( tan == Null<One> )
				tan = f.D2( t(s) ) / Length{1};

			tan.Normalize();
		}

		void Transition( Length s, VectorBundle<Length,One>& bundle ) const override{
			const Real ts = t(s);

			bundle.P = f.P( ts );
			bundle.T = f.D1( ts ) / Length{1};
			if( bundle.T == Null<One> )
				bundle.T = f.D2( t(s) ) / Length{1};

			bundle.T.Normalize();
		}

		void Transition( Length s, VectorBundle2<Length,One>& bundle ) const override{
			Real ts = t(s);
			Vector<One> D1{ f.D1(ts) / Length{1} }, D2{ f.D2(ts) / Length{1} };

			bundle.P = f.P( ts );
			if( D1 == Null<One> )
			// If D1 is zero, T = D1/|D1| is undefined, but we can define T then to be 
			// the direction of D2, since this is the direction D1 will have in the 
			// following point.
			{
				bundle.T = f.D2( t(s) ) / Length{1};
				if( ts == 1.f )
					// since there is no following point, we have
					//to take the direction at the previous one.
					bundle.T *= -1;

				bundle.T.Normalize();
				bundle.N = Null<One>; // trigger calculations below...
			}
			else{
				bundle.T = D1;
				bundle.T.Normalize();
				bundle.N = D2 - (D1*D2)/(D1*D1) * D1;
			}

			if( bundle.N.Length() < (1.0f + D2.Length()) * 10 * epsilon )
			// If N is quite small we try to derive it from a
			// neighbouring point. If this doesn't work, use Up.
			{
				ts += (ts >= 100*epsilon ? -100*epsilon : +100*epsilon);
				D1 = f.D1(ts) / Length{1};
				D2 = f.D2(ts) / Length{1};
				
				if( D1 == Null<One> )
					bundle.N = D2;
				else
					bundle.N = D2 - (D1*D2)/(D1*D1) * D1;

				if( bundle.N.Length() < (1.0f + D2.Length()) * 10 * epsilon )
					bundle.N = Up % bundle.T;
			}

			bundle.N = bundle.N - (bundle.N * bundle.T) * bundle.T;
			bundle.N.Normalize();
			assert( abs(bundle.T * bundle.N) < 10 * epsilon );
		}

		void Transition( Length s, Frame<Length,One>& frame ) const override{
			Transition( s, reinterpret_cast<VectorBundle2<Length,One>&>(frame) );
			frame.B = frame.T % frame.N;
		}

		std::vector<Length> ZeroSet() const override{
			if( !IsValid() )
				throw std::runtime_error( "This curve has to be created prior to receiving the zero set." );
			
			std::vector<Length> retval;
			VectorBundle2<Length,One> b2;
			Transition( 0_m, b2 );
			VectorBundle2<Length,One> b1;
			Length s1{0}, s2{0};
			do{
				s1 = s2;
				if( (s2 += ds) > L )
					s2 = L;

				b1 = b2;
				Transition( s2, b2 );
				if( b1.N * b2.N < -0.9 && !(b1.T * b2.T < -0.9) )
					retval.push_back( CloseInOnCurvatureZero( s1, b1.N, s2, b2.N ) );
			}
			while( s2 < L );

			return retval;
		}

		common::Interval<Length> Range() const override{
			if( !IsValid() )
				throw std::runtime_error( "This curve has to be created prior to receiving its maximum range." );

			return {0_m,L};
		}

		spat::Frame<Length,One> GetCurveLocalTransformation() const override{
			spat::Frame<Length,One> retval;
			Transition( 0_m, retval );
			return retval;
		}

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept(noexcept(f.Mirror(mirrorPlane))) override{
			if( !f.IsValid() )
				return false;

			return f.Mirror( mirrorPlane );
		}

		bool Equals( const Curve& toCurve, common::Interval<Length> range, Length epsilon_length, Angle epsilon_angle ) const noexcept override{
			if( this == &toCurve )
				return true;

			if( auto pToCurve = dynamic_cast<const Base*>(&toCurve) )
				return trax::Equals( GetData(), pToCurve->GetData(), range, epsilon_length, epsilon_angle );

			return false;
		}

		common::Interval<Length> Create( const DataType& data ) override{
			f.Create(data);
			return f.IsValid() ? Sample() : common::Interval<Length>{0_m,0_m};
		}

		const DataType& GetData() const noexcept override{
			return f.GetData();
		}

	protected:

		/// The default sample step might seem too coarse, especially if
		/// the progress of the curve changes fast with t. E.g. a spline
		/// assembled of cubics with different overshoots might cause 
		/// problems. These can get remedied by aligning the curves 
		/// lengthes with the sample step.
		Length SampleStep() const noexcept{
			return ds;
		}

		/// p(t0+dt) = p(t0) + D1*dt + D2/2*dt² + D3/3*dt³ + ...
		///
		/// dS = p(t0+dt) - p(t0) = D1*dt + D2/2*dt² + ...
		///
		/// |dS - D1*dt| = | D2/2*dt² + ...| <= m	(1)
		///
		/// That last equation is the positional error we'll make by our integration and we
		/// want it to stay smaller than some m. With n being the total number of integrator 
		/// steps it would hold:
		///
		/// dL <= m * n		with dL being the total error in curve length.
		///
		/// Since t runs from [0,1] for a fixed dt it would hold: n = 1/dt. So for that integrator step
		/// we know:
		/// 
		/// dL*dt <= m (2)
		/// 
		/// So lets assure (1) by demanding:
		///
		/// |dS - D1*dt| = | D2/2*dt² + ...| <= dL*dt <= m	(3)
		/// 
		/// For small enough dt (<= dtMax) this could be achieved by guaranteeing:
		/// 
		/// |D2|/2*dt² <= dL*dt or: dt <= 2*dL/|D2|
		common::Interval<Length> Sample(){
			assert( f.IsValid() );
			assert( ds > 0_m );
			assert( dtMax > One{0} );

			m_Samples.clear();
			m_Samples.reserve( static_cast<std::size_t>(std::ceil(10_m/ds)) + 1 );
			One t = 0;
			Length s{0};
			int i = 1;

			m_Samples.push_back( 0 );
			while( t < One{1} ){
				const auto d1 = f.D1(t);
				const auto d2 = f.D2(t);
				const Length D1 = d1.Length();
				const Length D2 = d2.Length();

				One dt = D2 > (2*dL/dtMax) ? 2*dL/D2 : dtMax;
				if( dt < dtMin )
					dt = dtMin;

				t += dt;
				if( t > One{1} ){
					dt -= t - One{1}; 
					t = One{1};
				}
				s += D1*dt;

				while( s >= i*ds )
				//sample
				{
					m_Samples.push_back( t - (s - i*ds) / D1 );
					++i;
				}
			}
			m_Samples.push_back( One{1} );
			m_Samples.shrink_to_fit();

			dsLast = s - (i-1)*ds + epsilon__length;
			L = s;
			return {0_m,L};
		}

		/// Parameter from arc length function to evaluate f.
		One t( Length s ) const noexcept{
			if( s <= 0_m ) return 0;
			if( L <= s ) return 1;

			Real integer;
			Real fraction = std::modf( s / ds, &integer );
			const std::size_t i = static_cast<std::size_t>(integer);

			if( i+1 == m_Samples.size()-1 )// the last sample does correspond not to ds but to dsLast
				fraction = fraction * ds / dsLast;

			return (1.f - fraction) * m_Samples[i] + fraction * m_Samples[i+1];
		}

		/// Arc length s from parameter t calculated by bisection.
		Length s( One t ) const noexcept{
			if( t <= 0 ) return 0_m;
			if( t >= 1 ) return L;

			common::Interval<int> r{ -1, narrow_cast<int>(m_Samples.size()) };
			while( r.Length() > 1 ){
				const int middle = r.Center();
				t >= m_Samples[middle] ? r.Near( middle ) : r.Far( middle );
			}

			return r.Near() * ds + (t - m_Samples[r.Near()]) / (m_Samples[r.Far()] - m_Samples[r.Near()]) * (r.Near() == narrow_cast<int>(m_Samples.size())-2 ? dsLast : ds);
		}

		bool CheckSamples() const noexcept{
			bool retval = true;
			for( std::size_t i = 1; i < m_Samples.size(); ++i )
				if( m_Samples[i] <= m_Samples[i-1] ){
					std::cerr << "Samples are not monotonically increasing! At s=" << ds * i << std::endl;
					retval = false;
				}
			return retval;
		}
	private:
		Length L{-1};							// total length of curve
		const Length dL = epsilon__length;		// maximum error in total curve length
		const Length ds = 100 * epsilon__length;// sample step
		const One dtMax = 1.0f/100;				// max integrator step
		const One dtMin = 1.0f/10000;			// min integrator step
		std::vector<One> m_Samples;				// samples of t(i*ds)
		Length dsLast{0};						// length of the last sample. L = n*ds + dsLast with n = m_Samples.size() - 2


		Length CloseInOnCurvatureZero( Length s1, const Vector<One>& N1, Length s2, const Vector<One>& N2 ) const{
			Length s = (s1+s2)/2;
			if( common::Equals( s1, s2, epsilon__length ) )
				return s;

			VectorBundle2<Length,One> bundle;
			Transition( s, bundle );

			if( N1 * bundle.N < 0 )
				return CloseInOnCurvatureZero( s1, N1, s, bundle.N );
			else if( bundle.N * N2 < 0 )
				return CloseInOnCurvatureZero( s, bundle.N, s2, N2 );

			return s;
		}

		inline Value<Dimension<-2,0,0>> k2( const Vector<Length>& D1, const Vector<Length>& D2, Area D1D1 ) const{
			if( D1D1 == 0_m2 ) return Value<Dimension<-2,0,0>>{0};
			const Value<Dimension<-2,0,0>> k2{ std::max( (D2*D2 - (pow<2>(D1*D2)/D1D1))/D1D1/D1D1, 0_1Im2 ) };
			return k2;
		}
	};


	/// \brief This calculates the normal vector, curvature and torsion numerically
	/// using the relations derived from the Frenet-Serret formulas:
	/// 
	/// N = dT/ds / |dT/ds|;
    /// k =  dT/ds * N;
    /// t = -dB/ds * N;
	/// 
	template<class Function, class Base>
	class CurveTInterpolator_Imp : public Curve_Imp<Function,Base>{
	public:
		AnglePerLength Curvature( Length s ) const override{
			return DTds( s ).Length();
		}

		AnglePerLength Torsion( Length s ) const override{
			Vector<One> T;
			Transition( s, T );
			return -DBds( s ) * N( T, DTds( s ) );
		}

		using Curve_Imp<Function,Base>::Transition;

		void Transition( Length s, VectorBundle2<Length,One>& bundle ) const override{
			Transition( s, bundle.P );
			Transition( s, bundle.T );
			bundle.N = N( bundle.T, DTds( s ) );
		}

		using Curve_Imp<Function,Base>::Range;
		using Curve_Imp<Function,Base>::GetData;
	protected:
		using Curve_Imp<Function,Base>::f;
	private:
		static inline const Length h4 = 0.15_m; // can not evaluate for s out of range; 4h ~ 1.8_m see finite_difference_derivative8

		Vector<AnglePerLength> DTds( Length s ) const{	
			const auto T = [this]( Length s ) -> Vector<One>{
				Vector<One> T;
				Transition( s, T );
				return T;
			};

			return finite_difference_derivative6<Vector<AnglePerLength>>( T, Range().Deflate( h4 ).Clip( s ) );
		}

		Vector<AnglePerLength> DBds( Length s ) const{
			const auto B = [this]( Length s ) -> Vector<One>{
				Frame<Length,One> F;
				Transition( s, F );
				return F.B;
			};

			return finite_difference_derivative6<Vector<AnglePerLength>>( B, Range().Deflate( h4 ).Clip( s ) );
		}

		static Vector<One> N( const Vector<One>& T, const Vector<AnglePerLength>& dTds ) noexcept{
			Vector<One> N;
			if( dTds != Null<AnglePerLength> )
				N = Normalize( dTds ).second;
			else{
				N = Up % T;
				if( N == Null<One> )
					N = Parallel( Up, Ex<One> ) ? Ey<One> : Ex<One>; 
			}

			N = N - (N * T) * T;
			N.Normalize();
			return N;
		}	
	};


	/// \brief If the curve function happens to be parameterized by arc length, this
	/// implementation for a curve can be used. 
	///
	/// In the general argument the dt/ds becomes 1 and the d²t/ds² becomes 0.
	/// From that it follows:
	///
	/// T = D1
	/// N = D2 / sqrt(D2*D2)
	/// k = sqrt(D2*D2)
	/// t = (D1 % D2) * D3 / (D2*D2)
	/// \see Curve_Imp
	template<class FunctionParametrizedByArcLength, class Base>
	class CurveArcLength_Imp : public Base{
	protected:
		FunctionParametrizedByArcLength f;
	public:
		using DataType = typename Base::Data;


		// Curve:
		bool IsValid() const noexcept override{
			return f.IsValid();
		}

		AnglePerLength Curvature( Length s ) const noexcept override{
			return f.D2(s).Length();
		}

		AnglePerLength Torsion( Length s ) const override		
		// (D1 % D2) * D3 / (D2*D2) (see Chapter3)
		{
			const auto D2 = f.D2(s);
			if( const auto D2D2 = D2*D2 )
				return (f.D1(s) % D2) * f.D3(s) / (D2D2);
			else
				return 0_1Im;
		}

		void Transition( Length s, Position<Length>& pos ) const override{
			pos = f(s);
		}

		void Transition( Length s, Vector<One>& tan ) const override{
			tan = f.D1(s);
		}

		void Transition( Length s, VectorBundle<Length,One>& bundle ) const override{
			bundle.P = f(s);
			bundle.T = f.D1(s);
		}

		void Transition( Length s, VectorBundle2<Length,One>& bundle ) const override{
			bundle.P = f(s);
			bundle.T = f.D1(s);

			bundle.N = f.D2(s) / Value<Dimension<-1,0,0>>{1};
			if( bundle.N.Length() < 10 * epsilon ){
				s += (s >= (f.Range().Normal() ? f.Range().Near() : f.Range().Far()) + epsilon__length ? -epsilon__length : +epsilon__length);

				bundle.N = f.D2(s) / Value<Dimension<-1,0,0>>{1};

				if( bundle.N.Length() < 10 * epsilon )
					bundle.N = Up % bundle.T;
			}

			bundle.N = bundle.N - (bundle.N * bundle.T) * bundle.T;
			bundle.N.Normalize();
			assert( abs(bundle.T * bundle.N) < 10 * epsilon );
		}

		void Transition( Length s, Frame<Length,One>& frame ) const override{
			Transition( s, reinterpret_cast<VectorBundle2<Length,One>&>(frame) );
			frame.B = frame.T % frame.N;
		}

		std::vector<Length> ZeroSet() const override{
			if( !IsValid() )
				throw std::runtime_error( "This curve has to be created prior to receiving the zero set." );

			return {};
		}

		common::Interval<Length> Range() const noexcept(noexcept(f.Range())) override{ 
			return f.Range();
		}

		spat::Frame<Length,One> GetCurveLocalTransformation() const override{
			spat::Frame<Length,One> retval;
			Transition( 0_m, retval );
			return retval;
		}

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept(noexcept(f.Mirror(mirrorPlane))) override{
			if( !f.IsValid() )
				return false;

			return f.Mirror( mirrorPlane );
		}

		bool Equals( const Curve& toCurve, common::Interval<Length> range, Length epsilon_length, Angle epsilon_angle ) const override{
			if( this == &toCurve )
				return true;

			if( auto pToCurve = dynamic_cast<const Base*>(&toCurve) )
				return trax::Equals( GetData(), pToCurve->GetData(), range, epsilon_length, epsilon_angle );

			return false;
		}

		common::Interval<Length> Create( const DataType& data ) override{
			f.Create(data);
			return f.Range();
		}

		const DataType& GetData() const noexcept override{
			return f.GetData();
		}
	};


	/// \brief Reparametrizes a Function to 0 - 1. 
	///
	/// Use it as Function for Curve_Imp. Access the original function
	/// by f.f. e.g. for creation.
	template<class Function>
	class Reparametrization{
	public:
		Reparametrization() noexcept
			:	m_Length	{-1},
				m_Length2	{-1},
				m_Length3	{-1}
		{
		}

		Reparametrization( Real length ) noexcept
			:	m_Length	{length},
				m_Length2	{m_Length*m_Length},
				m_Length3	{m_Length*m_Length2}
		{
			assert( m_Length >= 0 );
		}

		Reparametrization( const Reparametrization& ) = default;
		Reparametrization( Reparametrization&& ) = default;
		Reparametrization& operator=( const Reparametrization& ) = default;
		Reparametrization& operator=( Reparametrization&& ) = default;
		~Reparametrization() = default;


		void Length( Real length ) noexcept{
			assert( length >= 0 );
			m_Length	= length;
			m_Length2	= m_Length*m_Length;
			m_Length3	= m_Length*m_Length2;
		}

		Real Length() const noexcept{
			return m_Length;
		}

		bool IsValid() const noexcept{
			return f.IsValid() && m_Length >= 0;
		}

		inline Position<dim::Length> operator()( Real t ) const{
			return P(t);
		}

		Position<dim::Length> P( Real t ) const{
			return f( m_Length * t  );
		}

		Vector<dim::Length> D1( Real t ) const{
			return m_Length * f.D1( m_Length * t );
		}

		Vector<dim::Length> D2( Real t ) const{
			return m_Length2 * f.D2( m_Length * t );
		}

		Vector<dim::Length> D3( Real t ) const{
			return m_Length3 * f.D3( m_Length * t );
		}

		bool Mirror( const spat::VectorBundle<dim::Length,One>& mirrorPlane ){
			return f.Mirror( mirrorPlane );
		}

		common::Interval<Real> Create( const typename Function::DataType& data ){
			Length( f.Create(data).Length() );
			return { 0.f, 1.f };
		}

		const typename Function::DataType& GetData() const noexcept{
			return f.GetData();
		}

		Function f;
	private:
		One m_Length;
		One m_Length2;
		One m_Length3;
	};


	template<class MainBase>
	class CurvatureStrecher_Imp :	public MainBase,
									public CurvatureStrecher{
	public:

		AnglePerLength Start( Length s, const spat::Position<Length>& Z, common::Interval<AnglePerLength> curvatureLimits ) override{
			m_CurvatureLimits = curvatureLimits;
			m_ZStart = Z;
			spat::Position<Length> E;
			this->Transition( s, E );
			m_TargetOffset = (Z - E) * this->Direction( s );
			return this->Curvature(s);
		}
	protected:
		template<class FunctionType>
		AnglePerLength solve_root( FunctionType f, AnglePerLength bestGuess ) const{
			if( m_CurvatureLimits.Touches(bestGuess) ){
				constexpr boost::uintmax_t maxit = 30;
				boost::uintmax_t it = maxit;

				try{
					const Interval<AnglePerLength> bracket{ trax::bracket_and_solve_root(
						f, 
						bestGuess,																			// good news: we have quite a plausible guess value
						1.1f,																				// use small intervals, since f is not strictly monotonic
						f( bestGuess + epsilon__curvature ) - f( bestGuess - epsilon__curvature ) >= 0_m,	// f rising or falling around k? Cannot figure it out less stupid ...
						boost::math::tools::eps_tolerance<Real>{std::numeric_limits<Real>::digits-3},		// allow for inaccuracy in f(k)
						it																					// coming close to a straight line manifests in exceeding maxit...
					) };

					AnglePerLength k = bracket.Center();
					std::cout << "[" << bracket.Near() << "," << bracket.Far() << "] iterations: " << it << " k= " << k << std::endl;

					if( it <= maxit && m_CurvatureLimits.Touches( k ) )
						return k;
				}
				catch( const boost::math::evaluation_error& e ){
					std::cerr << e.what() << std::endl;
				}
				catch( const std::invalid_argument& e ){
					std::cerr << e.what() << std::endl;
				}
			}

			return bestGuess;
		}

		common::Interval<AnglePerLength>	m_CurvatureLimits = {epsilon__angle/maximum__length,180_deg/epsilon__length};
		spat::Position<Length>				m_ZStart = spat::Origin3D<Length>;
		Length								m_TargetOffset = 0_m;
	};
}