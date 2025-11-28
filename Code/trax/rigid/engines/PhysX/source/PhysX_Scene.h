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

#include "trax/rigid/source/Scene_Imp.h"

#include "PhysX_TrackJoint.h"

#ifdef _BOOST_THREAD
#include <boost/thread/thread.hpp> // NOLINT 
#include <boost/thread/mutex.hpp> // NOLINT 
#include <boost/thread/recursive_mutex.hpp> // NOLINT 
#else
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#endif

#include <queue>

namespace trax
{
	using namespace spat;

	class PhysX_Simulator;

	class PhysX_Scene :	public Scene_Imp,
						public physx::PxSimulationEventCallback
	{
	public:
		PhysX_Scene( const PhysX_Scene& ) = delete;
		PhysX_Scene( PhysX_Scene&& ) = delete;
		PhysX_Scene( const PhysX_Simulator& simulator, Vector<Acceleration> G = trax::g * trax::Up );
		PhysX_Scene( const PhysX_Simulator& simulator, Real engine_meters_per_unit, Real engine_kilograms_per_unit, Vector<Acceleration> G = trax::g * trax::Up );
		PhysX_Scene( const PhysX_Simulator& simulator, physx::PxScene& scene, Real engine_meters_per_unit, Real engine_kilograms_per_unit );
		~PhysX_Scene();

		PhysX_Scene& operator=( const PhysX_Scene& ) = delete;
		PhysX_Scene& operator=( PhysX_Scene&& ) = delete;

		// Scene:
		const char*	TypeName() const noexcept override{
			return "PhysXScene";
		}
		
		bool IsValid( bool bSilent = true ) const noexcept override;

		const PhysX_Simulator& Simulator() const { return m_Simulator; }

		void Up( const Vector<One>& up ) override;

		Vector<One> Up() const override;

		void Gravity( const spat::Vector<Acceleration>& gravityAccelerarion ) override;

		Vector<Acceleration> Gravity() const override;


		using Scene::CreateBody;

		std::unique_ptr<Body> CreateBody() const override;

		std::unique_ptr<Body> CreateBody( physx::PxRigidDynamic& actor ) const;

		using Scene::CreateShape;

		std::unique_ptr<Shape> CreateShape() const override;

		std::unique_ptr<Shape> CreateShape( physx::PxRigidStatic& actor ) const;

		using Scene::CreateGestalt;

		std::unique_ptr<Gestalt> CreateGestalt() const override;
		
		std::unique_ptr<GeomCuboid> CreateGeomCuboid() const override;

		std::unique_ptr<GeomCylinder> CreateGeomCylinder() const override;

		std::unique_ptr<GeomCapsule> CreateGeomCapsule() const override;

		std::unique_ptr<GeomSphere> CreateGeomSphere() const override;

		std::unique_ptr<GeomHeightField> CreateGeomHeightField() const override;

		std::unique_ptr<GeomMesh> CreateGeomConvexMesh() const override;

		std::unique_ptr<GeomMesh> CreateGeomTriangleMesh() const override;


		std::unique_ptr<HingeJoint> CreateHingeJoint( 
			Body* pBodyA, 
			const spat::Frame<Length,One>& localAnchorA, 
			Body* pBodyB, 
			const spat::Frame<Length,One>& localAnchorB ) const override;

		std::unique_ptr<SliderJoint> CreateSliderJoint( 
			Body* pBodyA, 
			const spat::Frame<Length,One>& localAnchorA, 
			Body* pBodyB, 
			const spat::Frame<Length,One>& localAnchorB ) const override;

		std::unique_ptr<BallAndSocketJoint> CreateBallAndSocketJoint( 
			Body* pBodyA, 
			const spat::Frame<Length,One>& localAnchorA, 
			Body* pBodyB, 
			const spat::Frame<Length,One>& localAnchorB ) const override;

		std::unique_ptr<DistanceJoint> CreateDistanceJoint( 
			Body* pBodyA, 
			const spat::Frame<Length,One>& localAnchorA, 
			Body* pBodyB, 
			const spat::Frame<Length,One>& localAnchorB ) const override;

		std::shared_ptr<class TrackJointFeeder> CreateTrackJointFeeder( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor ) noexcept override;

		std::shared_ptr<class TrackJointFeeder> CreateTrackJointFeeder( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor, 
			const Body& bodyTrack ) noexcept override;

		std::shared_ptr<class TrackJointFeederMotorModel> CreateTrackJointFeederMotorModel( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor ) noexcept override;

		std::shared_ptr<class TrackJointFeederMotorModel> CreateTrackJointFeederMotorModel( 
			const Body& bodyMoving, 
			const spat::Frame<Length,One>& localAnchor, 
			const Body& bodyTrack ) noexcept override;

		void Release( TrackJointFeeder& feeder ) noexcept override;

		void DumpTasksTo( std::ostream& stream ) const noexcept override;

		//physx::PxSimulationEventCallback:
		void onConstraintBreak( physx::PxConstraintInfo* constraints, physx::PxU32 count) override;

		void onWake( physx::PxActor** actors, physx::PxU32 count) override;

		void onSleep( physx::PxActor** actors, physx::PxU32 count) override;

		void onContact( const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs ) override;
		
		void onTrigger( physx::PxTriggerPair* pairs, physx::PxU32 count) override;

		void onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;

		inline physx::PxScene& Scene() const noexcept { return m_Scene; }

		inline Real 				EngineMetersPerUnit() const noexcept { return m_EngineMetersPerUnit; }
		inline Real 				EngineKilogramsPerUnit() const noexcept { return m_EngineKilogramsPerUnit; }
	
	protected:
		void StartStep( Time dt = fixed_timestep ) noexcept override;
		void Update( Time dt = fixed_timestep ) noexcept override;
		bool EndStep() noexcept override;

	private:
		const PhysX_Simulator& m_Simulator;
		physx::PxScene&	m_Scene;

		bool m_bSceneCreated;

		using JointNFeeder = std::shared_ptr<TrackJointFeeder>;
		std::vector<JointNFeeder>	m_TrackJoints; 
		const Real					m_EngineMetersPerUnit;
		const Real					m_EngineKilogramsPerUnit;

		physx::PxScene& InitScene( const Vector<Acceleration>& g );
		void ExitScene();

		class Dispatcher : public physx::PxCpuDispatcher{
		public:
			Dispatcher( unsigned int numThreads );
			~Dispatcher();

			virtual void submitTask( physx::PxBaseTask& task ) override;

			virtual physx::PxU32 getWorkerCount() const override;

			void JoinAllTasks() noexcept;

			void DumpTasksTo( std::ostream & stream ) const noexcept;
		private:

			struct ThreadInfo{
				ThreadInfo();
				~ThreadInfo();

#ifdef _BOOST_THREAD
				using mutex = boost::mutex;
				using condition_variable = boost::condition_variable;
				using thread = boost::thread;
				using unique_lock = boost::unique_lock<boost::mutex>;
#else
				using mutex = std::mutex;
				using condition_variable = std::condition_variable;
				using thread = std::thread;
				using unique_lock = std::unique_lock<std::mutex>;
#endif
				mutex							m_Mutex;
				condition_variable				m_Condition;
				thread*							m_pThread;

				std::queue<physx::PxBaseTask*>	m_Tasks;
				std::atomic_bool				m_bWorking;
				bool							m_bQuit;
				unsigned int					m_nTasksAccepted;


				static void ThreadFunc( ThreadInfo* pTI );

				void Loop();

				bool Push( physx::PxBaseTask& rTask, unsigned int attempts );

				bool IsWorking() const noexcept;

				void DumpTasksTo( std::ostream& stream ) const noexcept;
			};

			std::vector<ThreadInfo*> m_Threads;

		} m_CpuDispatcher;

		static physx::PxFilterFlags PhysX_SceneFilterShader(
			physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
			physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
			physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/);
	};
}