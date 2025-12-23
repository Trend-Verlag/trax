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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "SectionTrack_Imp.h"
#include "trax/Section.h"

namespace trax{
///////////////////////////////////////
std::shared_ptr<SectionTrack> SectionTrack::Make() noexcept
{
	try{
		if( std::shared_ptr<SectionTrack_Imp> pRetval = std::make_shared<SectionTrack_Imp>(); pRetval )
		{
	//		pRetval->SetWeakPointerToSelf( pRetval );
			return pRetval;
		}
	}
	catch( ... ){
	}
	
	return nullptr;
}
///////////////////////////////////////
Track::TrackType SectionTrack_Imp::GetTrackType() const noexcept{
	return TrackType::withGeoms;
}

bool SectionTrack_Imp::IsValid() const noexcept
{
	if( !Track_Imp::IsValid() )
		return false;
	if( CntSections() <= 0 )
		return false;

	return true;
}

int SectionTrack_Imp::Attach( std::shared_ptr<const Section> pSection ){
	if( CntSections() == maxCntSections )
		throw std::length_error{ "SectionTrack_Imp::Attach: maximum number of sections exceeded." };

	if( !pSection )
		throw std::invalid_argument{ "SectionTrack_Imp::Attach: pSection is nullptr." };

	if( pSection->CountPoints() < 3 )
		throw std::invalid_argument{ "SectionTrack_Imp::Attach: pSection has less than 3 section points." };

	if( auto iter = std::find( m_Sections.begin(), m_Sections.end(), pSection ); iter != m_Sections.end() )
		return std::distance( m_Sections.begin(), iter );

	m_Sections.push_back(pSection);
	OnGeometryChanged();
	return static_cast<int>(m_Sections.size()) - 1;
}

std::shared_ptr<const Section> SectionTrack_Imp::DetachSection( int index ) noexcept{
	if( index < 0 || index >= static_cast<int>(m_Sections.size()) )
		return nullptr;

	std::shared_ptr<const Section> retval{ m_Sections[index] };
	m_Sections.erase( m_Sections.begin() + index );
	OnGeometryChanged();
	return retval;
}

std::shared_ptr<const Section> SectionTrack_Imp::GetSection( int index ) const noexcept{
	if( index < 0 || index >= static_cast<int>(m_Sections.size()) )
		return nullptr;

	return m_Sections[index];
}

int SectionTrack_Imp::CntSections() const noexcept{
	return m_Sections.size();
}
///////////////////////////////////////
}
