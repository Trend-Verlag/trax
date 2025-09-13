//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Units.h"
#include "spat/Frame.h"

#include <memory>

namespace trax{

	class Bogie_Imp;

	struct BogieJoint{

		/// \brief Sets the name of the Joint.
		/// 
		/// This is for debug purposes; the string is not copied,
		/// only the pointer is stored.
		virtual void SetName( const char* name ) noexcept = 0;


		/// \returns The name of the Joint.
		virtual const char* GetName() const noexcept = 0;


		/// \brief The dominant bogie of the connection.
		virtual std::shared_ptr<Bogie_Imp> BogieA() const noexcept = 0;


		/// \brief The pose of the joint in the local coordinate system of bogie A.
		virtual spat::Frame<Length,One> LocalPoseA() const noexcept = 0;


		/// \brief The subordinate bogie of the connection.
		virtual std::shared_ptr<Bogie_Imp> BogieB() const noexcept = 0;


		/// \brief The pose of the joint in the local coordinate system of bogie B.
		virtual spat::Frame<Length,One> LocalPoseB() const noexcept = 0;

		virtual void SetBogieA( Bogie_Imp* pBogieA ) = 0;

		virtual void SetBogieB( Bogie_Imp* pBogieB ) = 0;

		virtual ~BogieJoint() = default;
	};

}
