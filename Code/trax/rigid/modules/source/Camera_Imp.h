//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2016 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "../Camera.h"
#include "trax/ImplementationHelper.h"
//#include "TriggerShape_Imp.h"

//#include "common/Interval.h"
#include "spat/Matrix.h"

#ifdef _WIN32
#	pragma warning( disable : 4250 ) // inherits via dominance warning
#endif

namespace trax{

	using namespace common;
	using namespace spat;

	struct Geom;

	using Camera_Base = ObjectID_Imp<Camera>;

	class Camera_Imp : public Camera_Base{
	public:
		Camera_Imp();
		Camera_Imp( const Camera_Imp& ) = delete;
		Camera_Imp( Camera_Imp&& ) = delete;
		~Camera_Imp();

		Camera_Imp& operator=( const Camera_Imp& ) = delete;
		Camera_Imp& operator=( Camera_Imp&& ) = delete;

		using Camera_Base::operator=;

		CameraType GetType() const override;

		const char* TypeName() const noexcept override;

		// Frustum:
		void SetFromFofy( Angle fovy = default_fovy, One aspect = default_aspect, Length zn = default_nearPlane, Length zf = default_farPlane ) override;

		void SetFromDimensions( Length width, Length height, Length zn = default_nearPlane, Length zf = default_farPlane ) override;

		void PlaneDistances( const Interval<Length>& planes ) override;

		Length NearPlaneDistance() const override;

		Length FarPlaneDistance() const override;

		void Width( Length width ) override;

		Length Width() const override;

		void Height( Length height ) override;

		Length Height() const override;

		void Fovy( Angle fovy ) override;

		Angle Fovy() const override;

		void AspectRatio( One aspect ) override;

		One AspectRatio() const override;

		void ProjectionMatrix( const Transformation<Real>& projection ) override;

		const Transformation<Real>& ProjectionMatrix() const override;

		Transformation<Real>& ProjectionMatrix() override;


		// Camera:
		Camera& operator=( const Camera& camera ) override;

		void SetFrame( const Frame<Length,One>& frame ) override;

		const Frame<Length,One>& GetFrame() const override;

		Frame<Length,One>& GetFrame() override;

		Vector<Velocity> GetVelocity() const override;

		void EyePoint( const Position<Length>& eyePoint ) override;

		Position<Length> EyePoint() const override;

		void Up( const Vector<One>& up ) override;

		Vector<One> Up() const override;

		void LookAt( const Position<Length>& lookAtPoint ) override;

		Position<Length> LookAt() const override;

		bool InFrustum( const Position<Length>& position ) const override;
	private:
		Transformation<Real>	m_ProjectionMatrix;
		Frame<Length,One>		m_ViewFrame;
	};


	class AnimatedCamera_Imp :	public virtual AnimatedCamera,
								public Camera_Imp{
	public:
		AnimatedCamera_Imp( std::unique_ptr<Geom> pCollisionGeom ) noexcept;

		Camera& operator=( const Camera& camera ) override;

		using Camera_Imp::operator=;

		using AnimatedCamera::operator=;

		using AnimatedCamera::GetFrame;

		void AttachTo( std::shared_ptr<Module> pModule ) override;

		std::shared_ptr<Module> GetModule() const override;

		Vector<Velocity> GetVelocity() const override;

		void SetVelocity( const Vector<Velocity>& ) override{}

		void SetVelocity( const Vector<Velocity>&, const Vector<AngularVelocity>& ) override{}

		void RotationalVelocity( const Vector<AngularVelocity>& ) override{}

		std::vector<GeomList*> OverlapQuery( unsigned int filterflags ) const override;

		GeomList* OverlapQueryAny( unsigned int filterflags ) const override;

		void Update() override;

		void StepBack() override;
	protected:
		Length TerrainHeight() const;
		void PreUpdate();
	private:
		std::shared_ptr<Module> m_pModule;
		std::unique_ptr<Geom> m_pCollisionGeom;
		Frame<Length,One> m_StepBackPose;

		void CalculateCollisionVolume();
	};

	class FirstPersonCamera : public AnimatedCamera_Imp{
	public:
		FirstPersonCamera( std::unique_ptr<Geom> pCollisionGeom );

		using AnimatedCamera_Imp::operator=;

		CameraType GetType() const override;

		void Update() override;

		void Stop() override;

		void Reset() override;

		void MaximumLinearVelocity( Velocity maxVelocity ) override;

		Velocity MaximumLinearVelocity() const override;
		
		void SetVelocity( const Vector<Velocity>& velocity ) override;

		void SetVelocity( const Vector<Velocity>& linearVelocity, const Vector<AngularVelocity>& rotationalVelocity ) override;

		void RotationalVelocity( const Vector<AngularVelocity>& rotvel ) override;

		void ControllerTranslateX( Velocity dx ) override;

		void ControllerTranslateY( Velocity dy ) override;

		void ControllerTranslateZ( Velocity dz ) override;

		void ControllerRotateX( AngularVelocity dx ) override;

		void ControllerRotateY( AngularVelocity dy ) override;

		void ControllerRotateZ( AngularVelocity dz ) override;

		void DampingTranslate( Real damping ) override;

		Real DampingTranslate() const override;

		void DampingRotate( Real damping ) override;

		Real DampingRotate() const override;

	protected:
		virtual void ComputeLinearVelocity( Vector<Velocity>& velocity );

		Vector<Velocity> m_dTranslationdt;
		Vector<AngularVelocity> m_dRotationdt;

	private:
		Real		m_DampingTranslation;
		Real		m_DampingRotation;
		Velocity	m_MaximumVelocity;
		//bool		m_bViewerRelativeTranslation;
	};

	class BodyOrbiter : public AnimatedCamera_Imp,
						public BodyCam{
	public:
		BodyOrbiter( std::unique_ptr<Geom> pCollisionGeom );

		CameraType GetType() const override;

		Camera& operator=( const Camera& camera ) override;

		void SetFrame( const Frame<Length,One>& frame ) override;

		void Update() override;

		void Stop() override;

		void Reset() override;

		void MaximumLinearVelocity( Velocity maxVelocity ) override;

		Velocity MaximumLinearVelocity() const override;

		void SetVelocity( const Vector<Velocity>& velocity ) override;

		void SetVelocity( const Vector<Velocity>& linearVelocity, const Vector<AngularVelocity>& rotationalVelocity ) override;

		void RotationalVelocity( const Vector<AngularVelocity>& rotvel ) override;

		void ControllerTranslateX( Velocity dx ) override;

		void ControllerTranslateY( Velocity dy ) override;

		void ControllerTranslateZ( Velocity dz ) override;

		void ControllerRotateX( AngularVelocity dx ) override;

		void ControllerRotateY( AngularVelocity dy ) override;

		void ControllerRotateZ( AngularVelocity dz ) override;

		void DampingTranslate( Real damping ) override;

		Real DampingTranslate() const override;

		void DampingRotate( Real damping ) override;

		Real DampingRotate() const override;

		using AnimatedCamera_Imp::AttachTo;

		void AttachTo( std::shared_ptr<Body> pBody ) override;

		std::shared_ptr<Body> GetBody() const override;

		void Center( const Frame<Length,One>& center ) override;

		const Frame<Length,One>& Center() const override;

		bool GetLoose() const override;

		bool BodyRealtiveRotations() const override;

		void PhiLimits( const Interval<Angle>& limits ) override;

		const Interval<Angle>& PhiLimits() const override;

		void PhiResetValue( Angle phi ) override;

		Angle PhiResetValue() const override;

		void Phi( Angle phi ) override;

		Angle Phi() const override;


		void ThetaLimits( const Interval<Angle>& limits ) override;

		const Interval<Angle>& ThetaLimits() const override;

		void ThetaResetValue( Angle theta ) override;

		Angle ThetaResetValue() const override;

		void Theta( Angle theta ) override;

		Angle Theta() const override;


		void RadiusLimits( const Interval<Length>& limits ) override;

		const Interval<Length>& RadiusLimits() const override;

		void RadiusResetValue( Length radius ) override;

		Length RadiusResetValue() const override;

		void Radius( Length radius ) override;

		Length Radius() const override;
	private:
		Angle m_rPhi;
		Angle m_rTheta;
		Length m_rRadius;
		Angle m_Phi;
		Angle m_Theta;
		Length m_Radius;
		AngularVelocity m_dPhidt;
		AngularVelocity m_dThetadt;
		Velocity m_dRadiusdt;
		Interval<Angle> m_PhiLimits;
		Interval<Angle> m_ThetaLimits;
		Interval<Length> m_RadiusLimits;

		Real m_DampingTranslation;
		Real m_DampingRotation;
		Velocity m_MaximumVelocity;

		std::shared_ptr<Body> m_pBody;
		Frame<Length,One> m_Center;
		bool m_bLoose;
		Velocity m_LoseLimitVelocity;

		void CalculateAnglesFromPoses();
	};

	class CabinCam :	public AnimatedCamera_Imp,
						public BodyCam{
	public:
		CabinCam() noexcept;

		CameraType GetType() const override;

		Camera& operator=( const Camera& camera ) override;

		void SetFrame( const Frame<Length,One>& frame ) override;

		void Update() override;

		void Stop() override;

		void Reset() override;

		void MaximumLinearVelocity( Velocity maxVelocity ) override;

		Velocity MaximumLinearVelocity() const override;

		void SetVelocity( const Vector<Velocity>& velocity ) override;

		void SetVelocity( const Vector<Velocity>& linearVelocity, const Vector<AngularVelocity>& rotationalVelocity ) override;

		void RotationalVelocity( const Vector<AngularVelocity>& rotvel ) override;

		void ControllerTranslateX( Velocity dx ) override;

		void ControllerTranslateY( Velocity dy ) override;

		void ControllerTranslateZ( Velocity dz ) override;

		void ControllerRotateX( AngularVelocity dx ) override;

		void ControllerRotateY( AngularVelocity dy ) override;

		void ControllerRotateZ( AngularVelocity dz ) override;

		void DampingTranslate( Real damping ) override;

		Real DampingTranslate() const override;

		void DampingRotate( Real damping ) override;

		Real DampingRotate() const override;

		using AnimatedCamera_Imp::AttachTo;

		void AttachTo( std::shared_ptr<Body> pBody ) override;

		std::shared_ptr<Body> GetBody() const override;

		void Center( const Frame<Length,One>& center ) override;

		const Frame<Length,One>& Center() const override;

		bool GetLoose() const override;

		bool BodyRealtiveRotations() const override;

		void PhiLimits( const Interval<Angle>& limits ) override;

		const Interval<Angle>& PhiLimits() const override;

		void PhiResetValue( Angle phi ) override;

		Angle PhiResetValue() const override;

		void Phi( Angle phi ) override;

		Angle Phi() const override;


		void ThetaLimits( const Interval<Angle>& limits ) override;

		const Interval<Angle>& ThetaLimits() const override;

		void ThetaResetValue( Angle theta ) override;

		Angle ThetaResetValue() const override;

		void Theta( Angle theta ) override;

		Angle Theta() const override;


		void RadiusLimits( const Interval<Length>& limits ) override;

		const Interval<Length>& RadiusLimits() const override;

		void RadiusResetValue( Length radius ) override;

		Length RadiusResetValue() const override;

		void Radius( Length radius ) override;

		Length Radius() const override;
	private:
		Angle m_rPhi;
		Angle m_rTheta;
		Angle m_Phi;
		Angle m_Theta;
		AngularVelocity m_dPhidt;
		AngularVelocity m_dThetadt;
		Interval<Angle> m_PhiLimits;
		Interval<Angle> m_ThetaLimits;

		Real m_DampingRotation;

		std::shared_ptr<Body> m_pBody;
		Frame<Length,One> m_Center;
		bool m_bLoose;

		void CalculateAnglesFromPoses();
	};


	class TerrainConfinedCamera : public FirstPersonCamera{
	public:
		TerrainConfinedCamera( std::unique_ptr<Geom> pCollisionGeom );

		using FirstPersonCamera::operator=;

		CameraType GetType() const override;

		void SetVelocity( const Vector<Velocity>& velocity ) override;

		void SetVelocity( const Vector<Velocity>& linearVelocity, const Vector<AngularVelocity>& rotationalVelocity ) override;

		void ControllerTranslateX( Velocity dx ) override;

		void ControllerTranslateY( Velocity dy ) override;

		void ControllerTranslateZ( Velocity dz ) override;

	protected:
		void ComputeLinearVelocity( Vector<Velocity>& velocity ) override;
	private:
		Length m_UprightHeight;
		Length m_BodyHeight;
	};


	using CameraCollection_Base = Container_Imp<Camera,CameraCollection>;

	class CameraCollection_Imp :	public CameraCollection_Base/*,
									public TriggerShapeCollection_Imp*/{
	public:
		CameraCollection_Imp();

		const char* TypeName() const noexcept override;

		void UserCameraType( CameraType type ) override;

		CameraType UserCameraType() const override;

		void UserStartCameraID( IDType id ) override;

		IDType UserStartCameraID() const override;

		//using TriggerShapeCollection_Imp::Update;

		//void Update( const Simulator& simulator ) override;
	private:
		CameraType m_UserCameraType;
		IDType m_UserStartCameraID;
	};



}