

#include "trax/collections/FixturesCollections.h"

#include "dim/DimensionedValues.h"

using namespace trax;
using namespace spat;

///////////////////////////////////////
TrackSystemFixture::TrackSystemFixture()
		: m_pTrackSystem	{ trax::TrackSystem::Make() }
{
	m_pTrackSystem->CreateCollection();
}

TrackSystemFixture::~TrackSystemFixture()
{
	if( m_pTrackSystem )
		m_pTrackSystem->DeCoupleAll();
}
///////////////////////////////////////
TrackSystemCircleFixture::TrackSystemCircleFixture()
{
	m_pTrackSystem->Add( m_pTrack1 );
	m_pTrackSystem->Add( m_pTrack2 );
	m_pTrackSystem->Add( m_pTrack3 );
	m_pTrackSystem->Add( m_pTrack4 );
}

TrackSystemCircleFixture::~TrackSystemCircleFixture()
{
}
///////////////////////////////////////
TrackAndTrackSystem::TrackAndTrackSystem()
{
	spat::Vector<Angle> rotator( 1.0f, 1.0f, 1.0f );
	rotator.Normalize();
	rotator *= pi/2;

	m_FrameA.Init();
	m_FrameA.Rotate( rotator );
	m_FrameB.Init();
	m_FrameB.Rotate( -rotator );
}
///////////////////////////////////////

