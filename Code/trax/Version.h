// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2018
//
//  "You're forgiven, not forgotten."
//
//						The Corrs

#pragma once

#include "Configuration.h"
#include "Units.h"

#include <iostream>

constexpr int TRAX_VERSION_MAJOR = 2;
constexpr int TRAX_VERSION_MINOR = 2;
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

