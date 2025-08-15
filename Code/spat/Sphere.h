// Spatial Coordinates And Vectors Library (spat)
// 
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2013 
//
//  "And the arms of the ocean are carrying me"
//
//				Florence + the Machine

#pragma once

#include "Position.h"

namespace spat{

	template<typename> struct Box;
	template<typename,typename> struct VectorBundle;
	
	/// \brief Sphere with center and radius.
	template<typename Valtype>
	struct Sphere
	{
		typedef Valtype value_type;

		Position<Valtype>	c;	///< Center of the sphere
		Valtype				r;	///< Radius of the sphere


		/// \name Construction
		/// \param center
		/// \param radius
		///@{		
		Sphere() noexcept = default;			///< Does not initialize the members.
		Sphere( const Position<Valtype>& center, Valtype radius ) noexcept;
		///@}


		/// \brief Initializes the sphere to unit sphere around origin.
		void Init() noexcept;


		///	\returns The geometric center of the sphere.
		Position<Valtype> Center() const noexcept;


		/// \returns The radius of the sphere.
		Valtype Radius() const noexcept;


		/// \returns True if the point lays inside the sphere. 
		///
		/// A point on the surface is not included in the sphere
		bool Includes( Valtype x, Valtype y, Valtype z ) const noexcept;


		/// \returns True if the point lays inside the sphere. 
		///
		/// A point on the surface is not included in the sphere
		bool Includes( const Position<Valtype>& pt ) const noexcept;


		/// \brief Gets the circumscribed box of the sphere
		Box<Valtype> ExBox() const noexcept;


		/// \brief Gets the inscribed box of the sphere
		Box<Valtype> InBox() const noexcept;
	};

	/// \name Mathematical Operators for Sphere
	///@{
	template<typename Valtype>
	bool	operator==( const Sphere<Valtype> a, const Sphere<Valtype> b ) noexcept;
	template<typename Valtype>
	bool	operator!=( const Sphere<Valtype> a, const Sphere<Valtype> b ) noexcept;
	///@}

	/// \brief Do the two spheres intersect?
	///@{
	template<typename Valtype>
	bool Intersecting( const Sphere<Valtype>& sphereA, const Sphere<Valtype>& sphereB ) noexcept;

	template<typename Valtype,typename ValtypeT>
	bool Intersecting( const Sphere<Valtype>& sphere, const VectorBundle<Valtype,ValtypeT>& ray ) noexcept;
	///@}

///////////////////////////////////////
template<typename Valtype> inline 
Sphere<Valtype>::Sphere( const Position<Valtype>& center, Valtype radius ) noexcept
	:	c{center},
		r{radius}
{}

template<typename Valtype> inline 
void Sphere<Valtype>::Init() noexcept{
	c.Init();
	r = static_cast<Valtype>(1);
}

template<typename Valtype> inline 
Position<Valtype> Sphere<Valtype>::Center() const noexcept{
	return c;
}

template<typename Valtype> inline 
Valtype Sphere<Valtype>::Radius() const noexcept{
	return r;
}

template<typename Valtype> inline 
bool Sphere<Valtype>::Includes( Valtype x, Valtype y, Valtype z ) const noexcept{
	return Includes( { x, y, z } );
}

template<typename Valtype> inline 
bool Sphere<Valtype>::Includes( const Position<Valtype>& pt ) const noexcept{
	return (pt - c).Length() < r;
}

template<typename Valtype> inline 
Box<Valtype> Sphere<Valtype>::ExBox() const noexcept{
	return Box<Valtype>( 
		c.x - r, c.y - r, c.z - r,
		c.x + r, c.y + r, c.z + r );
}

template<typename Valtype> inline 
Box<Valtype> Sphere<Valtype>::InBox() const noexcept{
	Valtype a = r / std::sqrt(3);
	return Box<Valtype>( 
		c.x - a, c.y - a, c.z - a,
		c.x + a, c.y + a, c.z + a );
}

template<typename Valtype> inline 
bool operator==( const Sphere<Valtype> a, const Sphere<Valtype> b ) noexcept{
	return a.Center() == b.Center() && a.Radius() == b.Radius();
}

template<typename Valtype> inline 
bool operator!=( const Sphere<Valtype> a, const Sphere<Valtype> b ) noexcept{
	return !(a == b);
}

template<typename Valtype>
bool Intersecting( const Sphere<Valtype>& sphereA, const Sphere<Valtype>& sphereB ) noexcept{
	return (sphereA.Center() - sphereB.Center()).Length() < sphereA.Radius() + sphereB.Radius();
}

template<typename Valtype,typename ValtypeT>
bool Intersecting( const Sphere<Valtype>& sphere, const VectorBundle<Valtype,ValtypeT>& ray ) noexcept{
	const Vector<Valtype> D = sphere.Center() - ray.P;
	return (D - (D*ray.T) * ray.T).Length() < sphere.Radius();
}

} // namespace spat