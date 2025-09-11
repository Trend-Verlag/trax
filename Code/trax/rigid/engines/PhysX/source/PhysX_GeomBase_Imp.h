//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2015 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/rigid/source/Geom_Imp.h"

#include "spat/Frame.h"

#include "PhysX_Imp.h"


#if defined(_MSC_VER)
#	pragma warning(disable: 4250) // virtual inheritance of methods via dominance
#endif

namespace trax{

	using namespace spat;

	/// \brief Base class for all PhysX Geom implementations.
	///
	/// This implementation uses exclusive physx::Shapes, so does not rely
	/// instancing on attaching physx:Shapes to different actors. Instead
	/// it implements instancing over the Geom::Instance() method, thereby
	/// sharing only the geometry data of convex or triangle mesh et al. This
	/// allows for instances to be attached by different frames and scalings 
	/// even to the same actors. For simple geoms like boxes or spheres, no
	/// data is instanced in this approach.
	/// The implementation has a comparably high data footprint, but is very
	/// forgiving in terms of the order of setting frame, geometry data and 
	/// attaching. If applicable (Cuboid,Meshes) scaling can be done by 
	/// scaled frames.
	class PhysX_GeomBase_Imp : public Geom_Imp{
	public:
		PhysX_GeomBase_Imp( Real engineMetersPerUnit ) noexcept;
		~PhysX_GeomBase_Imp();

		PhysX_GeomBase_Imp& operator=( const PhysX_GeomBase_Imp& ) = delete;

		
		// Geom:
		
		void SetName( const char* name ) noexcept override;

		void SetFrame( const Frame<Length,One>& frame ) noexcept override;

		void GetFrame( Frame<Length,One>& frame ) const noexcept override;

		void SetMaterial( const Material& material ) noexcept override;

		using Geom_Imp::TypeFilter;

		void TypeFilter( Filters typeFlags ) noexcept override;

		using Geom_Imp::CollisionFilter;

		void CollisionFilter( unsigned int collideWith ) noexcept override;


		// Implementation:
		virtual void OnAttach( const Shape_ImpBase& shape ) override;

		void OnDetach() override;

		physx::PxShape* Shape() const noexcept{
			return m_pShape;
		}

		physx::PxMaterial* GetPxMaterial() const noexcept{
			return m_pMaterial;
		}

		virtual const physx::PxGeometry& Geometry() const noexcept = 0;

	protected:
		virtual void OnAttach( physx::PxRigidActor& actor );

		void AdjustShapeGeometry();

		const Real					m_EngineMetersPerUnit;
	private:
		Frame<Length,One>			m_Frame;
		physx::PxMaterial*			m_pMaterial;
		physx::PxShape*				m_pShape;

		void SetFilter();
	};

	physx::PxQueryFlags::InternalType From( unsigned int filterFlags );
}