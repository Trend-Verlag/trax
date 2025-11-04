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

#include "trax/collections/support/Anl4TrackSystemReader.h"

namespace trax{

	struct Camera;
	struct CameraCollection;
	struct Module;
	struct ModuleCollection;

	namespace ptreesupport{

		class Anl4ModuleReader : public Anl4TrackSystemReader
		{
		public:
			dclspc Anl4ModuleReader( const char* pLocale = nullptr );
			dclspc Anl4ModuleReader( SocketRegistry& socketRegistry, const char* pLocale = nullptr );

			std::unique_ptr<ModuleCollection> dclspc ReadModuleCollection( const boost::property_tree::ptree& pt ) const;
			
			std::unique_ptr<Module> dclspc ReadModule( const boost::property_tree::ptree& pt ) const;

			std::unique_ptr<Module> dclspc ReadModule( const boost::property_tree::ptree& pt, int idx ) const;
		private:
			struct Kollektor{
				IDType m_ID;
				spat::Frame<Length,One> m_Frame;
				std::vector<IDType> m_Tracks;
			};


			std::unique_ptr<CameraCollection> CreateCameraCollection( const boost::property_tree::ptree& pt ) const;
	
			std::unique_ptr<Camera> CreateCamera( const boost::property_tree::ptree& pt ) const;

			
			void ConnectSockets( Module& _module ) const;
		};

	} // namespace ptreesupport

} // namespace trax