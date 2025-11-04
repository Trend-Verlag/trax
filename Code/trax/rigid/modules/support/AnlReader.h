// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024
//
// "His clothes are dirty but his hands are clean
//  And you're the best thing, he's ever seen"
//
//								Ministry

#pragma once

#include "trax/SocketRegistry.h"

#include <filesystem>

namespace trax{

	struct Module;
	struct ModuleCollection;
	struct TrackSystem;

	class AnlReader{
	public:
		dclspc AnlReader( const char* pLocale = nullptr );
		dclspc AnlReader( SocketRegistry& socketRegistry, const char* pLocale = nullptr );


		std::unique_ptr<ModuleCollection> dclspc ReadModuleCollection( const std::filesystem::path& path ) const;
				
		std::unique_ptr<ModuleCollection> dclspc ReadModuleCollection( const unsigned char* bufferStart, const unsigned char* bufferEnd ) const;


		/// \name Module Reading
		/// \brief Reads a module from a file or buffer.
		/// \param fromPath The path to the file to read from.
		/// \param atIdx The index of the module to read.
		/// \return A unique pointer to the module.
		//@{
		std::unique_ptr<Module> dclspc ReadModule( const std::filesystem::path& fromPath, int atIdx = 1 ) const;
				
		std::unique_ptr<Module> dclspc ReadModule( const unsigned char* bufferStart, const unsigned char* bufferEnd, int atIdx = 1  ) const;
		//@}

		/// \name Track System Reading
		/// \brief Reads a track system from a file or buffer.
		/// \param fromPath The path to the file to read from.
		/// \param atIdx The index of the track system to read.
		/// \return A shared pointer to the track system.
		//@{
		std::shared_ptr<TrackSystem> dclspc ReadTrackSystem( const std::filesystem::path& fromPath, int atIdx = 1 ) const;
				
		std::shared_ptr<TrackSystem> dclspc ReadTrackSystem( const unsigned char* bufferStart, const unsigned char* bufferEnd, int atIdx = 1 ) const;
		//@}
	private:
		std::unique_ptr<SocketRegistry> m_pSocketRegistry;
		const char* m_pLocale = nullptr;
	protected:
		SocketRegistry& m_SocketRegistry;
	};

} // namespace trax
