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

#include "GeomTrack_Imp.h"

namespace trax{
///////////////////////////////////////
std::shared_ptr<GeomTrack> GeomTrack::Make() noexcept
{
	try{
		if( std::shared_ptr<GeomTrack_Imp> pRetval = std::make_shared<GeomTrack_Imp>(); pRetval )
		{
			pRetval->SetWeakPointerToSelf( pRetval );
			return pRetval;
		}
	}
	catch( ... ){
	}
	
	return nullptr;
}
///////////////////////////////////////
Track::TrackType GeomTrack_Imp::GetTrackType() const noexcept{
	return TrackType::withGeoms;
}

bool GeomTrack_Imp::IsValid() const noexcept
{
	if( !Track_Imp::IsValid() )
		return false;
	if( !m_pSection )
		return false;

	return true;
}

void GeomTrack_Imp::Attach( std::shared_ptr<const Section> pSection ) noexcept{
	if( !pSection )
		return;

	m_pSection = pSection;
	OnGeometryChanged();
}

std::shared_ptr<const Section> GeomTrack_Imp::DetachSection() noexcept{
	std::shared_ptr<const Section> retval(m_pSection);
	m_pSection.reset();
	OnGeometryChanged();
	return retval;
}

std::shared_ptr<const Section> GeomTrack_Imp::GetSection() const noexcept{
	return m_pSection;
}
///////////////////////////////////////
}
