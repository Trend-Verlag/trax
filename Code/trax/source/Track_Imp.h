//	trax track library
//	AD 2013 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Jack.h"
#include "trax/Plug.h"
#include "trax/Track.h"
#include "trax/TrackLocation.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	using common::Interval;

	struct TrackEndTransition;

	template<class Base> class RoadwayTwist_Imp;

	typedef Pose_Imp<ObjectID_Imp<TrackBuilder>> TrackBase;

	class Track_Imp : public TrackBase,
					  public JackEnumerator,
					  public PlugEnumerator,
					  public std::enable_shared_from_this<Track_Imp>
	{
	public:
		Track_Imp() noexcept;
		Track_Imp( const Track_Imp& ti );
		Track_Imp( Track_Imp&& ) = delete;
		~Track_Imp() = default;

		Track_Imp& operator=( const Track_Imp& ) = delete;
		Track_Imp& operator=( Track_Imp&& ) = delete;


		/// \brief to return correct shared_ptr to this, this object have 
		/// to know about the use count structure.
	//	void SetWeakPointerToSelf( std::weak_ptr<Track_Imp> pThis ) noexcept;
		
		void AddConnector( Connector* pConnector, EndType atend ) noexcept;
		
		// Pose_Imp:
		void PropagateAbsoluteFrameToClients() noexcept override;

		// Track:
		const char* TypeName() const noexcept override;

		TrackType GetTrackType() const noexcept override;

		std::shared_ptr<const TrackBuilder> This() const noexcept override;

		std::shared_ptr<TrackBuilder> This() noexcept override;

		std::shared_ptr<const MovableTrack> GetMovableTrack() const noexcept override;

		std::shared_ptr<MovableTrack> GetMovableTrack() noexcept override;

		std::shared_ptr<const ParallelizableTrack> GetParallelizableTrack() const noexcept override;

		std::shared_ptr<ParallelizableTrack> GetParallelizableTrack() noexcept override;

		const std::string& Reference( const std::string& name ) const override{
			return TrackBase::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return TrackBase::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return TrackBase::ID();
		}

		void ID( IDType id ) noexcept override{
			return TrackBase::ID( id );
		}

		using TrackBase::SetFrame;

		void SetFrame( const spat::Frame<Length,One>& start, Length s, bool bAllowTwistOffset = true ) override;

		bool IsValid() const noexcept override;

		bool Diagnose( std::ostream& os ) const noexcept override;

		Length GetLength() const noexcept override;
		
		Interval<Length> Range() const noexcept override;

		TrackEnd TransitionEnd( EndType thisEnd ) const noexcept override;
				
		bool IsCoupled( EndType atend = EndType::any ) const noexcept override;

		void TNBFrame( Length s, spat::Frame<Length,One>& frame ) const override;

		AnglePerLength Curvature( Length s ) const override;

		AnglePerLength Torsion( Length s ) const override;

		Angle GetTwistValue( Length s ) const override;

		AnglePerLength GetTwistD1( Length s ) const override;

		bool IsFlat() const noexcept override;

		spat::Vector<One> LocalUp() const override;

		void Transition	( Length s, TrackData<Real>& td, Real engine_meters_per_unit ) const override;

		void Transition	( Length s, spat::Position<Length>& pos ) const override;

		void Transition	( Length s, spat::Vector<One>& tan ) const override;

		void Transition	( Length s, spat::VectorBundle<Length,One>& bundle ) const override;

		void Transition	( Length s, spat::VectorBundle2<Length,One>& bundle ) const override;

		void Transition	( Length s, spat::Frame<Length,One>& frame ) const override;

		void DoTrigger( const Interval<Length>& range, const Event& _event ) const override;

		bool DoSignal( const Interval<Length>& range, Orientation orientation, SignalTarget& signalTarget ) const override;

		Signal* GetSignal( const TrackLocation& loc ) const noexcept override;

		void Reserve( common::Interval<Length> range, IDType forID ) override;

		ReservationIterator BeginReservations() const noexcept override;

		ReservationIterator EndReservations() const noexcept override;

		void DeleteReservation( common::Interval<Length> inRange, IDType forID = anyID ) noexcept override;

		bool IsReserved( common::Interval<Length> inRange, IDType forID = anyID ) const noexcept override;

		std::vector<Overlap> Overlaps( IDType withID ) const override;

		void UserData( TrackUserData* pData ) noexcept override{
			m_pData = pData;
		}

		TrackUserData* UserData() const noexcept override{
			return m_pData;
		}

		// TrackBuilder:
		void Couple( std::pair<std::shared_ptr<TrackBuilder>,EndType> thisEnd, std::pair<std::shared_ptr<TrackBuilder>,EndType> othersEnd ) override;

		void DeCouple( EndType thisendbool = EndType::any, bool oneSided = false ) override;


		void Attach( std::shared_ptr<const Curve> pCurve, common::Interval<Length> curveSegment = {0_m,+infinite__length} ) override;

		void Attach( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> curve ) override;

		void Attach( std::pair<std::unique_ptr<const Curve>,common::Interval<Length>>& curve ) override;


		std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> DetachCurve() noexcept override;

		std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> GetCurve() const noexcept override;

		void Attach( std::unique_ptr<RoadwayTwist> pTwist ) override;

		std::unique_ptr<RoadwayTwist> DetachTwist() override;

		RoadwayTwist& GetTwist() const noexcept override;

		void Attach( std::shared_ptr<Sensor> pSensor, const TrackLocation& tl ) override;

		bool Attached( const Sensor& sensor, TrackLocation* pTrackLocation = nullptr ) const override;

		void Detach( const Sensor& sensor ) noexcept override;

		std::shared_ptr<Sensor> GetSensor( int idx ) const noexcept override;

		int CountSensors() const override;

		void Attach( std::shared_ptr<Signal> pSignal, const Interval<Length>& trackRange ) override;

		void Attach( std::shared_ptr<Signal> pSignal, const TrackLocation& location ) override;

		bool Attached( const Signal& signal, TrackRange* pTrackRange = nullptr ) const override;
		
		void Detach( const Signal& signal ) override;

		void Flip( bool flipAttached = false ) override;

		void CreateEndTransitionSignal( EndType atend ) override;

		void DestroyEndTransitionSignal( EndType atend ) override;

		Connector* GetConnector( EndType atend ) const noexcept override;

		Connector* GetConnector( const Orientation& inDirection ) const noexcept override;

		//JackEnumerator:
		int CountJacks() const noexcept override;

		//PlugEnumerator
		int CountPlugs() const override;

		std::shared_ptr<Body> GetBody() const noexcept override{ return nullptr; }
	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;

		virtual void OnGeometryChanged() noexcept{}; // called by Attach/Detach Curve and Twist.
	private:
	//	std::weak_ptr<Track_Imp>						m_pThis;
		std::pair<std::shared_ptr<Track_Imp>,EndType>	m_TrackFront;
		std::pair<std::shared_ptr<Track_Imp>,EndType>	m_TrackEnd;

		std::shared_ptr<const Curve>	m_pCurve;
		common::Interval<Length>		m_CurveSegment = {0_m,infinite__length};
		inline Length c( Length s ) const noexcept{
			return m_CurveSegment.Near() + (m_CurveSegment.Normal() ? +1.f : -1.f) * s;
		}

		std::unique_ptr<RoadwayTwist>		m_pTwist;
		std::shared_ptr<TrackEndTransition>	m_pTETFront;
		std::shared_ptr<TrackEndTransition>	m_pTETEnd;
		spat::Frame<Length,One>				m_TotalFrame;

		std::vector<std::pair<TrackLocation,std::shared_ptr<Sensor>>> m_Sensors;

		// We need a defined orientation, even if the
		// range length is zero.
		struct OrientedInterval : Interval<Length>{
			Orientation m_Orientation;

			OrientedInterval( const Interval<trax::Length>& interval ) noexcept
				: Interval<trax::Length>{interval}
			{
				m_Orientation = Normal() ? Orientation::Value::para : Orientation::Value::anti;
			}

			OrientedInterval( trax::Length _near, trax::Length _far, Orientation orientation ) noexcept
				:	//Interval<trax::Length>{near,far}, //  VS2019, railSample project: error C2059: syntax error: ','
					m_Orientation			{orientation}
			{
				Near( _near );
				Far( _far );
				assert( _near == _far || ((Normal() ? Orientation::Value::para : Orientation::Value::anti) == orientation) );
			}
		};

		// Signals operating on this track with their total 
		// range and orientation relative to this track:
		std::vector<std::pair<OrientedInterval,std::shared_ptr<Signal>>> m_Signals;
		Connector* m_pConnectorFront;
		Connector* m_pConnectorEnd;

		std::vector<Reservation> m_Reservations;
		bool m_ReserveLoopBrakerFront = false;
		bool m_ReserveLoopBrakerEnd = false;

		void ReserveConnected( const Reservation& reservation );
		void DeleteConnected( const Reservation& reservation ) noexcept;

		inline bool InvalidParameter( Length s ) const{
			return ( s > GetLength() + epsilon__length || s < 0_m );
		}

		void TestTransition( Length s ) const;
	
		// Resolves a given range relative to this track into a list of resolved 
		// track/ranges pairs. It contains all the tracks that the range intersects,
		// with the range resolved for the particular track. vector<> will be 
		// empty, if range does not touch this track at all.
		std::vector<std::pair<Track_Imp&,common::Interval<Length>>> GetRanges( const common::Interval<Length>& range );
		void RangeAt( EndType theEnd, Interval<Length> range, std::vector<std::pair<Track_Imp&,Interval<Length>>>& list );
		// Recalculates track parameters with respect to a connected track.
		Track* Transform( Interval<Length>& range, EndType& toTrackAtEnd ) const noexcept;

		mutable bool m_LoopBraker; // used by iterations along track chains
		TrackUserData* m_pData = nullptr;
	};
}