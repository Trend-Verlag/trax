

#pragma once

#include "dim/DimensionedValues.h"
#include "spat/Position.h"
#include "spat/Vector.h"
#include "spat/Matrix.h"

#if defined(_MSC_VER)
#   pragma warning( push )
#   pragma warning( disable: 4244 ) // conversion from 'double' to 'float', possible loss of data
#   pragma warning( disable: 4305 ) //'argument': truncation from 'double' to 'float'
#endif
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/quatf.h>
#include <pxr/base/tf/token.h>
#if defined(_MSC_VER)
#   pragma warning( pop )
#endif

namespace trax{

    template< class GfVect3Type >
	inline GfVect3Type From( const spat::Position<Length>& p, Real usdStageMetersPerUnit ){
		return GfVect3Type{ _m(p.x)/usdStageMetersPerUnit, _m(p.y)/usdStageMetersPerUnit, _m(p.z)/usdStageMetersPerUnit };
	}

	inline pxr::GfVec3f From( const spat::Position<One>& rotAngles ){
		return pxr::GfVec3f{ _deg(rotAngles.x), _deg(rotAngles.y), _deg(rotAngles.z) };
	}

    inline pxr::GfVec3f From( const spat::Vector<One>& rot ){
		return pxr::GfVec3f{ rot.dx, rot.dy, rot.dz };
	}

    // Function to calculate a quaternion from a rotation axis
    inline pxr::GfQuatf CalculateQuaternionFromAxis( const pxr::GfVec3f& axis, float angle )
    {
        // Normalize the rotation axis vector
        pxr::GfVec3f normalizedAxis = axis.GetNormalized();

        // Calculate the sine and cosine of half the rotation angle
        float halfAngle = angle / 2.0f;
        float sinHalfAngle = std::sin(halfAngle);
        float cosHalfAngle = std::cos(halfAngle);

        // Construct the quaternion
        pxr::GfQuatf quaternion{ 
            cosHalfAngle, 
            sinHalfAngle * normalizedAxis[0], 
            sinHalfAngle * normalizedAxis[1], 
            sinHalfAngle * normalizedAxis[2] };

        return quaternion;
    }

    inline pxr::GfQuatf From( const spat::Rotation<One>& rot )
	{
		return CalculateQuaternionFromAxis( From(-rot.RotationAxis()), _rad(rot.RotationAngle()) );
	}



}
