// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "spat/Spatial.h"
#include "spat/support/SpatSupportXML.h"
#include "spat/BoostTestSpatialHelpers.h"

#include "dim/DimensionedValues.h"
#include "dim/support/DimSupportStream.h"
#include "dim/BoostTestDimensionedValuesHelpers.h"

#include <boost/property_tree/ptree.hpp>

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable: 4459) // warning C4459: declaration of 'X' hides global declaration
#include <boost/property_tree/xml_parser.hpp>
#endif
#	if defined(_MSC_VER)
#	pragma warning(pop)
#endif

using namespace spat;
using namespace spat::ptreesupport;
using namespace dim;
using namespace std;


BOOST_AUTO_TEST_SUITE(xml_tests)
/*
BOOST_FIXTURE_TEST_CASE( tracks_rw_roundtrip, TrackCircle )
// write - read - write roundtrip with bytewise comparison
{
	using namespace trax;
	using boost::property_tree::ptree;
	ptree ptw;

	int cntTracks = m_pTrackSystem->Count();

	ptw << *m_pModuleSet->GetActive();

	write_xml( "..\\..\\Fixtures\\Module.xml", ptw, std::locale(),
		boost::property_tree::xml_parser::xml_writer_settings<std::string>( '\t', 1 ) );

	ptree ptr;

	read_xml("..\\..\\Fixtures\\Module.xml", ptr);

	PTReader reader( *m_pModuleSet.get(), m_Factory, ptr );
	
	cmnd::Macro macro;
	reader.Read( macro );

	macro.Execute();

	TrackSystem* pTrackSystem2 = m_pModuleSet->GetActive()->GetTrackSystem();
	BOOST_CHECK( pTrackSystem2 );
	BOOST_CHECK_EQUAL( pTrackSystem2->Count(), cntTracks );

	ptree ptw2;
	ptw2 << *m_pModuleSet->GetActive();

	write_xml("..\\..\\Fixtures\\Module2.xml", ptw2, std::locale(), 
		boost::property_tree::xml_parser::xml_writer_settings<std::string>( '\t', 1 ) );

	// compare files:
	std::ifstream TestFile1( "..\\..\\Fixtures\\Module.xml" );
	std::ifstream TestFile2( "..\\..\\Fixtures\\Module2.xml" );

	std::istream_iterator<char> b1(TestFile1), e1;
    std::istream_iterator<char> b2(TestFile2), e2;

    BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}
*/
//
//BOOST_FIXTURE_TEST_CASE( testTraxFilerollingstock_frame, TrackCircle )
//	// Check wether writing to a ptree and reading again changes the
//	// position and orientation of a rolling stock.
//{
//	std::vector<Position2D<Real>> axelpositions;
//	axelpositions.push_back( Position2D<Real>(-9,0.5) );
//		axelpositions.push_back( Position2D<Real>(-10,0.5) );
//		axelpositions.push_back( Position2D<Real>(-8,0.5) );
//	axelpositions.push_back( Position2D<Real>( 9,0.5) );
//		axelpositions.push_back( Position2D<Real>( 8,0.5) );
//		axelpositions.push_back( Position2D<Real>(10,0.5) );
//
//	std::shared_ptr<WheelFrame> pRollingStock = Factory::Instance().CreateWheelFrame( m_pModuleSet->GetSimulator().get(), "2'2'", axelpositions );
//	BOOST_CHECK(pRollingStock);
//
//	pRollingStock->Rail( m_pTrack1, TrackLocation( m_pTrack1->Length() / 2, true ) );
//	BOOST_CHECK(pRollingStock->IsRailed());
//
//	std::shared_ptr<Train> pTrain = Factory::Instance().CreateTrain();
//	pTrain->RollingStock( pRollingStock, Orientation::para );
//
//	Frame<Real> frameA;
//	pRollingStock->Get( frameA );
//
//	std::shared_ptr<Fleet> pFleet = Factory::Instance().CreateFleet();
//	m_pModuleSet->GetActive()->Attach( pFleet );
//
//	pFleet->Add( pRollingStock );
//	pFleet->Add( pTrain );
//
//	using namespace trax;
//	using boost::property_tree::ptree;
//
//	ptree ptr;
//	ptr << *m_pModuleSet->GetActive();
//	m_pModuleSet->GetActive()->Clear();
//	
//	PTReader reader( *m_pModuleSet.get(), m_Factory, ptr );
//	cmnd::Macro macro;
//	reader.Read( macro );
//
//	macro.Execute();
//
//	Frame<Real> frameB;
//	std::shared_ptr<Bogie> pBogie = dynamic_cast<Train*>(m_pModuleSet->GetActive()->GetFleet()->GetActive().get())->Get( 0 );
//	BOOST_CHECK(pBogie);
//	pBogie->Get( frameB );
//	
//	BOOST_CHECK(frameA.Equals( frameB, 0.001f ));
//}
//
//BOOST_FIXTURE_TEST_CASE ( testTraxFileRoundtrip, TraxFixture )
//	// reading and writing an trax file. 
//{
//	using namespace trax;
//	using boost::property_tree::ptree;
//
//	ptree ptr;
//	read_xml("..\\..\\Fixtures\\TestTrack.xml", ptr);
//
//	PTReader reader( *m_pModuleSet.get(), m_Factory, ptr );
//	
//	cmnd::Macro macro;
//	reader.Read( macro );
//
//	macro.Execute();
//
//	ptree ptr2;
//	ptr2 << *m_pModuleSet->GetActive();
//
//	write_xml("..\\..\\Fixtures\\TestTrackWrite.xml", ptr2, std::locale(), 
//		boost::property_tree::xml_parser::xml_writer_settings<std::string>( '\t', 1 ) );
//
//	ptree ptr3;
//	read_xml("..\\..\\Fixtures\\TestTrackWrite.xml", ptr3);
//
//
//	int nodescounter = 0;
//	BOOST_CHECK( Compare( ptr, ptr3, 0.5f, std::cout, nodescounter ) );
////	BOOST_CHECK( Compare( ptr, ptr3, 0.03f, std::cout, nodescounter ) );
//}

BOOST_AUTO_TEST_CASE( testRead )
{
	using boost::property_tree::ptree;

	ptree ptw;

	Vector<Real> VecX( Vector<Real>::specials::ex );
	Vector<Real> VecY( Vector<Real>::specials::ey );
	Vector<Real> VecZ( Vector<Real>::specials::ez );
	Frame<Real> frame;
	frame.Init();

	ptree ptSutrack;
	ptSutrack << VecX << VecY << VecZ;
	ptSutrack << frame;
	ptw.add_child( "sutrackp", ptSutrack );

	write_xml( "Write.xml", ptw, std::locale(),
		boost::property_tree::xml_parser::xml_writer_settings<std::string>( '\t', 1 ) );

	ptree pt;
	read_xml( "Write.xml", pt );

	Vector<Real> VecXB;
	Vector<Real> VecYB;
	Vector<Real> VecZB;
	Frame<Real> frameB;

	ptSutrack = pt.get_child( "sutrackp" );

	ptSutrack >> VecXB >> VecYB >> VecZB;
	ptSutrack >> frameB;

	BOOST_CHECK_EQUAL( VecX, VecXB );
	BOOST_CHECK_EQUAL( VecY, VecYB );
	BOOST_CHECK_EQUAL( VecZ, VecZB );
	BOOST_CHECK_EQUAL( frame, frameB );
}

BOOST_AUTO_TEST_CASE( testReadSpatDim )
{
	using boost::property_tree::ptree;

	ptree ptw;

	Vector<Velocity> VecX( Vector<Velocity>::specials::ex );
	Vector<Force> VecY( Vector<Force>::specials::ey );
	Vector<Torque> VecZ( Vector<Torque>::specials::ez );
	Frame<Length,One> frame;
	frame.Init();

	ptree ptSutrack;
	ptSutrack << VecX << VecY << VecZ;
	ptSutrack << frame;
	ptw.add_child( "sutrackp", ptSutrack );

	write_xml( "WriteDim.xml", ptw, std::locale(),
		boost::property_tree::xml_parser::xml_writer_settings<std::string>( '\t', 1 ) );

	ptree pt;
	read_xml( "WriteDim.xml", pt );

	Vector<Velocity> VecXB;
	Vector<Force> VecYB;
	Vector<Torque> VecZB;
	Frame<Length,One> frameB;

	ptSutrack = pt.get_child( "sutrackp" );

	ptSutrack >> VecXB >> VecYB >> VecZB;
	ptSutrack >> frameB;

	BOOST_CHECK_EQUAL( VecX, VecXB );
	BOOST_CHECK_EQUAL( VecY, VecYB );
	BOOST_CHECK_EQUAL( VecZ, VecZB );
	BOOST_CHECK_EQUAL( frame, frameB );
}


BOOST_AUTO_TEST_SUITE_END()
#endif