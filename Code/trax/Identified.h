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

#include <string>
#include <vector>

namespace trax{

	struct IDType;

	/// \brief Interface for identifying objects by ID or 
	/// reference values.
	///
	/// The template argument prevents that this becomes a common base.
	template<class I>
	struct Identified{

		/// \brief Gets a reference that was set for this object by name.
		/// \param name Name of the reference.
		/// \returns a reference string for additional data to the object.
		virtual const std::string& Reference( const std::string& name ) const = 0;


		/// \brief Sets the reference for the object.
		/// \param name Name of the reference.
		/// \param reference Reference string to be set.
		virtual void Reference( const std::string& name, const std::string& reference ) = 0;


		/// \brief Searches for all reference names that countain namePart as substring in the name.
		virtual const std::vector<char const*>& ReferenceNames( const std::string& namePart ) const = 0;

		/// \brief Searches for all reference names.
		virtual const std::vector<char const*>& ReferenceNames() const = 0;


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
