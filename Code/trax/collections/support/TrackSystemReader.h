//	trax track library
//	AD 2025 
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/SocketRegistry.h"

#include <filesystem>

namespace trax{

	struct TrackSystem;

	class TrackSystemReader{
	public:
		dclspc TrackSystemReader( const char* pLocale = nullptr );
		dclspc TrackSystemReader( SocketRegistry& socketRegistry, const char* pLocale = nullptr );


		/// \name Track System Reading
		/// \brief Reads a track system from a file or buffer.
		/// \param fromPath The path to the file to read from.
		/// \param atIdx The index of the track system to read.
		/// \return A shared pointer to the track system.
		//@{
		std::shared_ptr<TrackSystem> dclspc Read( const std::filesystem::path& fromPath, int atIdx = 1 ) const;
				
		std::shared_ptr<TrackSystem> dclspc Read( const unsigned char* bufferStart, const unsigned char* bufferEnd, int atIdx = 1 ) const;
		//@}
	private:
		std::unique_ptr<SocketRegistry> m_pSocketRegistry;
		const char* m_pLocale = nullptr;
	protected:
		SocketRegistry& m_SocketRegistry;
	};


}