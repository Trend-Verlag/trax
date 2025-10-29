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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Configuration.h"
#include "trax/Identified.h"
#include "trax/Units.h"

#include "spat/Frame.h"
#include "spat/Box.h"

#include <memory>

namespace trax{

	struct BallAndSocketJoint;
	struct Body;
	struct DistanceJoint;
	struct Geom;
	struct GeomCuboid;
	struct GeomCylinder;
	struct GeomCapsule;
	struct GeomSphere;
	struct GeomHeightField;
	struct GeomMesh;
	struct Gestalt;
	struct HingeJoint;
	struct Shape;
	struct Simulated;
	struct Simulator;
	struct SliderJoint;
	enum class GeomType : char;
	class TrackJointFeeder;
	class TrackJointFeederMotorModel;

	struct Scene : Identified<Scene>
	{

		/// \brief Makes a Scene object.
		static dclspc std::unique_ptr<Scene> Make( const Simulator& simulator ) noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Checks whether the scene is valid 
		/// \param bSilent If false there will be diagnostic output to the console.
		/// \returns true if the scene is valid.
		virtual bool IsValid( bool bSilent = true ) const noexcept = 0;


		/// \brief Sets the up direction for this simulator.
		/// \param up Vector pointing in the up direction
		virtual void Up( const spat::Vector<One>& up ) = 0;


		/// \brief Gets the up direction for this simulator.
		virtual spat::Vector<One> Up() const = 0;


		/// \brief Sets the vector of gravity acceleration.
		///
		/// Gravity acts according to the negative up direction.
		/// \param gravityAccelerarion Vector of gravity acceleration.
		virtual void Gravity( const spat::Vector<Acceleration>& gravityAccelerarion ) = 0;


		/// \brief Gets the gravity force applied with this simulator.
		virtual spat::Vector<Acceleration> Gravity() const = 0;


		/// \brief Registers a Simulated object for simulation.
		///
		/// Objects that are created with a scene reference register automatically,
		/// others will need manual registration.
		/// \param simulated The Simulated object to register.
		virtual void Register( Simulated& simulated ) = 0;


		/// \brief Unregisters a Simulated object from simulation.
		virtual void Unregister( const Simulated& simulated ) = 0;


		/// \brief Unregisters all Simulated objects from simulation.
		virtual void UnregisterAllSimulated() = 0;


		/// \brief Starts the simulation calculations.
		/// \param dt Fixed time step in milliseconds.
		virtual void StartStep( Time dt = fixed_timestep ) noexcept = 0;


		/// \brief Updates the simulation by the fixed time step.
		virtual void Update( Time dt = fixed_timestep ) = 0;


		/// \brief Requests the Simulator to end simulation.
		///
		/// \returns True if the simulation has finished; false if still calculations are in progress.
		virtual bool EndStep() noexcept = 0;


		/// \brief Completes one simulation step.
		virtual void Step( Time dt = fixed_timestep ) noexcept = 0;


		/// \name Loop
		/// \brief Enters a main simulation loop.
		///
		/// The loop is left when the simulation is stopped or the 
		/// PlugToStop is triggered.
		/// \param forTimePeriod Time to run simulation for.
		//@}
		virtual void Loop() noexcept = 0;

		virtual void Loop( Time forTimePeriod ) noexcept = 0;
		//@}


		/// \brief Stops the simulation loop.
		virtual void Stop() noexcept = 0;


		virtual std::unique_ptr<Body> CreateBody() const = 0;

		virtual std::unique_ptr<Shape> CreateShape() const = 0;

		virtual std::unique_ptr<Gestalt> CreateGestalt() const = 0;

		virtual std::unique_ptr<Gestalt> CreateGestalt( spat::Box<Length> box, Mass mass ) const noexcept = 0;
		
		virtual std::unique_ptr<Geom> CreateGeom( GeomType type ) const = 0;

		virtual std::unique_ptr<GeomCuboid> CreateGeomCuboid() const = 0;

		virtual std::unique_ptr<GeomCylinder> CreateGeomCylinder() const = 0;

		virtual std::unique_ptr<GeomCapsule> CreateGeomCapsule() const = 0;

		virtual std::unique_ptr<GeomSphere> CreateGeomSphere() const = 0;

		virtual std::unique_ptr<GeomHeightField> CreateGeomHeightField() const = 0;

		virtual std::unique_ptr<GeomMesh> CreateGeomConvexMesh() const = 0;

		virtual std::unique_ptr<GeomMesh> CreateGeomTriangleMesh() const = 0;


		virtual std::unique_ptr<HingeJoint> CreateHingeJoint( std::shared_ptr<Body> pBodyA, const spat::Frame<Length,One>& localAnchorA, std::shared_ptr<Body> pBodyB, const spat::Frame<Length,One>& localAnchorB ) const = 0;

		virtual std::unique_ptr<SliderJoint> CreateSliderJoint( std::shared_ptr<Body> pBodyA, const spat::Frame<Length,One>& localAnchorA, std::shared_ptr<Body> pBodyB, const spat::Frame<Length, One>& localAnchorB ) const = 0;

		virtual std::unique_ptr<BallAndSocketJoint> CreateBallAndSocketJoint( std::shared_ptr<Body> pBodyA, const spat::Frame<Length,One>& localAnchorA, std::shared_ptr<Body> pBodyB, const spat::Frame<Length, One>& localAnchorB ) const = 0;

		virtual std::unique_ptr<DistanceJoint> CreateDistanceJoint( std::shared_ptr<Body> pBodyA, const spat::Position<Length>& localAnchorA, std::shared_ptr<Body> pBodyB, const spat::Position<Length>& localAnchorB ) const = 0;


		virtual std::shared_ptr<TrackJointFeeder> CreateTrackJointFeeder( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor ) noexcept = 0;

		virtual std::shared_ptr<TrackJointFeeder> CreateTrackJointFeeder( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor, 
			const Body& bodyTrack ) noexcept = 0;

		virtual std::shared_ptr<TrackJointFeederMotorModel> CreateTrackJointFeederMotorModel( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor ) noexcept = 0;

		virtual std::shared_ptr<TrackJointFeederMotorModel> CreateTrackJointFeederMotorModel( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor, 
			const Body& bodyTrack ) noexcept = 0;

		virtual void Release( TrackJointFeeder& feeder ) noexcept = 0;


		/// \brief A Jack that triggers a pulse once a simulation step.
		///
		/// Use this to create a timer.
		virtual struct Jack& JackOnSimulationStep() noexcept = 0;


		/// \brief A Plug that stops a simulation loop.
		virtual struct MultiPlug& PlugToStop() noexcept = 0;


		virtual ~Scene() = default;
	};

}