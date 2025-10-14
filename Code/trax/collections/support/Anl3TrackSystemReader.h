// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "It's all over and I'm standing pretty
//   In the dust that was a city"
//
//						Goldfinger

#include "CollectionSupportXML.h"

#include "trax/Signal.h"
#include "trax/Switch.h"
#include "trax/Track.h"

#include <map>

namespace trax{

	struct IndicatorCollection;
	struct ConnectorCollection;
	struct SensorFilterJack;
	struct SignalCollection;
	struct TimerCollection;
	struct PulseCounterCollection;
	struct Signal;

	namespace ptreesupport{

		class Anl3TrackSystemReader : public PTreeReader
		{
		public:
			dclspc Anl3TrackSystemReader( const char* pLocale = nullptr );
			dclspc Anl3TrackSystemReader( SocketRegistry& socketRegistry, const char* pLocale = nullptr );

			std::shared_ptr<TrackSystem> dclspc ReadTrackSystem( const boost::property_tree::ptree& pt ) const override;
			
		protected:
			virtual std::shared_ptr<TrackSystem> CreateTrackSystem( const boost::property_tree::ptree& pt, 
				std::vector<std::pair<Track::Coupling,std::string>>& couplings, SignalCollection& signalCollection, IndicatorCollection& indicatorCollection, TimerCollection& timerCollection, PulseCounterCollection& pulseCounterCollection, std::map<IDType,Velocity>& travelVelocities, IDType& maxSensorID ) const;
		
			void ReadDreibein( const boost::property_tree::ptree& pt, spat::Frame<Length,One>& frame ) const;

			Signal::Status SignalStatusFromEEP( int stellung, const Signal& signal ) const;
			Switch::Status SwitchStatusFromEEP( int weichenstellung ) const noexcept;
			ThreeWaySwitch::Status ThreeWaySwitchStatusFromEEP( int weichenstellung ) const noexcept;
			SingleSlipSwitch::Status SingleSlipSwitchStatusFromEEP( int weichenstellung ) const noexcept;
			DoubleSlipSwitch::Status DoubleSlipSwitchStatusFromEEP( int weichenstellung ) const noexcept;
		private:
			std::shared_ptr<TrackBuilder> CreateTrack( const boost::property_tree::ptree& pt,
				ConnectorCollection& connectors, std::vector<std::tuple<std::shared_ptr<Signal>,TrackBuilder*,common::Interval<Length>>>& signals, IndicatorCollection& indicatorCollection, TimerCollection& timerCollection, PulseCounterCollection& pulseCounterCollection, std::map<IDType,Velocity>& travelVelocities, IDType& maxSensorID ) const;

			std::pair<Track::Coupling,std::string> CreateTrackCoupling( const boost::property_tree::ptree& pt, 
				const TrackSystem& trackSystem ) const;

			std::shared_ptr<Signal> CreateSignal( const boost::property_tree::ptree& pt, 
				IndicatorCollection& indicatorCollection, common::Interval<Length>& trackRange, std::map<IDType,Velocity>& travelVelocities, std::vector<int>& functionMap, bool bCreateSignalSemaphore, bool bCreateVorsignalSemaphore ) const;
			
			std::unique_ptr<SensorFilterJack> CreateKontakt( const boost::property_tree::ptree& pt, 
				TimerCollection& timerCollection, PulseCounterCollection& pulseCounterCollection, TrackLocation& trackLocation, IDType& maxSensorID ) const;

			Track::EndType From( const std::string& name ) const;

		};

		std::shared_ptr<TrackSystem> dclspc ReadTrackSystemFromANL3( const ptreesupport::PTreeReader& reader, const boost::property_tree::ptree& pt, int atIdx );
	} // namespace ptreesupport
} // namespace trax