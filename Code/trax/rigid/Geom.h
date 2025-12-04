//	trax track library
//	AD 2014 
//
//  "Now I'm ready 
//   to rise again"
//
//				Gabrielle
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
#include "trax/Units.h"
//#include "GeomType.h"

#include "spat/Frame.h"
#include "spat/Box.h"
#include "spat/Position2D.h"

#include <memory>
#include <vector>

namespace trax{

	struct Material;
	enum class GeomType : char;

	/// \brief A Geom is used for collision detection. It differs from a Shape in that it
	/// defines the most simple elements of a shape. A Shape is a collection of Geoms and
	/// itself a Geom.
	struct Geom{

		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Creates a new geom that will share shape data
		/// with the existing one.
		/// 
		/// It deponds from the geom wether instancing of data is applicable or not, but
		/// the method will give a geom in any case.
		virtual std::unique_ptr<Geom> Instance() const noexcept = 0;


		// Geom's name:

		/// \brief Sets the name of the Geom.
		/// 
		/// This is for debug purposes; the string is not copied,
		/// only the pointer is stored.
		virtual void SetName( const char* name ) noexcept = 0;


		/// \returns The name of the Body.
		virtual const char* GetName() const noexcept = 0;



		///	\brief Sets position and orientation (pose) relative to 
		/// a Body's frame.
		/// 
		/// \param frame Frame of reference.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) noexcept = 0;


		///	\brief Gets the pose relative to a Body's frame.
		virtual void GetFrame( spat::Frame<Length,One>& frame ) const noexcept = 0;


		/// \brief Gets the minimum axis aligned box that includes the 
		/// the Geom completely.
		virtual spat::Box<Length> GetExtent() const noexcept = 0;


		/// \returns The volume of the Geom.
		virtual Volume GetVolume() const noexcept = 0;


		/// \brief Sets the material to be applied with the Geom.
		virtual void SetMaterial( const Material& material ) noexcept = 0;


		/// \returns The material active for the Geom.
		virtual const Material& GetMaterial() const noexcept = 0;


		/// \returns The type of the Geom.
		virtual GeomType GetGeomType() const noexcept = 0;


		/// \brief Filter flags that will serve in fast collision testing.
		///
		/// Each Geom carries a type flag and specifies with what other types a
		/// collision will be possible. 
		enum Filters : unsigned int{
			none			= 0,
			fBogie			= 0x00000001,
			fChildBogie		= 0x00000002,
			fWheelset		= 0x00000004,
			fCargo			= 0x00000008,
			fTerrain		= 0x00000010,
			fBogieSpace		= 0x00000020,
			fCrane			= 0x00000040,
			fConstruction	= 0x00000080,

			fHook			= 0x00000100,
			fHookFixed		= 0x00000200,
			fCouplingNorth	= 0x00000400,
			fCouplingSouth	= 0x00000800,

			fTriggerShape	= 0x00001000,
			fCabin			= 0x00002000,
			fBogieNoCabin	= 0x00004000,
			fBuffer			= 0x00008000,
			fTrack			= 0x00010000,
			fMovableTrack	= 0x00020000,

			fAll			= 0x2fffffff
		};


		/// \brief Specify a flag for the type of this Geom.
		///
		/// The type will be typically represented by one bit and
		/// will be used on collision detection.
		virtual void TypeFilter( Filters type ) noexcept = 0;


		/// \returns the type bitfield.
		virtual Filters TypeFilter() const noexcept = 0;


		/// \brief Specify the collision bitfield.
		///
		/// This will typically be one bit set for each type to collide
		/// this geom with. Collisions between geoms with mismatching type and
		/// collision fields will be ignored if they collide but be aware
		/// that if one geom can not collide with another it still is
		/// possible that the other collides with the geom. I.e. the collision
		/// happens if:
		/// \verbatim
		/// (typeA & collionB) || (typeB & collisionA)
		/// \endverbatim
		virtual void CollisionFilter( unsigned int collideWith ) noexcept = 0;


		/// \returns the collision bitfield.
		virtual unsigned int CollisionFilter() const noexcept = 0;


		virtual ~Geom() = default;
	};


	dclspc std::string ToString( Geom::Filters filter );

	dclspc Geom::Filters GeomFilter( const std::string& filter );

	dclspc unsigned int CollisionFilterFor( Geom::Filters filter ) noexcept;

	constexpr bool IsTrigger( unsigned int filter ) noexcept{
		return (filter & (Geom::fHook | Geom::fHookFixed | Geom::fCouplingNorth | Geom::fCouplingSouth)) ? true : false;
	}


	/// \brief Rectangular cuboid shape.
	struct GeomCuboid : virtual Geom{

		/// \brief Set the edge lengthes of the cuboid shape.
		virtual void Diagonal( const spat::Vector<Length>& diagonal ) = 0;


		/// \brief Get the edge lengthes of the cuboid.
		virtual const spat::Vector<Length>& Diagonal() const = 0;


		/// \brief Set the dimensions and the frame of the cuboid to represent an axis aligned box.
		virtual void SetFromBox( const spat::Box<Length>& box ) = 0;
	};


	/// \brief Cylinder shaped Geom.
	///
	/// The cylinder is aligned along it's local z (binormal) axis
	/// and centered around the origin.
	struct GeomCylinder : virtual Geom{

		virtual void Radius( Length radius ) = 0;

		virtual Length Radius() const = 0;

		virtual void SetLength( Length length ) = 0;

		virtual Length GetLength() const = 0;
	};


	/// \brief A Capsule is a Cylinder with round caps.
	///
	/// The radius is the radius of the spherical caps and the 
	/// radius of the cylinder part of the capsule. The length
	/// is the length of the cylinder part only.
	/// The capsule is aligned along it's local z (binormal) axis
	/// and centered around the origin.
	struct GeomCapsule : virtual Geom{

		/// \brief Sets the radius aof the cylinder part and the caps
		virtual void Radius( Length radius ) = 0;


		/// \brief Gets the radius aof the cylinder part and the caps
		virtual Length Radius() const = 0;


		/// \brief Sets the length of the capsule without the caps.
		virtual void SetLength( Length length ) = 0;


		/// \brief Gets the length of the capsule without the caps.
		virtual Length GetLength() const = 0;
	};


	/// \brief A sphere shaped Geom.
	struct GeomSphere : virtual Geom{

		virtual void Radius( Length radius ) = 0;

		virtual Length Radius() const = 0;
	};


	struct GeomHeightField : virtual Geom{

		/// \brief Creates a heightfield from data
		///
		/// The quads are tesselated as [i],[i+nCols+1],[i+1] and [i],[i+nCols],[i+nCols+1].
		/// If all three adjacent samples of a triangle have the cooresponding hole information flag set to true,
		/// the triangle would not be created as a collision shape.
		/// \param pSamples Array with the elevation data of the sample points. Length must be at least nRows*nCols.
		/// \param pbHoles Array with the hole information. Length must be at least nRows*nCols.
		/// \param nRows Number of rows in the heightfield.
		/// \param nCols Number of columns in the heightfield.
		/// \param vertScale Vertical scale factor.
		/// \param horzScale Horizontal scale factor.
		virtual bool Create( const short* pSamples, const bool* pbHoles, int nRows, int nCols, Real vertScale, Real horzScale ) = 0;


		/// \brief creates a heightfield from data in the EEP style.
		///
		/// There will only be quads erased, no single triangles. A quad is erased if its topleft, topright and bottomright
		/// sample show the hole flag.
		virtual bool CreateEEPStyle( const short* pSamples, const bool* pbHoles, int nRows, int nCols, Real vertScale, Real horzScale ) = 0;


		virtual Length Height( const spat::Position2D<Length>& parameter ) const = 0;


		virtual void Get( const spat::Position2D<Length>& parameter, spat::Position<Length>& pos ) const= 0;


		virtual void Get( const spat::Position2D<Length>& parameter, spat::Frame<Length,One>& frame ) const = 0;
	};


	struct GeomMesh : virtual Geom{

		virtual bool Create( const std::vector<spat::Position<Length>>& points ) = 0;

		virtual bool Create( const std::vector<spat::Position<Length>>& points, const std::vector<int>& indices ) = 0;


	};
}