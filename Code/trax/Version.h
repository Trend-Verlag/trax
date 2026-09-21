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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "Configuration.h"
#include "Units.h"

#include <iostream>

constexpr int TRAX_VERSION_MAJOR = 3;
constexpr int TRAX_VERSION_MINOR = 13;
constexpr int TRAX_VERSION_PATCH = 0;

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
		dclspc static bool HasFeature( const char* name ) noexcept;

		/// \returns the bytesize of the internally used floating point type.
		dclspc static int SizeOfReal() noexcept;

		/// \brief Gives some descriptionary text about the library.
		/// \returns A description of the library.
		dclspc static const char* LongDescription() noexcept;

		/// Returns a long integer that describes the ABI of the library. This 
		/// is used to check if the library was compiled with the same toolset 
		/// and settings as the client code.
		dclspc static long AbiTag() noexcept;
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

	inline constexpr long TraxAbiTag() noexcept {
		long tag = 0;
		#if defined(_MSC_VER)
			tag = tag * 100000 + _MSC_VER;
		#endif
		#if defined(_ITERATOR_DEBUG_LEVEL)
			tag = tag * 10 + _ITERATOR_DEBUG_LEVEL;
		#endif
		#if defined(_DLL)                 // /MD (1) vs /MT (0)
			tag = tag * 10 + 1;
		#else
			tag = tag * 10 + 0;
		#endif
			return tag;
	}
	constexpr long TRAX_ABI_TAG = TraxAbiTag();


	inline bool CheckRuntime(){
		if( TRAX_ABI_TAG != Version::AbiTag() ){   // AbiTag() = TraxAbiTag() compiled into the DLL
			std::cerr << Verbosity::error << "Trax runtime/ABI mismatch: client(" << TRAX_ABI_TAG
			          << ") != dll(" << Version::AbiTag() << "). "
			          << "Match toolset, Debug/Release (_ITERATOR_DEBUG_LEVEL) and /MD vs /MT." << std::endl;
			return false;
		}
		return true;
	}
};

