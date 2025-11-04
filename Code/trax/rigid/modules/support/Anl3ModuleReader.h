// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/collections/support/Anl3TrackSystemReader.h"

namespace trax{

	struct Camera;
	struct CameraCollection;
	struct Module;
	struct ModuleCollection;

	namespace ptreesupport{

		class Anl3ModuleReader : public Anl3TrackSystemReader
		{
		public:
			dclspc Anl3ModuleReader( const char* pLocale = nullptr );
			dclspc Anl3ModuleReader( SocketRegistry& socketRegistry, const char* pLocale = nullptr );

			std::unique_ptr<ModuleCollection> dclspc ReadModuleCollection( const boost::property_tree::ptree& pt ) const;
			
			std::unique_ptr<Module> dclspc ReadModule( const boost::property_tree::ptree& pt ) const;
		private:
			struct Kollektor{
				IDType m_ID;
				spat::Frame<Length,One> m_Frame;
				std::vector<IDType> m_Tracks;
			};
			
			std::unique_ptr<CameraCollection> ReadCameraCollection( const boost::property_tree::ptree& pt ) const;
			
			std::unique_ptr<Camera> ReadCamera( const boost::property_tree::ptree& pt ) const;
		
			void ReadUserCamera( const boost::property_tree::ptree& pt, 
				CameraCollection& cameraCollection ) const;

			void ReadGebaeudesammlung( const boost::property_tree::ptree& pt, 
				const std::vector<std::pair<Track::Coupling,std::string>>& couplings, const std::vector<Kollektor>& kollektor, const TrackSystem& trackSystem, IndicatorCollection& indicatorCollection ) const;

			std::pair<Track::End,Track::End> GetEnds( const Connector& connector, int slot, const std::vector<std::pair<Track::Coupling,std::string>>& couplings ) const;	

			void ConnectSockets( Module& _module ) const;
		};

	} // namespace ptreesupport
} // namespace trax