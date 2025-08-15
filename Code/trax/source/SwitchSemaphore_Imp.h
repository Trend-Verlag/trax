//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once
#include "trax/Switch.h"
#include "Indicator_Imp.h"

namespace trax{

	typedef Pose_Imp<ObjectID_Imp<BinaryIndicator>> SwitchSemaphore_Base;

	class SwitchSemaphore : public SwitchSemaphore_Base,
							public SwitchAligned,
							public PlugEnumerator,
							public JackEnumerator
	{
	public:
		SwitchSemaphore();

		const char* TypeName() const noexcept override;

		void Attach( std::shared_ptr<Connector> pSwitch ) noexcept override;

		bool PreserveUpDirection() const noexcept override;

		void PreserveUpDirection( bool bPreserveUpDirection ) noexcept override;

		IDType GetSwitchRef() const noexcept override;

		std::shared_ptr<Connector> GetSwitch() const noexcept override;

		void Get( spat::Frame<Length,One>& frame ) const override;

		Status Set( Status status, bool pulse = true ) override;

		void Toggle( bool pulse = true ) override;

		Status Get() const noexcept override;

		bool IsValidState( Status status ) const noexcept override;

		void RotateWithStatus( Status status, Real angle ) noexcept override;

		Real RotateWithStatus( Status status ) const noexcept override;

		Plug& PlugToToggle() noexcept override;

		Jack& JackOnChange() noexcept override;

		Jack& JackOn( Status status ) override;

		Plug& PlugTo( Status status ) override;

		Jack& JackOnOne() noexcept override;

		Plug& PlugToOne() noexcept override;

		Jack& JackOnTwo() noexcept override;

		Plug& PlugToTwo() noexcept override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;


		const std::string& Reference( const std::string& name ) const override{
			return SwitchSemaphore_Base::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return SwitchSemaphore_Base::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return SwitchSemaphore_Base::ID();
		}

		void ID( IDType id ) noexcept override{
			return SwitchSemaphore_Base::ID( id );
		}

		int CountPlugs() const noexcept override;

		int CountJacks() const noexcept override;
	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;
	private:
		Status								m_Status;
		std::shared_ptr<Connector>			m_pSwitch;
		bool								m_bPreserveUpDirection;
		Real								m_RotAngleOne;
		Real								m_RotAngleTwo;

		Jack_Imp m_JackOnGo{ "JackOnOne" };
		Jack_Imp m_JackOnBranch{ "JackOnTwo" };
		Jack_Imp m_JackOnChange{ "JackOnChange" };

		StatusHolder_Plug<SwitchSemaphore>	m_PlugToGo;
		StatusHolder_Plug<SwitchSemaphore>	m_PlugToBranch;
		Toggle_Pug<SwitchSemaphore>			m_PlugToToggle;
	};


	class SwitchMultiSemaphore : public Multicator,
								 public SwitchAligned
	{
	public:
		SwitchMultiSemaphore() noexcept;

		const char* TypeName() const noexcept override;

		// Multicator
		using Multicator::Get;

		void Get( spat::Frame<Length,One>& frame ) const override;


		// SwitchAligned
		void Attach( std::shared_ptr<Connector> pSwitch ) noexcept override;

		bool PreserveUpDirection() const noexcept override;

		void PreserveUpDirection(bool bPreserveUpDirection) noexcept override;

		IDType GetSwitchRef() const noexcept override;

		std::shared_ptr<Connector> GetSwitch() const noexcept override;

	private:
		std::shared_ptr<Connector>		m_pSwitch;
		bool							m_bPreserveUpDirection;
	};

}
