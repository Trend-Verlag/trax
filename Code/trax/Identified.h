// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2016 
//
//  "Tough and Competent"
//
//				Gene Kranz

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
		virtual const std::vector<char const *>& ReferenceNames( const std::string& namePart ) const = 0;


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
