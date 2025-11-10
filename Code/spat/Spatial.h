//	trax track library
//	AD 2015
//
//  "Pure Vernunft darf niemals siegen"
//
//							Tocotronic
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

/// \page docu_spat Spatial Computations
/// 
/// \section spat_intro Introduction
/// 
/// For calculations in three dimensional space, the spat library provides
/// various templated structures. A \link spat::Position Position \endlink
/// represents a point in space, while a \link spat::Vector Vector \endlink
/// represents a direction. Both are combined to form a \link spat::VectorBundle 
/// VectorBundle \endlink, which represents a point and a direction in space.
/// A \link spat::Frame Frame \endlink handles a frame of reference in space
/// and allows transformations of Positions and Vectors between different frames.
/// There are also \link spat::Matrix Matrix \endlink classes for more general 
/// transformation calculations and geometric shapes like \link spat::Box Box 
/// \endlink or \link spat::Sphere Sphere \endlink for spatial queries. Conceptually
/// all these classes have 2D variants as well, e.g. \link spat::Position2D 
/// Position2D \endlink.
/// 


/// \brief The namespace provides classes and methods for
/// spatial computations.
namespace spat{
}

#include "Box.h"
#include "Frame.h"
#include "Matrix.h"
#include "Position.h"
#include "Position2D.h"
#include "PositionH.h"
#include "Rect.h"
#include "Sphere.h"
#include "Vector.h"
#include "Vector2D.h"
#include "VectorBundle.h"
#include "VectorBundle2.h"



