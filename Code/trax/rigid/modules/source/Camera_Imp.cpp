//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2016 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



#include "Camera_Imp.h"

#include "trax/rigid/modules/Module.h"

#include "trax/rigid/Body.h"
#include "trax/rigid/Geom.h"
//#include "trax/Units.h"
//#include "trax/Module.h"
//#include "trax/Simulator.h"
//#include "trax/Fleet.h"
//#include "trax/Batch.h"
//#include "trax/Terrain.h"
//#include "trax/Train.h"

#include "spat/Matrix.h"
#include "spat/Vector2D.h"

#include <sstream>

namespace trax{

const Angle Frustum::default_fovy	= pi/4;
const One Frustum::default_aspect	= 4.f/3;
const Length Frustum::default_nearPlane = 1_m;
const Length Frustum::default_farPlane = 1_km;

///////////////////////////////////////
std::unique_ptr<Camera> Camera::Make( CameraType /*type*/ ) noexcept{
	try{
		return std::make_unique<Camera_Imp>();
	}
	catch( ... ){
		return nullptr;
	}
}
///////////////////////////////////////
const char* From( CameraType type ) noexcept{
	switch( type ){
	case CameraType::cam_basic:
		return "basic";
	case CameraType::cam_firstperson:
		return "firstperson";
	case CameraType::cam_orbiter:
		return "orbiter";
	case CameraType::cam_terrainconfined:
		return "terrainconfined";
	case CameraType::cam_dolly:
		return "dolly";
	case CameraType::cam_cabin:
		return "cabin";
	default:
		return "unknown";
	};
}

CameraType CameraTypeFrom( const std::string& name ){
	if( name == "basic" )
		return CameraType::cam_basic;
	else if( name == "firstperson" )
		return CameraType::cam_firstperson;
	else if( name == "orbiter" )
		return CameraType::cam_orbiter;
	else if( name == "terrainconfined" )
		return CameraType::cam_terrainconfined;
	else if( name == "dolly" )
		return CameraType::cam_dolly;
	else if( name == "cabin" )
		return CameraType::cam_cabin;

	std::ostringstream stream;
	stream << "Unknown camera type! (" << name << ")" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}
///////////////////////////////////////
Camera_Imp::Camera_Imp(){
	m_ProjectionMatrix.CreateCameraProjection( 
		default_fovy, 
		default_aspect, 
		default_nearPlane.Units(), 
		default_farPlane.Units() );

	m_ViewFrame.Init();
}

Camera_Imp::~Camera_Imp()
{
}

CameraType Camera_Imp::GetType() const{
	return CameraType::cam_basic;
}

const char* Camera_Imp::TypeName() const noexcept{
	return "Camera";
}

void Camera_Imp::SetFromFofy( Angle Fovy, One Aspect, Length zn, Length zf ){
	m_ProjectionMatrix.CreateCameraProjection( Fovy, Aspect, zn.Units(), zf.Units() );

	assert( common::Equals( Camera_Imp::Fovy(), Fovy, 0.0001_rad) );
	assert( common::Equals( Camera_Imp::AspectRatio(), Aspect, One{0.0001f} ) );
	assert( common::Equals( NearPlaneDistance(), zn, 0.1_mm ) );
	assert( common::Equals( FarPlaneDistance(), zf, 5_m ) );
}

void Camera_Imp::SetFromDimensions( Length width, Length height, Length zn, Length zf ){
	m_ProjectionMatrix.CreateCameraProjection( tan( width / (2*zn) ), width/height, zn.Units(), zf.Units() );
}

void Camera_Imp::PlaneDistances( const Interval<Length>& planes ){
	m_ProjectionMatrix.CreateCameraProjection( Fovy(), AspectRatio(), planes.Near().Units(), planes.Far().Units() );
}

Length Camera_Imp::NearPlaneDistance() const{
	return Length{m_ProjectionMatrix( 3, 2 ) / -m_ProjectionMatrix( 2, 2 )};
}

Length Camera_Imp::FarPlaneDistance() const{
	return Length{m_ProjectionMatrix( 3, 2 ) / (1 - m_ProjectionMatrix( 2, 2 ))};
}

void Camera_Imp::Width( Length width ){
	if( width > 0_m )
		m_ProjectionMatrix( 0, 0 ) = NearPlaneDistance() / (width/2);
}

Length Camera_Imp::Width() const{
	return Length{2 * m_ProjectionMatrix( 3, 2 ) / (-m_ProjectionMatrix( 2, 2 )*m_ProjectionMatrix( 0, 0 ))};
}

void Camera_Imp::Height( Length height ){
	if( height > 0_m )
		m_ProjectionMatrix( 1, 1 ) = NearPlaneDistance() / (height/2);
}

Length Camera_Imp::Height() const{
	return Length{2 * m_ProjectionMatrix( 3, 2 ) / (-m_ProjectionMatrix( 2, 2 )*m_ProjectionMatrix( 1, 1 ))};
}

void Camera_Imp::Fovy( Angle fovy ){
	if( fovy > 0 ){
		Real aspectRatio = AspectRatio();
		m_ProjectionMatrix( 0, 0 ) = 1.0f / tan( fovy / 2 );
		m_ProjectionMatrix( 1, 1 ) = m_ProjectionMatrix( 0, 0 ) * aspectRatio;
	}
}

Angle Camera_Imp::Fovy() const{
	return 2.0f * std::atan( 1.0f / m_ProjectionMatrix( 0, 0 ) );
}

void Camera_Imp::AspectRatio( One aspect ){
	Height(  Width() / aspect );
}

One Camera_Imp::AspectRatio() const{
	return m_ProjectionMatrix( 1, 1 ) / m_ProjectionMatrix( 0, 0 );
}

void Camera_Imp::ProjectionMatrix( const Transformation<Real>& projection ){
	m_ProjectionMatrix = projection;
}

const Transformation<Real>& Camera_Imp::ProjectionMatrix() const{
	return m_ProjectionMatrix;
}

Transformation<Real>& Camera_Imp::ProjectionMatrix(){
	return m_ProjectionMatrix;
}

Camera& Camera_Imp::operator=( const Camera& camera ){
	m_ProjectionMatrix = camera.ProjectionMatrix();
	m_ViewFrame = camera.GetFrame();
	return *this;
}

void Camera_Imp::SetFrame( const Frame<Length,One>& frame ){
	m_ViewFrame = frame;
	m_ViewFrame.OrthoNormalize();
}

const Frame<Length,One>& Camera_Imp::GetFrame() const{
	return m_ViewFrame;
}

Frame<Length,One>& Camera_Imp::GetFrame(){
	return m_ViewFrame;
}

Vector<Velocity> Camera_Imp::GetVelocity() const{
	return Null<Velocity>;
}

void Camera_Imp::EyePoint( const Position<Length>& eyePoint ){
	m_ViewFrame.P = eyePoint;
}

Position<Length> Camera_Imp::EyePoint() const{
	return m_ViewFrame.P;
}

void Camera_Imp::Up( const Vector<One>& up ){
	m_ViewFrame.LookBAt( up );
}

Vector<One> Camera_Imp::Up() const{
	return m_ViewFrame.B;
}

void Camera_Imp::LookAt( const Position<Length>& lookAtPoint ){
	m_ViewFrame.LookTAt( (lookAtPoint - m_ViewFrame.P) / Length{1} );
}

Position<Length> Camera_Imp::LookAt() const{
	return m_ViewFrame.P + 1_m * m_ViewFrame.T;
}

bool Camera_Imp::InFrustum( const Position<Length>& position ) const{
	Position<Length> cameraPt{ position };
	m_ViewFrame.FromParent( cameraPt );

	Position<Real> frustumPt{ cameraPt.y / Length{1}, cameraPt.z / Length{1}, cameraPt.x / Length{1} };
	frustumPt = m_ProjectionMatrix * frustumPt;

	return	-1 <= frustumPt.x && frustumPt.x <= +1 &&
			-1 <= frustumPt.y && frustumPt.y <= +1 &&
			 0 <= frustumPt.z && frustumPt.z <= +1;
}
///////////////////////////////////////
AnimatedCamera_Imp::AnimatedCamera_Imp( std::unique_ptr<Geom> pCollisionGeom ) noexcept
	:	m_pCollisionGeom{ std::move( pCollisionGeom ) }
{
	m_StepBackPose.Init();
	CalculateCollisionVolume();
}

Camera& AnimatedCamera_Imp::operator=( const Camera& camera ){
	Camera_Imp::operator=( camera );
	
	return *this;
}

void AnimatedCamera_Imp::AttachTo( std::shared_ptr<Module> pModule ){
	m_pModule = pModule;
}

std::shared_ptr<Module> AnimatedCamera_Imp::GetModule() const{
	return m_pModule;
}

Vector<Velocity> AnimatedCamera_Imp::GetVelocity() const{
	return (GetFrame().P - m_StepBackPose.P) / fixed_timestep;
}

std::vector<GeomList*> AnimatedCamera_Imp::OverlapQuery( unsigned int /*filterflags*/ ) const{
	//if( m_pModule )
	//	if( auto pSimulator = m_pModule->GetSimulator() )
	//		return pSimulator->OverlapQuery( *m_pCollisionGeom, filterflags );

	return {};
}

GeomList* AnimatedCamera_Imp::OverlapQueryAny( unsigned int /*filterflags*/ ) const{
	//if( m_pModule )
	//	if( auto pSimulator = m_pModule->GetSimulator() )
	//		return pSimulator->OverlapQueryAny( *m_pCollisionGeom, filterflags );

	return nullptr;
}

void AnimatedCamera_Imp::Update(){
//	if( m_pCollisionGeom )
//		m_pCollisionGeom->SetFrame( GetFrame() );
}

void AnimatedCamera_Imp::StepBack(){
	GetFrame() = m_StepBackPose;
}

Length AnimatedCamera_Imp::TerrainHeight() const{
//	if( auto pModule = GetModule() )
//		if( const auto* pTerrain = pModule->GetTerrain() )
//			return pTerrain->Height( Position2D<Length>{EyePoint()} );
//
	return 0_m;
}

void AnimatedCamera_Imp::PreUpdate(){
	m_StepBackPose = GetFrame();
}

void AnimatedCamera_Imp::CalculateCollisionVolume(){
//	if( m_pCollisionGeom == nullptr )
//		return;
//
//	dynamic_cast<GeomCuboid&>(*m_pCollisionGeom).SetFromBox( {
//			0_m,
//			-Width()/2,
//			-Height()/2,
//			NearPlaneDistance(),
//			Width()/2,
//			Height()/2 } );
}
///////////////////////////////////////
FirstPersonCamera::FirstPersonCamera( std::unique_ptr<Geom> pCollisionGeom )
	:	AnimatedCamera_Imp	{ std::move(pCollisionGeom) },
		m_dTranslationdt	{ Vector<Velocity>::specials::null },
		m_dRotationdt		{ Vector<AngularVelocity>::specials::null },
		m_DampingTranslation{ std::pow( 0.01f, _s(fixed_timestep) ) },
		m_DampingRotation	{ std::pow( 0.01f, _s(fixed_timestep) ) },
		m_MaximumVelocity	{ +infinite }
{
}

CameraType FirstPersonCamera::GetType() const{
	return CameraType::cam_firstperson;
}

void FirstPersonCamera::Update(){
	PreUpdate();

	ComputeLinearVelocity( m_dTranslationdt );

	m_dRotationdt *= m_DampingRotation;

	GetFrame().TransportTo( Origin3D<Length> + fixed_timestep * m_dTranslationdt );
	GetFrame().RotateTan( m_dRotationdt.dx * fixed_timestep );
	GetFrame().RotateNor( m_dRotationdt.dy * fixed_timestep );
	GetFrame().Rotate( Ez<One>, m_dRotationdt.dz * fixed_timestep );

	AnimatedCamera_Imp::Update();
}

void FirstPersonCamera::Stop(){
	m_dTranslationdt.Init( Vector<Velocity>::specials::null );
	m_dRotationdt.Init( Vector<AngularVelocity>::specials::null );
}

void FirstPersonCamera::Reset(){
	Stop();
	GetFrame().Init();
}

void FirstPersonCamera::MaximumLinearVelocity( Velocity maxVelocity ){
	m_MaximumVelocity = maxVelocity;
	if( m_MaximumVelocity < 0_mIs )
		m_MaximumVelocity = 0_mIs;
}

Velocity FirstPersonCamera::MaximumLinearVelocity() const{
	return m_MaximumVelocity;
}

void FirstPersonCamera::SetVelocity( const Vector<Velocity>& velocity ){
	m_dTranslationdt = velocity;
}

void FirstPersonCamera::SetVelocity( 
	const Vector<Velocity>& linearVelocity, 
	const Vector<AngularVelocity>& rotationalVelocity )
{
	m_dTranslationdt = linearVelocity;
	m_dRotationdt = rotationalVelocity;
}

void FirstPersonCamera::RotationalVelocity( const Vector<AngularVelocity>& rotvel ){
	m_dRotationdt = rotvel;
}

void FirstPersonCamera::ControllerTranslateX( Velocity dx ){
	m_dTranslationdt.dx += dx;
}

void FirstPersonCamera::ControllerTranslateY( Velocity dy ){
	m_dTranslationdt.dy += dy;
}

void FirstPersonCamera::ControllerTranslateZ( Velocity dz ){
	m_dTranslationdt.dz += dz;
}

void FirstPersonCamera::ControllerRotateX( AngularVelocity dx ){
	m_dRotationdt.dx += dx;
}

void FirstPersonCamera::ControllerRotateY( AngularVelocity dy ){
	m_dRotationdt.dy += Fovy() / AspectRatio() * dy;
}

void FirstPersonCamera::ControllerRotateZ( AngularVelocity dz ){
	m_dRotationdt.dz += Fovy() * dz;
}

void FirstPersonCamera::DampingTranslate( Real damping ){
	if( damping >= 0 )
		m_DampingTranslation = damping;
}

Real FirstPersonCamera::DampingTranslate() const{
	return m_DampingTranslation;
}

void FirstPersonCamera::DampingRotate( Real damping ){
	if( damping >= 0 )
		m_DampingRotation = damping;
}

Real FirstPersonCamera::DampingRotate() const{
	return m_DampingRotation;
}

void FirstPersonCamera::ComputeLinearVelocity( Vector<Velocity>& velocity ){
	if( velocity.Length() > m_MaximumVelocity )
		velocity *= m_MaximumVelocity / velocity.Length();

	velocity *= m_DampingTranslation;
}
///////////////////////////////////////
BodyOrbiter::BodyOrbiter( std::unique_ptr<Geom> pCollisionGeom )
	:	AnimatedCamera_Imp( std::move( pCollisionGeom ) ),
		m_rPhi				{ 0 },
		m_rTheta			{ pi/2 },
		m_rRadius			{ 25 },
		m_Phi				{ m_rPhi },
		m_Theta				{ m_rTheta },
		m_Radius			{ m_rRadius },
		m_dPhidt			{ 0 },
		m_dThetadt			{ 0 },
		m_dRadiusdt			{ 0 },
		m_PhiLimits			{ -infinite, infinite},
		m_ThetaLimits		{ 0.01f, 3.14f },
		m_RadiusLimits		{ epsilon__length, 50_m },
		m_DampingTranslation{ std::pow( 0.01f, _s(fixed_timestep) ) },
		m_DampingRotation	{ std::pow( 0.01f, _s(fixed_timestep) ) },
		m_MaximumVelocity	{ +infinite },
		m_bLoose			{ false },
		m_LoseLimitVelocity	{ trax::bolt / 4 }
{
	m_Center.Init();
}

CameraType BodyOrbiter::GetType() const{
	return CameraType::cam_orbiter;
}

Camera& BodyOrbiter::operator=( const Camera& camera ){
	AnimatedCamera_Imp::operator=( camera );

	if( auto pBodyCam = dynamic_cast<const BodyOrbiter*>(&camera) ){
		m_rPhi = pBodyCam->m_rPhi;
		m_rTheta = pBodyCam->m_rTheta;
		m_rRadius = pBodyCam->m_rRadius;
		m_Phi = pBodyCam->m_Phi;
		m_Theta = pBodyCam->m_Theta;
		m_Radius = pBodyCam->m_Radius;
		m_dPhidt = pBodyCam->m_dPhidt;
		m_dThetadt = pBodyCam->m_dThetadt;
		m_dRadiusdt = pBodyCam->m_dRadiusdt;

		m_PhiLimits = pBodyCam->m_PhiLimits;
		m_ThetaLimits = pBodyCam->m_ThetaLimits;
		m_RadiusLimits = pBodyCam->m_RadiusLimits;

		m_DampingTranslation = pBodyCam->m_DampingTranslation;
		m_DampingRotation = pBodyCam->m_DampingRotation;
		m_MaximumVelocity = pBodyCam->m_MaximumVelocity;

		m_pBody = pBodyCam->m_pBody;
	}
	else
		CalculateAnglesFromPoses();

	return *this;
}

void BodyOrbiter::SetFrame( const Frame<Length,One>& frame ){
	AnimatedCamera_Imp::SetFrame( frame );
	CalculateAnglesFromPoses();
}

void BodyOrbiter::Update(){
	PreUpdate();

	//if( m_pBody ){
	//	if( abs(m_dRadiusdt) > m_MaximumVelocity )
	//		m_dRadiusdt = Sign( m_dRadiusdt ) * m_MaximumVelocity;

	//	m_dPhidt	*= m_DampingRotation;
	//	m_dThetadt	*= m_DampingRotation;
	//	m_dRadiusdt *= m_DampingTranslation;

	//	if( EyePoint().z < TerrainHeight() + 1_m ){
	//		assert( m_Radius );
	//		m_dThetadt = -0.1_1Is - (1_m + TerrainHeight() - EyePoint().z) / (m_Radius*fixed_timestep);
	//	}

	//	m_Phi += m_dPhidt * fixed_timestep;
	//	if( m_Phi < m_PhiLimits.Near() ){
	//		m_Phi = m_PhiLimits.Near();
	//		m_dPhidt = -m_dPhidt / 2;
	//	}
	//	else if( m_Phi > m_PhiLimits.Far() ){
	//		m_Phi = m_PhiLimits.Far();
	//		m_dPhidt = -m_dPhidt / 2;
	//	}

	//	m_Theta += m_dThetadt * fixed_timestep;
	//	if( m_Theta < m_ThetaLimits.Near() ){
	//		m_Theta = m_ThetaLimits.Near();
	//		m_dThetadt = -m_dThetadt / 2;
	//	}
	//	else if( m_Theta > m_ThetaLimits.Far() ){
	//		m_Theta = m_ThetaLimits.Far();
	//		m_dThetadt = -m_dThetadt / 2;
	//	}

	//	m_Radius += m_dRadiusdt * fixed_timestep;
	//	if( m_Radius < m_RadiusLimits.Near() )
	//	{
	//		m_Radius = m_RadiusLimits.Near();
	//		m_dRadiusdt = -m_dRadiusdt / 2;
	//	}
	//	else if( m_Radius > m_RadiusLimits.Far() )
	//	{
	//		m_Radius = m_RadiusLimits.Far();
	//		m_dRadiusdt = -m_dRadiusdt / 2;

	//		Vector<Velocity> v;
	//		m_pBody->GetLinearVelocity( v );
	//		if( v.Length() <= m_LoseLimitVelocity  )
	//			m_bLoose = true;
	//	}
	//	else
	//		m_bLoose = false;

	//	Frame<Length,One> bodyFrame;
	//	m_pBody->GetFrame( bodyFrame );
	//	Frame<Length,One> center{ m_Center };
	//	bodyFrame.ToParent( center.P );

	//	using std::sin;
	//	using std::cos;
	//	GetFrame() = {
	//		{	center.P.x + m_Radius * cos( m_Phi ) * sin( m_Theta ),
	//			center.P.y + m_Radius * sin( m_Phi ) * sin( m_Theta ),
	//			center.P.z + m_Radius * cos( m_Theta ) },
	//		{	-cos( m_Phi ) * sin( m_Theta ),
	//			-sin( m_Phi ) * sin( m_Theta ),
	//			-cos( m_Theta ) },
	//		{	sin( m_Phi ),
	//			-cos( m_Phi ),
	//			0 },
	//		{	-cos( m_Phi ) * cos( m_Theta ),
	//			-sin( m_Phi ) * cos( m_Theta ),
	//			sin( m_Theta ) }
	//	};
	//}
	//else
	//	m_bLoose = true;

	AnimatedCamera_Imp::Update();
}

void BodyOrbiter::Stop(){
	m_dThetadt  = 0_1Is;
	m_dPhidt	= 0_1Is;
	m_dRadiusdt	= 0_mIs;
}

void BodyOrbiter::Reset(){
	Stop();
	m_Phi		= m_rPhi;
	m_Theta		= m_rTheta;
	m_Radius	= m_rRadius;
}

void BodyOrbiter::MaximumLinearVelocity( Velocity maxVelocity ){
	m_MaximumVelocity = maxVelocity;
	if( m_MaximumVelocity < 0_mIs )
		m_MaximumVelocity = 0_mIs;
}

Velocity BodyOrbiter::MaximumLinearVelocity() const{
	return m_MaximumVelocity;
}

void BodyOrbiter::SetVelocity( const Vector<Velocity>& velocity ){
	m_dRadiusdt = -velocity.dx;
	m_dPhidt = -Fovy() * AnglePerLength{One{0.1f} / trax::pi} * velocity.dy;
	m_dThetadt = -Fovy() / AspectRatio() * AnglePerLength{One{0.1f} / trax::pi} * velocity.dz;
}

void BodyOrbiter::SetVelocity( const Vector<Velocity>& linearVelocity, const Vector<AngularVelocity>& rotationalVelocity ){
	m_dRadiusdt = -linearVelocity.dx;
	m_dPhidt = rotationalVelocity.dz - Fovy() * AnglePerLength{One{0.1f} / trax::pi} * linearVelocity.dy;
	m_dThetadt = rotationalVelocity.dy - Fovy() / AspectRatio() * AnglePerLength{One{0.1f} / trax::pi} * linearVelocity.dz;
}

void BodyOrbiter::RotationalVelocity( const Vector<AngularVelocity>& rotvel ){
	m_dPhidt = rotvel.dz;
	m_dThetadt = rotvel.dy;
}

void BodyOrbiter::ControllerTranslateX( Velocity dx ){
	m_dRadiusdt -= dx;
}

void BodyOrbiter::ControllerTranslateY( Velocity dy ){
	m_dPhidt -= Fovy() / trax::pi * dy / 10_m;
}

void BodyOrbiter::ControllerTranslateZ( Velocity dz ){
	m_dThetadt -= Fovy() / AspectRatio() / trax::pi * dz / 10_m;
}

void BodyOrbiter::ControllerRotateX( AngularVelocity ){
}

void BodyOrbiter::ControllerRotateY( AngularVelocity dy ){
	m_dThetadt += Fovy() / AspectRatio() * dy;
}

void BodyOrbiter::ControllerRotateZ( AngularVelocity dz ){	
	m_dPhidt -= Fovy() * dz;
}

void BodyOrbiter::DampingTranslate( Real damping ){
	m_DampingTranslation = damping;
}

Real BodyOrbiter::DampingTranslate() const{
	return m_DampingTranslation;
}

void BodyOrbiter::DampingRotate( Real damping ){
	m_DampingRotation = damping;
}

Real BodyOrbiter::DampingRotate() const{
	return m_DampingRotation;
}

void BodyOrbiter::AttachTo( std::shared_ptr<Body> pBody ){
	m_pBody = pBody;
	CalculateAnglesFromPoses();
}

std::shared_ptr<Body> BodyOrbiter::GetBody() const{
	return m_pBody;
}

void BodyOrbiter::Center( const Frame<Length,One>& center ){
	m_Center = center;
	m_Center.OrthoNormalize();
	CalculateAnglesFromPoses();
}

const Frame<Length,One>& BodyOrbiter::Center() const{
	return m_Center;
}

bool BodyOrbiter::GetLoose() const{
	return m_bLoose;
}

bool BodyOrbiter::BodyRealtiveRotations() const{
	return false;
}

void BodyOrbiter::PhiLimits( const Interval<Angle>& limits ){
	m_PhiLimits = limits;
	m_PhiLimits.Normalize();
	m_PhiLimits.Clip( m_rPhi );
}

const Interval<Angle>& BodyOrbiter::PhiLimits() const{
	return m_PhiLimits;
}

void BodyOrbiter::PhiResetValue( Angle phi ){
	if( m_PhiLimits.Touches( phi ) )
		m_rPhi = phi;
	else
		throw std::invalid_argument( "Angle reset value phi out of limits." );
}

Angle BodyOrbiter::PhiResetValue() const{
	return m_rPhi;
}

void BodyOrbiter::Phi( Angle phi ){
	m_Phi = phi;
}

Angle BodyOrbiter::Phi() const{
	return m_Phi;
}

void BodyOrbiter::ThetaLimits( const Interval<Angle>& limits ){
	m_ThetaLimits = limits;
	m_ThetaLimits.Normalize();
	m_ThetaLimits.Clip( m_rTheta );
}

const Interval<Angle>& BodyOrbiter::ThetaLimits() const{
	return m_ThetaLimits;
}

void BodyOrbiter::ThetaResetValue( Angle theta ){
	if( m_ThetaLimits.Touches( theta ) )
		m_rTheta = theta;
	else
		throw std::invalid_argument( "Angle reset value theta out of limits." );
}

Angle BodyOrbiter::ThetaResetValue() const{
	return m_rTheta;
}

void BodyOrbiter::Theta( Angle theta ){
	m_Theta = theta;
}

Angle BodyOrbiter::Theta() const{
	return m_Theta;
}

void BodyOrbiter::RadiusLimits( const Interval<Length>& limits ){
	Interval<Length> _limits{ limits };
	_limits.Normalize();
	if( epsilon__length <= _limits.Near() ){
		m_RadiusLimits = _limits;
		m_RadiusLimits.Clip( m_rRadius );
	}
	else
		throw std::invalid_argument( "Radius limits can not be smaller zero." );
}

const Interval<Length>& BodyOrbiter::RadiusLimits() const{
	return m_RadiusLimits;
}

void BodyOrbiter::RadiusResetValue( Length radius ){
	if( m_RadiusLimits.Touches( radius ) )
		m_rRadius = radius;
	else
		throw std::invalid_argument( "Radius reset value out of limits." );
}

Length BodyOrbiter::RadiusResetValue() const{
	return m_rRadius;
}

void BodyOrbiter::Radius( Length radius ){
	m_Radius = radius;
}

Length BodyOrbiter::Radius() const{
	return m_Radius;
}

void BodyOrbiter::CalculateAnglesFromPoses(){
	if( m_pBody ){
		Frame<Length,One> bodyFrame;
		m_pBody->GetFrame( bodyFrame );
		Frame<Length,One> center{ m_Center };
		bodyFrame.ToParent( center.P );

		m_Radius	= (GetFrame().P - center.P).Length();
		m_Phi		= (GetFrame().N.dx < 0 ? -1.0f : 1.0f) * std::acos( -GetFrame().N.dy );
		m_Theta		= std::acos( -GetFrame().T.dz );
	}
}
///////////////////////////////////////
CabinCam::CabinCam() noexcept
	:	AnimatedCamera_Imp	{ nullptr },
		m_rPhi				{ 0 },
		m_rTheta			{ pi/2 },
		m_Phi				{ m_rPhi },
		m_Theta				{ m_rTheta },
		m_dPhidt			{ 0 },
		m_dThetadt			{ 0 },
		m_PhiLimits			{ -infinite, infinite },
		m_ThetaLimits		{ 0.01f, 3.14f },
		m_DampingRotation	{ std::pow( 0.01f, _s(fixed_timestep) ) },
		m_bLoose			{ false }
{
	m_Center.Init();
}

CameraType CabinCam::GetType() const{
	return CameraType::cam_cabin;
}

Camera& CabinCam::operator=( const Camera& camera ){
	AnimatedCamera_Imp::operator=( camera );

	if( auto pBodyCam = dynamic_cast<const CabinCam*>(&camera) ){
		m_rPhi = pBodyCam->m_rPhi;
		m_rTheta = pBodyCam->m_rTheta;
		m_Phi = pBodyCam->m_Phi;
		m_Theta = pBodyCam->m_Theta;
		m_dPhidt = pBodyCam->m_dPhidt;
		m_dThetadt = pBodyCam->m_dThetadt;

		m_PhiLimits = pBodyCam->m_PhiLimits;
		m_ThetaLimits = pBodyCam->m_ThetaLimits;

		m_DampingRotation = pBodyCam->m_DampingRotation;

		m_pBody = pBodyCam->m_pBody;
	}
	else
		CalculateAnglesFromPoses();

	return *this;
}

void CabinCam::SetFrame( const Frame<Length,One>& frame ){
	AnimatedCamera_Imp::SetFrame( frame );
	CalculateAnglesFromPoses();
}

void CabinCam::Update(){
	PreUpdate();

	if( m_pBody )
	{
		m_dPhidt	*= m_DampingRotation;
		m_dThetadt	*= m_DampingRotation;

		m_Phi += m_dPhidt * fixed_timestep;
		if( m_Phi < m_PhiLimits.Near() ){
			m_Phi = m_PhiLimits.Near();
			m_dPhidt = -m_dPhidt / 2;
		}
		if( m_Phi > m_PhiLimits.Far() ){
			m_Phi = m_PhiLimits.Far();
			m_dPhidt = -m_dPhidt / 2;
		}

		m_Theta += m_dThetadt * fixed_timestep;
		if( m_Theta < m_ThetaLimits.Near() ){
			m_Theta = m_ThetaLimits.Near();
			m_dThetadt = -m_dThetadt / 2;
		}
		if( m_Theta > m_ThetaLimits.Far() ){
			m_Theta = m_ThetaLimits.Far();
			m_dThetadt = -m_dThetadt / 2;
		}

		Frame<Length,One> bodyFrame;
		m_pBody->GetFrame( bodyFrame );
		Frame<Length,One> center{ m_Center };
		bodyFrame.ToParent( center );

		using std::sin;
		using std::cos;
		GetFrame() = {
			{	0_m,
				0_m,
				0_m },
			{	cos( m_Phi ) * sin( m_Theta ),
				sin( m_Phi ) * sin( m_Theta ),
				cos( m_Theta ) },
			{	-sin( m_Phi ),
				cos( m_Phi ),
				0 },
			{	-cos( m_Phi ) * cos( m_Theta ),
				-sin( m_Phi ) * cos( m_Theta ),
				sin( m_Theta ) }
		};

		center.ToParent( GetFrame() );
	}

	AnimatedCamera_Imp::Update();
}

void CabinCam::Stop(){
	m_dThetadt  = 0_1Is;
	m_dPhidt	= 0_1Is;
}

void CabinCam::Reset(){
	Stop();
	m_Phi		= m_rPhi;
	m_Theta		= m_rTheta;
}

void CabinCam::MaximumLinearVelocity( Velocity ){
}

Velocity CabinCam::MaximumLinearVelocity() const{
	return 0_mIs;
}

void CabinCam::SetVelocity( const Vector<Velocity>& velocity ){
	if( velocity.dx < 0_mIs ) m_bLoose = true;
	m_dPhidt = Fovy() * AnglePerLength{One{0.1f} / trax::pi} * velocity.dy;
	m_dThetadt = -Fovy() / AspectRatio() * AnglePerLength{One{0.1f} / trax::pi} * velocity.dz;
}

void CabinCam::SetVelocity( const Vector<Velocity>& linearVelocity, const Vector<AngularVelocity>& rotationalVelocity ){
	if( linearVelocity.dx < 0_mIs || rotationalVelocity.dx ) m_bLoose = true;
	m_dPhidt = Fovy() * rotationalVelocity.dz + Fovy() * AnglePerLength{One{0.1f} / trax::pi} * linearVelocity.dy;
	m_dThetadt = Fovy() / AspectRatio() * rotationalVelocity.dy - Fovy() / AspectRatio() * AnglePerLength{One{0.1f} / trax::pi} * linearVelocity.dz;
}

void CabinCam::RotationalVelocity( const Vector<AngularVelocity>& rotvel ){
	if( rotvel.dx ) m_bLoose = true;
	m_dPhidt += Fovy() * rotvel.dz;
	m_dThetadt += Fovy() / AspectRatio() * rotvel.dy;
}

void CabinCam::ControllerTranslateX( Velocity dx ){
	if( dx < 0_mIs ) m_bLoose = true;
}

void CabinCam::ControllerTranslateY( Velocity dy ){
	m_dPhidt += Fovy() / trax::pi * dy / 10_m;
}

void CabinCam::ControllerTranslateZ( Velocity dz ){
	m_dThetadt -= Fovy() / AspectRatio() / trax::pi * dz / 10_m;
}

void CabinCam::ControllerRotateX( AngularVelocity dx ){
	if( dx ) m_bLoose = true;
}

void CabinCam::ControllerRotateY( AngularVelocity dy ){
	m_dThetadt += Fovy() / AspectRatio() * dy;
}

void CabinCam::ControllerRotateZ( AngularVelocity dz ){
	m_dPhidt += Fovy() * dz;
}

void CabinCam::DampingTranslate( Real ){
}

Real CabinCam::DampingTranslate() const{
	return 0;
}

void CabinCam::DampingRotate( Real damping ){
	m_DampingRotation = damping;
}

Real CabinCam::DampingRotate() const{
	return m_DampingRotation;
}

void CabinCam::AttachTo( std::shared_ptr<Body> pBody ){
	m_pBody = pBody;
	CalculateAnglesFromPoses();
}

std::shared_ptr<Body> CabinCam::GetBody() const{
	return m_pBody;
}

void CabinCam::Center( const Frame<Length,One>& center ){
	m_Center = center;
	m_Center.OrthoNormalize();
	CalculateAnglesFromPoses();
}

const Frame<Length,One>& CabinCam::Center() const{
	return m_Center;
}

bool CabinCam::GetLoose() const{
	return m_bLoose;
}

bool CabinCam::BodyRealtiveRotations() const{
	return true;
}

void CabinCam::PhiLimits( const Interval<Angle>& limits ){
	m_PhiLimits = limits;
	m_PhiLimits.Normalize();
	m_PhiLimits.Clip( m_rPhi );
}

const Interval<Angle>& CabinCam::PhiLimits() const{
	return m_PhiLimits;
}

void CabinCam::PhiResetValue( Angle phi ){
	if( m_PhiLimits.Touches( phi ) )
		m_rPhi = phi;
	else
		throw std::invalid_argument( "Angle reset value phi out of limits." );
}

Angle CabinCam::PhiResetValue() const{
	return m_rPhi;
}

void CabinCam::Phi( Angle phi ){
	m_Phi = phi;
}

Angle CabinCam::Phi() const{
	return m_Phi;
}

void CabinCam::ThetaLimits( const Interval<Angle>& limits ){
	m_ThetaLimits = limits;
	m_ThetaLimits.Normalize();
	m_ThetaLimits.Clip( m_rTheta );
}

const Interval<Angle>& CabinCam::ThetaLimits() const{
	return m_ThetaLimits;
}

void CabinCam::ThetaResetValue( Angle theta ){
	if( m_ThetaLimits.Touches( theta ) )
		m_rTheta = theta;
	else
		throw std::invalid_argument( "Angle reset value theta out of limits." );
}

Angle CabinCam::ThetaResetValue() const{
	return m_rTheta;
}

void CabinCam::Theta( Angle theta ){
	m_Theta = theta;
}

Angle CabinCam::Theta() const{
	return m_Theta;
}

void CabinCam::RadiusLimits( const Interval<Length>& ){
}

const Interval<Length>& CabinCam::RadiusLimits() const{
	static Interval<Length> limits{0_m,0_m};
	return limits;
}

void CabinCam::RadiusResetValue( Length ){
}

Length CabinCam::RadiusResetValue() const{
	return 0_m;
}

void CabinCam::Radius( Length ){
}

Length CabinCam::Radius() const{
	return 0_m;
}

void CabinCam::CalculateAnglesFromPoses(){
	if( m_pBody ){
		Frame<Length,One> bodyFrame;
		m_pBody->GetFrame( bodyFrame );
		Frame<Length,One> center{ m_Center };
		bodyFrame.ToParent( center );

		m_Phi		= (GetFrame().T * center.N < 0 ? -1.0f : +1.0f) * std::acos( GetFrame().N * center.N );
		m_Theta		= std::acos( GetFrame().T * center.B );
	}
}
///////////////////////////////////////
TerrainConfinedCamera::TerrainConfinedCamera( std::unique_ptr<Geom> pCollisionGeom )
	:	FirstPersonCamera	{ std::move( pCollisionGeom ) },
		m_UprightHeight		{ 1.7f * units_per_meter },
		m_BodyHeight		{ 1.7f * units_per_meter }
{}

CameraType TerrainConfinedCamera::GetType() const{
	return CameraType::cam_terrainconfined;
}

void TerrainConfinedCamera::SetVelocity( const Vector<Velocity>& velocity ){
	m_dTranslationdt.dx = velocity.dx;
	m_dTranslationdt.dy = velocity.dy;
}

void TerrainConfinedCamera::SetVelocity( const Vector<Velocity>& linearVelocity, const Vector<AngularVelocity>& rotationalVelocity ){
	m_dTranslationdt.dx = linearVelocity.dx;
	m_dTranslationdt.dy = linearVelocity.dy;
	m_dRotationdt = rotationalVelocity;
}

void TerrainConfinedCamera::ControllerTranslateX( Velocity dx ){
	Vector<Velocity> dV = dx * (GetFrame().T - GetFrame().T.dz * Ez<One>);
	GetFrame().FromParent( dV );

	m_dTranslationdt += dV;
}

void TerrainConfinedCamera::ControllerTranslateY( Velocity dy ){
	Vector<Velocity> dV = dy * (GetFrame().N - GetFrame().N.dz * Ez<One>);
	GetFrame().FromParent( dV );

	m_dTranslationdt += dV;
}

void TerrainConfinedCamera::ControllerTranslateZ( Velocity dz ){
	if( dz > 0_mIs && GetFrame().P.z - TerrainHeight() < m_BodyHeight + epsilon__length ) 
		// jump:
	{
		m_dTranslationdt.dz = 5_mIs;
		GetFrame().P.z += 10.0f * epsilon__length;
	}
	else if( dz < 0_mIs )
		// crouch:
		m_BodyHeight = m_UprightHeight / 2;
}

void TerrainConfinedCamera::ComputeLinearVelocity( Vector<Velocity>& velocity )
{
	GetFrame().ToParent( velocity );

	Vector2D<Velocity> verticalVelocity{ velocity };
	if( verticalVelocity.Length() > MaximumLinearVelocity() ){
		verticalVelocity *= MaximumLinearVelocity() / verticalVelocity.Length();
		velocity = verticalVelocity;
	}

	velocity.dx *= DampingTranslate();
	velocity.dy *= DampingTranslate();

	const Length height = TerrainHeight();
	if( GetFrame().P.z - height > m_BodyHeight + epsilon__length )
		velocity.dz += g * fixed_timestep;
	else{
		velocity.dz = 0_mIs;
		GetFrame().P.z = height + m_BodyHeight;
	}

	m_BodyHeight = m_UprightHeight;
	GetFrame().FromParent( velocity );
}
///////////////////////////////////////
std::unique_ptr<CameraCollection> CameraCollection::Make() noexcept{
	try{
		return std::make_unique<CameraCollection_Imp>();
	}
	catch( ... ){
		return nullptr;
	}
}

CameraCollection_Imp::CameraCollection_Imp()
	: CameraCollection_Base	{}
	, m_UserCameraType		{ CameraType::cam_firstperson }
	, m_UserStartCameraID	{ 1 }
{
}

const char* CameraCollection_Imp::TypeName() const noexcept{
	return "CameraCollection";
}

void CameraCollection_Imp::UserCameraType( CameraType type ){
	m_UserCameraType = type;
}

CameraType CameraCollection_Imp::UserCameraType() const{
	return m_UserCameraType;
}

void CameraCollection_Imp::UserStartCameraID( IDType id ){
	if( Get( id ) == nullptr ){
		std::ostringstream stream;
		stream << "Unknown camera id! (" << id << ")" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::out_of_range( stream.str() );
	}

	m_UserStartCameraID = id;
}

IDType CameraCollection_Imp::UserStartCameraID() const{
	return m_UserStartCameraID;
}
//
//void CameraCollection_Imp::Update( const Simulator& simulator ){
//	if( auto pCamera = Get( UserStartCameraID() ) )
//		Update( simulator, pCamera->GetFrame() );
//}
///////////////////////////////////////
//std::vector<std::shared_ptr<Train>> GetVisibleTrains( const Fleet& fleet, const Camera& camera ){
//	std::vector<std::shared_ptr<Train>> retval;
//	for( auto pRailRunner = fleet.GetFirst(); pRailRunner; pRailRunner = fleet.GetNext( pRailRunner ) ){
//		if( auto pTrain = std::dynamic_pointer_cast<Train>(pRailRunner) ){
//			for( auto pBogie = pTrain->Get( 0 ); pBogie; pBogie = pTrain->GetNext( *pBogie ) ){
//				Frame<Length,One> frame;
//				pBogie->GetFrame( frame );
//				if( camera.InFrustum( frame.P ) ){
//					retval.push_back( pTrain );
//					break;
//				}
//			}
//		}
//	}
//
//	return retval;
//}
///////////////////////////////////////
}