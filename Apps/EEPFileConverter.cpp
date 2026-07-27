// Copyright (c) 2021 TREND Redaktions- und Verlagsgesellschaft mbH


#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/support/AnlReader.h"
#include "trax/rigid/modules/support/AnlWriter.h"
#include "trax/collections/TrackSystem.h"

#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>

std::string version{ "EEPFileConverter, Version: 1.12.0" };

int wmain( int argc, wchar_t* argv[] )
{
	try{
		boost::program_options::options_description desc(
"EEPFileConverter is a tool to convert EEP layout files.\n \n \
Usage: EEPFileConverter [options]\n \n \
Allowed options");
		desc.add_options()
			("help,H", "Produces help message.")
			("version,V", "Prints the version string.")
			("input,I",  boost::program_options::wvalue<std::wstring>(), "Input *.anl3 or *.anl4 file, obligatory.")
			("output,O", boost::program_options::wvalue<std::wstring>(), "Output *.anl4 file, obligatory.")
			("polygonal_chain,P", "Convert track curves to polygonal chains." )
			("verbosity", boost::program_options::wvalue<std::wstring>(), "Output verbosity. Options are: 'silent', 'error', 'normal' (the default), 'detailed', 'verbose'.")
			("quiet,Q", "Disables all output except errors.")
			;

		boost::program_options::positional_options_description p;
		p.add("output", -1);

		boost::program_options::variables_map vm;
		boost::program_options::store(
			boost::program_options::wcommand_line_parser(argc, argv).options(desc).positional(p).run(), vm );
		boost::program_options::notify(vm);

		if( vm.count("version") ){
			std::cout << version << std::endl;
			return 0;
		}

		if( vm.count("help") ){
			std::cout << version << std::endl;
			std::cout << desc << std::endl;
			return 0;
		}

		if( vm.count("input") == 0 ){
			std::cerr << "EEPFileConverter: Input file is missing! Use --help for usage manual." << std::endl;
			return 0;
		}

		if( vm.count("output") == 0 ){
			std::cerr << "EEPFileConverter: Output file is missing! Use --help for usage manual." << std::endl;
			return 0;
		}

		if( vm.count("verbosity") ){
			const auto& verbosity = vm["verbosity"].as<std::wstring>();
			if( verbosity == L"verbose" ){
				trax::SetReportVerbosity( trax::Verbosity::verbose );
			}
			else if( verbosity == L"detailed" ){
				trax::SetReportVerbosity( trax::Verbosity::detailed );
			}
			else if( verbosity == L"error" ){
				trax::SetReportVerbosity( trax::Verbosity::error );
			}
			else if( verbosity == L"quiet" ){
				trax::SetReportVerbosity( trax::Verbosity::silent );
			}
		}

		if( vm.count("quiet") ){
			trax::SetReportVerbosity( trax::Verbosity::silent );
		}

		// On Windows std::wstring is the native path encoding, so construct the
		// path directly - no quoting or code-page conversion involved.
		const std::filesystem::path inputPath { vm["input"].as<std::wstring>() };
		const std::filesystem::path outputPath{ vm["output"].as<std::wstring>() };
				
		std::wcout << trax::Verbosity::normal << L"Input: "  << inputPath  << std::endl;
		std::wcout << trax::Verbosity::normal << L"Output: " << outputPath << std::endl;

		if( std::unique_ptr<trax::ModuleCollection> pModuleCollection = trax::AnlReaderBase{}.ReadModuleCollection( inputPath );
			pModuleCollection )
		{
			if( auto pModule = pModuleCollection->GetFirst() )
			{
				if( vm.count( "polygonal_chain") ){
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



