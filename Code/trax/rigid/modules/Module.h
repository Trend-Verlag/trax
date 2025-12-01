//	trax track library
//	AD 2025 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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

#include "trax/Configuration.h"
#include "trax/Identified.h"
#include "trax/Units.h"
#include "trax/SocketRegistry.h"
#include "trax/collections/ObjectIDDecorator.h"

#include "spat/Box.h"
#include "spat/Frame.h"


namespace trax{

	struct CameraCollection;
	struct CargoCollection;
	struct Fleet;
	struct IndicatorCollection;
	struct PulseCounterCollection;
	struct SignalCollection;
	struct TimerCollection;
	struct TrackSystem;


	/// \brief A module is a chunck of railroad stuff that is all located in one rectangular volume.
	///
	/// A module holds collection types of various kind to store the items. So TrackSystem, 
	/// Fleet and Batch collections can be assigned to it (one of each only). A module maintains
	/// a frame of reference which is used to determine the general situation of the module. 
	/// A track will give module local coordinates.
	struct Module : Identified<Module>,
					virtual SocketRegistry
	{
		/// \brief Makes a standard Module object.
		static dclspc std::unique_ptr<Module> Make( bool bCreateCollections = true ) noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Checks whether the elements in this module are valid 
		/// \returns true if all the elements are valid build.
		virtual bool IsValid() const noexcept = 0;


		/// \brief Set frame of reference.
		///
		/// The module's frame of reference gets applied to track locations 
		/// and thereby rolling stock positions etc, so these will always 
		/// give ModuleSet relative values.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) noexcept = 0;


		/// \brief Get frame of reference.
		///
		/// The module's frame of reference gets applied to track locations 
		/// and thereby rolling stock positions etc, so these will always 
		/// give ModuleSet relative values.
		virtual const spat::Frame<Length,One>& GetFrame() const noexcept = 0;


		/// \brief Gets the volume this module occupies.
		virtual void SetVolume( const spat::Box<Length>& volume ) noexcept = 0;


		/// \brief Sets the volume this module occupies.
		virtual const spat::Box<Length>& GetVolume() const noexcept = 0;


		/// \brief Attaches a TrackSystem to this Module.
		/// \param pTrackSystem Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<TrackSystem> pTrackSystem ) noexcept = 0;


		/// \returns A pointer to the attached TrackSystem.
		virtual std::shared_ptr<TrackSystem> GetTrackSystem() const noexcept = 0;


		/// \brief Attaches a Fleet to this Module.
		/// \param pFleet Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<Fleet> pFleet ) noexcept = 0;


		/// \returns A pointer to the attached Fleet.
		virtual std::shared_ptr<Fleet> GetFleet() const noexcept = 0;


		/// \brief Attaches a SignalCollection to this Module.
		/// \param pSignalCollection Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<SignalCollection> pSignalCollection ) noexcept = 0;


		/// \returns A pointer to the attached SignalCollection.
		virtual std::shared_ptr<SignalCollection> GetSignalCollection() const noexcept = 0;


		/// \brief Attaches an IndicatorCollection to the track system.
		/// \param pIndicatorCollection Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<IndicatorCollection> pIndicatorCollection ) noexcept = 0;


		/// \returns A pointer to the IndicatorCollection attached to this TrackSystem.
		virtual std::shared_ptr<IndicatorCollection> GetIndicatorCollection() const noexcept = 0;

		
		/// \brief Attaches an CargoCollection to the track system.
		/// \param pCargoCollection Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<CargoCollection> pCargoCollection ) noexcept = 0;


		/// \returns A pointer to the attached CargoCollection.
		virtual std::shared_ptr<CargoCollection> GetCargoCollection() const noexcept = 0;


		/// \brief Attaches a TimerCollection to the module.
		/// \param pTimerCollection Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<TimerCollection> pTimerCollection ) noexcept = 0;


		/// \returns A pointer to the attached TimerCollection.
		virtual std::shared_ptr<TimerCollection> GetTimerCollection() const noexcept = 0;


		/// \brief Attaches a PulseCounterCollection to the module.
		/// \param pPulseCounterCollection Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<PulseCounterCollection> pPulseCounterCollection ) noexcept = 0;


		/// \returns A pointer to the attached PulseCounterCollection.
		virtual std::shared_ptr<PulseCounterCollection> GetPulseCounterCollection() const noexcept = 0;


		/// \brief Attaches an CameraCollection to the track system.
		/// \param pCameraCollection Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<CameraCollection> pCameraCollection ) noexcept = 0;


		/// \returns A pointer to the attached Batch.
		virtual std::shared_ptr<CameraCollection> GetCameraCollection() const noexcept = 0;


		/// \brief Moves all objects from all the collections to this module.
		///
		/// Objects only can get moved if the appropriate collections are
		/// attached to this module. The function will use the Remove() and Add()
		/// functions of decorators if applicable.
		virtual void Take( Module& fromModule ) = 0;


		/// \brief Clear the Module from all attached objects.
		virtual void Clear() = 0;


		/// \brief Clears all the attached collections.
		virtual void ClearCollections() = 0;
	};

}
