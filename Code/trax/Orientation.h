// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2015
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "Configuration.h"

#include <ostream>
#include <string>

namespace trax{

	/// \brief Provides two values for orientation.
	struct Orientation{

		/// \brief Values for orientation
		enum class Value : char{
			none = -1,	///< Denoting the unknown orientation
			anti = 0,	///< Denoting the non parallel or opposite orientation 
			para = 1	///< Denoting the aligned, parallel orientation
		};


		/// \name Construction
		/// \param value Value to initialize
		/// \param bvalue boolean to initialize true == para
		/// \param ivalue integer to initialize 0 == anti, para everything else.
		//{@
		Orientation() noexcept
			: m_Value{ Value::para }
		{}

		Orientation( Value value ) noexcept
			: m_Value{ value }
		{}

		explicit Orientation( bool bvalue ) noexcept
			: m_Value{ bvalue ? Value::para : Value::anti } 
		{}

		explicit Orientation( int ivalue ) noexcept
			: m_Value{ ivalue == 0 ? Value::anti : Value::para } 
		{}
		//@}

		void Flip() noexcept{
			m_Value = m_Value == Value::para ? Value::anti : Value::para;
		}

		/// \brief Test for equality of value.
		bool operator==( Value value ) const noexcept{
			return m_Value == value;
		}

		/// \brief Test for inequality of value.
		bool operator!=( Value value ) const noexcept{
			return m_Value != value;
		}

		/// \returns true if anti and false if para.
		///
		/// Together with the Value constructor this lets turn
		/// an orientation O by O = !O.
		Value operator!() const noexcept{
			return m_Value == Value::para ? Value::anti : Value::para;
		}

		operator bool() const noexcept{
			return m_Value == Value::para;
		}

		operator int() const noexcept{
			return m_Value == Value::para ? +1 : -1;
		}

	private:
		Value m_Value;

		friend bool operator==( const Orientation& a, const Orientation& b ) noexcept;
		friend bool operator!=( const Orientation& a, const Orientation& b ) noexcept;
		friend bool operator==( const Orientation::Value& a, const Orientation& b ) noexcept;
		friend bool operator!=( const Orientation::Value& a, const Orientation& b ) noexcept;
	};


	/// \returns true if the two orientations are equal, otherwise false.
	inline bool operator==( const Orientation& a, const Orientation& b ) noexcept;


	/// \returns true if the two orientations are inequal, otherwise false.
	inline bool operator!=( const Orientation& a, const Orientation& b ) noexcept;


	/// \returns true if the two orientations are equal, otherwise false.
	inline bool operator==( const Orientation::Value& a, const Orientation& b ) noexcept;


	/// \returns true if the two orientations are inequal, otherwise false.
	inline bool operator!=( const Orientation::Value& a, const Orientation& b ) noexcept;


	/// \returns The absolute orientation of b if it is understood relative to a.
	inline Orientation operator+( const Orientation& a, const Orientation& b ) noexcept;


	/// \brief Assigns to a the absolute orientation of b if it is understood relative to a.
	/// \returns A reference to a.
	inline Orientation& operator+=( Orientation& a, const Orientation& b ) noexcept;


	dclspc std::string ToString( Orientation orient );

	dclspc Orientation ToOrientation( const std::string& orient );


//inlines:
	inline bool operator==( const Orientation& a, const Orientation& b ) noexcept{
		return a.m_Value == b.m_Value;
	}

	inline bool operator!=( const Orientation& a, const Orientation& b ) noexcept{
		return a.m_Value != b.m_Value;
	}

	inline bool operator==( const Orientation::Value& a, const Orientation& b ) noexcept{
		return a == b.m_Value;
	}

	inline bool operator!=( const Orientation::Value& a, const Orientation& b ) noexcept{
		return a != b.m_Value;
	}

	Orientation operator+( const Orientation& a, const Orientation& b ) noexcept{
		return a == b ? Orientation::Value::para : Orientation::Value::anti;
	}

	Orientation& operator+=( Orientation& a, const Orientation& b ) noexcept{
		a = a + b;
		return a;
	}
}
