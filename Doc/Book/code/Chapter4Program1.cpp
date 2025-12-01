
#include "trax/Units.h"
#include "trax/Factory.h"
#include "trax/Location.h"
#include "common/Interval.h"


/// \brief A sample program that shows how to create a track circle and how to use a Location on it.
int main(){
	using namespace trax;
	using namespace spat;
	using namespace common;

	assert( CheckUnits() );

	auto pTrack1 = TrackBuilder::Make();
	auto pTrack2 = TrackBuilder::Make();
	auto pTrack3 = TrackBuilder::Make();
	auto pTrack4 = TrackBuilder::Make();
		
	// We build a track circle with radius 10_m around the Origin from the four pieces:
	auto pArc1 = ArcP::Make();
	pArc1->Create( { Origin3D<Length>, { 1, 0, 0 }, { 0, 10, 0 } } );
	pTrack1->Attach( std::move(pArc1), {0_m,10_m*pi/2} );

	auto pArc2 = ArcP::Make();
	pArc2->Create( { Origin3D<Length>, { 0, 1, 0 }, { -10, 0, 0 } } );
	pTrack2->Attach( std::move(pArc2), {0_m,10_m*pi/2} );

	auto pArc3 = ArcP::Make();
	pArc3->Create( { Origin3D<Length>, { -1, 0, 0 }, { 0, -10, 0 } } );
	pTrack3->Attach( std::move(pArc3), {0_m,10_m*pi/2} );

	auto pArc4 = ArcP::Make();
	pArc4->Create( { Origin3D<Length>, { 0, -1, 0 }, { 10, 0, 0 } } );
	pTrack4->Attach( std::move(pArc4), {0_m, 10_m*pi/2} );

	pTrack1->Couple( std::make_pair(pTrack1, Track::EndType::end), std::make_pair(pTrack2, Track::EndType::front) );
	pTrack2->Couple( std::make_pair(pTrack2, Track::EndType::end), std::make_pair(pTrack3, Track::EndType::front) );
	pTrack3->Couple( std::make_pair(pTrack3, Track::EndType::end), std::make_pair(pTrack4, Track::EndType::front) );
	pTrack4->Couple( std::make_pair(pTrack4, Track::EndType::end), std::make_pair(pTrack1, Track::EndType::front) );

	Location loc{ pTrack1, { 0_m, true } };

	Frame<Length,One> frame1, frame2;
	loc.Transition( frame1 );
	loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength() + pTrack4->GetLength() );
	loc.Transition( frame2 );
	assert( frame1.Equals( frame2, epsilon__length, 0.001f ) );

	loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength()/2 );
	auto pTrack = loc.GetTrack();
	assert( pTrack == pTrack3 );

	pTrack1->DeCouple();
	pTrack3->DeCouple();

	Release();
}