//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2016 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once
#include "trax/Indicator.h"

#include "Plug_Imp.h"
#include "Jack_Imp.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	typedef Pose_Imp<ObjectID_Imp<Indicator>> Multicator_Base;

	class Multicator :	public Multicator_Base,
						public PlugEnumerator,
						public JackEnumerator{
	public:
		Multicator();

		// Inherited via Multicator_Base
		void Toggle( bool pulse = true ) override;

		Status Set( Status status, bool pulse = true ) override;

		Status Get() const override;

		bool IsValidState( Status status ) const override;

		void RotateWithStatus( Status status, Real angle ) override;

		Real RotateWithStatus( Status status ) const override;

		Jack& JackOn( Status status ) override;

		Plug& PlugTo( Status status ) override;

		Plug& PlugToToggle() noexcept override;

		Jack& JackOnChange() noexcept override;

		void RegisterSockets( SocketRegistry& registry ) override;

		void UnregisterSockets( SocketRegistry& registry ) override;

		// Inherited via PlugEnumerator
		const char* TypeName() const noexcept override;
		
		const std::string& Reference( const std::string & name ) const override;
		
		IDType ID() const noexcept override;
		
		int CountPlugs() const override;

		// Inherited via JackEnumerator
		int CountJacks() const override;

	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;

	private:
		Indicator::Status m_Status;
		bool m_StatusChangeBlocker;

		struct IndicatorState{
			IndicatorState( Multicator& parent, Indicator::Status status );

			Real											m_RotAngle;
			Jack_Imp										m_JackOnStatus;
			MultiPlug_Imp<StatusHolder_Plug<Multicator>>	m_PlugToStatus;
		};

		std::map<Status, IndicatorState> m_IndicatorStates;

		// Create if not already created.
		void Create( Status status );

		Jack_Imp m_JackOnChange{ "JackOnChange" };
		Toggle_Pug<Multicator>	m_PlugToToggle;
	};

}
