//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2016 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "EEPCurve_Imp.h"

#include "trax/Units.h"
#include "HelixP_Imp.h"
#include "spat/support/SpatSupportStream.h"

namespace trax{

std::unique_ptr<EEPCurve> EEPCurve::Make( CurveType type ) noexcept
{
	try{
		switch( type ){
		case CurveType::EEPCurve: 
			return std::make_unique<EEPCurve_Imp>();
		case CurveType::EEPResidual: 
			//return std::unique_ptr<EEPCurve>{ new EEPResidualCurve_Imp };
		case CurveType::EEPAlternative: 
			//return std::unique_ptr<EEPCurve>{ new EEPAlternativeCurve_Imp };
		default:
			return nullptr;
		}
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
	/*
///////////////////////////////////////
std::string EEPResidualCurve_Imp::TypeName() const{
	return "EEPResidualCurve";
}

std::unique_ptr<Curve> EEPResidualCurve_Imp::Clone() const{
	return std::unique_ptr<Curve>(new EEPResidualCurve_Imp{*this});
}

common::Interval<Length> EEPResidualCurve_Imp::Create(const spat::Frame<Length,One>& start,const spat::Position<Length>& end)
{
	return {0_m,0_m};
}
///////////////////////////////////////
std::string EEPAlternativeCurve_Imp::TypeName() const{
	return "EEPAlternativeCurve";
}

common::Interval<Length> EEPAlternativeCurve_Imp::Range() const{
	return {0_m,Length{f.GetData().gc_Laenge}};
}

std::unique_ptr<Curve> EEPAlternativeCurve_Imp::Clone() const{
	return std::unique_ptr<Curve>{ new EEPAlternativeCurve_Imp{*this} };
}

common::Interval<Length> EEPAlternativeCurve_Imp::Create(const spat::Frame<Length,One>& start,const spat::Position<Length>& end)
{
	return {0_m,0_m};
}
*/
///////////////////////////////////////
EEPCurve_Imp::EEPCurve_Imp() noexcept
	:	m_k2pw2				{0},
		m_sqrt_k2pw2		{0},
		m_sqrt_ku2pw2		{0},
		m_kdiv_k2pw2		{-1},
		m_wdiv_sqrt_k2pw2	{-1},
		m_kdiv_sqrt_k2pw2	{-1}
{
	m_Ursprung.Init();
}

const char*	EEPCurve_Imp::TypeName() const noexcept{
	return "EEPCurve";
}

Curve::CurveType EEPCurve_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::EEPCurve;
}

bool EEPCurve_Imp::IsValid() const noexcept{
	return true;
}

AnglePerLength EEPCurve_Imp::Curvature( Length ) const noexcept{
	//TODO: curvature and torsion not correct in general. Integrating EEPCurve for PhysX?
	return AnglePerLength{ 100.0f / units_per_meter * (std::abs(m_Data.gc_Kruemmung) + std::abs(m_Data.gc_Kurve/m_Data.gc_Laenge)) };
}

AnglePerLength EEPCurve_Imp::Torsion( Length ) const noexcept{
	//TODO: curvature and torsion not correct in general. Integrating EEPCurve for PhysX?
	return AnglePerLength{ 100.0f / units_per_meter * m_Data.gc_Windung };
}

bool EEPCurve_Imp::IsFlat() const noexcept{
	return false;
}

void EEPCurve_Imp::Transition( Length s, Position<Length>& pos_ ) const{
	assert( s >= 0_m );
	if( s < 0_m )
		throw std::logic_error( "STE_INKONSISTENTE_PARAMETER" );

	if( !Range().Touches( s ) )
		throw std::logic_error( "STE_GLEIS_ZUENDE" );

	s *= 100.0f * meters_per_unit;
	Position<Real> pos;

	if( s == 0_m ) // BCA
	{
		pos = m_Data.m_AnfangsBein.P;
		pos /= 100.0f * meters_per_unit;
		pos_ = pos.ptr();
		return;
	}

	if( IstGerade() && m_Data.gc_Kurve == 0.0f )	//!!!!
	{
		pos.Init();
		pos.x = s.Units();
	}
	else
	{
		Positions_Kalk( s.Units(), pos );
	}

	//Mit der folgenden Transformation wird die Lage der
	//Spirale im Raum einbezogen:
	m_Ursprung.ToParent( pos );

	pos /= 100.0f * meters_per_unit;
	pos_ = pos.ptr();
}

void EEPCurve_Imp::Transition( Length s, Vector<One>& tan_ ) const{
	assert( s >= 0_m );
	if( s < 0_m )
		throw std::logic_error( "STE_INKONSISTENTE_PARAMETER" );

	if( !Range().Touches( s ) )
		throw std::logic_error( "STE_GLEIS_ZUENDE" );

	s *= 100.0f * meters_per_unit;
	Vector<Real> tan{ tan_.ptr() };

	if( IstGerade() && m_Data.gc_Kurve == 0.0f )	//!!!!
	{
		tan.Init( Vector<Real>::specials::ex );
	}
	else
		//Eine Kurve mit konstanter Kruemmung und konstanter
		//Windung (Torsion) ist eine Spirale. Wir berechnen 
		//zunaechst das Dreibein, wie es auf einer aufrecht
		//auf der x/y - Ebene stehenden Spirale liegen wuerde:
	{
		Tangenten_Kalk( s.Units(), tan );
	}

	//Mit der folgenden Transformation wird die Lage der
	//Spirale im Raum einbezogen:
	m_Ursprung.ToParent( tan );
	tan_ = tan.ptr();
}

void EEPCurve_Imp::Transition( Length s, VectorBundle<Length,One>& bundle_ ) const{
	assert( s >= 0_m );
	if( s < 0_m )
		throw std::logic_error( "STE_INKONSISTENTE_PARAMETER" );

	if( !Range().Touches( s ) )
		throw std::logic_error( "STE_GLEIS_ZUENDE" );

	s *= 100.0f * meters_per_unit;
	VectorBundle<Real> bundle;

	if( IstGerade() && m_Data.gc_Kurve == 0.0f )	//!!!!
	{
		bundle.Init();
		bundle.P.x = s.Units();
	}
	else
		//Eine Kurve mit konstanter Kruemmung und konstanter
		//Windung (Torsion) ist eine Spirale. Wir berechnen 
		//zunaechst das Dreibein, wie es auf einer aufrecht
		//auf der x/y - Ebene stehenden Spirale liegen wuerde:
	{
		Positions_Kalk( s.Units(), bundle.P );
		Tangenten_Kalk( s.Units(), bundle.T );
	}

	//Mit der folgenden Transformation wird die Lage der
	//Spirale im Raum einbezogen:
	m_Ursprung.ToParent( bundle );
	
	bundle.P /= 100.0f * meters_per_unit;
	bundle_ = bundle.ptr();
}

void EEPCurve_Imp::Transition( Length s, VectorBundle2<Length,One>& bundle ) const{
	Frame<Length,One> frame;
	Transition( s, frame );
	bundle = frame;
}

void EEPCurve_Imp::Transition( Length s, Frame<Length,One>& frame_ ) const{
	s *= 100.0f * meters_per_unit;
	Frame<Real> frame;

	if( s == 0_m && m_Data.gc_Kurve == 0.0f ) // BCA
	{
		frame = m_Data.m_AnfangsBein;
		frame.P /= 100.0f * meters_per_unit;
		frame_ = frame.ptr();
		return;
	}

	if( IstGerade() && m_Data.gc_Kurve == 0.0f )//!!!!!
	{
		frame.Init();
		frame.P.x = s.Units();
	}
	else
		//Eine Kurve mit konstanter Kruemmung und konstanter
		//Windung (Torsion) ist eine Spirale. Wir berechnen 
		//zunaechst das Dreibein, wie es auf einer aufrecht
		//auf der x/y - Ebene stehenden Spirale liegen wuerde:
	{
		Positions_Kalk( s.Units(), frame.P );
		Tangenten_Kalk( s.Units(), frame.T );
		Normalen_Kalk( s.Units(), frame.N );

		frame.B = frame.T % frame.N;
	}

	//Wir drehen das Dreibein um den sich aus der
	//Verdrillung und dem Parameter ergebenden Wert.
	const Vector<Real> Rotator = frame.T *
		(m_Data.m_FuehrungsVerdrehung + m_Data.gc_Verdrillung * s.Units());

	Rotate( &(frame.N), 2, Rotator );

	//Mit der folgenden Transformation wird die Lage der
	//Spirale im Raum einbezogen:
	m_Ursprung.ToParent( frame );

	frame.P /= 100.0f * meters_per_unit;
	frame_ = frame.ptr();
}

common::Interval<Length> EEPCurve_Imp::Range() const{
	return {0_m,_cm(m_Data.gc_Laenge)};
}

spat::Vector<One> EEPCurve_Imp::LocalUp() const noexcept{
	return spatial_cast<One>(m_Ursprung.B);
}

spat::Frame<Length, One> EEPCurve_Imp::GetCurveLocalTransformation() const noexcept{
	spat::Frame<Length,One> retval;
	Transition( 0_m, retval );
	return retval;
}

std::unique_ptr<Curve> EEPCurve_Imp::Clone() const{
	return std::make_unique<EEPCurve_Imp>(*this);
}

bool EEPCurve_Imp::Equals(
	const Curve& toCurve,
	common::Interval<Length> range,
	Length epsilon_length,
	Angle epsilon_angle ) const noexcept
{
	if( this == &toCurve )
		return true;

	if( auto pToCurve = dynamic_cast<const EEPCurve*>(&toCurve) )
		return trax::Equals( GetData(), pToCurve->GetData(), range, epsilon_length, epsilon_angle );

	return false;
}
/*
common::Interval<Length> EEPCurve_Imp::Create( 
	const spat::Frame<Length,One>& start,
	const spat::Position<Length>& end_ )
{
	if( IsValid() ){
		spat::Frame<Length,One> existingStart;
		spat::Position<Length> existingEnd;
		Transition(0_m,existingStart);
		Transition(Length{m_Data.gc_Laenge/100},existingEnd);

		if( start.Equals( existingStart, epsilon__length, trax::epsilon__angle ) && 
			end_.Equals( existingEnd, epsilon__length ) )
			return Range();
	}

	if( start.P.Equals( end_, 100 * trax::epsilon__length ) )
		throw std::invalid_argument{ "Can not create curve with zero length!" };

	Data data = GetData();
	data.m_AnfangsBein.P = { _cm(start.P.x), _cm(start.P.y), _cm(start.P.z) };
	data.m_AnfangsBein.T = spatial_cast<Real>(start.T);
	data.m_AnfangsBein.N = spatial_cast<Real>(start.N);
	data.m_AnfangsBein.B = spatial_cast<Real>(start.B);
	const spat::Position<Real> end{ _cm(end_.x), _cm(end_.y), _cm(end_.z) };
	Create( ErstelleGleisLauf( data, end ) );
	return Range();
}
 */
EEPCurve::Data EEPCurve_Imp::ErstelleGleisLauf( Data gc, const spat::Position<Real>& end )
{
		//1.rot D i N wok. z
		Real fAngle_s1;
		{
			spat::Vector<Real> T{ gc.m_AnfangsBein.T };
			Real l = std::sqrt(T.dx*T.dx + T.dy*T.dy);
			Real alpha;
			if(std::fabs(l) < 0.00000000001f)
				alpha = 0.0f;
			else
				alpha = asin(T.dy/l);
			if(T.dx < -0.000001f) alpha = pi-alpha;

			spat::Vector<Real> vN{gc.m_AnfangsBein.N};

			vN.Rotate( spat::Ez<Real>, alpha );

			l = std::sqrt(l*l + T.dz*T.dz);
			alpha = asin(T.dz/l);

			vN.Rotate( spat::Ey<Real>, -alpha );

			T = vN;
			l = T.Length();
			alpha = asin(T.dz/l);
			if(T.dy < -0.000001f) 
				alpha = pi-alpha;

			fAngle_s1 = alpha;
		}


	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

		//Wir berechnen zunaechst ein Kreisbogenstueck
		//in der Ebene die durch *pUp definiert wird.
		//Hierzu projezieren wir das m_Anfangsbein auf
		//diese Ebene:

 		//m_AnfangsBein.Dir = m_AnfangsBein.Dir - (m_AnfangsBein.Dir * *pUp) * *pUp;
		//m_AnfangsBein.Nor = m_AnfangsBein.Nor - (m_AnfangsBein.Nor * *pUp) * *pUp;

		const Real dz = gc.m_AnfangsBein.N.dz;

		gc.m_AnfangsBein.T.dz = 0.0f;
		gc.m_AnfangsBein.N.dz = 0.0f;

		if(std::fabs(gc.m_AnfangsBein.N.dx) + std::fabs(gc.m_AnfangsBein.N.dy) < 0.00001f)
		{
			gc.m_AnfangsBein.N.dx = -gc.m_AnfangsBein.B.dx*dz;
			gc.m_AnfangsBein.N.dy = -gc.m_AnfangsBein.B.dy*dz;
		}

		gc.m_AnfangsBein.OrthoNormalize();


		const spat::Vector<Real> D = end - gc.m_AnfangsBein.P;

		//D.dz = sin(gc.gc_Kurve)*(gc.gc_Laenge/gc.gc_Kurve);

		//sutl::CRichtungf d = D - (D * *pUp) * *pUp;			//Projektion auf die Ebene
		spat::Vector<Real> d = D;			//Projektion auf die Ebene
		d.dz = 0.0f;

		//Alpha ist der Winkel zwischen Verbindungslinie d
		//von Anfangs und Endpunkt in der Ebene und dem Vektor,
		//der zum Kurvenkreismittelpunkt zeigt:


		Real len = d.Length();
		if(gc.gc_Kurve != 0.0f)
		{
			const Real rKurv = gc.gc_Laenge / gc.gc_Kurve;
			const Real fMlt = gc.gc_Laenge/(sin(gc.gc_Kurve) * rKurv);
			len *= fMlt;
		}

		Real cosAlpha = (d.Length() == 0.0f) ? 0.0f : (gc.m_AnfangsBein.N * d) / len;


		if( cosAlpha >  1.0f ) cosAlpha =  1.0f;
		if( cosAlpha < -1.0f ) cosAlpha = -1.0f;

		//Sigma ist die Steigung der Kurve:	// Sigma jest nachyleniem krzywej:
		//Hoehengewinn pro laengs der Kurve	// Przyrost wysokoci wzd krzywej na
		//zurueckgelegter Strecke.			// przebyt odleg

		Real Sigma = 0.0f;

		if (common::Equals(cosAlpha, Real{0}, Real{1e-5f})) //cosAlpha == 0.0f )
			//Wir haben es mit einer Geraden zu tun.
		{
			gc.gc_Kruemmung		= 0.0f;
			gc.gc_Windung		= 0.0f;
			gc.gc_Laenge		= D.Length();//!!!!!!!!1

			if(gc.gc_Kurve != 0.0f)
			{
				const Real rKurv = gc.gc_Laenge / gc.gc_Kurve;
				const Real fMlt = gc.gc_Laenge/(sin(gc.gc_Kurve) * rKurv);
				gc.gc_Laenge *= fMlt;
			}

			if( common::Equals( gc.gc_Laenge, Real{0}, Real{1e-6f} ) )
				throw std::invalid_argument{ "Can not create curve with zero length!" };

			//Sigma = (D * *pUp) / gc.gc_Laenge;
			Sigma = D.dz / gc.gc_Laenge;
		}
		else{
			Real Alpha    = acos( cosAlpha );

			if( (gc.m_AnfangsBein.T * d) < 0.0f ){
				if( cosAlpha > 0 )
					Alpha *= -1.0f;
				else
					Alpha -= pi;
			}

			const Real r = d.Length() / (2 * cosAlpha );    //Radius des ebenen Kreisbogens

			const Real sb = r * (pi - 2*Alpha);			   //Bogenlaenge des ebenen Kreisbogens

			//float z			= D * *pUp;				//Steighoehe der Kurve 
			const Real z			= D.dz;				//Steighoehe der Kurve 
			gc.gc_Laenge	= std::sqrt( z*z + sb*sb );  //Bogenlaenge der Raumkurve
			if(gc.gc_Laenge > 10000.0f)	gc.gc_Laenge = 10000.0f;

			//!!!!!!!!!!!!! trzaba uwzglednic kurve
			if(gc.gc_Kurve != 0.0f)
			{
				const Real rKurv = gc.gc_Laenge / gc.gc_Kurve;
				const Real fMlt = gc.gc_Laenge/(sin(gc.gc_Kurve) * rKurv);
				gc.gc_Laenge *= fMlt;
			}


			if( common::Equals( gc.gc_Laenge, Real{0}, Real{1e-6f} ) )
				throw std::invalid_argument{ "Can not create curve with zero length!" };

			Sigma = z / gc.gc_Laenge;
			if(Sigma > 1.0f) Sigma = 1.0f;
			else
				if(Sigma < -1.0f) Sigma = -1.0f;

			if( std::fabs(gc.m_AnfangsBein.N.dz) > 0.0001) Sigma = 0.0;

			gc.gc_Kruemmung   = (1 - Sigma*Sigma) / r;
			if(gc.m_AnfangsBein.B.dz < 0.0f) gc.gc_Kruemmung *= -1.0f;
			gc.gc_Windung     = Sigma * std::sqrt( 1 - Sigma*Sigma ) / std::fabs(r);
			if (std::fabs(gc.gc_Windung) < 0.0000001f) gc.gc_Windung = 0.0f;
			if (std::fabs(gc.gc_Kruemmung) < 0.0000001f) gc.gc_Kruemmung = 0.0f;
		}
	
		{
			if( Sigma >  1.0f ) Sigma =  1.0f;
			if( Sigma < -1.0f ) Sigma = -1.0f;

				//Anstellwinkel fuer das Anfangsdreibein ueber
				//der durch *pUp definierten Ebene:

			if( std::fabs(gc.m_AnfangsBein.N.dz) > 0.0001) Sigma = 0.0;

			const Real Tau = - asin( Sigma );

			spat::Vector<Real> Rotator = Tau * gc.m_AnfangsBein.N;
			gc.m_AnfangsBein.T.Rotate( Rotator );
			gc.m_AnfangsBein.B.Rotate( Rotator );

			//skret: Bin, Nor
			Rotator = fAngle_s1 * gc.m_AnfangsBein.T;
			gc.m_AnfangsBein.B.Rotate( Rotator );
			gc.m_AnfangsBein.N.Rotate( Rotator );

			if( gc.m_AnfangsBein.B.dz < 0 )
			{
				gc.m_AnfangsBein.N *= -1.0f;
				gc.m_AnfangsBein.B *= -1.0f;
			}

			Round(gc.m_AnfangsBein.P.x,6);
			Round(gc.m_AnfangsBein.P.y,6);
			Round(gc.m_AnfangsBein.P.z,6);
			Round(gc.m_AnfangsBein.T.dx,6);
			Round(gc.m_AnfangsBein.T.dy,6);
			Round(gc.m_AnfangsBein.T.dz,6);
			Round(gc.m_AnfangsBein.N.dx,6);
			Round(gc.m_AnfangsBein.N.dy,6);
			Round(gc.m_AnfangsBein.N.dz,6);
			Round(gc.m_AnfangsBein.B.dx,6);
			Round(gc.m_AnfangsBein.B.dy,6);
			Round(gc.m_AnfangsBein.B.dz,6);
		}

	return gc;
}

common::Interval<Length> EEPCurve_Imp::Create( const Data& data )
{
	if( data.gc_Laenge <= 0 )
		throw std::invalid_argument( "EEPCurve::Data::gc_Laenge can not be zero" );

	m_Data = data;

	m_k2pw2 =	m_Data.gc_Kruemmung * m_Data.gc_Kruemmung +
				m_Data.gc_Windung   * m_Data.gc_Windung;


	const Real ku2pw2 = m_Data.gc_Windung   * m_Data.gc_Windung
					+ m_Data.gc_Kurve/m_Data.gc_Laenge * m_Data.gc_Kurve/m_Data.gc_Laenge;


	m_sqrt_k2pw2 = std::sqrt( m_k2pw2 );
	m_sqrt_ku2pw2 = std::sqrt( ku2pw2 );


	if( !IstGerade() ){
		m_kdiv_k2pw2		= m_Data.gc_Kruemmung / m_k2pw2;
		m_wdiv_sqrt_k2pw2	= m_Data.gc_Windung   / m_sqrt_k2pw2;
		m_kdiv_sqrt_k2pw2	= m_Data.gc_Kruemmung / m_sqrt_k2pw2;
	}
	else{
		m_kdiv_k2pw2		= -1;
		m_wdiv_sqrt_k2pw2	= -1;
		m_kdiv_sqrt_k2pw2	= -1;
	}

	KalkuliereUrsprung();
	return Range();
}

const EEPCurve::Data& EEPCurve_Imp::GetData() const noexcept{
	return m_Data;
}

void EEPCurve_Imp::KalkuliereUrsprung()
{
#ifdef _DEBUG
	if( !m_Data.m_AnfangsBein.IsOrthoNormal( epsilon__angle, epsilon__angle ) ){
		OrthogonalityDump( std::cout, m_Data.m_AnfangsBein );
		assert( !"Frame misses to be orthogonal!" );
	}
#endif

	//Fuehrungsverdrehung rueckgaengig machen:
	const Vector<Real> Rotator = -m_Data.m_FuehrungsVerdrehung * m_Data.m_AnfangsBein.T;
	Frame<Real> DBnull  = m_Data.m_AnfangsBein;
	Rotate( &(DBnull.N), 2, Rotator );

	if( !IstGerade() )//|| m_Charakteristik.gc_Kurve != 0.0f)
					  //Das Ursprungsdreibein wird hier in Einheiten des 
					  //Anfangsdreibeins formuliert:
	{
		const Real costau = std::fabs( m_kdiv_sqrt_k2pw2 );
		const Real sintau = m_wdiv_sqrt_k2pw2;

		m_Ursprung.P =  DBnull.P + m_kdiv_k2pw2 * DBnull.N;
		m_Ursprung.T = -DBnull.N;

		m_Ursprung.N =  DBnull.T * costau - DBnull.B * sintau;
		m_Ursprung.B =  DBnull.T * sintau + DBnull.B * costau;
	}
	else
	{
		m_Ursprung	= DBnull;
	}
}

void EEPCurve_Imp::Positions_Kalk( Real s, Position<Real>& pos ) const noexcept{
	Real dx, dy, dz;

	if( m_kdiv_k2pw2 == -1 )	//prosty na boki (kiedys tu nie wchodzil ale teraz tak wiec trzeba sprawdzic)
	{
		dx = s;
		dy = 0.0f;
		dz = 0.0f;
	}
	else
	{
		//----
		//oryg
		dx = m_kdiv_k2pw2 * cos( m_sqrt_k2pw2 * s );
		dy = std::fabs( m_kdiv_k2pw2 ) * sin( m_sqrt_k2pw2 * s );
		dz = m_wdiv_sqrt_k2pw2 * s;
		//----
	}

	//----

	if( m_Data.gc_Kurve == 0.0f )	//prosty w gore
	{
		pos.x = dx;	//oryg
		pos.y = dy;	//forward
		pos.z = dz;	//up
	}
	else if( m_Data.gc_Laenge == 0 ){
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
	}
	else	//wygiety w gore
	{

		const Real fStep = s/m_Data.gc_Laenge;	//0..1
		const Real aKurv = fStep * m_Data.gc_Kurve;
		const Real rKurv = m_Data.gc_Laenge / m_Data.gc_Kurve;	//promien krzywizny w gore

		if( m_kdiv_k2pw2 == -1 )	//prosty na boki
		{
			pos.x = sin( aKurv ) * rKurv;	//wzdluz toru
			pos.y = 0.0;
			pos.z = rKurv - cos( aKurv )*rKurv;
		}
		else	//wygiety na boki
		{
			const Real fy = sin( aKurv ) * rKurv;

			dx = m_kdiv_k2pw2 * cos( m_sqrt_k2pw2 * fy );
			dy = std::fabs( m_kdiv_k2pw2 ) * sin( m_sqrt_k2pw2 * fy );
			dz = rKurv - cos( aKurv )*rKurv;

			pos.x = dx;	//oryg
			pos.y = dy;	//forward
			pos.z = dz;	//up
		}
	}
}

void EEPCurve_Imp::Tangenten_Kalk( Real s, Vector<Real>& tan ) const noexcept{
	Real dx, dy, dz;

	if( m_kdiv_k2pw2 == -1 )	//prosty na boki
	{
		dx = 1.0f;
		dy = 0.0f;
		dz = 0.0f;
	}
	else
	{
		dx = -m_kdiv_sqrt_k2pw2 * sin( m_sqrt_k2pw2 * s );
		dy =   std::fabs( m_kdiv_sqrt_k2pw2 ) * cos( m_sqrt_k2pw2 * s );
		dz =   m_wdiv_sqrt_k2pw2;
	}

	if( m_Data.gc_Kurve == 0.0f )
	{
		tan.dx = dx;
		tan.dy = dy;
		tan.dz = dz;
		tan.Normalize();
	}
	else if( m_Data.gc_Laenge == 0 ){
		tan.dx = 1;
		tan.dy = 0;
		tan.dz = 0;
	}
	else
	{
		const Real fStep = s/m_Data.gc_Laenge;	//0..1		
		const Real aKurv = fStep * m_Data.gc_Kurve;

		if( m_kdiv_k2pw2 == -1 )	//prosty na boki
		{
			tan.dx = cos( aKurv );
			tan.dy = 0.0;
			tan.dz = sin( aKurv );
		}
		else
		{
			const Real rKurv = m_Data.gc_Laenge / m_Data.gc_Kurve;	//promien krzywizny w gore

			const Real fy = sin( aKurv ) * rKurv;
			const Real lebVer = sin( aKurv );
			const Real lenHor = cos( aKurv );

			Real fDirDx = -m_kdiv_sqrt_k2pw2 * sin( m_sqrt_k2pw2 * fy );
			Real fDirDy =   std::fabs( m_kdiv_sqrt_k2pw2 ) * cos( m_sqrt_k2pw2 * fy );
			const Real d = std::sqrt( fDirDx*fDirDx + fDirDy*fDirDy );
			fDirDx = fDirDx/d * lenHor;
			fDirDy = fDirDy/d * lenHor;


			tan.dx = fDirDx;
			tan.dy = fDirDy;
			tan.dz = lebVer;

			tan.Normalize();
		}
	}
}

void EEPCurve_Imp::Normalen_Kalk( Real s, Vector<Real>& normal ) const noexcept{
	Real dx = 0;
	Real dy = 0;
	Real dz = 0;

	if( m_kdiv_k2pw2 == -1 )	//prosty na boki
	{
		normal.dx = 0.0f;
		normal.dy = 1.0f;
		normal.dz = 0.0f;
	}
	else
	{
		dx = -std::fabs( m_Data.gc_Kruemmung ) * cos( m_sqrt_k2pw2 * s );
		dy = -m_Data.gc_Kruemmung * sin( m_sqrt_k2pw2 * s );
		dz = 0;
	}


	if( m_Data.gc_Kurve == 0.0f )	//prosty w gore
	{
		normal.dx = dx;	//oryg
		normal.dy = dy;	//forward
		normal.dz = dz;	//up
		normal.Normalize();
	}
	else if( m_Data.gc_Laenge == 0 ){
		normal.dx = 0;
		normal.dy = 1;
		normal.dz = 0;
	}
	else	//wygiety w gore
	{
		const Real fStep = s/m_Data.gc_Laenge;	//0..1
		const Real aKurv = fStep * m_Data.gc_Kurve;
		const Real rKurv = m_Data.gc_Laenge / m_Data.gc_Kurve;	//promien krzywizny w gore

		if( m_kdiv_k2pw2 == -1 )	//prosty na boki
		{
			normal.dx = 0.0f;
			normal.dy = 1.0f;
			normal.dz = 0.0f;
		}
		else
		{
			const Real fy = sin( aKurv ) * rKurv;

			normal.dx = -std::fabs( m_Data.gc_Kruemmung ) * cos( m_sqrt_k2pw2 * fy );
			normal.dy = -m_Data.gc_Kruemmung * sin( m_sqrt_k2pw2 * fy );
			normal.dz = 0.0;
			normal.Normalize();
		}
	}
}
///////////////////////////////////////
bool Equals( 
	const EEPCurve::Data& /*A*/, 
	const EEPCurve::Data& /*B*/, 
	common::Interval<Length> /*range*/, 
	Length /*epsilon_length*/, 
	Angle /*epsilon_angle*/ ) noexcept
{
	assert( !"Not implemented yet!" );
	return false;
}
///////////////////////////////////////
}
