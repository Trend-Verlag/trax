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

#include "trax/Orientation.h"

#include "spat/Frame.h"


namespace trax{

	struct Bogie;
	struct WheelFrame;
	struct Camera;
	struct CameraCollection;
	struct Fleet;
	struct Module;
	struct ModuleCollection;
	struct RollingStock;
	struct Train;

	namespace ptreesupport{

		class Anl4ModuleReader : public Anl4TrackSystemReader
		{
		public:
			dclspc Anl4ModuleReader( Scene& scene, const char* pLocale = nullptr );
			dclspc Anl4ModuleReader( Scene& scene, SocketRegistry& socketRegistry, const char* pLocale = nullptr );

			std::unique_ptr<ModuleCollection> dclspc ReadModuleCollection( const boost::property_tree::ptree& pt ) const;
			
			std::unique_ptr<Module> dclspc ReadModule( const boost::property_tree::ptree& pt ) const;

			std::unique_ptr<Module> dclspc ReadModule( const boost::property_tree::ptree& pt, int idx ) const;
		private:
			Scene& m_Scene;
			struct Kollektor{
				IDType m_ID;
				spat::Frame<Length,One> m_Frame;
				std::vector<IDType> m_Tracks;
			};


			std::unique_ptr<CameraCollection> CreateCameraCollection( const boost::property_tree::ptree& pt ) const;
	
			std::unique_ptr<Camera> CreateCamera( const boost::property_tree::ptree& pt ) const;

			std::shared_ptr<Fleet> CreateFleet( const boost::property_tree::ptree& pt ) const;

			std::pair<std::shared_ptr<Train>,Orientation> CreateTrain( const boost::property_tree::ptree& pt ) const;

			std::pair<std::shared_ptr<RollingStock>,Orientation> CreateRollingStock( const boost::property_tree::ptree& pt ) const;

			void ReadBogie( const boost::property_tree::ptree& pt, Bogie& bogie ) const;

			std::shared_ptr<Bogie> CreateBogie( const boost::property_tree::ptree& pt ) const;
			
			std::shared_ptr<WheelFrame> CreateWheelFrame( const boost::property_tree::ptree& pt ) const;

			void ConnectSockets( Module& _module ) const;
		};

	} // namespace ptreesupport

} // namespace trax