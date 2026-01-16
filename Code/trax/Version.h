//	trax track library
//	AD 2018
//
//  "You're forgiven, not forgotten."
//
//						The Corrs
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

#include "Configuration.h"
#include "Units.h"

#include <iostream>

constexpr int TRAX_VERSION_MAJOR = 3;
constexpr int TRAX_VERSION_MINOR = 2;
constexpr int TRAX_VERSION_PATCH = 3;

namespace trax{
	
	/// \brief Class for versioning the trax library.
	class Version{
	public:
		Version() = delete;
		Version& operator=( const Version& ) = delete;

		dclspc static int Major() noexcept; ///< Cardinal version number
		dclspc static int Minor() noexcept; ///< Subversion number
		dclspc static int Patch() noexcept; ///< Revision number

		/// \returns A human readable string containing the version numbers.
		dclspc static const char* Readable();

		/// \returns true if the library has the specified version or grater.
		dclspc static bool IsAtLeast( int major, int minor, int patch ) noexcept;

		/// \returns true if a named feature is available with the library.
		dclspc static bool HasFeature( const std::string& name ) noexcept;

		/// \returns the bytesize of the internally used floating point type.
		dclspc static int SizeOfReal() noexcept;

		/// \brief Gives some descriptionary text about the library.
		///
		/// Use this also to check wether the standard library runtime is
		/// linked via dll. If the destructor of the std::string crashes this 
		/// is not he case.
		/// \returns A description of the library.
		dclspc static const char* LongDescription() noexcept;
	private:
		static bool bNeedsInit;
		static std::string readable;
	};

	/// \brief Compares the version identifiers returned by the library dll with the 
	/// header file's version identifiers, compiled with the client.
	inline bool CheckVersion(){
		if( TRAX_VERSION_MAJOR != Version::Major() ){
			std::cerr << "Trax Library Version mismatch: " << "TRAX_VERSION_MAJOR(" 
				<< TRAX_VERSION_MAJOR << ") != Version::Major()(" << Version::Major() << ") " << std::endl;
			return false;
		}
		if( TRAX_VERSION_MINOR != Version::Minor() ){
			std::cerr << "Trax Library Version mismatch: " << "TRAX_VERSION_MINOR(" 
				<< TRAX_VERSION_MINOR << ") != Version::Minor()(" << Version::Minor() << ") " << std::endl;
			return false;
		}
		if( TRAX_VERSION_PATCH != Version::Patch() ){
			std::cerr << "Trax Library Version mismatch: " << "TRAX_VERSION_PATCH(" 
				<< TRAX_VERSION_PATCH << ") != Version::Patch()(" << Version::Patch() << ") " << std::endl;
			return false;
		}
		if( sizeof(Real) != Version::SizeOfReal() ){
			std::cerr << "Trax Library floating point  type mismatch: " << "sizeof(Real)" 
				<< sizeof(Real) << ") != Version::SizeOfReal()(" << Version::SizeOfReal() << ") " << std::endl;

			return false;
		}

		return true;
	}

	inline bool CheckRuntime(){
		std::string longDescription = Version::LongDescription();
		return true;
		// if this crashes, a wrong vcruntime is selected. Try static library or make the calling
		// process use dll version, too.
	}
};

