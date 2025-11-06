//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2016 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "trax/Curve_Imp.h"

namespace trax{
	/*
	/// The EEP curve evaluates to straight lines, circles and spirals - wich can be constructed
	/// with other curves from trax. This here is the remaining curve shape that comes into play
	/// if u != 0, l!= 0 and pow<2>(k)+pow<2>(w) != 0.
	class EEPResidualCurveFunction{
	public:
		using DataType = typename EEPCurve::Data;

		Position<Length> operator()( Real t ) const{
			return P(t);
		}

		bool IsValid() const{
			return u != 0 && l > 0 && c > 0;
		}

		/// Curve shape P(t)
		Position<Length> P( Real t ) const{
			Real b = u/l*t;
			Real a = c * l/u * sin(b);

			return { k/(c*c)			* cos(a),
					 std::fabs(k)/(c*c)	* sin(a),
					 l/u * (1-cos(b))
			};
		}

		/// First derivative dP(t)/dt
		Vector<Length> D1( Real t ) const{
			Real b = u/l*t;
			Real a = c * l/u * sin(b);

			return { -k/c			* sin(a) * cos(b),
					 std::fabs(k)/c	* cos(a) * cos(b),
					 sin(b) };
		}

		/// Second derivative pow<2>(d)P(t)/pow<2>(dt)
		Vector<Length> D2( Real t ) const{
			Real b = u/l*t;
			Real a = c * l/u * sin(b);

			return { -k				* (cos(a) * std::pow(cos(b),2) - u/(l*c) * sin(a) * sin(b)),
					 -std::fabs(k)	* (sin(a) * std::pow(cos(b),2) + u/(l*c) * cos(a) * sin(b) ),
					 u/l			* cos(b) };
		}

		/// Third derivative pow<3>(d)P(t)/pow<2>(dt)
		Vector<Length> D3( Real t ) const{
			Real b = u/l*t;
			Real a = c * l/u * sin(b);

			return { k*				u/l * cos(b) * ( (std::pow(cos(b),2) * c*l/u + u/(l*c)) * sin(a) + 3*sin(b) * cos(a)),
					 std::fabs(k)*	u/l * cos(b) * (-(std::pow(cos(b),2) * c*l/u + u/(l*c)) * cos(a) + 3*sin(b) * sin(a)),
							-std::pow(u/l,2) * sin(b) };
		}


		common::Interval<Length> Create( const DataType& data ){
			if( data.gc_Kurve == 0 || data.gc_Laenge == 0 || (data.gc_Kruemmung == 0 && data.gc_Windung == 0) )
				throw std::logic_error( "The residual EEP curve expects to be u != 0, l!= 0 and pow<2>(k)+pow<2>(w) != 0. The other cases resolve to Line, Arc and Helix" );

			k = data.gc_Kruemmung;
			w = data.gc_Windung;
			l = data.gc_Laenge;
			u = data.gc_Kurve;
			c = std::sqrt(k*k+w*w);
			return Range();
		}

		common::Interval<Length> Range() const noexcept{ 
			return {0_m,Length{m_Data.gc_Laenge/100}};
		}

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		DataType m_Data;

		Real k = -1;
		Real w = -1;
		Real l = -1;
		Real u = -1;
		Real c = -1; // = sqrt(k*k+w*w)
	};


	/// With a slight modification, the residual EEPCurve can be parameterized by arc length.
	class EEPAlternativeCurveFunction{
	public:
		using DataType = typename EEPCurve::Data;

		Position<Length> operator()( Length s ) const{
			return P(s);
		}

		bool IsValid() const{
			return u != 0 && l > 0_m && c > 0_1Im;
		}

		AnglePerLength Curvature() const noexcept{
			return D2(0_m).Length();
		}

		/// Curve shape P(s)
		Position<Length> P( Length s ) const{
			Angle b = c * u/(l*k) * s;
			Angle a = c * l/u * sin(b);

			return { k/(c*c)		* cos(a),
					 abs(k)/(c*c)	* sin(a),
					 l*k/(u*c) * (1-cos(b))
			};
		}

		/// First derivative dP(t)/dt
		Vector<One> D1( Length s ) const{
			Angle b = c * u/(l*k) * s;
			Angle a = c * l/u * sin(b);

			return {	   -	sin(a) * cos(b),
					 Sign(k) *  cos(a) * cos(b),
										 sin(b)
			};
		}

		/// Second derivative pow<2>(d)P(t)/pow<2>(dt)
		Vector<Value<Dimension<-1,0,0>>> D2( Length s ) const{
			Angle b = c * u/(l*k) * s;
			Angle a = c * l/u * sin(b);

			return {	c/k				* (-c*std::pow(cos(b),2) * cos(a) + u/l*sin(b) * sin(a)),
					   -c/k * Sign(k)	* ( c*std::pow(cos(b),2) * sin(a) + u/l*sin(b) * cos(a)),
						c/k * u/l		* cos(b)  };

		}

		/// Third derivative pow<3>(d)P(t)/pow<3>(dt)
		Vector<Value<Dimension<-2,0,0>>> D3( Length s ) const{
			Angle b = c * u/(l*k) * s;
			Angle a = c * l/u * sin(b);

			return {	c*c*c/(k*k)*					u/l * cos(b) * ( (std::pow(cos(b),2) * c*l/u + u/(l*c)) * sin(a) + 3*sin(b) * cos(a)),
						c*c*c/(k*k*k)*	abs(k)*	u/l * cos(b) * (-(std::pow(cos(b),2) * c*l/u + u/(l*c)) * cos(a) + 3*sin(b) * sin(a)),
						c*c/(k*k)*				-pow<2>(u/l) * sin(b) };
		}


		common::Interval<Length> Create( const DataType& data ){
			if( data.gc_Kurve == 0 || data.gc_Laenge == 0 || (data.gc_Kruemmung == 0 && data.gc_Windung == 0) )
				throw std::logic_error( "The residual EEP curve expects to be u != 0, l!= 0 and pow<2>(k)+pow<2>(w) != 0. The other cases resolve to Line, Arc and Helix" );

			k = AnglePerLength{data.gc_Kruemmung};
			w = AnglePerLength{data.gc_Windung};
			l = Length{data.gc_Laenge};
			u = data.gc_Kurve;
			c = sqrt(k*k+w*w);
			return Range();
		}

		common::Interval<Length> Range() const noexcept{ 
			return {0_m,Length{m_Data.gc_Laenge/100}};
		}

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		DataType m_Data;

		AnglePerLength k{-1};
		AnglePerLength w{-1};
		Length l{-1};
		Real u = -1;
		AnglePerLength c{-1}; // = sqrt(k*k+w*w)
	};


	class EEPResidualCurve_Imp : public Curve_Imp<Reparametrization<EEPResidualCurveFunction>,EEPCurve>{
	public:
		// Curve:
		std::string	TypeName() const override;

		std::unique_ptr<Curve> Clone() const override;

		// EEPCurve:
		common::Interval<Length> Create( const spat::Frame<Length,One>& start, const spat::Position<Length>& end ) override;
	};


	class EEPAlternativeCurve_Imp : public CurveArcLength_Imp<EEPAlternativeCurveFunction,EEPCurve>{
	public:
		// Curve:
		std::string	TypeName() const override;

		using CurveArcLength_Imp<EEPAlternativeCurveFunction,EEPCurve>::Curvature;

		common::Interval<Length> Range() const override;

		std::unique_ptr<Curve> Clone() const override;

		// EEPCurve:
		common::Interval<Length> Create( const spat::Frame<Length,One>& start, const spat::Position<Length>& end ) override;
	};
	*/

	class EEPCurve_Imp : public EEPCurve{
	public:
		EEPCurve_Imp() noexcept;

		// Curve:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		bool IsValid() const noexcept override;

		AnglePerLength Curvature( Length s ) const noexcept override;

		AnglePerLength Torsion( Length s ) const noexcept override;

		bool IsFlat() const noexcept override;

		void Transition( Length s, Position<Length>& pos ) const override;

		void Transition( Length s, Vector<One>& tan ) const override;

		void Transition( Length s, VectorBundle<Length,One>& bundle ) const override;

		void Transition( Length s, VectorBundle2<Length,One>& bundle ) const override;

		void Transition( Length s, Frame<Length,One>& frame ) const override;

		std::vector<Length> ZeroSet() const  override { return {}; }

		common::Interval<Length> Range() const override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;
		
		bool Mirror( const spat::VectorBundle<Length,One>& ) noexcept override{
			return false;
		}

		bool Equals( const Curve& toCurve, common::Interval<Length> range, 
			Length epsilon_length = epsilon__length, 
			Angle epsilon_angle = epsilon__angle ) const noexcept override;


		// EEPCurve:
	//	common::Interval<Length> Create( const spat::Frame<Length,One>& start, const spat::Position<Length>& end ) override;

		common::Interval<Length> Create( const Data& data ) override;

		const Data& GetData() const noexcept override;
	private:
		Data m_Data;

		//Vorkalkulierte Werte:
		Real m_k2pw2;
		Real m_sqrt_k2pw2;
		Real m_sqrt_ku2pw2;
		Real m_kdiv_k2pw2;
		Real m_wdiv_sqrt_k2pw2;
		Real m_kdiv_sqrt_k2pw2;

		Frame<Real> m_Ursprung;	//Ursprungspunkt fuer eine 
										//aufrecht stehende Spirale, welche
										//die Form der Kurve wiedergiebt.

		bool IstGerade() const noexcept { return m_sqrt_k2pw2 == 0; }
		void KalkuliereUrsprung();
		void Positions_Kalk( Real s, Position<Real>& position ) const noexcept;
		void Tangenten_Kalk( Real s, Vector<Real>& tangent ) const noexcept;
		void Normalen_Kalk( Real s, Vector<Real>& normal ) const noexcept;

		static Data ErstelleGleisLauf( Data gc, const spat::Position<Real>& end );
	};
}
