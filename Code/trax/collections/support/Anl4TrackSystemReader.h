//	trax track library
//	AD 2014 
//
//  "If you belive it,
//  don't keep it all inside"
//
//					Stiltskin
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

#include "CollectionSupportXML.h"

#include "trax/collections/Interfaces.h"
#include "trax/Track.h"

namespace trax{

	namespace ptreesupport{

		class Anl4TrackSystemReader : public PTreeReader
		{
		public:
			Anl4TrackSystemReader( const char* pLocale = nullptr );
			Anl4TrackSystemReader( SocketRegistry& socketRegistry, const char* pLocale = nullptr );

			std::shared_ptr<TrackSystem> ReadTrackSystem( const boost::property_tree::ptree& pt ) const override;

		protected:
			std::shared_ptr<TrackSystem> CreateTrackSystem( const boost::property_tree::ptree& pt ) const;
			
			std::shared_ptr<TrackCollection> CreateTrackCollection( const boost::property_tree::ptree& pts, 
				std::vector<Track::Coupling>& couplings ) const;

			std::shared_ptr<TrackBuilder> CreateTrack( const boost::property_tree::ptree& pt, 
				std::vector<Track::Coupling>& couplings ) const;
			
			std::unique_ptr<ConnectorCollection> CreateConnectorCollection( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;

			std::unique_ptr<Switch> CreateSwitch( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;
	
			std::unique_ptr<ThreeWaySwitch> CreateThreeWaySwitch( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;
	
			std::unique_ptr<SingleSlipSwitch> CreateSingleSlipSwitch( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;
	
			std::unique_ptr<DoubleSlipSwitch> CreateDoubleSlipSwitch( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;

			std::unique_ptr<Location> CreateLocation( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;

			std::unique_ptr<IndicatorCollection> CreateIndicatorCollection( const boost::property_tree::ptree& pt, 
				const ConnectorCollection& connectorCollection, const SignalCollection& signalCollection ) const;
			
			std::unique_ptr<BinaryIndicator> CreateSwitchSemaphore( const boost::property_tree::ptree& pt, 
				const ConnectorCollection& connectorCollection ) const;

			std::unique_ptr<Indicator> CreateSwitchMultiSemaphore( const boost::property_tree::ptree& pt, 
				const ConnectorCollection& connectorCollection ) const;

			std::unique_ptr<Indicator> CreateVelocityControlSemaphore( const boost::property_tree::ptree& pt, 
				const SignalCollection& signalCollection ) const;

			std::unique_ptr<SensorFilterJack> CreateSensor( const boost::property_tree::ptree& pt, 
				TrackLocation& trackLocation ) const;

			std::unique_ptr<VelocitySensor> CreateVelocitySensor( const boost::property_tree::ptree& pt, 
				TrackLocation& trackLocation ) const;

			std::unique_ptr<WeighSensor> CreateWeighSensor( const boost::property_tree::ptree& pt, 
				TrackLocation& trackLocation ) const;

			std::unique_ptr<IntervalSensor> CreateTractionSensor( const boost::property_tree::ptree& pt, 
				TrackLocation& trackLocation ) const;

			std::unique_ptr<SignalCollection> CreateSignalCollection( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem/*, const Fleet& fleet*/ ) const;
			
			std::shared_ptr<VelocityControl> CreateVelocityControl( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;

			std::shared_ptr<JumpSite> CreateJumpSignal( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem/*, const Fleet& fleet*/ ) const;

			std::unique_ptr<Section> CreateSection( const boost::property_tree::ptree& pt ) const;

			std::unique_ptr<PulseCounterCollection> CreatePulseCounterCollection( const boost::property_tree::ptree& pt ) const;

			std::unique_ptr<PulseCounter> CreatePulseCounter( const boost::property_tree::ptree& pt ) const;

			std::unique_ptr<TimerCollection> CreateTimerCollection( const boost::property_tree::ptree& pt ) const;

			std::unique_ptr<Timer> CreateTimer( const boost::property_tree::ptree& pt ) const;

			/// \name Explicit XML Reading
			///@{
			void ReadJumpSignalTarget( const boost::property_tree::ptree& pt, std::shared_ptr<JumpSite> pSignal, const TrackSystem& trackSystem/*, const Fleet& fleet*/ ) const;
			void ReadSensor( const boost::property_tree::ptree& pt, SensorFilterJack& sensor, TrackLocation& trackLocation ) const;
			void ReadSignal( const boost::property_tree::ptree& pt, std::shared_ptr<Signal> pSignal, const TrackSystem& trackSystem ) const;
			void ReadIndicator( const boost::property_tree::ptree& pt, Indicator& indicator, const ConnectorCollection& connectorCollection ) const;
			void ReadJack( const boost::property_tree::ptree& pt, Jack& jack ) const;
			void ReadPlug( const boost::property_tree::ptree& pt, Plug& plug ) const;
			void ReadPlug( const boost::property_tree::ptree& pt, MultiPlug& plug ) const;
			void ReadSection( const boost::property_tree::ptree& pt, trax::TrackBuilder& track ) const noexcept;
			///@}
		private:
		};


		/// \name Explicit XML Reading
		///@{
		dclspc void ReadEnd( const boost::property_tree::ptree& pt, TrackBuilder& track, Track::EndType endtype, std::vector<trax::Track::Coupling>& couplings );
		///@}

		/// \brief Reads a track system from a property tree.
		///
		/// \param reader The reader to use.
		/// \param pt The property tree to read from.
		/// \param atIdx The index of the track system to read [1,+inf[.
		std::shared_ptr<trax::TrackSystem> dclspc ReadTrackSystemFromANL4( const ptreesupport::PTreeReader& reader, const boost::property_tree::ptree& pt, int atIdx );
	} // namespace ptreesupport
} // namespace trax