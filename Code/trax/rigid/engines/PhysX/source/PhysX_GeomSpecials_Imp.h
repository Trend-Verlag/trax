//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2015 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "PhysX_GeomBase_Imp.h"

namespace trax{

	class PhysX_Scene;

	class PhysX_Cuboid_Imp :	public GeomCuboid,
								public PhysX_GeomBase_Imp{
	public:
		PhysX_Cuboid_Imp( Real engineMetersPerUnit );
		PhysX_Cuboid_Imp( const Box<Length>& box, Real engineMetersPerUnit );

		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		void SetFrame( const Frame<Length,One>& frame ) noexcept override;

		GeomType GetGeomType() const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		void Diagonal( const Vector<Length>& diagonal ) override;

		const Vector<Length>& Diagonal() const override;

		void SetFromBox( const Box<Length>& box ) override;

		const physx::PxGeometry& Geometry() const noexcept override{
			return m_BoxGeometry;
		}
	private:
		physx::PxBoxGeometry	m_BoxGeometry;
		spat::Vector<Length>	m_Diagonal;
	};

	class PhysX_Capsule_Imp :	public GeomCapsule,
								public PhysX_GeomBase_Imp{
	public: 
		PhysX_Capsule_Imp( Real engineMetersPerUnit );
		PhysX_Capsule_Imp( Length radius, Length length, Real engineMetersPerUnit );

		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		GeomType GetGeomType() const noexcept override;

		void SetFrame( const Frame<Length,One>& frame ) noexcept override;

		void GetFrame( Frame<Length,One>& frame ) const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		void OnAttach( physx::PxRigidActor& actor ) override;

		void Radius( Length radius ) override;

		Length Radius() const override;

		void SetLength( Length length ) override;

		Length GetLength() const override;

		const physx::PxGeometry& Geometry() const noexcept override{
			return m_CapsuleGeometry;
		}
	private:
		physx::PxCapsuleGeometry m_CapsuleGeometry;
	};


	class PhysX_Sphere_Imp :	public GeomSphere,
								public PhysX_GeomBase_Imp{
	public:
		PhysX_Sphere_Imp( Real engineMetersPerUnit ) noexcept;
		PhysX_Sphere_Imp( Length radius, Real engineMetersPerUnit );

		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		GeomType GetGeomType() const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		void Radius( Length radius ) override;

		Length Radius() const override;

		const physx::PxGeometry& Geometry() const noexcept override{
			return m_SphereGeometry;
		}
	private:
		physx::PxSphereGeometry m_SphereGeometry;
	};


	class PhysX_Cylinder_Imp :	public GeomCylinder,
								public PhysX_GeomBase_Imp{
	public:
		PhysX_Cylinder_Imp( const PhysX_Scene& scene );
		PhysX_Cylinder_Imp( const PhysX_Scene& scene, Length radius, Length length );
	
		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		GeomType GetGeomType() const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		void Radius( Length radius ) override;

		Length Radius() const override;

		void SetLength( Length length ) override;

		Length GetLength() const override;

		const physx::PxGeometry& Geometry() const noexcept override{
			return m_CylinderGeometry;
		}
	private: 
		const PhysX_Scene& m_Scene;
		Length m_Radius;
		Length m_Length;
		physx::PxConvexMeshGeometry m_CylinderGeometry;
		void CookConvexMesh();
	};


	class PhysX_HeightField :	public GeomHeightField,
								public PhysX_GeomBase_Imp{
	public:
		PhysX_HeightField( const PhysX_Scene& scene );

		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		GeomType GetGeomType() const noexcept override;

		void SetFrame( const Frame<Length,One>& frame ) noexcept override;

		void GetFrame( Frame<Length,One>& frame ) const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		bool Create( const short* pSamples, const bool* pbHoles, int nRows, int nCols, Real vertScale, Real horzScale ) override;

		bool CreateEEPStyle( const short* pSamples, const bool* pbHoles, int nRows, int nCols, Real vertScale, Real horzScale ) override;

		Length Height( const Position2D<Length>& parameter ) const override;

		void Get( const Position2D<Length>& parameter, Position<Length>& pos ) const override;

		void Get( const Position2D<Length>& parameter, Frame<Length,One>& frame ) const override;

		const physx::PxGeometry& Geometry() const noexcept override{
			return m_HeightFieldGeometry;
		}
	private:
		const PhysX_Scene&				m_Scene;
		physx::PxRigidDynamic*			m_pHeightFieldActor;
		physx::PxHeightFieldGeometry	m_HeightFieldGeometry;
		Frame<Length,One>				m_Frame;
	};


	class PhysX_ConvexMesh :	public GeomMesh,
								public PhysX_GeomBase_Imp{
	public:
		PhysX_ConvexMesh( const PhysX_Scene& scene );

		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		void SetFrame( const Frame<Length,One>& frame ) noexcept override;

		GeomType GetGeomType() const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		bool Create( const std::vector<Position<Length>>& points ) override;

		bool Create( const std::vector<Position<Length>>& points, const std::vector<int>& indices ) override;


		const physx::PxGeometry& Geometry() const noexcept override{
			return m_ConvexMesGeometry;
		}
	private:
		const PhysX_Scene&			m_Scene;
		physx::PxConvexMeshGeometry m_ConvexMesGeometry;

		bool CookConvexMesh( const std::vector<Position<Length>>& points );
		bool CookConvexMesh( const std::vector<Position<Length>>& points, const std::vector<int>& indices );
	};


	class PhysX_TriangleMesh :	public GeomMesh,
								public PhysX_GeomBase_Imp{
	public:
		PhysX_TriangleMesh( const PhysX_Scene& scene );

		const char*	TypeName() const noexcept override;

		std::unique_ptr<Geom> Instance() const noexcept override;

		GeomType GetGeomType() const noexcept override;

		spat::Box<Length> GetExtent() const noexcept override;

		bool Create( const std::vector<Position<Length>>& points ) override;

		bool Create( const std::vector<Position<Length>>& points, const std::vector<int>& indices ) override;


		const physx::PxGeometry& Geometry() const noexcept override{
			return m_TriangleMeshGeometry;
		}
	private:
		const PhysX_Scene&				m_Scene;
		physx::PxTriangleMeshGeometry	m_TriangleMeshGeometry;

		bool CookTriangleMeshStream( const std::vector<Position<Length>>& points, const std::vector<int>& indices );
		bool CookTriangleMesh( const std::vector<Position<Length>>& points, const std::vector<int>& indices );
	};

}