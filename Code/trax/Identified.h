//	trax track library
//	AD 2016 
//
//  "Tough and Competent"
//
//				Gene Kranz
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

#include "common/Helpers.h"
#include "trax/Configuration.h"

#include <string>
#include <vector>

namespace trax{

	struct IDType;

	/// \brief Interface for identifying objects by ID or 
	/// reference values.
	///
	/// The template argument prevents that this becomes a common base.
	/// The returned pointers to value strings are guaranteed to be 
	/// globally unique for equal strings. This allows string comparisons 
	/// to be done by pointer comparison.
	template<class I>
	struct Identified : virtual DllHeap{

		/// \brief Gets a reference that was set for this object by name.
		/// 
		/// \param name Name of the reference.
		/// \returns a reference string for additional data to the object.
		/// Empty string is returned if no reference was set for the name.
		/// This will never return nullptr.
		virtual const char* Reference( const char* name ) const = 0;

		const char* Reference( const std::string& name ) const
		{
			return Reference( name.c_str() );
		}


		/// \brief Sets the reference for the object.
		/// \param name Name of the reference.
		/// \param reference Reference string to be set.
		virtual void Reference( const char* name, const char* reference ) = 0;

		void Reference( const char* name, const std::string& reference )
		{
			return Reference( name, reference.c_str() );
		}

		void Reference( const std::string& name, const std::string& reference )
		{
			return Reference( name.c_str(), reference.c_str() );
		}


		/// \brief Returns a pointer to the empty string that is used to return empty references. 
		/// 
		/// This allows to compare empty references by pointer comparison.
		/// You can also simply use *Reference( [key] ) for an IsEmpty test.
		virtual const char* EmptyReference() const noexcept = 0;


		bool IsEmptyReference( const char* name ) const
		{
			return Reference( name ) == EmptyReference();
		}

		bool IsEmptyReference( const std::string& name ) const
		{
			return Reference( name ) == EmptyReference();
		}

		bool IsReference( const char* name, const char* reference ) const
		{
			if( name == nullptr || *name == '\0' )
				return false;
			if( reference == nullptr || *reference == '\0' )
				return false;

			return strcmp( Reference( name ), reference ) == 0;
		}

		bool IsReference( const char* name, const std::string& reference ) const
		{
			return IsReference( name, reference.c_str() );
		}

		bool IsReference( const std::string& name, const std::string& reference ) const
		{
			return IsReference( name.c_str(), reference.c_str() );
		}


		/// \brief Searches for all reference names that contain namePart as substring in the name.
		virtual common::Span<const char*> ReferenceNames( const char* namePart ) const = 0;

		common::Span<const char*> ReferenceNames( const std::string& namePart ) const
		{
			return ReferenceNames( namePart.c_str() );
		}


		/// \brief Searches for all reference names.
		virtual common::Span<const char*> ReferenceNames() const = 0;


		/// \returns the object's id.
		virtual IDType ID() const noexcept = 0;


		/// \brief Sets the object's id. 
		///
		/// Collections will typically set the id of an object added to them
		/// if id is not set. If the id of an object has a certain value, the 
		/// collection will try to use that value but will fail if it is not unique.
		virtual void ID( IDType id ) noexcept = 0;


		virtual ~Identified() = default;
		Identified( const Identified& ) = delete; // if identified would copy, they would no longer be identified.
		Identified& operator=( const Identified& ) = delete;
	protected:
		Identified() = default;
		Identified( Identified&& ) = default;
		Identified& operator=( Identified&& ) = default;
	};

}
