//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2016
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Configuration.h"
#include "CameraType.h"
#include "trax/Identified.h"
#include "trax/Units.h"
#include "trax/collections/Collection.h"

#include "spat/Frame.h"
//#include "TriggerShape.h"

namespace common{
	template<typename> struct Interval;
}

namespace spat{
	template<typename> class Transformation;
}

namespace trax{

	struct Body;
	struct GeomList;
	struct Module;

	class Location;

	/// \brief Projective frustum. 
	struct Frustum{
		static const Angle default_fovy;
		static const One default_aspect;
		static const Length default_nearPlane;
		static const Length default_farPlane;

		virtual void SetFromFofy( Angle fovy = default_fovy, One aspect = default_aspect, Length zn = default_nearPlane, Length zf = default_farPlane ) = 0;

		virtual void SetFromDimensions( Length width, Length height, Length zn = default_nearPlane, Length zf = default_farPlane ) = 0;

		virtual void PlaneDistances( const common::Interval<Length>& planes ) = 0;

		virtual Length NearPlaneDistance() const = 0;

		virtual Length FarPlaneDistance() const = 0;

		virtual void Width( Length width ) = 0;

		virtual Length Width() const = 0;

		virtual void Height( Length height ) = 0;

		virtual Length Height() const = 0;

		/// \brief The Field Of View angle in radiants
		virtual void Fovy( Angle fovy ) = 0;

		/// \returns The Field Of View angle in radiants
		virtual Angle Fovy() const = 0;

		/// \param aspect The ratio of width to height of the near projection plane.
		virtual void AspectRatio( One aspect ) = 0;

		/// \returns The ratio of width to height of the near projection plane.
		virtual One AspectRatio() const = 0;

		virtual void ProjectionMatrix( const spat::Transformation<Real>& projection ) = 0;

		virtual const spat::Transformation<Real>& ProjectionMatrix() const = 0;

		virtual spat::Transformation<Real>& ProjectionMatrix() = 0;

		virtual ~Frustum() = default;
		Frustum( const Frustum& ) = delete;
		Frustum( Frustum&& ) = delete;
		Frustum& operator=( const Frustum& ) = delete;
		Frustum& operator=( Frustum&& ) = delete;
	protected:
		Frustum() = default;
	};

	/// \brief Base Camera interface.
	struct Camera : Frustum,
					Identified<Camera>
	{
		/// \brief Makes a Camera object.
		static dclspc std::unique_ptr<Camera> Make( CameraType type = CameraType::cam_basic ) noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		virtual Camera& operator=( const Camera& camera ) = 0;


		/// \returns The Type of this camera object.
		virtual CameraType GetType() const = 0;


		// Object's pose:

		///	\brief Sets position and orientation (pose) in global coordinates.
		/// 
		/// \param frame Frame of reference.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) = 0;


		/// \name GetFrame
		///	\brief Gets the pose in global coordinates.
		///@{
		virtual const spat::Frame<Length,One>& GetFrame() const = 0;

		virtual spat::Frame<Length,One>& GetFrame() = 0;
		///@}


		/// \brief Gets the current velocity.
		/// \returns The linear velocity the camera is moving with in global
		/// coordinates and units per second.
		virtual spat::Vector<Velocity> GetVelocity() const = 0;

		/// \brief Sets the camera to look at eyePoint.
		virtual void EyePoint( const spat::Position<Length>& eyePoint ) = 0;

		virtual spat::Position<Length> EyePoint() const = 0;

		virtual void Up( const spat::Vector<One>& up ) = 0;

		virtual spat::Vector<One> Up() const = 0;

		virtual void LookAt( const spat::Position<Length>& lookAtPoint ) = 0;

		virtual spat::Position<Length> LookAt() const = 0;


		/// \returns true if the position lays inside the camera's frustum.
		virtual bool InFrustum( const spat::Position<Length>& position ) const = 0;
	};

	/// \brief Interface for movable cameras.
	struct AnimatedCamera : virtual Camera{

		using Camera::operator=;

		/// \brief If attached to a module, the camera will stay on top
		/// of the terrain and will not penetrate geoms in the scene.
		virtual void AttachTo( std::shared_ptr<Module> pModule ) = 0;


		/// \returns the Module this camera is attached to.
		virtual std::shared_ptr<Module> GetModule() const = 0;


		virtual std::vector<GeomList*> OverlapQuery( unsigned int filterflags ) const = 0;

		virtual GeomList* OverlapQueryAny( unsigned int filterflags ) const = 0;


		/// \brief Updates the camera by the fixed time step.
		///
		/// Call this function once per simulation step.
		virtual void Update() = 0;


		/// \brief Reverts the last positional change done by Update()
		virtual void StepBack() = 0;


		/// \brief Stops any animated inertial movement of the camera.
		virtual void Stop() = 0;


		/// \brief Resets the position of the camera.
		virtual void Reset() = 0;


		/// \brief Sets a maximum velocity that will not be exceeded
		/// \param maxVelocity A value in the range [0,+infinite]. 
		/// The default is +infinite.
		virtual void MaximumLinearVelocity( Velocity maxVelocity ) = 0;

		virtual Velocity MaximumLinearVelocity() const = 0;


		/// \brief Set the camera velocity directly.
		virtual void SetVelocity( const spat::Vector<Velocity>& velocity ) = 0;

		virtual void SetVelocity( const spat::Vector<Velocity>& linearVelocity, const spat::Vector<AngularVelocity>& rotationalVelocity ) = 0;

		/// \brief Set the rotational velocity directly.
		virtual void RotationalVelocity( const spat::Vector<AngularVelocity>& rotvel ) = 0;

		/// \name Controller Input
		/// \brief Call these functions to provide the (scaled)
		/// input from the camera controller.
		///
		/// call these functions with the input gotten from the controllers
		/// like keyboard, mouse or joystick.
		/// The linear controller values are added to the linear velocities
		/// of the camera.
		//@{
		virtual void ControllerTranslateX( Velocity dx ) = 0;

		virtual void ControllerTranslateY( Velocity dy ) = 0;

		virtual void ControllerTranslateZ( Velocity dz ) = 0;

		virtual void ControllerRotateX( AngularVelocity dx ) = 0;

		virtual void ControllerRotateY( AngularVelocity dy ) = 0;

		virtual void ControllerRotateZ( AngularVelocity dz ) = 0;
		//@}

		/// \name Damping
		/// \brief Attenuation of movement
		///
		//@{
		virtual void DampingTranslate( Real damping ) = 0;

		virtual Real DampingTranslate() const = 0;

		virtual void DampingRotate( Real damping ) = 0;

		virtual Real DampingRotate() const = 0;
		//@}
	};

	/// \brief Interface for cameras that can get attached to a Body.
	struct BodyCam : virtual AnimatedCamera{

		using Camera::operator=;

		using AnimatedCamera::AttachTo;

		/// \brief 
		///
		virtual void AttachTo( std::shared_ptr<Body> pBody ) = 0;


		/// \returns the Body this camera is attached to.
		virtual std::shared_ptr<Body> GetBody() const = 0;


		/// \name Center
		/// \brief Sets the center relative to that a BodyCam is
		/// moving.
		/// \param center The center relative to the body's frame.
		//@{
		virtual void Center( const spat::Frame<Length,One>& center ) = 0;

		virtual const spat::Frame<Length,One>& Center() const = 0;
		//@}


		/// \brief Signals that the user put the camera into a state that might
		/// signal to cut loose from the body.
		virtual bool GetLoose() const = 0;


		/// \returns true if the camera rotates relative to the body frame, false
		/// if the rotation angles relate to the global frame.
		virtual bool BodyRealtiveRotations() const = 0;

		virtual void PhiLimits( const common::Interval<Angle>& limits ) = 0;

		virtual const common::Interval<Angle>& PhiLimits() const = 0;

		virtual void PhiResetValue( Angle phi ) = 0;

		virtual Angle PhiResetValue() const = 0;

		virtual void Phi( Angle phi ) = 0;

		virtual Angle Phi() const = 0;


		virtual void ThetaLimits( const common::Interval<Angle>& limits ) = 0;

		virtual const common::Interval<Angle>& ThetaLimits() const = 0;

		virtual void ThetaResetValue( Angle theta ) = 0;

		virtual Angle ThetaResetValue() const = 0;

		virtual void Theta( Angle theta ) = 0;

		virtual Angle Theta() const = 0;


		virtual void RadiusLimits( const common::Interval<Length>& limits ) = 0;

		virtual const common::Interval<Length>& RadiusLimits() const = 0;

		virtual void RadiusResetValue( Length radius ) = 0;

		virtual Length RadiusResetValue() const = 0;

		virtual void Radius( Length radius ) = 0;

		virtual Length Radius() const = 0;
	};


	/// \brief Interface for Cameras movable along tracks.
	struct DollyCam : Camera{

		using Camera::operator=;

		virtual const Location& GetLocation() const = 0;
	};

	/// \brief Collection to store cameras.
	struct CameraCollection :	Collection<CameraCollection,Camera>/*,
								virtual TriggerShapeCollection*/
	{
		/// \brief Makes a CameraCollection object.
		static dclspc std::unique_ptr<CameraCollection> Make() noexcept;


		virtual void UserCameraType( CameraType type ) = 0;

		virtual CameraType UserCameraType() const = 0;

		virtual void UserStartCameraID( IDType id ) = 0;

		virtual IDType UserStartCameraID() const = 0;

		//virtual void Update( const Simulator& simulator ) = 0;

		//using TriggerShapeCollection::Update;
	};

	struct Fleet;
	struct Train;


	/// \returns A list of Trains that are in view of a Camera.
	//dclspc std::vector<std::shared_ptr<Train>> GetVisibleTrains( const Fleet& fleet, const Camera& camera );
}