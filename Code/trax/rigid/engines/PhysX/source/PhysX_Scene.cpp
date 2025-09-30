// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2025 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "PhysX_Scene.h"
#include "PhysX_Adapter.h"
#include "PhysX_GeomSpecials_Imp.h"
#include "PhysX_Gestalt_Imp.h"
#include "PhysX_Simulator.h"

#include "trax/rigid/Joint.h"
#include "trax/rigid/TrackJointFeeder.h"

namespace trax
{
///////////////////////////////////////
std::unique_ptr<Scene> Scene::Make( const Simulator& simulator ) noexcept
{
	try{
		if( const PhysX_Simulator* pPhysX_Simulator = dynamic_cast<const PhysX_Simulator*>(&simulator); pPhysX_Simulator )
			return std::make_unique<PhysX_Scene>( *pPhysX_Simulator );
		else{
			std::cerr << "Scene::Make: Simulator is of wrong type!" << std::endl;
		}
	}
	catch( const std::runtime_error& e ){
		std::cerr << e.what() << std::endl;
	}
	catch( const std::bad_alloc& e ){
		std::cerr << e.what() << std::endl;
	}
	catch( ... ){
		std::cerr << "An unknown error occurred." << std::endl;
	}

	return nullptr;
}
///////////////////////////////////////
PhysX_Scene::PhysX_Scene( const PhysX_Simulator& simulator, Vector<Acceleration> _G )
	: Scene_Imp					{}
	, m_Simulator				{ simulator }
	, m_Scene					{ InitScene( _G ) }
	, m_bSceneCreated			{ false }
	, m_EngineMetersPerUnit		{ meters_per_unit }
	, m_EngineKilogramsPerUnit	{ kilograms_per_unit }
	, m_CpuDispatcher			{ std::thread::hardware_concurrency() }
{
}

PhysX_Scene::PhysX_Scene( 
	const PhysX_Simulator& simulator, 
	Real engine_meters_per_unit, 
	Real engine_kilograms_per_unit, 
	Vector<Acceleration> _G )
	: Scene_Imp					{}
	, m_Simulator				{ simulator }
	, m_Scene					{ InitScene( _G ) }
	, m_bSceneCreated			{ false }
	, m_EngineMetersPerUnit		{ engine_meters_per_unit }
	, m_EngineKilogramsPerUnit	{ engine_kilograms_per_unit }
	, m_CpuDispatcher			{ std::thread::hardware_concurrency() }
{
}

PhysX_Scene::PhysX_Scene( 
	const PhysX_Simulator& simulator,
	physx::PxScene& scene, 
	Real engine_meters_per_unit, 
	Real engine_kilograms_per_unit )
	: Scene_Imp					{}
	, m_Simulator				{ simulator }
	, m_Scene					{ scene }
	, m_bSceneCreated			{ false }
	, m_EngineMetersPerUnit		{ engine_meters_per_unit }
	, m_EngineKilogramsPerUnit	{ engine_kilograms_per_unit }
	, m_CpuDispatcher			{ std::thread::hardware_concurrency() }
{
}

PhysX_Scene::~PhysX_Scene()
{
	UnregisterAllSimulated();

	m_TrackJoints.clear();

	if( m_bSceneCreated ){
		ExitScene();
	}
}

void PhysX_Scene::Up( const Vector<One>& up ){
	Gravity( Normalize( Gravity() ).first * up );
}

Vector<One> PhysX_Scene::Up() const{
	return Normalize( VecFrom<Acceleration>( Scene().getGravity() ) ).second;
}

void PhysX_Scene::Gravity( const spat::Vector<Acceleration>& gravityAccelerarion ){
	Scene().setGravity( From(gravityAccelerarion) );
}

Vector<Acceleration> PhysX_Scene::Gravity() const{
	return VecFrom<Acceleration>( Scene().getGravity() );
}

void PhysX_Scene::Update( Time dt ) noexcept
{
	Scene_Imp::Update( dt );

	for( const auto& jnf : m_TrackJoints )
		jnf->Update( dt );
}

void PhysX_Scene::StartStep( Time dt ) noexcept
{
	Scene().simulate(static_cast<physx::PxReal>(_s(dt)));
}

bool PhysX_Scene::EndStep() noexcept
{
	physx::PxU32 errorState = 0;

	if( !Scene().fetchResults(false,&errorState) ){
		if( errorState ){
			assert(errorState);
			return true; 
		}

		return false;
	}

	return true;
}

std::unique_ptr<Body> PhysX_Scene::CreateBody() const{
	return std::make_unique<PhysX_Body_Imp>( Scene(), m_EngineMetersPerUnit, m_EngineKilogramsPerUnit );
}

std::unique_ptr<Body> PhysX_Scene::CreateBody( physx::PxRigidDynamic& actor ) const{
	return std::make_unique<PhysX_Body_ImpBase>( actor, m_EngineMetersPerUnit, m_EngineKilogramsPerUnit );
}

std::unique_ptr<Shape> PhysX_Scene::CreateShape() const{
	return std::make_unique<PhysX_Shape_Imp>( Scene(), m_EngineMetersPerUnit );
}

std::unique_ptr<Shape> PhysX_Scene::CreateShape( physx::PxRigidStatic& actor ) const{
	return std::make_unique<PhysX_Shape_ImpBase>( actor, m_EngineMetersPerUnit );
}

std::unique_ptr<Gestalt> PhysX_Scene::CreateGestalt() const{
	return std::make_unique<PhysX_Gestalt_Imp>( Scene(), m_EngineMetersPerUnit, m_EngineKilogramsPerUnit );
}

std::unique_ptr<GeomCuboid> PhysX_Scene::CreateGeomCuboid() const{
	return std::make_unique<PhysX_Cuboid_Imp>( m_EngineMetersPerUnit );
}

std::unique_ptr<GeomCylinder> PhysX_Scene::CreateGeomCylinder() const{
	return std::make_unique<PhysX_Cylinder_Imp>( *this );
}

std::unique_ptr<GeomCapsule> PhysX_Scene::CreateGeomCapsule() const{
	return std::make_unique<PhysX_Capsule_Imp>( m_EngineMetersPerUnit );
}

std::unique_ptr<GeomSphere> PhysX_Scene::CreateGeomSphere() const{
	return std::make_unique<PhysX_Sphere_Imp>( m_EngineMetersPerUnit );
}

std::unique_ptr<GeomHeightField> PhysX_Scene::CreateGeomHeightField() const{
	return std::make_unique<PhysX_HeightField>( *this );
}

std::unique_ptr<GeomMesh> PhysX_Scene::CreateGeomConvexMesh() const{
	return std::make_unique<PhysX_ConvexMesh>( *this );
}

std::unique_ptr<GeomMesh> PhysX_Scene::CreateGeomTriangleMesh() const{
	return std::make_unique<PhysX_TriangleMesh>( *this );
}

std::unique_ptr<HingeJoint> PhysX_Scene::CreateHingeJoint( std::shared_ptr<Body> /*pBodyA*/, const spat::Frame<Length, One>& /*localAnchorA*/, std::shared_ptr<Body> /*pBodyB*/, const spat::Frame<Length, One>& /*localAnchorB*/ ) const
{
	std::cerr << "PhysX_Scene::CreateHingeJoint: This function is not implemented in PhysX_Scene yet." << std::endl;
	return nullptr;
}

std::unique_ptr<SliderJoint> PhysX_Scene::CreateSliderJoint( std::shared_ptr<Body> /*pBodyA*/, const spat::Frame<Length, One>& /*localAnchorA*/, std::shared_ptr<Body> /*pBodyB*/, const spat::Frame<Length, One>& /*localAnchorB*/ ) const
{
	std::cerr << "PhysX_Scene::CreateSliderJoint: This function is not implemented in PhysX_Scene yet." << std::endl;
	return nullptr;
}

std::unique_ptr<BallAndSocketJoint> PhysX_Scene::CreateBallAndSocketJoint( std::shared_ptr<Body> /*pBodyA*/, const spat::Frame<Length, One>& /*localAnchorA*/, std::shared_ptr<Body> /*pBodyB*/, const spat::Frame<Length,One>& /*localAnchorB*/ ) const
{
	std::cerr << "PhysX_Scene::CreateBallAndSocketJoint: This function is not implemented in PhysX_Scene yet." << std::endl;
	return nullptr;
}

std::unique_ptr<DistanceJoint> PhysX_Scene::CreateDistanceJoint( std::shared_ptr<Body> /*pBodyA*/, const spat::Position<Length>& /*localAnchorA*/, std::shared_ptr<Body> /*pBodyB*/, const spat::Position<Length>& /*localAnchorB*/ ) const
{
	std::cerr << "PhysX_Scene::CreateDistanceJoint: This function is not implemented in PhysX_Scene yet." << std::endl;
	return nullptr;
}

std::shared_ptr<TrackJointFeeder>
PhysX_Scene::CreateTrackJointFeeder( 
	const Body& bodyMoving, 
	const spat::Frame<Length,One>& localAnchor ) noexcept
{
	m_TrackJoints.push_back( std::make_shared<TrackJointFeeder_Imp<physx::PxReal,TrackJointFeeder>>( 
		*new PhysXTrackJoint{	m_Simulator.Physics(), 
								&dynamic_cast<const PhysX_Body_ImpBase&>(bodyMoving).Actor(), 
								PoseFrom( localAnchor ),
								nullptr },
		m_EngineMetersPerUnit, 
		m_EngineKilogramsPerUnit 
	) );

	return m_TrackJoints.back();	
}

std::shared_ptr<TrackJointFeeder> 
PhysX_Scene::CreateTrackJointFeeder( 
	const Body& bodyMoving, 
	const spat::Frame<Length,One>& localAnchor, 
	const Body& bodyTrack ) noexcept
{
	m_TrackJoints.push_back( std::make_shared<TrackJointFeeder_Imp<physx::PxReal,TrackJointFeeder>>( 
		*new PhysXTrackJoint{	m_Simulator.Physics(), 
								&dynamic_cast<const PhysX_Body_ImpBase&>(bodyMoving).Actor(), 
								PoseFrom( localAnchor ),
								&dynamic_cast<const PhysX_Body_ImpBase&>(bodyTrack).Actor() },
		m_EngineMetersPerUnit, 
		m_EngineKilogramsPerUnit 
	) );

	return m_TrackJoints.back();
}

std::shared_ptr<TrackJointFeederMotorModel> 
PhysX_Scene::CreateTrackJointFeederMotorModel( 
	const Body& bodyMoving, 
	const spat::Frame<Length,One>& localAnchor ) noexcept
{
	auto pFeeder = std::make_shared<TrackJointFeeder_Imp<physx::PxReal,TrackJointFeederMotorModel>>( 
		*new PhysXTrackJoint{	m_Simulator.Physics(), 
								&dynamic_cast<const PhysX_Body_ImpBase&>(bodyMoving).Actor(), 
								PoseFrom( localAnchor ),
								nullptr },
		m_EngineMetersPerUnit, 
		m_EngineKilogramsPerUnit 
	);

	m_TrackJoints.push_back( pFeeder );

	return pFeeder;	
}

std::shared_ptr<TrackJointFeederMotorModel> 
PhysX_Scene::CreateTrackJointFeederMotorModel( 
	const Body& bodyMoving, const spat::Frame<Length, One>& localAnchor, const Body& bodyTrack ) noexcept
{
	auto pFeeder = std::make_shared<TrackJointFeeder_Imp<physx::PxReal,TrackJointFeederMotorModel>>( 
		*new PhysXTrackJoint{	m_Simulator.Physics(), 
								&dynamic_cast<const PhysX_Body_ImpBase&>(bodyMoving).Actor(), 
								PoseFrom( localAnchor ),
								&dynamic_cast<const PhysX_Body_ImpBase&>(bodyTrack).Actor() },
		m_EngineMetersPerUnit, 
		m_EngineKilogramsPerUnit 
	);

	m_TrackJoints.push_back( pFeeder );

	return pFeeder;
}

void PhysX_Scene::Release( TrackJointFeeder& feeder ) noexcept
{
	if( auto fiter = std::find_if( 
		m_TrackJoints.begin(), 
		m_TrackJoints.end(), 
		[&feeder]( const JointNFeeder& entry )
		{ 
			return &feeder == entry.get();
		} ) ; fiter != m_TrackJoints.end() )
	{
		m_TrackJoints.erase( fiter );
	}
	else
	{
		std::cerr << "PhysX_Scene::Release: TrackJointFeeder not found!" << std::endl;
	}
}

void PhysX_Scene::onConstraintBreak( physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/ )
{
}

void PhysX_Scene::onWake( physx::PxActor** /*actors*/, physx::PxU32 /*count*/ )
{
	// TODO
}

void PhysX_Scene::onSleep( physx::PxActor** /*actors*/, physx::PxU32 /*count*/ )
{
}

void PhysX_Scene::onContact( const physx::PxContactPairHeader& /*pairHeader*/, const physx::PxContactPair* /*pairs*/, physx::PxU32 /*nbPairs*/ )
{
	//for(physx::PxU32 i=0; i < nbPairs; i++)	// NOLINT // Evaluate {$pairs,nbPairs}
	//{
	//	const physx::PxContactPair& cp = pairs[i];

	//	if(cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
	//	{
	//		std::unique_ptr<physx::PxContactPairPoint[]> pPointBuffer(new physx::PxContactPairPoint[cp.contactCount]);
	//		physx::PxU32 contactcount = cp.extractContacts(pPointBuffer.get(), cp.contactCount);

	//		PhysX_Bogie_Imp* pBogie0 = nullptr;
	//		PhysX_Bogie_Imp* pBogie1 = nullptr;
	//		Cargo* pCargo0 = nullptr;
	//		Cargo* pCargo1 = nullptr;

	//		if( cp.shapes[0]->getSimulationFilterData().word0 & (Geom::fBogie | Geom::fChildBogie | Geom::fWheelset) )
	//			pBogie0 = dynamic_cast<PhysX_Bogie_Imp*>(static_cast<GeomList*>(pairHeader.actors[0]->userData));

	//		else if( cp.shapes[0]->getSimulationFilterData().word0 & (Geom::fCargo) )
	//			pCargo0 = dynamic_cast<Cargo_Imp*>(static_cast<GeomList*>(pairHeader.actors[0]->userData));

	//		if( cp.shapes[1]->getSimulationFilterData().word0 & (Geom::fBogie | Geom::fChildBogie | Geom::fWheelset) )
	//			pBogie1 = dynamic_cast<PhysX_Bogie_Imp*>(static_cast<GeomList*>(pairHeader.actors[1]->userData));

	//		else if( cp.shapes[1]->getSimulationFilterData().word0 & (Geom::fCargo) )
	//			pCargo1 = dynamic_cast<Cargo_Imp*>(static_cast<GeomList*>(pairHeader.actors[1]->userData));
	//

	//		int filterCombination = cp.shapes[0]->getSimulationFilterData().word0 | cp.shapes[1]->getSimulationFilterData().word0;
	//		Materials materialCombination( cp.shapes[0]->getSimulationFilterData().word2, cp.shapes[1]->getSimulationFilterData().word2 );

	//		for( physx::PxU32 contact = 0; contact < contactcount; contact++){
	//			VectorBundle<Real> hitData( PosFrom<Real>(pPointBuffer[contact].position), 
	//										VecFrom<Real>(pPointBuffer[contact].impulse) );

	//			if( pBogie0 && pBogie0->GetFleet() )
	//			{
	//				assert( pBogie0->GetFleet() );
	//				pBogie0->GetFleet()->Hit( pBogie0, hitData, filterCombination, materialCombination );
	//			}
	//			else if( pCargo0 && pCargo0->GetBatch() )
	//				pCargo0->GetBatch()->Hit( pCargo0, hitData, filterCombination, materialCombination );

	//			hitData.T *= -1; // each object should get the impulse that it physically receives...
	//			materialCombination.Swap();

	//			if( pBogie1 && pBogie1->GetFleet() )
	//			{
	//				assert( pBogie1->GetFleet() );
	//				pBogie1->GetFleet()->Hit( pBogie1, hitData, filterCombination, materialCombination );
	//			}
	//			else if( pCargo1 && pCargo1->GetBatch() )
	//				pCargo1->GetBatch()->Hit( pCargo1, hitData, filterCombination, materialCombination );
	//		}
	//	}
	//}
}

void PhysX_Scene::onTrigger( physx::PxTriggerPair* /*pairs*/, physx::PxU32 /*count*/ )
{
}

void PhysX_Scene::onAdvance( const physx::PxRigidBody* const* /*bodyBuffer*/, const physx::PxTransform* /*poseBuffer*/, const physx::PxU32 /*count*/ )
{
}

physx::PxScene& PhysX_Scene::InitScene( const Vector<Acceleration>& _G  )
{
	//physx::pxtask::CudaContextManagerDesc cudaContextManagerDesc;
	//m_pCudaContextManager = physx::pxtask::createCudaContextManager(PhysX_Factory_Imp::Instance().Foundation(), cudaContextManagerDesc, nullptr );
	//if( m_pCudaContextManager )
	//{
	//	if( !m_pCudaContextManager->contextIsValid() )
	//	{
	//		m_pCudaContextManager->release();
	//		m_pCudaContextManager = NULL;
	//	}
	//}

	physx::PxSceneDesc sceneDesc{ m_Simulator.Physics().getTolerancesScale() };
	sceneDesc.gravity					= From( _G );
	sceneDesc.simulationEventCallback	= this;

	if(!sceneDesc.cpuDispatcher)
		sceneDesc.cpuDispatcher = &m_CpuDispatcher;

	sceneDesc.filterShader = PhysX_SceneFilterShader;
	////if(!sceneDesc.filterShader)
	////	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;

	////if(!sceneDesc.gpuDispatcher && m_pCudaContextManager)
	////{
	////	sceneDesc.gpuDispatcher = m_pCudaContextManager->getGpuDispatcher();
	////}


	physx::PxScene* pScene = m_Simulator.Physics().createScene( sceneDesc );
	if (!pScene)
		throw std::runtime_error("createScene failed!");

	if( physx::PxTaskManager* pTM = pScene->getTaskManager(); pTM )
		pTM->setCpuDispatcher( m_CpuDispatcher );

	bool result = false;
#ifdef _DEBUG
	pScene->getScenePvdClient()->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );								  
	pScene->getScenePvdClient()->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
	pScene->getScenePvdClient()->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );

	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eSCALE, 2.0f);			//+
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eWORLD_AXES, 2.0f);			//+
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eBODY_AXES, 2.0f);				//+
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eBODY_MASS_AXES, 0.1f); 		//+
	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);		//+
	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eBODY_ANG_VELOCITY, 1.0f);	//+
	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCONTACT_POINT, 100.0f);		//+
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCONTACT_NORMAL, 10.0f);		//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCONTACT_ERROR, 10.0f);		//-	
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCONTACT_FORCE, 10.0f);		//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);			//+
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);		//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_AXES, 1.0f);		//+
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_COMPOUNDS, 1.0f);
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_FNORMALS, 1.0f);	//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_EDGES, 1.0f);		//+ expensive
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_STATIC, 1.0f);
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_DYNAMIC, 1.0f);		//+
	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 100.0f);	//+ wireframe mode only
	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eJOINT_LIMITS, 10.0f);			//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eCULL_BOX, 1.0f);				//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eMBP_REGIONS, 1.0f);			//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eSIMULATION_MESH, 1.0f);		//-
//	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eSDF, 1.0f);					//+
#else
	result = pScene->setVisualizationParameter( physx::PxVisualizationParameter::eSCALE, 0.0f);
#endif

	m_bSceneCreated = true;
	return *pScene;
}

void PhysX_Scene::ExitScene(){
	m_Scene.release();

	//if( m_pCudaContextManager ){
	//	m_pCudaContextManager->release();
	//	m_pCudaContextManager = nullptr;
	//}
}

physx::PxFilterFlags PhysX_Scene::PhysX_SceneFilterShader(
        physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
        physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
        physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/)
{
	// handle triggers
	if(physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		if((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1)){
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlag::eDEFAULT;
		}

		return physx::PxFilterFlag::eSUPPRESS;
	}

	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B or vice versa.
	if((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1)){
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	}

	return physx::PxFilterFlag::eDEFAULT;
}

PhysX_Scene::Dispatcher::Dispatcher( unsigned int numThreads )
{
	while( numThreads-- )
		m_Threads.push_back( new ThreadInfo() );
}

PhysX_Scene::Dispatcher::~Dispatcher(){
	for( size_t i = 0; i < m_Threads.size(); ++i )
		delete m_Threads[i];
}

void PhysX_Scene::Dispatcher::submitTask( physx::PxBaseTask& task ){
	if( m_Threads.empty() )
	// no worker threads, run directly
	{
		task.run();
		task.release();
		return;
	}

	unsigned int attempts = 0;
	for(;;)
	{
		++attempts;

		for( unsigned short i = 0; i < m_Threads.size(); ++ i ){
			if( m_Threads[i]->Push( task, attempts ) )
				return;
		}
	}
}

physx::PxU32 PhysX_Scene::Dispatcher::getWorkerCount() const{
	return common::narrow_cast<physx::PxU32>(m_Threads.size());
}

void PhysX_Scene::Dispatcher::JoinAllTasks() noexcept{
	for( size_t i = 0; i < m_Threads.size(); ++i )
		while( m_Threads[i]->IsWorking() )
			;
}

void PhysX_Scene::Dispatcher::DumpTasks() const{
	for( auto pThreadInfo : m_Threads )
		pThreadInfo->DumpTasks();
}

PhysX_Scene::Dispatcher::ThreadInfo::ThreadInfo()
	:	m_bWorking		{ true },
		m_bQuit			{ false },
		m_nTasksAccepted{ 0 }
{
	assert(m_bWorking.is_lock_free());
	m_pThread = new thread(ThreadFunc,this);
}

PhysX_Scene::Dispatcher::ThreadInfo::~ThreadInfo(){
	if( m_pThread ){
		{
			unique_lock lock(m_Mutex);
			m_bQuit = true;
			m_Condition.notify_one();
		}

		m_pThread->join();
		delete m_pThread;
	}

	std::clog << "PhysX Dispatcher shut down. Number of PhysX Tasks processed: " << m_nTasksAccepted << std::endl; 
}

void PhysX_Scene::Dispatcher::ThreadInfo::ThreadFunc( ThreadInfo* pTI ){
	if( pTI )
		pTI->Loop();
}

void PhysX_Scene::Dispatcher::ThreadInfo::Loop(){
	while( !m_bQuit ){
		unique_lock lock(m_Mutex);
		while( m_Tasks.empty() ){
			if (m_bQuit) {
				m_bWorking = false;
				return;
			}

			m_bWorking = false;
			m_Condition.wait(lock);
		}

		auto task = m_Tasks.front();
		m_Tasks.pop();
		lock.unlock();

		task->run();
		task->release();
	}

	m_bWorking = false;
}

bool PhysX_Scene::Dispatcher::ThreadInfo::Push( physx::PxBaseTask& rTask, unsigned int attempts ){
	if( !m_Mutex.try_lock() )
		return false;

	if( attempts > m_Tasks.size() ){
		m_Tasks.push( &rTask );
		m_bWorking = true;
		m_Condition.notify_one();
		m_Mutex.unlock();
		++m_nTasksAccepted;
		return true;
	}

	m_Mutex.unlock();
	return false;
}

bool PhysX_Scene::Dispatcher::ThreadInfo::IsWorking() const noexcept{
	//unique_lock lock(m_Mutex); //this is causing temporary locks with std::thread on win32 ...
	return m_bWorking;
}

void PhysX_Scene::Dispatcher::ThreadInfo::DumpTasks() const{
	std::clog << "PhysX Dispatcher number of PhysX Tasks processed: " << m_nTasksAccepted << std::endl;
}

}