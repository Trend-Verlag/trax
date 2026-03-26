//	trax track library
//	AD 2013 
//
// "maloca bocada fubanga"
//
//				Sepultura
//
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
// For further information, please contact: horstmann.marc@trendverlag.de



#pragma once

#include "NarrowCast.h"

#include <set>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <cmath>
#include <cassert>

/// \brief Namespace of common utility classes and methods.
namespace common{

	///
	///	\brief This is usefull because it saves a temp.
	///	\param val Value to be squared.
	///	\returns the square of val.
	/// \deprecated use std::pow(val,2) or pow<2>(val) instead
	template<typename T> inline constexpr T Square( T val ) noexcept{
		return val * val;
	}

	/// \returns val risen to the third power.
	/// \deprecated use std::pow(val,3) or pow<3>(val) instead
	template<typename T> inline constexpr T Cube( T val ) noexcept{
		return val * val * val;
	}


	// Helper trait to compute the result type of T^Y
	template<int Y, typename T, bool Positive = (Y > 0)>
	struct pow_result;

	template<typename T>
	struct pow_result<0,T,false> {
		using type = decltype(T{}/T{}); // dimensionless for Y=0
	};

	template<typename T>
	struct pow_result<1,T,true> {
		using type = T;
	};

	template<int Y, typename T>
	struct pow_result<Y,T,true> {
		using type = decltype(std::declval<T>() * std::declval<typename pow_result<Y-1,T>::type>());
	};

	// For negative exponents Y < 0: T^Y = 1 / T^(-Y)
	template<int Y, typename T>
	struct pow_result<Y, T, false> {
		using type = decltype(std::declval<typename pow_result<0,T>::type>() / 
							  std::declval<typename pow_result<-Y,T>::type>());
	};

	/// \brief power function with templated integer exponent.
	template<int Y, typename T>
	inline constexpr auto pow(T val) noexcept -> typename pow_result<Y,T>::type {
		return static_cast<typename pow_result<Y, T>::type>(std::pow(val, Y));
	}

	///	\brief Tests equality in the sense |a-b| < epsilon.
	template<typename T> inline constexpr bool Equals( T a, T b, T epsilon ) noexcept{	
		using std::abs; // prevent using int version when inadequate.
		return abs( a - b ) <= epsilon;
	}

	/// \returns +1 if val > 0, -1 if val < 0, 0 if val == 0.
	template <typename T> inline constexpr auto Sign(T val) noexcept -> decltype(T{}/T{}){
		return static_cast<decltype(T{}/T{})>((T{0} < val) - (val < T{0}));
	}

	/// \returns n!
	inline constexpr int Factorial( int n ) {
		assert( n >= 0 );
		return (n == 1 || n == 0) ? 1 : Factorial(n - 1) * n;
	}

	///	\brief Clips a val to a specified range.
	///	\param val ReferenceValue to clip.
	///	\param min lower clip range.
	///	\param max upper clip range.
	///	\returns the clipped value.
	template<typename T1,typename T2> inline 
	T1 Clip( T1& val, const T2& min, const T2& max ) noexcept{
		if( min > max )
			return Clip( val, max, min );

		if( val < min )
			val = static_cast<T1>(min);
		else if( val > max )
			val = static_cast<T1>(max);

		return val;
	}


	///	\brief Clips a val to a specified range.
	///
	/// Use std::clamp instead.
	template<class T>
	constexpr const T& Clamp( const T& v, const T& lo, const T& hi ) noexcept{
		if( v < lo ) return lo;
		if( hi < v ) return hi;
		return v;
	}

	///	\brief Wraps a val to a specified range as if 
	/// max would be actually min.
	///	\param val ReferenceValue to wrap.
	///	\param min lower wrap range.
	///	\param max upper wrap range.
	///	\returns the wrapped value.
	template<typename T1,typename T2> inline 
	T1 Wrap( const T1& val, const T2& min, const T2& max ) noexcept{
		if( max == min )
			return val;

		if( min > max )
			return Wrap( val, max, min );
		
		T1 retVal = val;
		if( val >= max ) 
			retVal = static_cast<T1>(std::fmod(min + (val - max), max - min));
		else if( val < min )
			retVal = static_cast<T1>(std::fmod(max - (min - val), max - min ));
		
		return retVal;
	}

	/// \brief Rounding of floating point number to a certain digit aftzer the point
	///
	/// \param value Input value to round
	/// \param toDigit Ordinal number of the digit to round to. 0 would round to integral number.
	/// \returns The rounded number.
	template<class T> inline
	T Round( T value, int toDigit ) noexcept
	{
		const float pow10{ std::pow( 10.f, static_cast<float>(toDigit) ) };
		return round( value * pow10 ) / pow10;
	}


	/// \brief Make too small numbers zero. 
	///
	/// Deals with subnormal numbers.
	template<typename Valtype> inline
		Valtype DealDenormalizedNumbers( Valtype val ){
		if( abs( val ) < (std::numeric_limits<Valtype>::min)() )
			return Valtype{ 0 };
		return val;
	}

	/// \brief Center an equidistant row of elements (pearls).
	/// \param cntPearls Number of elements.
	/// \param distPearls distance between elements.
	/// \returns The position of the 'leftmost' element.
	template<typename Valtype>
	inline constexpr Valtype FirstPearl( int cntPearls, Valtype distPearls ) noexcept{
		return ( 1 - cntPearls ) * 0.5f * distPearls;
	}

	inline char to_uppercase( unsigned char c)
	{
		return narrow_cast<char>(std::toupper(c));
	}

	inline char to_lowercase( unsigned char c)
	{
		return narrow_cast<char>(std::tolower(c));
	}

	/// \brief Construct a quoted string
	inline std::string quoted( const std::string& str ){
		std::string quotedStr = "\"";
		quotedStr += str;
		quotedStr += "\"";
		return quotedStr;
	}


	/// \brief Removes the outmost apostrophes (''' and '"') from a string.
	/// Does nothing if the string isn't properly enclosed with apostrophes.
	inline std::string StripApostrophes( const std::string& str ){
		std::string _str(str);
		if(_str.size() >= 2 ){
			if( (*_str.cbegin() == '\"' && *_str.crbegin() == '\"') ||
				(*_str.cbegin() == '\'' && *_str.crbegin() == '\'') ){
				_str.erase( _str.begin() );
				_str.erase( _str.end()-1 );
			}
		}

		return _str;
	}

	/// \brief Removes characters that might not be allowed for file names.
	inline std::string StripReservedCharacters( const std::string& str ){
		std::string retval;
		for( std::size_t i = 0; i < str.size(); ++i ){
			const char& c = str.at(i);
			if( c != '/' &&
				c != '\\' &&
				c != '?' &&
				c != '%' &&
				c != '*' &&
				c != ':' &&
				c != '|' &&
				c != '"' &&
				c != '\'' &&
				c != '<' &&
				c != '>' &&
				c != '.' )
				retval += c;
		}
		return retval;
	}

	/// \brief Needed since for unknown reason, the straightforward implementation
	/// std::string{ indent, ' ' } does not work 
	inline std::string Indent( int indent ){
		//return std::string{ static_cast<std::size_t>(indent), ' ' };
		std::string retval;
		for( int i = indent; i != 0; --i )
			retval += " ";

		return retval;
	}

	//inline int Determinant_sign(const boost::numeric::ublas::permutation_matrix<std::size_t>& pm)
	//{
	//	int pm_sign=1;
	//	std::size_t size = pm.size();
	//	for (std::size_t i = 0; i < size; ++i)
	//		if (i != pm(i))
	//			pm_sign *= -1; // swap_rows would swap a pair of rows here, so we change sign
	//	return pm_sign;
	//}

	//template<typename Valtype> inline
	//Valtype Determinant( boost::numeric::ublas::matrix<Valtype>& m ) {
	//	using namespace boost::numeric::ublas;

	//	permutation_matrix<std::size_t> pm(m.size1());
	//	Valtype det = 1;
	//	if( lu_factorize(m,pm) ) {
	//		det = 0;
	//	} else {
	//		for(std::size_t i = 0; i < m.size1(); i++)
	//			det *= m(i,i); // multiply by elements on diagonal
	//		det = det * Determinant_sign( pm );
	//	}
	//	return det;
	//}


	/// \brief Attenuates value by the factor dattenuation. 
	///
	/// Handles sign and zeroes the result for dattenuation > 1
	template<typename Valtype> inline
	Valtype AttenuateValue( Valtype value, Valtype dattenuation ) noexcept
	{
		const Valtype dvalue = -dattenuation * value;
		if( std::abs(value) < std::abs(dvalue) )
			return 0.0f;
		
		return value + dvalue;
	}


	///	\brief Set of unique keys. 
	template<typename T> class UniqueKey{
		std::set<T> m_OccupiedKeys;
	public:

		/// \brief Reserve a key.
		bool Occupy( T key ){
			return m_OccupiedKeys.insert( key ).second;
		}


		/// \brief Make a key available in the future.
		bool Free( T key ){
			return m_OccupiedKeys.erase( key ) ? true : false;
		}


		/// \brief Release all the keys, making them available.
		void FreeAll() noexcept{
			m_OccupiedKeys.clear();
		}


		/// \brief Get a free key. The key will be occupied after the call.
		T GetFree(){
			T x = 1;
			for( auto csiter = m_OccupiedKeys.begin();
				csiter != m_OccupiedKeys.end(); ++x, ++csiter )
			{
				if( x == *csiter )
					continue;

				m_OccupiedKeys.insert( x );
				return x;
			}

			m_OccupiedKeys.insert( x );
			return x;
		}


		/// \brief Is the key already occupied?
		bool IsFree( T key ) const{
			return (m_OccupiedKeys.find( key ) == m_OccupiedKeys.end());
		}
	};

	/// \brief Calculates the impulse change of body 1 in a collision event.
	///
	/// \param m1 Mass of body 1
	/// \param m2 Mass of body 2
	/// \param v1 Velocity of body 1
	/// \param v2 Velocity of body 2
	/// \param k collision module. 0: inelastic ciollision; 1: elastic collision.
	/// \returns Chnage of impulse of body1 (body2 would be the negative of this value)
	template<typename Valtype>
	constexpr Valtype Collision( Valtype m1, Valtype m2, Valtype v1, Valtype v2, Valtype k ){
		return m1*m2/ (m1+m2) * (v2-v1) * (1+k);
	}

	inline void TranslateEscapeCharacters( std::string& string ){
		for( std::size_t pos = string.find( "[e]" ); pos != string.npos; pos = string.find( "[e]", pos ) )
			string.replace( pos, 3, "\n" );
	}

	/// \brief convert multibyte character string (UTF-8) to ANSI string.
//	inline std::string utf8_to_string( const std::string& utf8str, const std::locale& loc = std::locale("") )
//	{
//#if defined(_MSC_VER)
//		// Windows: use Win32 API to avoid deprecated std::codecvt_utf8
//		// Step 1: UTF-8 -> wide string
//		if( utf8str.empty() )
//			return std::string();
//
//		int wlen = ::MultiByteToWideChar( CP_UTF8, 0,
//			utf8str.data(), narrow_cast<int>(utf8str.size()),
//			nullptr, 0 );
//		if( wlen <= 0 )
//			return std::string();
//
//		std::wstring wstr( static_cast<std::size_t>(wlen), L'\0' );
//		::MultiByteToWideChar( CP_UTF8, 0,
//			utf8str.data(), narrow_cast<int>(utf8str.size()),
//			wstr.data(), wlen );
//
//		// Step 2: wide string -> ANSI (system default code page)
//		int alen = ::WideCharToMultiByte( CP_ACP, 0,
//			wstr.data(), narrow_cast<int>(wstr.size()),
//			nullptr, 0, nullptr, nullptr );
//		if( alen <= 0 )
//			return std::string();
//
//		std::string result( static_cast<std::size_t>(alen), '\0' );
//		::WideCharToMultiByte( CP_ACP, 0,
//			wstr.data(), narrow_cast<int>(wstr.size()),
//			result.data(), alen, nullptr, nullptr );
//
//		return result;
//#else
//		// POSIX: use locale-aware C functions
//		// Step 1: UTF-8 -> wide string using mbsrtowcs with a UTF-8 locale
//		std::locale prevLoc = std::locale::global( std::locale("en_US.UTF-8") );
//
//		std::mbstate_t state{};
//		const char* src = utf8str.c_str();
//		std::size_t wlen = std::mbsrtowcs( nullptr, &src, 0, &state );
//		if( wlen == static_cast<std::size_t>(-1) ){
//			std::locale::global( prevLoc );
//			return std::string();
//		}
//
//		std::wstring wstr( wlen, L'\0' );
//		src = utf8str.c_str();
//		state = std::mbstate_t{};
//		std::mbsrtowcs( wstr.data(), &src, wstr.size() + 1, &state );
//
//		// Restore original locale
//		std::locale::global( prevLoc );
//
//		// Step 2: wide string -> narrow string using the provided locale
//		std::vector<char> buf( wstr.size() );
//		std::use_facet<std::ctype<wchar_t>>( loc ).narrow(
//			wstr.data(), wstr.data() + wstr.size(), '?', buf.data() );
//
//		return std::string( buf.data(), buf.size() );
//#endif
//	}


	/// \brief Flips a flag for the lifetime of the class object.
	class FlagBlocker{
		bool& m_Flag;
	public:
		/// \name Construction
		/// \param blocker Flag to be flipped.
		///@{
		FlagBlocker( bool& blocker ) noexcept
			: m_Flag{blocker}
		{
			m_Flag = !m_Flag;
		}
		FlagBlocker() = delete;
		FlagBlocker( const FlagBlocker& ) = delete;
		FlagBlocker( FlagBlocker&& ) = delete;
		///@}

		~FlagBlocker() noexcept{
			m_Flag = !m_Flag;
		}

		FlagBlocker& operator=( const FlagBlocker& ) = delete;
		FlagBlocker& operator=( FlagBlocker&& ) = delete;
	};


	/// \brief Sets a variable for the lifetime of the class object.
	template<typename ValueType>
	class Resetter{
		ValueType& m_Variable;
		ValueType m_OriginalValue;
		bool m_bDismiss = false;
	public:
		Resetter() = delete;
		Resetter( const Resetter& ) = delete;
		Resetter( Resetter&& ) = delete;
		Resetter( ValueType& variable, ValueType toValue ) noexcept
		:	m_Variable		{ variable },
			m_OriginalValue	{ variable }
		{
			m_Variable = toValue;
		}

		~Resetter(){
			if( !m_bDismiss )
				m_Variable = m_OriginalValue;
		}

		Resetter& operator=( const Resetter& ) = delete;
		Resetter& operator=( Resetter&& ) = delete;


		void Dismiss(){
			m_bDismiss = true;
		}
	};


	/// \brief stl container adapter extension for accessing the underlying container.
	template <class ContainerAdapter>
	class ContainerAdapterExtension : public ContainerAdapter {
	public:
		typedef typename ContainerAdapter::container_type container_type;

		const container_type& get_container() const noexcept { return this->c; }
	};
}