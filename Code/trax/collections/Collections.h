//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

/**
\page docu_collections Collections

\section collections_intro Introduction
There is an abundance of different objects provided by the trax library. 
Typically they are created with smart pointers, managing their lifetimes 
automatically. However, who is holding these smart pointers? The answer 
is: Collections. 

The master of all collections is the ModuleCollection. It holds Module 
objects that in turn hold one TrackSystem and one Fleet. A TrackSystem 
maintains TrackCollection objects that are used to group and locate tracks 
inside of a TrackSystem. A frame is understood as to be relative to it's 
parent in the following transformation hierarchy (classes in braces do not 
apply a transformation):
 
\verbatim

(ModuleCollection)
	   |
	Module ____________________________________________________________________
       |                                                                       |
(TrackSystem)------------------------------------------------               (Fleet)
	   |                               |                     |                 |
(TrackCollectionContainer)   (ConnectorCollection)   (SignalCollection)     (Train)
       |                               |                     |                 |
       |                          (Connector)             (Signal)      ---------------
TrackCollection									                        |              |
       |                                                           (RollingStock)    (Train)
       |                                                                |              
       |                                                              Bogie     
     Track                                                              |
       |                                                            ChildBogie
    (Curve)

\endverbatim 

\note A track will deliver module relative coordinates with its Transition()
methods, taking into account the TrackCollection's frame of reference.

The Fleet solely holds Trains, which in turn hold RollingStocks and other Trains.
A RollingStock holds Bogies and WheelFrames. A Bogie can hold child Bogies, which
most probably will be WheelFrames.
*/


#include "Collection.h"
#include "CollectionDecorator.h"
#include "ConnectorCollection.h"
#include "ObjectIDDecorator.h"
#include "TrackCollection.h"
#include "TrackCollectionContainer.h"
#include "TrackSystem.h"

namespace trax{

} // namespace trax

