//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2015 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "PhysX_GeomSpecials_Imp.h"
#include "PhysX_Imp.h"
#include "PhysX_Scene.h"
#include "PhysX_Simulator.h"
#include "PhysX_Adapter.h"

#include "trax/rigid/GeomType.h"

#include "common/NarrowCast.h"

namespace trax{
	using namespace common;
///////////////////////////////////////
PhysX_Cuboid_Imp::PhysX_Cuboid_Imp( Real engineMetersPerUnit )
	: PhysX_GeomBase_Imp{ engineMetersPerUnit }
	, m_BoxGeometry		{ 0.5f, 0.5f, 0.5f }
	, m_Diagonal		{ 1_m, 1_m, 1_m }
{}

PhysX_Cuboid_Imp::PhysX_Cuboid_Imp( const Box<Length>& box, Real engineMetersPerUnit )
	:	PhysX_GeomBase_Imp( engineMetersPerUnit )
{
	SetFromBox( box );
}

const char*	PhysX_Cuboid_Imp::TypeName() const noexcept{
	return "GeomCuboid";
}

std::unique_ptr<Geom> PhysX_Cuboid_Imp::Instance() const noexcept{
	return std::make_unique<PhysX_Cuboid_Imp>( *this );
}

void PhysX_Cuboid_Imp::SetFrame( const Frame<Length,One>& frame ) noexcept
{
	PhysX_GeomBase_Imp::SetFrame( frame );
	Diagonal( m_Diagonal );
}

GeomType PhysX_Cuboid_Imp::GetGeomType() const noexcept{
	return GeomType::box;
}

spat::Box<Length> PhysX_Cuboid_Imp::GetExtent() const noexcept
{
	Frame<Length,One> frame;
	GetFrame( frame );
	spat::Box<Length> extent{ Diagonal() };
	return frame.ToParent( extent );
}

Volume PhysX_Cuboid_Imp::GetVolume() const noexcept{
	return m_Diagonal.dx * m_Diagonal.dy * m_Diagonal.dz;
}

void PhysX_Cuboid_Imp::Diagonal( const Vector<Length>& diagonal ){
	m_Diagonal = diagonal;

	m_BoxGeometry.halfExtents.x = static_cast<physx::PxReal>(_m(abs(m_Diagonal.dx)/2)/m_EngineMetersPerUnit);
	m_BoxGeometry.halfExtents.y = static_cast<physx::PxReal>(_m(abs(m_Diagonal.dy)/2)/m_EngineMetersPerUnit);
	m_BoxGeometry.halfExtents.z = static_cast<physx::PxReal>(_m(abs(m_Diagonal.dz)/2)/m_EngineMetersPerUnit);

	spat::Frame<Length,One> frame;
	GetFrame( frame );
	spat::Vector<One> scale = Normalize( frame ).first;
	m_BoxGeometry.halfExtents.x *= scale.dx;
	m_BoxGeometry.halfExtents.y *= scale.dy;
	m_BoxGeometry.halfExtents.z *= scale.dz;

	// <= 0 extent would cause an error ...
	if( m_BoxGeometry.halfExtents.x < epsilon )
		m_BoxGeometry.halfExtents.x = epsilon;
	if( m_BoxGeometry.halfExtents.y < epsilon )
		m_BoxGeometry.halfExtents.y = epsilon;
	if( m_BoxGeometry.halfExtents.z < epsilon )
		m_BoxGeometry.halfExtents.z = epsilon;

	AdjustShapeGeometry();
}

const Vector<Length>& PhysX_Cuboid_Imp::Diagonal() const{
	return m_Diagonal;
}

void PhysX_Cuboid_Imp::SetFromBox( const Box<Length>& box ){
	Diagonal( box.Diagonal() );

	Frame<Length,One> frame;
	frame.Init();
	frame.TransportTo( box.Center() );
	SetFrame( frame );

	AdjustShapeGeometry();
}
///////////////////////////////////////
PhysX_Capsule_Imp::PhysX_Capsule_Imp( Real engineMetersPerUnit )
	: PhysX_GeomBase_Imp{ engineMetersPerUnit }
	, m_CapsuleGeometry	{ 0.5f, 1 }
{
	PhysX_Capsule_Imp::SetFrame( Identity<Length,One> );
}

PhysX_Capsule_Imp::PhysX_Capsule_Imp( Length radius, Length length, Real engineMetersPerUnit )
	: PhysX_GeomBase_Imp{ engineMetersPerUnit }
	, m_CapsuleGeometry	{ 
			static_cast<physx::PxReal>(_m(radius)/engineMetersPerUnit), 
			static_cast<physx::PxReal>(_m(length/2)/engineMetersPerUnit) }
{
	PhysX_Capsule_Imp::SetFrame( Identity<Length,One> );
}

const char*	PhysX_Capsule_Imp::TypeName() const noexcept{
	return "GeomCapsule";
}

std::unique_ptr<Geom> PhysX_Capsule_Imp::Instance() const noexcept{
	return std::make_unique<PhysX_Capsule_Imp>( *this );
}

GeomType PhysX_Capsule_Imp::GetGeomType() const noexcept{
	return GeomType::capsule;
}

void PhysX_Capsule_Imp::SetFrame( const Frame<Length,One>& frame ) noexcept{
	assert( frame.IsOrthoNormal() );

	// PhysX capsules are aligned the x axis. Change axes, so we get
	// the capsule aligned z.
	PhysX_GeomBase_Imp::SetFrame( Frame<Length,One>(frame.P,frame.B,frame.N,-frame.T) );
}

void PhysX_Capsule_Imp::GetFrame( Frame<Length,One>& frame ) const noexcept{
	PhysX_GeomBase_Imp::GetFrame( frame );
	std::swap( frame.B, frame.T );
	frame.T *= -1;
}

spat::Box<Length> PhysX_Capsule_Imp::GetExtent() const noexcept
{
	Frame<Length,One> frame;
	GetFrame( frame );
	spat::Box<Length> extent{ 2*Radius(), 2*Radius(), GetLength()+2*Radius() };
	extent.Move( (GetLength()/2 +Radius())  * Ez<One> );
	return frame.ToParent( extent );
}

Volume PhysX_Capsule_Imp::GetVolume() const noexcept
{
	Length r = Radius();
	return pi*pow<2>(r) * (GetLength() + (4_1/3_1)*r);
}

void PhysX_Capsule_Imp::OnAttach( physx::PxRigidActor& actor ){
	PhysX_GeomBase_Imp::OnAttach(actor);

	SceneLockWrite lock{ actor.getScene() };
	Frame<Length,One> frame;
	GetFrame( frame );
	SetFrame( frame );
}

void PhysX_Capsule_Imp::Radius( Length radius ){
	assert( radius > 0_m );
	m_CapsuleGeometry.radius = static_cast<physx::PxReal>(_m(radius) / m_EngineMetersPerUnit);
	AdjustShapeGeometry();
}

Length PhysX_Capsule_Imp::Radius() const{
	return _m( static_cast<Real>(m_CapsuleGeometry.radius) * m_EngineMetersPerUnit );
}

void PhysX_Capsule_Imp::SetLength( Length length ){
	assert( length > 0_m );
	m_CapsuleGeometry.halfHeight = static_cast<physx::PxReal>(_m(length/2) / m_EngineMetersPerUnit);
	AdjustShapeGeometry();
}

Length PhysX_Capsule_Imp::GetLength() const{
	return _m(static_cast<Real>(2 * m_CapsuleGeometry.halfHeight) * m_EngineMetersPerUnit);
}
///////////////////////////////////////
PhysX_Sphere_Imp::PhysX_Sphere_Imp( Real engineMetersPerUnit) noexcept
	:	PhysX_GeomBase_Imp	{ engineMetersPerUnit }
	,	m_SphereGeometry	{ 0.5f }
{}

PhysX_Sphere_Imp::PhysX_Sphere_Imp( Length radius, Real engineMetersPerUnit )
	:	PhysX_GeomBase_Imp	{ engineMetersPerUnit }
	,	m_SphereGeometry	{ static_cast<physx::PxReal>(_m(radius)/engineMetersPerUnit) }
{}

const char*	PhysX_Sphere_Imp::TypeName() const noexcept{
	return "GeomSphere";
}

std::unique_ptr<Geom> PhysX_Sphere_Imp::Instance() const noexcept{
	return std::make_unique<PhysX_Sphere_Imp>( *this );
}

GeomType PhysX_Sphere_Imp::GetGeomType() const noexcept{
	return GeomType::sphere;
}

spat::Box<Length> PhysX_Sphere_Imp::GetExtent() const noexcept
{
	Frame<Length,One> frame;
	GetFrame( frame );
	spat::Box<Length> extent{ 2*Radius(), 2*Radius(), 2*Radius() };
	return frame.ToParent( extent );
}

Volume PhysX_Sphere_Imp::GetVolume() const noexcept
{
	return (4_1/3_1) * pi * pow<3>( Radius() );
}

void PhysX_Sphere_Imp::Radius( Length radius ){
	assert( radius > 0_m );
	m_SphereGeometry.radius = static_cast<physx::PxReal>(_m(radius)/m_EngineMetersPerUnit);
	AdjustShapeGeometry();
}

Length PhysX_Sphere_Imp::Radius() const{
	return _m(static_cast<Real>(m_SphereGeometry.radius)*m_EngineMetersPerUnit);
}
///////////////////////////////////////
PhysX_Cylinder_Imp::PhysX_Cylinder_Imp( const PhysX_Scene& scene )
	: PhysX_GeomBase_Imp{ scene.EngineMetersPerUnit() }
	, m_Scene			{ scene }
	, m_Radius			{ 0.5f }
	, m_Length			{ 1 }
	, m_CylinderGeometry{}
{}

PhysX_Cylinder_Imp::PhysX_Cylinder_Imp( const PhysX_Scene& scene, Length radius, Length length )
	: PhysX_GeomBase_Imp{ scene.EngineMetersPerUnit() }
	, m_Scene			{ scene }
	, m_Radius			{ radius }
	, m_Length			{ length }
	, m_CylinderGeometry{}
{
	assert( m_Radius > 0_m );
	assert( m_Length > 0_m );

	CookConvexMesh();
}

const char*	PhysX_Cylinder_Imp::TypeName() const noexcept{
	return "GeomCylinder";
}

std::unique_ptr<Geom> PhysX_Cylinder_Imp::Instance() const noexcept{
	// TODO: do real instancing...
	return std::make_unique<PhysX_Cylinder_Imp>( *this );
}

GeomType PhysX_Cylinder_Imp::GetGeomType() const noexcept{
	return GeomType::cylinder;
}

spat::Box<Length> PhysX_Cylinder_Imp::GetExtent() const noexcept
{
	Frame<Length,One> frame;
	GetFrame( frame );
	spat::Box<Length> extent{ 2*Radius(), 2*Radius(), GetLength()/2 };
	return frame.ToParent( extent );
}

Volume PhysX_Cylinder_Imp::GetVolume() const noexcept
{
	return pi * pow<2>( Radius() ) * GetLength();
}

void PhysX_Cylinder_Imp::Radius( Length radius ){
	if( radius > 0_m ){
		m_Radius = radius;
		CookConvexMesh();
		AdjustShapeGeometry();
	}
}

Length PhysX_Cylinder_Imp::Radius() const{
	return m_Radius;
}

void PhysX_Cylinder_Imp::SetLength( Length length ){
	if( length > 0_m ){
		m_Length = length;
		CookConvexMesh();
		AdjustShapeGeometry();
	}
}

Length PhysX_Cylinder_Imp::GetLength() const{
	return m_Length;
}

void PhysX_Cylinder_Imp::CookConvexMesh(){
	const physx::PxReal lh = static_cast<physx::PxReal>(_m(m_Length/2)/m_EngineMetersPerUnit);
	const physx::PxReal r = static_cast<physx::PxReal>(_m(m_Radius)/m_EngineMetersPerUnit);

	const physx::PxVec3 convexVerts[] ={
		physx::PxVec3( r*cos( 0*physx::PxPi/8 ), r*sin( 0*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 1*physx::PxPi/8 ), r*sin( 1*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 2*physx::PxPi/8 ), r*sin( 2*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 3*physx::PxPi/8 ), r*sin( 3*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 4*physx::PxPi/8 ), r*sin( 4*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 5*physx::PxPi/8 ), r*sin( 5*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 6*physx::PxPi/8 ), r*sin( 6*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 7*physx::PxPi/8 ), r*sin( 7*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 8*physx::PxPi/8 ), r*sin( 8*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 9*physx::PxPi/8 ), r*sin( 9*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 10*physx::PxPi/8 ), r*sin( 10*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 11*physx::PxPi/8 ), r*sin( 11*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 12*physx::PxPi/8 ), r*sin( 12*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 13*physx::PxPi/8 ), r*sin( 13*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 14*physx::PxPi/8 ), r*sin( 14*physx::PxPi/8 ), -lh ),
		physx::PxVec3( r*cos( 15*physx::PxPi/8 ), r*sin( 15*physx::PxPi/8 ), -lh ),

		physx::PxVec3( r*cos( 0*physx::PxPi/8 ), r*sin( 0*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 1*physx::PxPi/8 ), r*sin( 1*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 2*physx::PxPi/8 ), r*sin( 2*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 3*physx::PxPi/8 ), r*sin( 3*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 4*physx::PxPi/8 ), r*sin( 4*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 5*physx::PxPi/8 ), r*sin( 5*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 6*physx::PxPi/8 ), r*sin( 6*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 7*physx::PxPi/8 ), r*sin( 7*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 8*physx::PxPi/8 ), r*sin( 8*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 9*physx::PxPi/8 ), r*sin( 9*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 10*physx::PxPi/8 ), r*sin( 10*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 11*physx::PxPi/8 ), r*sin( 11*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 12*physx::PxPi/8 ), r*sin( 12*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 13*physx::PxPi/8 ), r*sin( 13*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 14*physx::PxPi/8 ), r*sin( 14*physx::PxPi/8 ), lh ),
		physx::PxVec3( r*cos( 15*physx::PxPi/8 ), r*sin( 15*physx::PxPi/8 ), lh ),
	};


	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count     = 32;
	convexDesc.points.stride    = sizeof( physx::PxVec3 );
	convexDesc.points.data      = convexVerts;
	convexDesc.flags            = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
#if (PX_PHYSICS_VERSION_MAJOR < 5)
	if( !m_Scene.Simulator().Cooking().cookConvexMesh( convexDesc, buf, &result ) )
		return;
#else
	if( !PxCookConvexMesh( physx::PxCookingParams{ physx::PxTolerancesScale{} }, convexDesc, buf, &result ) )
		return;
#endif

	physx::PxDefaultMemoryInputData input( buf.getData(), buf.getSize() );
	m_CylinderGeometry.convexMesh = m_Scene.Simulator().Physics().createConvexMesh( input );
}
///////////////////////////////////////
PhysX_HeightField::PhysX_HeightField( const PhysX_Scene& scene )
	: PhysX_GeomBase_Imp	{ scene.EngineMetersPerUnit() }
	, m_Scene				{ scene }
	, m_pHeightFieldActor	{ nullptr }
{
	PhysX_HeightField::SetFrame( Identity<Length,One> );
}

const char*	PhysX_HeightField::TypeName() const noexcept{
	return "HeightField";
}

std::unique_ptr<Geom> PhysX_HeightField::Instance() const noexcept{
	// TODO: do real instancing...
	return std::make_unique<PhysX_HeightField>( *this );
}

GeomType PhysX_HeightField::GetGeomType() const noexcept{
	return GeomType::heightField;
}

void PhysX_HeightField::SetFrame( const Frame<Length,One>& frame ) noexcept{
	m_Frame = frame;

	Frame<Length,One> Fup;
	Fup.Init();
	Fup.LookNAt( m_Scene.Up() );

	PhysX_GeomBase_Imp::SetFrame( m_Frame * Fup );
}

void PhysX_HeightField::GetFrame( Frame<Length,One>& frame ) const noexcept{
	frame = m_Frame;
}

spat::Box<Length> PhysX_HeightField::GetExtent() const noexcept
{
	return spat::Box<Length>();
}

Volume PhysX_HeightField::GetVolume() const noexcept
{
	return 0_m3;
}

bool PhysX_HeightField::Create( const short* pSamples, const bool* pbHoles, int nRows, int nCols, Real vertScale, Real horzScale )
{
	m_pHeightFieldActor = m_Scene.Simulator().Physics().createRigidDynamic(physx::PxTransform{physx::PxIdentity});
	if( m_pHeightFieldActor )
	{
		m_pHeightFieldActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

		const int nSamples = nRows*nCols;
		std::unique_ptr<physx::PxHeightFieldSample[]> hfsamples( new physx::PxHeightFieldSample[nSamples] );
		for( int i = 0; i < nSamples; ++i )
		{
			hfsamples[i].height = pSamples[i];
			//hfsamples[i].materialIndex0 = physx::PxBitAndByte(1); //physx::PxHeightFieldMaterial::eHOLE;
			//hfsamples[i].materialIndex1 = physx::PxBitAndByte(1);
			hfsamples[i].materialIndex0 = (pbHoles[i] && pbHoles[i+nCols] && pbHoles[i+1 + nCols]) ? physx::PxHeightFieldMaterial::eHOLE : physx::PxBitAndByte( 1 );
			hfsamples[i].materialIndex1 = (pbHoles[i] && pbHoles[i+1] && pbHoles[i+1 + nCols]) ? physx::PxHeightFieldMaterial::eHOLE : physx::PxBitAndByte( 1 );
			hfsamples[i].materialIndex0 = hfsamples[i].materialIndex1;
			hfsamples[i].setTessFlag();
		}	

		physx::PxHeightFieldDesc hfDesc;
		hfDesc.format             = physx::PxHeightFieldFormat::eS16_TM;
		hfDesc.nbRows             = nRows;
		hfDesc.nbColumns          = nCols;
//		hfDesc.thickness		  = -1 * units_per_meter;
		//hfDesc.convexEdgeThreshold= 3;
		//hfDesc.flags				= physx::PxHeightFieldFlag::eNO_BOUNDARY_EDGES;
		hfDesc.samples.data       = hfsamples.get();
		hfDesc.samples.stride     = sizeof(physx::PxHeightFieldSample);

#if (PX_PHYSICS_VERSION_MAJOR < 5)
		physx::PxHeightField* pHeightField = m_Scene.Simulator().Cooking().createHeightField( hfDesc, m_Scene.Simulator().Physics().getPhysicsInsertionCallback() );
#else
		physx::PxHeightField* pHeightField = PxCreateHeightField( hfDesc, m_Scene.Simulator().Physics().getPhysicsInsertionCallback() );
		//physx::PxHeightField* pHeightField = Scene( *m_pSimulator).getPhysics().createHeightField(hfDesc);
#endif

		m_HeightFieldGeometry = physx::PxHeightFieldGeometry(pHeightField, physx::PxMeshGeometryFlags(), static_cast<physx::PxReal>(vertScale), static_cast<physx::PxReal>(horzScale), static_cast<physx::PxReal>(horzScale) );
		
		m_Scene.Scene().addActor(*m_pHeightFieldActor);

		OnAttach( *m_pHeightFieldActor );

		return true;
	}

	return false;
}

bool PhysX_HeightField::CreateEEPStyle( const short* pSamples, const bool* pbHoles, int nRows, int nCols, Real vertScale, Real horzScale ){
	m_pHeightFieldActor = m_Scene.Simulator().Physics().createRigidDynamic(physx::PxTransform{physx::PxIdentity});
	if( m_pHeightFieldActor )
	{	
		m_pHeightFieldActor->setRigidBodyFlag( physx::PxRigidBodyFlag::eKINEMATIC, true);

		const int nSamples = nRows*nCols;
		std::unique_ptr<physx::PxHeightFieldSample[]> hfsamples( new physx::PxHeightFieldSample[nSamples] );
		for( int i = 0; i < nSamples; ++i )
		{
			hfsamples[i].height = pSamples[i];
			hfsamples[i].materialIndex0 = hfsamples[i].materialIndex1 
				= (pbHoles[i] && pbHoles[i+1] && pbHoles[i+1 + nCols]) ? physx::PxHeightFieldMaterial::eHOLE : physx::PxBitAndByte( 1 );
			hfsamples[i].setTessFlag();
		}

		physx::PxHeightFieldDesc hfDesc;
		hfDesc.format             = physx::PxHeightFieldFormat::eS16_TM;
		hfDesc.nbRows             = nRows;
		hfDesc.nbColumns          = nCols;
//		hfDesc.thickness		  = -1 * units_per_meter;
		hfDesc.samples.data       = hfsamples.get();
		hfDesc.samples.stride     = sizeof(physx::PxHeightFieldSample);

#if (PX_PHYSICS_VERSION_MAJOR < 5)
		physx::PxHeightField* pHeightField = m_Scene.Simulator().Cooking().createHeightField( hfDesc, m_Scene.Simulator().Physics().getPhysicsInsertionCallback() );
#else
		physx::PxHeightField* pHeightField = PxCreateHeightField( hfDesc, m_Scene.Simulator().Physics().getPhysicsInsertionCallback() );
		//physx::PxHeightField* pHeightField = Scene( *m_pSimulator).getPhysics().createHeightField(hfDesc);
#endif

		m_HeightFieldGeometry = physx::PxHeightFieldGeometry(pHeightField, physx::PxMeshGeometryFlags(), static_cast<physx::PxReal>(vertScale), static_cast<physx::PxReal>(horzScale), static_cast<physx::PxReal>(horzScale) );
		
		m_Scene.Scene().addActor(*m_pHeightFieldActor);

		OnAttach( *m_pHeightFieldActor );

		return true;
	}

	return false;
}

Length PhysX_HeightField::Height( const Position2D<Length>& parameter ) const{
	if( m_HeightFieldGeometry.heightField )
		return Length{ m_HeightFieldGeometry.heightScale * m_HeightFieldGeometry.heightField->getHeight(
			static_cast<physx::PxReal>(_m(parameter.x)/m_EngineMetersPerUnit) / m_HeightFieldGeometry.rowScale, 
			m_HeightFieldGeometry.heightField->getNbColumns() - static_cast<physx::PxReal>(_m(parameter.y)/m_EngineMetersPerUnit) / m_HeightFieldGeometry.columnScale - 2) };

	return 0_m;
}

void PhysX_HeightField::Get( const Position2D<Length>& parameter, Position<Length>& pos ) const{
	pos.x = parameter.x;
	pos.y = parameter.y;
	pos.z = Height( parameter );
}

void PhysX_HeightField::Get( const Position2D<Length>& /*parameter*/, Frame<Length,One>& /*frame*/ ) const{
	assert( !"Not implemented yet!" );
	//getTriangleNormal(PxTriangleID triangleIndex)
}
///////////////////////////////////////
PhysX_ConvexMesh::PhysX_ConvexMesh( const PhysX_Scene& scene )
	: PhysX_GeomBase_Imp	{ scene.EngineMetersPerUnit() }
	, m_Scene				{ scene }
	, m_ConvexMesGeometry	{}
{
}

const char*	PhysX_ConvexMesh::TypeName() const noexcept{
	return "ConvexMesh";
}

std::unique_ptr<Geom> PhysX_ConvexMesh::Instance() const noexcept{
	// TODO: do real instancing...
	return std::make_unique<PhysX_ConvexMesh>( *this );
}

void PhysX_ConvexMesh::SetFrame( const Frame<Length,One>& frame ) noexcept
{
	PhysX_GeomBase_Imp::SetFrame( frame );

	m_ConvexMesGeometry.scale.scale.x = frame.T.Length();
	m_ConvexMesGeometry.scale.scale.y = frame.N.Length();
	m_ConvexMesGeometry.scale.scale.z = frame.B.Length();

	AdjustShapeGeometry();
}

GeomType PhysX_ConvexMesh::GetGeomType() const noexcept{
	return GeomType::convexMesh;
}

spat::Box<Length> PhysX_ConvexMesh::GetExtent() const noexcept{
	return spat::Box<Length>{ 
		PosFrom<Length>( m_ConvexMesGeometry.convexMesh->getLocalBounds().minimum ),
		PosFrom<Length>( m_ConvexMesGeometry.convexMesh->getLocalBounds().maximum ) };
}

Volume PhysX_ConvexMesh::GetVolume() const noexcept
{
	assert( !"PhysX_ConvexMesh::GetVolume(): Not implemented yet!" );
	std::cout << Verbosity::verbose << "PhysX_ConvexMesh::GetVolume(): Not implemented yet!" << std::endl;
	return 0_m3;
}

bool PhysX_ConvexMesh::Create( const std::vector<Position<Length>>& points ){
	if( points.size() > 2 ){
		if( CookConvexMesh( points ) ){
			AdjustShapeGeometry();
			return true;
		}
	}

	return false;
}

bool PhysX_ConvexMesh::Create( const std::vector<Position<Length>>& points, const std::vector<int>& indices ){
	if( points.size() > 2 ){
		if( indices.size() > 2 ){
			if( CookConvexMesh( points, indices ) ){
				AdjustShapeGeometry();
				return true;
			}
		}
		else
			return Create( points );
	}

	return false;
}

bool PhysX_ConvexMesh::CookConvexMesh( 
	const std::vector<Position<Length>>& points )
{
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count     = narrow_cast<physx::PxU32>(points.size());
	convexDesc.points.stride    = sizeof(Position<Length>);
	convexDesc.points.data      = points.data();
	convexDesc.flags            = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	//convexDesc.vertexLimit    = points.size();

	physx::PxDefaultMemoryOutputStream buf;
#if (PX_PHYSICS_VERSION_MAJOR < 5)
	if(!m_Scene.Simulator().Cooking().cookConvexMesh( convexDesc, buf ))
		return false;
#else
	if(!PxCookConvexMesh( physx::PxCookingParams{ physx::PxTolerancesScale{} }, convexDesc, buf ))
		return false;
#endif

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	m_ConvexMesGeometry.convexMesh = m_Scene.Simulator().Physics().createConvexMesh(input);
	return true;
}

bool PhysX_ConvexMesh::CookConvexMesh( 
	const std::vector<Position<Length>>& points, 
	const std::vector<int>& indices )
	// not behaving very well in this state ....
{
	std::vector<physx::PxHullPolygon> hullPolygons;
	hullPolygons.resize( indices.size() / 3u );
	for( size_t i = 0; i < hullPolygons.size(); ++i )
	{
		hullPolygons[i].mIndexBase = common::narrow_cast<physx::PxU16>(i*3u);
		hullPolygons[i].mNbVerts = 3u;

		const Position<Length>& p1 = points.at( indices.at(i*3u) );
		const Position<Length>& p2 = points.at( indices.at(i*3u + 1) );
		const Position<Length>& p3 = points.at( indices.at(i*3u + 2) );

		Vector<One> n = Normalize((p2-p1) % (p3-p1)).second;

		hullPolygons[i].mPlane[0] = n.dx;
		hullPolygons[i].mPlane[1] = n.dy;
		hullPolygons[i].mPlane[2] = n.dz;
		hullPolygons[i].mPlane[3] = -_m(n * (p1 - spat::Origin3D<Length>)) / m_EngineMetersPerUnit;
	}

	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count     = common::narrow_cast<physx::PxU32>(points.size());
	convexDesc.points.stride    = sizeof(Position<Length>);
	convexDesc.points.data      = points.data();
	convexDesc.polygons.count   = common::narrow_cast<physx::PxU32>(hullPolygons.size());
	convexDesc.polygons.stride  = sizeof(physx::PxHullPolygon);
	convexDesc.polygons.data    = hullPolygons.data();
	convexDesc.indices.count    = common::narrow_cast<physx::PxU32>(indices.size());
	convexDesc.indices.stride   = sizeof(int);
	convexDesc.indices.data     = indices.data();
	convexDesc.flags            = physx::PxConvexFlags{};

	physx::PxDefaultMemoryOutputStream buf;
#if (PX_PHYSICS_VERSION_MAJOR < 5)
	if( !m_Scene.Simulator().Cooking().cookConvexMesh( convexDesc, buf ) )
		return false;
#else
	if( !PxCookConvexMesh( physx::PxCookingParams{ physx::PxTolerancesScale{} }, convexDesc, buf ) )
		return false;
#endif

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	m_ConvexMesGeometry.convexMesh = m_Scene.Simulator().Physics().createConvexMesh(input);
	return true;
}
///////////////////////////////////////
PhysX_TriangleMesh::PhysX_TriangleMesh( const PhysX_Scene& scene )
	: PhysX_GeomBase_Imp	{ scene.EngineMetersPerUnit() }
	, m_Scene				{ scene }
	, m_TriangleMeshGeometry{}
{
}

const char*	PhysX_TriangleMesh::TypeName() const noexcept{
	return "TriangleMesh";
}

std::unique_ptr<Geom> PhysX_TriangleMesh::Instance() const noexcept{
	// TODO: do real instancing...
	return std::make_unique<PhysX_TriangleMesh>( *this );
}

GeomType PhysX_TriangleMesh::GetGeomType() const noexcept{
	return GeomType::triangleMesh;
}

spat::Box<Length> PhysX_TriangleMesh::GetExtent() const noexcept
{
	return spat::Box<Length>();
}

Volume PhysX_TriangleMesh::GetVolume() const noexcept
{
	assert( !"PhysX_TriangleMesh::GetVolume(): Not implemented yet!" );
	std::cout << Verbosity::verbose << "PhysX_TriangleMesh::GetVolume(): Not implemented yet!" << std::endl;
	return 0_m3;
}

bool PhysX_TriangleMesh::Create( const std::vector<Position<Length>>& /*points*/ ){
	throw std::runtime_error( "Invalid call!" );
}

bool PhysX_TriangleMesh::Create( const std::vector<Position<Length>>& points,const std::vector<int>& indices )
{
	assert( indices.size() % 3 == 0 );

	if( points.size() > 2 && indices.size() > 2 )
		return CookTriangleMeshStream( points, indices );

	return false;
}

bool PhysX_TriangleMesh::CookTriangleMeshStream( const std::vector<Position<Length>>& points, const std::vector<int>& indices )
{
	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count           = common::narrow_cast<physx::PxU32>(points.size());
	meshDesc.points.stride          = sizeof(Position<Length>);
	meshDesc.points.data            = points.data();

	meshDesc.triangles.count        = common::narrow_cast<physx::PxU32>(indices.size()) / 3;
	meshDesc.triangles.stride       = 3*sizeof(int);
	meshDesc.triangles.data         = indices.data();

	physx::PxDefaultMemoryOutputStream writeBuffer;
#if (PX_PHYSICS_VERSION_MAJOR < 5)
	if(!m_Scene.Simulator().Cooking().cookTriangleMesh( meshDesc, writeBuffer ))
		return false;
#else
	if(!PxCookTriangleMesh( physx::PxCookingParams{ physx::PxTolerancesScale{} }, meshDesc, writeBuffer ))
		return false;
#endif

	physx::PxDefaultMemoryInputData readBuffer( writeBuffer.getData(), writeBuffer.getSize() );
	m_TriangleMeshGeometry.triangleMesh = m_Scene.Simulator().Physics().createTriangleMesh(readBuffer);
	return true;
}

bool PhysX_TriangleMesh::CookTriangleMesh( const std::vector<Position<Length>>& points, const std::vector<int>& indices ){
	physx::PxTolerancesScale scale;
	physx::PxCookingParams params{scale};
	// disable mesh cleaning - perform mesh validation on development configurations
	params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	// disable edge precompute, edges are set for each triangle, slows contact generation
	params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	// lower hierarchy for internal mesh
//	params.meshCookingHint = physx::PxMeshCookingHint::eCOOKING_PERFORMANCE;
	params.midphaseDesc.mBVH33Desc.meshCookingHint = physx::PxMeshCookingHint::eCOOKING_PERFORMANCE;

	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count           = common::narrow_cast<physx::PxU32>(points.size());
	meshDesc.points.stride          = sizeof(Position<Length>);
	meshDesc.points.data            = points.data();

	meshDesc.triangles.count        = common::narrow_cast<physx::PxU32>(indices.size()) / 3;
	meshDesc.triangles.stride       = 3*sizeof(int);
	meshDesc.triangles.data         = indices.data();


#if (PX_PHYSICS_VERSION_MAJOR < 5)
	// mesh should be validated before cooked without the mesh cleaning
	assert( m_Scene.Simulator().Cooking().validateTriangleMesh(meshDesc) );
	m_TriangleMeshGeometry.triangleMesh = m_Scene.Simulator().Cooking().createTriangleMesh(meshDesc,
		m_Scene.Simulator().Physics().getPhysicsInsertionCallback());
#else
	// mesh should be validated before cooked without the mesh cleaning
	assert( PxValidateTriangleMesh( params, meshDesc ) );
	m_TriangleMeshGeometry.triangleMesh = PxCreateTriangleMesh( params, meshDesc, m_Scene.Simulator().Physics().getPhysicsInsertionCallback() );
#endif

	return true;
}
///////////////////////////////////////
}