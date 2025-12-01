//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
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


#include <filesystem>

namespace trax{

	class RollingStockParser;
	struct TrainScene;

	///\brief Base class for file readers.
	class FileReader{
	public:
		FileReader( std::filesystem::path basePath = std::filesystem::path{} ) noexcept;

		inline bool operator()( std::filesystem::path filePath ){
			return Read( filePath );
		}

		///\brief Reads an object from a file.
		///\returns true if the file could be read successfuly; false if the 
		/// file could not be found.
		/// \throws std::runtime_error on any kind of malformatted file content.
		virtual bool Read( std::filesystem::path filePath ) = 0;

	protected:
		inline std::filesystem::path GetBasePath() const noexcept{
			return m_BasePath;
		}

		bool MakeValidPath( std::filesystem::path& path ) const noexcept;
	private:
		std::filesystem::path m_BasePath;
	};


	class RollingStockFileReader_Base : public FileReader{
	public:
		RollingStockFileReader_Base( RollingStockParser& parser, std::filesystem::path basePath = std::filesystem::path{} ) noexcept;

	protected:
		RollingStockParser& m_Parser;

	};

	class RollingStockFileReader : public RollingStockFileReader_Base{
	public:
		RollingStockFileReader( RollingStockParser& parser, std::filesystem::path basePath = std::filesystem::path{} ) noexcept;

		bool Read( std::filesystem::path filePath ) override;
	};

} // namespace trax

