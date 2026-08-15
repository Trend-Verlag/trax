// Copyright (c) 2021 TREND Redaktions- und Verlagsgesellschaft mbH


#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/support/AnlReader.h"
#include "trax/rigid/modules/support/AnlWriter.h"
#include "trax/collections/TrackSystem.h"

#include "appframe/CommandLine.h"
#include <filesystem>
#include <iostream>

std::string version{ "EEPFileConverter, Version: 1.13.0" };

namespace
{
	void PrintUsage()
	{
		std::cout << version << std::endl;
		std::cout <<
"EEPFileConverter is a tool to convert EEP layout files.\n\n"
"Usage: EEPFileConverter [options] <output>\n\n"
"Allowed options:\n"
"  -H, --help              Produces help message.\n"
"  -V, --version           Prints the version string.\n"
"  -I, --input <file>      Input *.anl3 or *.anl4 file, obligatory.\n"
"  -O, --output <file>     Output *.anl4 file, obligatory.\n"
"  -P, --polygonal_chain   Convert track curves to polygonal chains.\n"
"      --verbosity <level> Output verbosity. Options are: 'silent', 'error',\n"
"                          'normal' (the default), 'detailed', 'verbose'.\n"
"  -Q, --quiet             Disables all output except errors.\n"
			<< std::endl;
	}
}

int wmain( int argc, wchar_t* argv[] )
{
	try{
		appframe::WCommandLine cmd( argc, argv );

		if( cmd.Has( L"version" ) || cmd.Has( L"V" ) ){
			std::cout << version << std::endl;
			return 0;
		}

		if( cmd.Has( L"help" ) || cmd.Has( L"H" ) ){
			PrintUsage();
			return 0;
		}

		// Accept both the long and short option spellings.
		const bool hasInput =
			cmd.HasValue( L"input" ) || cmd.HasValue( L"I" );

		// The output may be given as an option or as the first positional
		// argument (matching the previous Boost positional behaviour).
		const bool hasOutputOption =
			cmd.HasValue( L"output" ) || cmd.HasValue( L"O" );
		const bool hasOutput =
			hasOutputOption || cmd.PositionalCount() > 0;

		if( !hasInput ){
			std::cerr << "EEPFileConverter: Input file is missing! Use --help for usage manual." << std::endl;
			return 0;
		}

		if( !hasOutput ){
			std::cerr << "EEPFileConverter: Output file is missing! Use --help for usage manual." << std::endl;
			return 0;
		}

		if( cmd.HasValue( L"verbosity" ) ){
			const std::wstring verbosity = cmd.Get( L"verbosity" );
			if( verbosity == L"verbose" ){
				trax::SetReportVerbosity( trax::Verbosity::verbose );
			}
			else if( verbosity == L"detailed" ){
				trax::SetReportVerbosity( trax::Verbosity::detailed );
			}
			else if( verbosity == L"error" ){
				trax::SetReportVerbosity( trax::Verbosity::error );
			}
			else if( verbosity == L"silent" || verbosity == L"quiet" ){
				trax::SetReportVerbosity( trax::Verbosity::silent );
			}
		}

		if( cmd.Has( L"quiet" ) || cmd.Has( L"Q" ) ){
			trax::SetReportVerbosity( trax::Verbosity::silent );
		}

		// On Windows std::wstring is the native path encoding, so construct the
		// path directly - no quoting or code-page conversion involved.
		const std::wstring inputValue =
			cmd.HasValue( L"input" ) ? cmd.Get( L"input" ) : cmd.Get( L"I" );

		const std::wstring outputValue =
			hasOutputOption
				? ( cmd.HasValue( L"output" ) ? cmd.Get( L"output" ) : cmd.Get( L"O" ) )
				: cmd.Positional( 0 );

		const std::filesystem::path inputPath { inputValue };
		const std::filesystem::path outputPath{ outputValue };

		std::wcout << trax::Verbosity::normal << L"Input: "  << inputPath  << std::endl;
		std::wcout << trax::Verbosity::normal << L"Output: " << outputPath << std::endl;

		if( std::unique_ptr<trax::ModuleCollection> pModuleCollection = trax::AnlReaderBase{}.ReadModuleCollection( inputPath );
			pModuleCollection )
		{
			if( auto pModule = pModuleCollection->GetFirst() )
			{
				if( cmd.Has( L"polygonal_chain" ) || cmd.Has( L"P" ) ){
					if( auto pTrackSystem = pModule->GetTrackSystem() ){
						for( auto& track : *pTrackSystem ){
							try{
								auto pNewTrack = trax::MakeParallelTrackWithPolygonalChain( track, track.Range() );
								track.SetFrame( pNewTrack->GetFrame() );
								track.Attach( pNewTrack->DetachCurve() );
								track.Attach( pNewTrack->DetachTwist() );
							}
							catch( std::exception& e ){
								std::cerr << trax::Verbosity::error  << "EEPFileConverter: Error: " << e.what() << std::endl;
								std::cerr << trax::Verbosity::error  << "Could not convert track to polygonal chain. Track ID: " << track.ID() << std::endl;
							}
						}
					}
				}

				trax::Write( *pModule, outputPath );
			}
			else
				std::wcerr << trax::Verbosity::error  << L"EEPFileConverter: No module found in file: " << inputPath << std::endl;

			std::wcout << trax::Verbosity::normal << L"EEPFileConverter: file " << outputPath << L" successfully created." << std::endl;
		}
	}
	catch( std::exception& e ) {
		std::cerr << trax::Verbosity::error << "EEPFileConverter: error: " << e.what() << std::endl;
		return 1;
	}
	catch( ... ) {
		std::cerr << trax::Verbosity::error << "EEPFileConverter: Exception of unknown type!" << std::endl;
		return 1;
	}

	return 0;
}



