//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "appframe/Command.h"

class TestCommand : public cmnd::Command{
public:
	void Execute() override {}
	std::unique_ptr<cmnd::Command> Clone() const override {
		return std::make_unique<TestCommand>(*this);
	}
};

BOOST_AUTO_TEST_SUITE(appframe_tests)

BOOST_AUTO_TEST_CASE( CommandCreation )
{
	std::unique_ptr<cmnd::Command> pCommand = std::make_unique<TestCommand>();
	BOOST_CHECK( pCommand );
}

BOOST_AUTO_TEST_SUITE_END() //appframe_tests
#endif //WITH_BOOST_TESTS
