//	trax track library
//	AD 2015
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "End.h"

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


	/// \brief Get the front end of an oriented element.
	/// \param orientation The orientation of the element.
	/// \returns The end type corresponding to the given orientation.
	inline EndType North( Orientation orientation ) noexcept;


	/// \brief Get the back end of an oriented element.
	/// \param orientation The orientation of the element.
	/// \returns The end type corresponding to the given orientation.
	inline EndType South( Orientation orientation ) noexcept;


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

	inline EndType North( Orientation orientation ) noexcept{
		return orientation ? EndType::north : EndType::south;
	}

	inline EndType South( Orientation orientation ) noexcept{
		return orientation ? EndType::south : EndType::north;
	}
}
