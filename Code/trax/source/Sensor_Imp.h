// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once
#include "trax/Sensor.h"
#include "trax/ObjectID.h" 
#include "Plug_Imp.h"

#include "common/Interval.h"

#if defined(_MSC_VER)
#	pragma warning(disable: 4250) // virtual inheritance of methods via dominance
#endif

namespace trax{

	using namespace spat;
/*
	class Sensor_Imp :	public ObjectID_Imp<Sensor>,
						public JackEnumerator
	{
	public:
		Sensor_Imp();
		Sensor_Imp( const Sensor_Imp& sensor );
		Sensor_Imp( Sensor_Imp&& ) = default;
		~Sensor_Imp() = default;

		Sensor_Imp& operator=( const Sensor_Imp& sensor ) = delete;
		Sensor_Imp& operator=( Sensor_Imp&& ) = delete;

		std::string	TypeName() const noexcept override;

		const std::string& Reference( const std::string& name ) const override{
			return ObjectID_Imp<Sensor>::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return ObjectID_Imp<Sensor>::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return ObjectID_Imp<Sensor>::ID();
		}

		void ID( IDType id ) noexcept override{
			return ObjectID_Imp<Sensor>::ID( id );
		}

		void Trigger( const Event& _event ) override;

		Jack& JackOnTrigger() noexcept override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		void Attach( TrackBuilder* pTrackBuilder ) noexcept override;

		TrackBuilder* TrackAttached() const noexcept override;

		int CountJacks() const noexcept override;
	protected:
		const Jack& _GetJack( int idx ) const override;

	private:
		TrackBuilder* m_pTrackBuilder;
		Jack_Imp m_JackOnTrigger{ "JackOnTrigger" };
	};
*/


	class Sensor_Imp :	public ObjectID_Imp<SensorFilterJack>,
						public JackEnumerator
	{
	public:
		Sensor_Imp( int flags );
		Sensor_Imp( const Sensor_Imp& sensor );
		Sensor_Imp( Sensor_Imp&& ) = default;
		~Sensor_Imp() = default;

		Sensor_Imp& operator=( const Sensor_Imp& sensor ) = delete;
		Sensor_Imp& operator=( Sensor_Imp&& ) = delete;

		const char*	TypeName() const noexcept override;

		const std::string& Reference( const std::string& name ) const override{
			return ObjectID_Imp<SensorFilterJack>::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return ObjectID_Imp<SensorFilterJack>::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return ObjectID_Imp<SensorFilterJack>::ID();
		}

		void ID( IDType id ) noexcept override{
			return ObjectID_Imp<SensorFilterJack>::ID( id );
		}

		void Trigger( const Event& _event ) override;

		int GetEffect() const noexcept override;

		void SetEffect( int effect ) noexcept override;

		Jack& JackOnTrigger() noexcept override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		void Attach( TrackBuilder* pTrackBuilder ) noexcept override;

		TrackBuilder* TrackAttached() const noexcept override;

		int CountJacks() const noexcept override;
	protected:
		const Jack& _GetJack( int idx ) const override;

	private:
		TrackBuilder* m_pTrackBuilder;
		int m_FilterFlags = 0;

		Jack_Imp m_JackOnTrigger{ "JackOnTrigger" };
	};

	class VelocitySensor_Imp :	public VelocitySensor,
								public Sensor_Imp{
	public:
		VelocitySensor_Imp( int flags );

		const char*	TypeName() const noexcept override;

		void Trigger( const Event& _event ) override;

		void SetVelocity( Velocity velocityMin, Velocity velocityMax ) noexcept override;

		void SetVelocity( const common::Interval<Velocity>& velocity ) noexcept override;

		const common::Interval<Velocity>& GetVelocity() const noexcept override;

		void VelocityMin( Velocity velocityMin ) noexcept override;

		Velocity VelocityMin() const noexcept override;

		void VelocityMax( Velocity velocityMax ) noexcept override;

		Velocity VelocityMax() const noexcept override;

		void TriggerInside( bool bTriggerInside ) noexcept override;

		bool TriggerInside() const noexcept override;


	private:
		common::Interval<Velocity> m_Velocity;
		bool m_bTriggerInside;
	};

	class WeighSensor_Imp : public WeighSensor,
							public Sensor_Imp{
	public:
		WeighSensor_Imp( int flags );

		const char*	TypeName() const noexcept override;

		void Trigger( const Event& _event ) override;

		void Weight( Mass weightMin, Mass weightMax ) noexcept override;

		void Weight( const common::Interval<Mass>& weight ) noexcept override;

		const common::Interval<Mass>& Weight() const noexcept override;

		void WeightMin( Mass weightMin ) noexcept override;

		Mass WeightMin() const noexcept override;

		void WeightMax( Mass weightMax ) noexcept override;

		Mass WeightMax( ) const noexcept override;

		void TriggerInside( bool bTriggerInside ) noexcept override;

		bool TriggerInside() const noexcept override;

		void WeighTrain( bool bTrain ) noexcept override;

		bool WeighTrain() const noexcept override;
	private:
		common::Interval<Mass> m_Weight;
		bool m_bTriggerInside;
		bool m_bTrain;
	};


	class IntervalSensor_Imp :	public IntervalSensor,
								public Sensor_Imp{
	public:
		IntervalSensor_Imp( int flags );

		void Value( Real valueMin, Real valueMax ) noexcept override;

		void Value( const common::Interval<Real>& interval ) noexcept override;

		const common::Interval<Real>& Value() const noexcept override;

		void ValueMin( Real valueMin ) noexcept override;

		Real ValueMin() const noexcept override;

		void ValueMax( Real valueMax ) noexcept override;

		Real ValueMax() const noexcept override;

		void TriggerInside( bool bTriggerInside ) noexcept override;

		bool TriggerInside() const noexcept override;
	protected:
		bool ShallTrigger( Real value ) const noexcept;
	private:
		common::Interval<Real> m_Value;
		bool m_bTriggerInside;
	};


	class TractionSensor_Imp :	public IntervalSensor_Imp{
	public:
		TractionSensor_Imp( int flags );

		const char*	TypeName() const noexcept override;

		void Trigger( const Event& _event ) override;
	};

}