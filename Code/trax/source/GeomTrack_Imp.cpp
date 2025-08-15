// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


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
