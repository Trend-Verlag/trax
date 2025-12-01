//	trax track library
//	AD 2013
//
//  "Say My Name"
//
//			Florence + The Machine
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
#include "common/NarrowCast.h"

#include <istream>
#include <ostream>
#include <string>

namespace trax{

	/// \brief Type used for IDs in the trax library.
	///
	/// This is a separate type for two reasons:
	/// 1. Overloading of function parameters can distinguish between having an IDType parameter or another (build in) type.
	/// 2. The real type of the number can be changed in one place only (change the type of Type)
	/// In general we need ids for one reason only: we can not allocate objects at an address we want to. So if we
	/// want to restore any program state - e.g. from file - we have to have platform independend references...
	struct IDType{

		/// \brief Type to store index.
		typedef unsigned int Type;


		/// \name Construction
		/// \param uiidval
		/// \param iidval
		/// \param uliidval
		/// \param liidval
		///@{
		constexpr IDType() noexcept
			: m_id{0}
		{}

		constexpr IDType( unsigned int uiidval ) noexcept
			: m_id{uiidval}
		{}

		IDType( int iidval )
			: m_id{common::narrow_cast<Type>(iidval)}
		{}

		IDType( unsigned long int uliidval )
			: m_id{common::narrow_cast<Type>(uliidval)}
		{}

		IDType( long int liidval )
			: m_id{common::narrow_cast<Type>(liidval)}
		{}
		///@}

		/// \brief converts to true if the IDType is a valid id.
		operator bool() const noexcept{
			return m_id > 0;
		}

		/// \brief converts to int.
		operator int() const{
			return common::narrow_cast<int>(m_id);
		}

		Type GetID() const noexcept{ return m_id; }


		/// \name Mathematical Operators for IDType
		///@{
		friend bool operator==( const IDType& idA, const IDType& idB ) noexcept{
			return idA.m_id == idB.m_id;
		}

		template<typename ComType>
		friend bool operator==( const IDType& idA, const ComType& idB ) noexcept{
			return idA.m_id == static_cast<Type>(idB);
		}

		template<typename ComType>
		friend bool operator==( const ComType& idA, const IDType& idB ) noexcept{
			return static_cast<Type>(idA) == idB.m_id;
		}

		friend bool operator!=( const IDType& idA, const IDType& idB ) noexcept{
			return idA.m_id != idB.m_id;
		}

		template<typename ComType>
		friend bool operator!=( const IDType& idA, const ComType& idB ) noexcept{
			return idA.m_id != static_cast<Type>(idB);
		}

		template<typename ComType>
		friend bool operator!=( const ComType& idA, const IDType& idB ) noexcept{
			return static_cast<Type>(idA) != idB.m_id;
		}

		friend bool operator<( const IDType& idA, const IDType& idB ) noexcept{
			return idA.m_id < idB.m_id;
		}

		friend bool operator<=( const IDType& idA, const IDType& idB ) noexcept{
			return idA.m_id <= idB.m_id;
		}

		friend bool operator>( const IDType& idA, const IDType& idB ) noexcept{
			return idA.m_id > idB.m_id;
		}

		friend bool operator>=( const IDType& idA, const IDType& idB ) noexcept{
			return idA.m_id >= idB.m_id;
		}

		friend IDType operator+( const IDType& idA, const IDType& idB ) noexcept{
			return idA.m_id + idB.m_id;
		}

		friend IDType operator+( const IDType& idA, const int& idB ) noexcept{
			return idA.m_id + idB;
		}

		friend IDType operator+( const int& idA, const IDType& idB ) noexcept{
			return idA + idB.m_id;
		}

		friend IDType& operator++( IDType& id ) noexcept{
			++(id.m_id);
			return id;
		}

		friend IDType& operator--( IDType& id ) noexcept{
			--(id.m_id);
			return id;
		}

		friend IDType operator++( IDType& id, int ) noexcept{
			IDType retval{ id };
			id.m_id++;
			return retval;
		}

		friend IDType operator--( IDType& id, int ) noexcept{
			IDType retval{ id };
			id.m_id--;
			return retval;
		}
		///@}


		/// \name Streaming of IDType
		///@{
		friend std::basic_ostream<char>& operator << ( std::basic_ostream<char>& os, const IDType& id ){
			os << id.m_id;
			return os;
		}

		friend std::basic_istream<char>& operator >> ( std::basic_istream<char>& is, IDType& id ){
			is >> id.m_id;
			return is;
		}
		///@}

		friend std::string to_string( const IDType& id ){
			return std::to_string(id.m_id);
		}
	private:
		Type m_id;
	};

	constexpr IDType anyID;

}