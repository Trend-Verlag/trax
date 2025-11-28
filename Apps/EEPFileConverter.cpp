// Copyright (c) 2021 TREND Redaktions- und Verlagsgesellschaft mbH


#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/support/AnlReader.h"
#include "trax/rigid/modules/support/AnlWriter.h"
#include "trax/collections/TrackSystem.h"

#include <boost/program_options.hpp> // NOLINT 

#include <filesystem>
#include <iostream>

std::string version{ "EEPFileConverter, Version: 1.12.0"};

int main( int argc, char* argv[] )
{
	try{
		boost::program_options::options_description desc(
"EEPFileConverter is a tool to convert EEP layout files.\n \n \
Usage: EEPFileConverter [options]\n \n \
Allowed options");
		desc.add_options()
			("help,H", "Produces help message.")
			("version,V", "Prints the version string.")
			("input,I", boost::program_options::value<std::filesystem::path>(), "Input *.anl3 or *.anl4 file, obligatory.")
			("output,O", boost::program_options::value<std::filesystem::path>(), "Output *.anl4 file, obligatory.")
			("polygonal_chain,P", "Convert track curves to polygonal chains." )
			("verbosity", boost::program_options::value<std::string>(), "Output verbosity. Options are: 'silent', 'error', 'normal' (the default), 'detailed', 'verbose'.")
			("quiet,Q", "Disables all output except errors.")
			;

        boost::program_options::positional_options_description p;
        p.add("output", -1);

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm );
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
			const auto& verbosity = vm["verbosity"].as<std::string>();
			if( verbosity == "verbose" ){
				trax::SetReportVerbosity( trax::Verbosity::verbose );
			}
			else if( verbosity == "detailed" ){
				trax::SetReportVerbosity( trax::Verbosity::detailed );
			}
			else if( verbosity == "error" ){
				trax::SetReportVerbosity( trax::Verbosity::error );
			}
			else if( verbosity == "quiet" ){
				trax::SetReportVerbosity( trax::Verbosity::silent );
			}
		}

		if( vm.count("quiet") ){
			trax::SetReportVerbosity( trax::Verbosity::silent );
		}

		std::cout << trax::Verbosity::normal << "Input: " << vm["input"].as<std::filesystem::path>() << std::endl;
		std::cout << trax::Verbosity::normal << "Output: " << vm["output"].as<std::filesystem::path>() << std::endl;

		if( std::unique_ptr<trax::ModuleCollection> pModuleCollection = trax::AnlReader{}.ReadModuleCollection( vm["input"].as<std::filesystem::path>() ); 
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
								std::cerr << trax::Verbosity::error  << "Could not convert track to polygonal chain.Track ID: " << track.ID() << std::endl;
							}
						}
					}
				}

				trax::Write( *pModule, vm["output"].as<std::filesystem::path>() );
			}
			else
				std::cerr << trax::Verbosity::error  << "EEPFileConverter: No module found in file: " << vm["input"].as<std::filesystem::path>() << std::endl;

			std::cout << trax::Verbosity::normal << "EEPFileConverter: file " << vm["output"].as<std::filesystem::path>() << " successfully created." << std::endl;
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



