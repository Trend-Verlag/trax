//	trax track library
//	AD 2013 
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

#include "Section_Imp.h"

#include "common/NarrowCast.h"

#include <sstream>

namespace trax{
	using namespace common;
	using namespace spat;

///////////////////////////////////////
std::unique_ptr<Section> Section::Make( 
	SpecialSections type ) noexcept
{
	try{
		if( auto pSection = std::make_unique<Section_Imp>() ){
			pSection->Create(type);
			return pSection;
		}

		return nullptr;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
Section_Imp::Section_Imp() noexcept
	:	m_SectionType	{SpecialSections::unknown},
		m_GaugeLeft		{0},
		m_GaugeRight	{0}
{}

std::string	Section_Imp::TypeName() const noexcept{
	return "Section";
}

Section::SpecialSections Section_Imp::GetSectionType() const noexcept{
	return m_SectionType;
}

void Section_Imp::Create( SpecialSections specialSection )
{
	m_SectionType = specialSection;
	m_GaugeLeft = m_GaugeRight = 0;

	switch( m_SectionType ){
	case SpecialSections::unknown:
	case SpecialSections::empty:
	case SpecialSections::custom:
		SetCntPoints( 0 );
		break;
	case SpecialSections::standard:
		CreateStandardProfile( *this );
		break;
	case SpecialSections::vignol_UIC60:
		CreateVignolProfile_UIC60( *this );
		break;
	case SpecialSections::eepv7_grooved_convextread:
		SetCntPoints( 37 );
		Set( 0, { {-2.0999999_m,-0.699999988_m} } );
		Set( 1, { {-1.85000002_m,-0.340000033_m} } );
		Set( 2, { {-1.6400001_m,-0.221000001_m} } );
		Set( 3, { {-1.32000017_m,-0.200000018_m} } );
		Set( 4, { {-1.29999995_m,-0.180000007_m} } );
		Set( 5, { {-0.829999983_m,-0.170000017_m} } );
		Set( 6, { {-0.829999983_m,-0.151999995_m} } );
		Set( 7, { {-0.75_m,-0.129999995_m} } );
		Set( 8, { {-0.75_m,-0.0450000018_m} } );
		Set( 9, { {-0.777999997_m,-0.0219999999_m} } );
		Set( 10, { {-0.777999997_m,0.0_m} } );
		Set( 11, { {-0.74000001_m,0.00400000019_m} } );
		Set( 12, { {-0.702000022_m,0.0_m} }, true );
		Set( 13, { {-0.702000022_m,-0.0219999999_m} } );
		Set( 14, { {-0.730000079_m,-0.0450000018_m} } );
		Set( 15, { {-0.730000079_m,-0.129999995_m} } );
		Set( 16, { {-0.649999976_m,-0.151999995_m} } );
		Set( 17, { {-0.649999976_m,-0.170000017_m} } );
		Set( 18, { {0.0_m,-0.189999998_m} } );
		Set( 19, { {0.649999976_m,-0.170000017_m} } );
		Set( 20, { {0.649999976_m,-0.151999995_m} } );
		Set( 21, { {0.730000079_m,-0.129999995_m} } );
		Set( 22, { {0.730000079_m,-0.0450000018_m} } );
		Set( 23, { {0.702000022_m,-0.0219999999_m} } );
		Set( 24, { {0.702000022_m,0.0_m} }, false, true );
		Set( 25, { {0.74000001_m,0.00400000019_m} } );
		Set( 26, { {0.777999997_m,0.0_m} } );
		Set( 27, { {0.777999997_m,-0.0219999999_m} } );
		Set( 28, { {0.75_m,-0.0450000018_m} } );
		Set( 29, { {0.75_m,-0.129999995_m} } );
		Set( 30, { {0.829999983_m,-0.151999995_m} } );
		Set( 31, { {0.829999983_m,-0.170000017_m} } );
		Set( 32, { {1.29999995_m,-0.180000007_m} } );
		Set( 33, { {1.32000017_m,-0.200000018_m} } );
		Set( 34, { {1.6400001_m,-0.221000001_m} } );
		Set( 35, { {1.85000002_m,-0.340000033_m} } );
		Set( 36, { {2.0999999_m,-0.699999988_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_embankment_simple:
		SetCntPoints( 23 );
		Set( 0, { {-2.25000024_m,-18.0000019_m} } );
		Set( 1, { {-21.0000038_m,-18.0000019_m} } );
		Set( 2, { {-11.6599998_m,-9.00000095_m} } );
		Set( 3, { {-3._m,-0.659999967_m} } );
		Set( 4, { {-2.25000024_m,-0.640000045_m} } );
		Set( 5, { {-1.91000032_m,-0.410000026_m} } );
		Set( 6, { {-1.60000026_m,-0.200000033_m} } );
		Set( 7, { {-0.800000131_m,-0.150000021_m} } );
		Set( 8, { {-0.780000031_m,.0_m} } );
		Set( 9, { {-0.699999988_m,.0_m} }, true );
		Set( 10, { {-0.680000067_m,-0.150000021_m} } );
		Set( 11, { {0.0_m,-0.170000017_m} } );
		Set( 12, { {0.680000067_m,-0.150000021_m} } );
		Set( 13, { {0.699999988_m,.0_m} }, false, true );
		Set( 14, { {0.780000031_m,.0_m} } );
		Set( 15, { {0.800000131_m,-0.150000021_m} } );
		Set( 16, { {1.60000026_m,-0.200000033_m} } );
		Set( 17, { {1.91000032_m,-0.410000026_m} } );
		Set( 18, { {2.25000024_m,-0.640000045_m} } );
		Set( 19, { {3._m,-0.659999967_m} } );
		Set( 20, { {11.6599998_m,-9.00000095_m} } );
		Set( 21, { {21.0000038_m,-18.0000019_m} } );
		Set( 22, { {2.25000024_m,-18.0000019_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_concrete_roadway:
		SetCntPoints( 41 );
		Set( 0, { {-2.30500007_m,-0.899999976_m} } );
		Set( 1, { {-2.24000001_m,-0.400000036_m} } );
		Set( 2, { {-1.41400003_m,-0.379999995_m} } );
		Set( 3, { {-1.09000003_m,-0.219999999_m} } );
		Set( 4, { {-1.02999997_m,-0.180000007_m} } );
		Set( 5, { {-0.829999983_m,-0.170000017_m} } );
		Set( 6, { {-0.829999983_m,-0.151999995_m} } );
		Set( 7, { {-0.75_m,-0.129999995_m} } );
		Set( 8, { {-0.75_m,-0.0450000018_m} } );
		Set( 9, { {-0.777999997_m,-0.0200000014_m} } );
		Set( 10, { {-0.777999997_m,.0_m} } );
		Set( 11, { {-0.74000001_m,0.00400000019_m} } );
		Set( 12, { {-0.702000022_m,.0_m} }, true );
		Set( 13, { {-0.702000022_m,-0.0219999999_m} } );
		Set( 14, { {-0.730000079_m,-0.0450000018_m} } );
		Set( 15, { {-0.730000079_m,-0.129999995_m} } );
		Set( 16, { {-0.649999976_m,-0.151999995_m} } );
		Set( 17, { {-0.649999976_m,-0.170000017_m} } );
		Set( 18, { {-0.460000008_m,-0.180000007_m} } );
		Set( 19, { {-0.400000036_m,-0.219999999_m} } );
		Set( 20, { {.0_m,-0.243000001_m} } );
		Set( 21, { {0.400000036_m,-0.219999999_m} } );
		Set( 22, { {0.460000008_m,-0.180000007_m} } );
		Set( 23, { {0.649999976_m,-0.170000017_m} } );
		Set( 24, { {0.649999976_m,-0.151999995_m} } );
		Set( 25, { {0.730000079_m,-0.129999995_m} } );
		Set( 26, { {0.730000079_m,-0.0450000018_m} } );
		Set( 27, { {0.702000022_m,-0.0219999999_m} } );
		Set( 28, { {0.702000022_m,.0_m} }, false, true );
		Set( 29, { {0.74000001_m,0.00400000019_m} } );
		Set( 30, { {0.777999997_m,.0_m} } );
		Set( 31, { {0.777999997_m,-0.0200000014_m} } );
		Set( 32, { {0.75_m,-0.0450000018_m} } );
		Set( 33, { {0.75_m,-0.129999995_m} } );
		Set( 34, { {0.829999983_m,-0.151999995_m} } );
		Set( 35, { {0.829999983_m,-0.170000017_m} } );
		Set( 36, { {1.02999997_m,-0.180000007_m} } );
		Set( 37, { {1.09000003_m,-0.219999999_m} } );
		Set( 38, { {1.41400003_m,-0.379999995_m} } );
		Set( 39, { {2.24000001_m,-0.400000036_m} } );
		Set( 40, { {2.30500007_m,-0.899999976_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_rack_railway:
		SetCntPoints( 54 );
		Set( 0, { {0.0289999992_m,-0.172000006_m} } );
		Set( 1, { {0.0320000015_m,0.0400000028_m} } );
		Set( 2, { {-0.0209999997_m,0.0469999984_m} } );
		Set( 3, { {0.0300000012_m,0.0500000045_m} } );
		Set( 4, { {0.0300000012_m,0.0810000002_m} } );
		Set( 5, { {-0.0280000009_m,0.0790000036_m} } );
		Set( 6, { {-0.0300000012_m,-0.177000001_m} } );
		Set( 7, { {-0.150000006_m,-0.182000011_m} } );
		Set( 8, { {-0.0379999988_m,-0.173000008_m} } );
		Set( 9, { {-0.0209999997_m,0.0419999994_m} } );
		Set( 10, { {-0.0329999998_m,-0.0270000007_m} } );
		Set( 11, { {-0.0410000011_m,-0.0250000022_m} } );
		Set( 12, { {-0.0370000005_m,0.0879999995_m} } );
		Set( 13, { {0.0390000008_m,0.0879999995_m} } );
		Set( 14, { {0.0400000028_m,-0.0260000005_m} } );
		Set( 15, { {0.0350000001_m,-0.17900002_m} } );
		Set( 16, { {0.150000006_m,-0.182000011_m} } );
		Set( 17, { {-2.0999999_m,-0.699999988_m} } );
		Set( 18, { {-1.85000002_m,-0.340000033_m} } );
		Set( 19, { {-1.6400001_m,-0.221000001_m} } );
		Set( 20, { {-1.32000017_m,-0.200000018_m} } );
		Set( 21, { {-1.29999995_m,-0.180000007_m} } );
		Set( 22, { {-0.829999983_m,-0.170000017_m} } );
		Set( 23, { {-0.829999983_m,-0.151999995_m} } );
		Set( 24, { {-0.75_m,-0.129999995_m} } );
		Set( 25, { {-0.75_m,-0.0450000018_m} } );
		Set( 26, { {-0.777999997_m,-0.0219999999_m} } );
		Set( 27, { {-0.777999997_m,0_m} } );
		Set( 28, { {-0.74000001_m,0.00400000019_m} } );
		Set( 29, { {-0.702000022_m,0_m} }, true );
		Set( 30, { {-0.702000022_m,-0.0219999999_m} } );
		Set( 31, { {-0.730000079_m,-0.0450000018_m} } );
		Set( 32, { {-0.730000079_m,-0.129999995_m} } );
		Set( 33, { {-0.649999976_m,-0.151999995_m} } );
		Set( 34, { {-0.649999976_m,-0.170000017_m} } );
		Set( 35, { {.0_m,-0.189999998_m} } );
		Set( 36, { {0.649999976_m,-0.170000017_m} } );
		Set( 37, { {0.649999976_m,-0.151999995_m} } );
		Set( 38, { {0.730000079_m,-0.129999995_m} } );
		Set( 39, { {0.730000079_m,-0.0450000018_m} } );
		Set( 40, { {0.702000022_m,-0.0219999999_m} } );
		Set( 41, { {0.702000022_m,0_m} }, false, true );
		Set( 42, { {0.74000001_m,0.00400000019_m} } );
		Set( 43, { {0.777999997_m,0_m} } );
		Set( 44, { {0.777999997_m,-0.0219999999_m} } );
		Set( 45, { {0.75_m,-0.0450000018_m} } );
		Set( 46, { {0.75_m,-0.129999995_m} } );
		Set( 47, { {0.829999983_m,-0.151999995_m} } );
		Set( 48, { {0.829999983_m,-0.170000017_m} } );
		Set( 49, { {1.29999995_m,-0.180000007_m} } );
		Set( 50, { {1.32000017_m,-0.200000018_m} } );
		Set( 51, { {1.6400001_m,-0.221000001_m} } );
		Set( 52, { {1.85000002_m,-0.340000033_m} } );
		Set( 53, { {2.0999999_m,-0.699999988_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep7_track1435:
		SetCntPoints( 41 );
		Set( 0, { {-3.00000048_m,-0.660000086_m} } );
		Set( 1, { {-2.25_m,-0.640000105_m} } );
		Set( 2, { {-1.60000014_m,-0.200000018_m} } );
		Set( 3, { {-0.815000176_m,-0.170000017_m} } );
		Set( 4, { {-0.815000176_m,-0.159000024_m} } );
		Set( 5, { {-0.790000021_m,-0.158000007_m} } );
		Set( 6, { {-0.749000013_m,-0.140000001_m} } );
		Set( 7, { {-0.749000013_m,-0.0510000065_m} } );
		Set( 8, { {-0.779000163_m,-0.0400000066_m} } );
		Set( 9, { {-0.776000023_m,.0_m} } );
		Set( 10, { {-0.776000023_m,.0_m} } );
		Set( 11, { {-0.74000001_m,0.00500000082_m} } );
		Set( 12, { {-0.704000115_m,.0_m} } );
		Set( 13, { {-0.704000115_m,.0_m} }, true );
		Set( 14, { {-0.701000094_m,-0.0400000066_m} } );
		Set( 15, { {-0.731000006_m,-0.0510000065_m} } );
		Set( 16, { {-0.731000006_m,-0.140000001_m} } );
		Set( 17, { {-0.690000117_m,-0.158000007_m} } );
		Set( 18, { {-0.665000021_m,-0.159000024_m} } );
		Set( 19, { {-0.665000021_m,-0.170000017_m} } );
		Set( 20, { {.0_m,-0.190000027_m} } );
		Set( 21, { {0.665000021_m,-0.170000017_m} } );
		Set( 22, { {0.665000021_m,-0.159000024_m} } );
		Set( 23, { {0.690000117_m,-0.158000007_m} } );
		Set( 24, { {0.731000006_m,-0.140000001_m} } );
		Set( 25, { {0.731000006_m,-0.0510000065_m} } );
		Set( 26, { {0.701000094_m,-0.0400000066_m} } );
		Set( 27, { {0.704000115_m,.0_m} }, false, true );
		Set( 28, { {0.704000115_m,.0_m} } );
		Set( 29, { {0.74000001_m,0.00500000082_m} } );
		Set( 30, { {0.776000023_m,.0_m} } );
		Set( 31, { {0.776000023_m,.0_m} } );
		Set( 32, { {0.779000163_m,-0.0400000066_m} } );
		Set( 33, { {0.749000013_m,-0.0510000065_m} } );
		Set( 34, { {0.749000013_m,-0.140000001_m} } );
		Set( 35, { {0.790000021_m,-0.158000007_m} } );
		Set( 36, { {0.815000176_m,-0.159000024_m} } );
		Set( 37, { {0.815000176_m,-0.170000017_m} } );
		Set( 38, { {1.60000014_m,-0.200000018_m} } );
		Set( 39, { {2.25_m,-0.640000105_m} } );
		Set( 40, { {3.00000048_m,-0.660000086_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_simple:
		SetCntPoints( 15 );
		Set( 0, { {-1.80000007_m,-0.900000036_m} } );
		Set( 1, { {-1.70000005_m,-0.600000083_m} } );
		Set( 2, { {-1.24000013_m,-0.200000033_m} } );
		Set( 3, { {-0.800000131_m,-0.150000021_m} } );
		Set( 4, { {-0.780000031_m,.0_m} } );
		Set( 5, { {-0.699999988_m,.0_m} }, true );
		Set( 6, { {-0.680000067_m,-0.150000021_m} } );
		Set( 7, { {.0_m,-0.170000017_m} } );
		Set( 8, { {0.680000067_m,-0.150000021_m} } );
		Set( 9, { {0.699999988_m,.0_m} }, false, true );
		Set( 10, { {0.780000031_m,.0_m} } );
		Set( 11, { {0.800000131_m,-0.150000021_m} } );
		Set( 12, { {1.24000013_m,-0.200000033_m} } );
		Set( 13, { {1.70000005_m,-0.600000083_m} } );
		Set( 14, { {1.80000007_m,-0.900000036_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_grooved_convextread:
		SetCntPoints( 41 );
		Set( 0, { {-1.80000019_m,-0.900000095_m} } );
		Set( 1, { {-1.70000005_m,-0.600000024_m} } );
		Set( 2, { {-1.24000001_m,-0.200000018_m} } );
		Set( 3, { {-0.815000176_m,-0.170000017_m} } );
		Set( 4, { {-0.815000176_m,-0.159000024_m} } );
		Set( 5, { {-0.790000021_m,-0.158000007_m} } );
		Set( 6, { {-0.749000013_m,-0.140000001_m} } );
		Set( 7, { {-0.749000013_m,-0.0510000065_m} } );
		Set( 8, { {-0.779000163_m,-0.0400000066_m} } );
		Set( 9, { {-0.776000023_m,.0_m} } );
		Set( 10, { {-0.776000023_m,.0_m} } );
		Set( 11, { {-0.74000001_m,0.00500000082_m} } );
		Set( 12, { {-0.704000115_m,.0_m} } );
		Set( 13, { {-0.704000115_m,.0_m} }, true );
		Set( 14, { {-0.701000094_m,-0.0400000066_m} } );
		Set( 15, { {-0.731000006_m,-0.0510000065_m} } );
		Set( 16, { {-0.731000006_m,-0.140000001_m} } );
		Set( 17, { {-0.690000117_m,-0.158000007_m} } );
		Set( 18, { {-0.665000021_m,-0.159000024_m} } );
		Set( 19, { {-0.665000021_m,-0.170000017_m} } );
		Set( 20, { {.0_m,-0.190000027_m} } );
		Set( 21, { {0.665000021_m,-0.170000017_m} } );
		Set( 22, { {0.665000021_m,-0.159000024_m} } );
		Set( 23, { {0.690000117_m,-0.158000007_m} } );
		Set( 24, { {0.731000006_m,-0.140000001_m} } );
		Set( 25, { {0.731000006_m,-0.0510000065_m} } );
		Set( 26, { {0.701000094_m,-0.0400000066_m} } );
		Set( 27, { {0.704000115_m,.0_m} }, false, true );
		Set( 28, { {0.704000115_m,.0_m} } );
		Set( 29, { {0.74000001_m,0.00500000082_m} } );
		Set( 30, { {0.776000023_m,.0_m} } );
		Set( 31, { {0.776000023_m,.0_m} } );
		Set( 32, { {0.779000163_m,-0.0400000066_m} } );
		Set( 33, { {0.749000013_m,-0.0510000065_m} } );
		Set( 34, { {0.749000013_m,-0.140000001_m} } );
		Set( 35, { {0.790000021_m,-0.158000007_m} } );
		Set( 36, { {0.815000176_m,-0.159000024_m} } );
		Set( 37, { {0.815000176_m,-0.170000017_m} } );
		Set( 38, { {1.24000001_m,-0.200000018_m} } );
		Set( 39, { {1.70000005_m,-0.600000024_m} } );
		Set( 40, { {1.80000019_m,-0.900000095_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_flatballast:
		SetCntPoints( 14 );
		Set( 0, { {-2.5999999_m,-0.449999988_m} } );
		Set( 1, { {-2.5_m,-0.349999994_m} } );
		Set( 2, { {-2.25_m,-0.300000012_m} } );
		Set( 3, { {-1.39999998_m,-0.254999995_m} } );
		Set( 4, { {-0.74000001_m,-0.200000018_m} }, true );
		Set( 5, { {-0.449999988_m,-0.195000008_m} } );
		Set( 6, { {-0.109999999_m,-0.224999994_m} } );
		Set( 7, { {0.109999999_m,-0.224999994_m} } );
		Set( 8, { {0.449999988_m,-0.195000008_m} } );
		Set( 9, { {0.787999988_m,-0.200000018_m} }, false, true );
		Set( 10, { {1.39999998_m,-0.254999995_m} } );
		Set( 11, { {2.25_m,-0.300000012_m} } );
		Set( 12, { {2.5_m,-0.349999994_m} } );
		Set( 13, { {2.5999999_m,-0.449999988_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eepv7_track1000:
		SetCntPoints( 41 );
		Set( 0, { {-1.35000002_m,-0.900000095_m} } );
		Set( 1, { {-1.28000021_m,-0.600000024_m} } );
		Set( 2, { {-0.930000186_m,-0.200000018_m} } );
		Set( 3, { {-0.641000152_m,-0.170000017_m} } );
		Set( 4, { {-0.641000152_m,-0.159000024_m} } );
		Set( 5, { {-0.616000116_m,-0.158000007_m} } );
		Set( 6, { {-0.575000107_m,-0.140000001_m} } );
		Set( 7, { {-0.575000107_m,-0.0510000065_m} } );
		Set( 8, { {-0.602000117_m,.0_m} } );
		Set( 9, { {-0.605000019_m,-0.0400000066_m} } );
		Set( 10, { {-0.601000011_m,.0_m} } );
		Set( 11, { {-0.566000104_m,0.00500000082_m} } );
		Set( 12, { {-0.531000018_m,0._m} } );
		Set( 13, { {-0.530000091_m,0._m} }, true );
		Set( 14, { {-0.52700001_m,-0.0400000066_m} } );
		Set( 15, { {-0.557000101_m,-0.0510000065_m} } );
		Set( 16, { {-0.557000101_m,-0.140000001_m} } );
		Set( 17, { {-0.516000092_m,-0.158000007_m} } );
		Set( 18, { {-0.491000086_m,-0.159000024_m} } );
		Set( 19, { {-0.491000086_m,-0.170000017_m} } );
		Set( 20, { {0._m,-0.190000027_m} } );
		Set( 21, { {0.491000086_m,-0.170000017_m} } );
		Set( 22, { {0.491000086_m,-0.159000024_m} } );
		Set( 23, { {0.516000092_m,-0.158000007_m} } );
		Set( 24, { {0.557000101_m,-0.140000001_m} } );
		Set( 25, { {0.557000101_m,-0.0510000065_m} } );
		Set( 26, { {0.52700001_m,-0.0400000066_m} } );
		Set( 27, { {0.530000091_m,0._m} }, false, true );
		Set( 28, { {0.531000018_m,0._m} } );
		Set( 29, { {0.566000104_m,0.00500000082_m} } );
		Set( 30, { {0.601000011_m,0._m} } );
		Set( 31, { {0.602000117_m,0._m} } );
		Set( 32, { {0.605000019_m,-0.0400000066_m} } );
		Set( 33, { {0.575000107_m,-0.0510000065_m} } );
		Set( 34, { {0.575000107_m,-0.140000001_m} } );
		Set( 35, { {0.616000116_m,-0.158000007_m} } );
		Set( 36, { {0.641000152_m,-0.159000024_m} } );
		Set( 37, { {0.641000152_m,-0.170000017_m} } );
		Set( 38, { {0.930000186_m,-0.200000018_m} } );
		Set( 39, { {1.28000021_m,-0.600000024_m} } );
		Set( 40, { {1.35000002_m,-0.900000095_m} } );
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_simple2:
		SetCntPoints( 19 );
		Set( 0, { {-2.25000024_m,-0.659999967_m} } );
		Set( 1, { {-3._m,-0.659999967_m} } );
		Set( 2, { {-2.25000024_m,-0.640000045_m} } );
		Set( 3, { {-1.91000032_m,-0.410000026_m} } );
		Set( 4, { {-1.60000026_m,-0.200000033_m} } );
		Set( 5, { {-0.800000131_m,-0.150000021_m} } );
		Set( 6, { {-0.780000031_m,0._m} } );
		Set( 7, { {-0.699999988_m,0._m} }, true );
		Set( 8, { {-0.680000067_m,-0.150000021_m} } );
		Set( 9, { {0._m,-0.170000017_m} } );
		Set( 10, { {0.680000067_m,-0.150000021_m} } );
		Set( 11, { {0.699999988_m,0._m} }, false, true );
		Set( 12, { {0.780000031_m,0._m} } );
		Set( 13, { {0.800000131_m,-0.150000021_m} } );
		Set( 14, { {1.60000026_m,-0.200000033_m} } );
		Set( 15, { {1.91000032_m,-0.410000026_m} } );
		Set( 16, { {2.25000024_m,-0.640000045_m} } );
		Set( 17, { {3._m,-0.659999967_m} } );
		Set( 18, { {2.25000024_m,-0.659999967_m} } );	
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_simple3:
		SetCntPoints( 15 );
		Set( 0, { {-1.35000002_m,-0.900000036_m} } ); 
		Set( 1, { {-1.28000009_m,-0.600000083_m} } ); 
		Set( 2, { {-0.930000007_m,-0.200000033_m} } ); 
		Set( 3, { {-0.600000083_m,-0.150000021_m} } ); 
		Set( 4, { {-0.590000093_m,0._m} } ); 
		Set( 5, { {-0.530000091_m,0._m} }, true ); 
		Set( 6, { {-0.51000005_m,-0.150000021_m} } ); 
		Set( 7, { {0._m,-0.170000017_m} } ); 
		Set( 8, { {0.51000005_m,-0.150000021_m} } ); 
		Set( 9, { {0.530000091_m,0._m} }, false, true ); 
		Set( 10, { {0.590000093_m,0._m} } ); 
		Set( 11, { {0.600000083_m,-0.150000021_m} } ); 
		Set( 12, { {0.930000007_m,-0.200000033_m} } ); 
		Set( 13, { {1.28000009_m,-0.600000083_m} } ); 
		Set( 14, { {1.35000002_m,-0.900000036_m} } ); 
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::eep_flatballast_rods:
		SetCntPoints( 18 );
		Set( 0, { {0.665000021_m,-0.172000006_m} } ); 
		Set( 1, { {0.665000021_m,-0.160000011_m} } ); 
		Set( 2, { {0.689999998_m,-0.155000001_m} } ); 
		Set( 3, { {0.730000079_m,-0.140000001_m} } ); 
		Set( 4, { {0.730000079_m,-0.0500000045_m} } ); 
		Set( 5, { {0.703000069_m,-0.0400000028_m} } ); 
		Set( 6, { {0.703000069_m,-0.0400000028_m} } ); 
		Set( 7, { {0.705000043_m,-0.00300000003_m} } ); 
		Set( 8, { {0.717999995_m,0._m} } ); 
		Set( 9, { {0.762000024_m,0._m} } ); 
		Set( 10, { {0.762000024_m,0._m} } ); 
		Set( 11, { {0.774999976_m,-0.00300000003_m} } ); 
		Set( 12, { {0.77700007_m,-0.0400000028_m} } ); 
		Set( 13, { {0.75_m,-0.0500000045_m} } ); 
		Set( 14, { {0.75_m,-0.140000001_m} } ); 
		Set( 15, { {0.790000021_m,-0.155000001_m} } ); 
		Set( 16, { {0.814999998_m,-0.160000011_m} } ); 
		Set( 17, { {0.814999998_m,-0.172000006_m} } ); 
		CalculateNormals();
		CalculateTextureCoordinates();
		break;
	case SpecialSections::square:
		CreatePipeline(*this, 4 );
		break;
	case SpecialSections::hexagon:
		CreatePipeline(*this, 6 );
		break;
	case SpecialSections::pipeline:
		CreatePipeline(*this, 12 );
		break;
	}
}

int Section_Imp::CountPoints() const{
	return narrow_cast<int>(m_SectionPoints.size());
}

void Section_Imp::SetCntPoints( int cnt ){
	m_SectionPoints.resize( cnt );
}

const Section::SectionPoint& Section_Imp::Get( int id ) const{
	return m_SectionPoints.at(id);
}

Interval<One> Section_Imp::TextureExtent() const{
	if( m_SectionPoints.size() < 2 )
		throw std::logic_error( "Profile not created yet!" );

	Interval<One> extent{ m_SectionPoints.front().t };

	for( const auto& sectionPoint : m_SectionPoints )
		extent.Expand( sectionPoint.t );

	return extent;
}

Length Section_Imp::PolygonChainLength() const noexcept{
	if( m_SectionPoints.size() < 2 )
		return 0_m;

	Length length = 0_m;
	Position2D<Length> pos{ m_SectionPoints.front().p };
	for( const SectionPoint& point : m_SectionPoints )
	{	
		length += (point.p - pos).Length();
		pos = point.p;
	}

	return length;
}

Length Section_Imp::Gauge() const noexcept{
	if( m_SectionPoints.size() < 2 )
		return 0_m;

	return (m_SectionPoints[m_GaugeRight].p - m_SectionPoints[m_GaugeLeft].p).Length();
}

spat::Rect<Length> Section_Imp::Clearance() const noexcept
{
	spat::Rect<Length> clearance{ 0_m, 0_m };

	for( const SectionPoint& point : m_SectionPoints )	
		clearance.Expand( point.p );

	return clearance;
}

void Section_Imp::Set( int id, const SectionPoint& pt, bool bGaugeLeft, bool bGaugeRight )
{
	m_SectionPoints.at(id) = pt;

	if( bGaugeLeft ) 
		m_GaugeLeft = id;
	if( bGaugeRight ) 
		m_GaugeRight = id;
}

void Section_Imp::Scale( One x_scale, One y_scale ){
	for( auto piter = m_SectionPoints.begin(); 
				piter != m_SectionPoints.end(); ++piter )
		(*piter).Scale(x_scale,y_scale);
}

void Section_Imp::CalculateNormals() noexcept{
	if( m_SectionPoints.size() == 0 )
		return;

	if( m_SectionPoints.size() == 1 ){
		m_SectionPoints[0].n = Vector2D<One>(0, 1);
		return;
	}

	Vector2D<Length> dpi = m_SectionPoints[1].p - m_SectionPoints[0].p;
	dpi.Normalize();
	m_SectionPoints[0].n = Normalize(Vector2D<Length>{-dpi.dy,dpi.dx}).second;
	
	unsigned int i = 1;
	for( ; i < m_SectionPoints.size()-1; ++i )
	{
		Vector2D<Length> _dmi = m_SectionPoints[static_cast<std::size_t>(i)-1].p - m_SectionPoints[i].p;
		Vector2D<Length> _dpi = m_SectionPoints[static_cast<std::size_t>(i)+1].p - m_SectionPoints[i].p;

		_dmi.Normalize();
		_dpi.Normalize();

		const Vector2D<Length> _n1(_dmi.dy,-_dmi.dx);
		const Vector2D<Length> _n2(-_dpi.dy,_dpi.dx);

		m_SectionPoints[i].n = Normalize( _n1 + _n2 ).second;
	}

	Vector2D<Length> dmi = m_SectionPoints[i-1].p - m_SectionPoints[i].p;
	dmi.Normalize();
	m_SectionPoints[i].n = Normalize(Vector2D<Length>{ dmi.dy,-dmi.dx }).second;
}

void Section_Imp::CalculateTextureCoordinates(){
	if( m_SectionPoints.size() == 0 )
		return;

	if( m_SectionPoints.size() == 1 ){
		m_SectionPoints[0].t = 0;
		return;
	}

	std::vector<Length> lengthes{ 1, 0_m };
	for( std::size_t i = 1; i < m_SectionPoints.size(); ++i )
		lengthes.push_back( lengthes.back() + (m_SectionPoints[i].p - m_SectionPoints[i-1].p).Length() );

	if( lengthes.back() ){
		for( std::size_t i = 0; i < m_SectionPoints.size(); ++i )
			m_SectionPoints[i].t = lengthes[i] / lengthes.back();
	}
}
///////////////////////////////////////
std::string ToString( Section::SpecialSections specialSection ){
	switch( specialSection ){
	case Section::SpecialSections::unknown:
		return "unknown";
	case Section::SpecialSections::empty:
		return "empty";
	case Section::SpecialSections::custom:
		return "custom";
	case Section::SpecialSections::standard:
		return "standard";
	case Section::SpecialSections::vignol_UIC60:
		return "vignol_UIC60";
	case Section::SpecialSections::eepv7_grooved_convextread:
		return "eepv7_grooved_convextread";
	case Section::SpecialSections::eep_embankment_simple:
		return "eep_embankment_simple";
	case Section::SpecialSections::eep_concrete_roadway:
		return "eep_concrete_roadway";
	case Section::SpecialSections::eep_rack_railway:
		return "eep_rack_railway";
	case Section::SpecialSections::eep7_track1435:
		return "eep7_track1435";
	case Section::SpecialSections::eep_simple:
		return "eep_simple";
	case Section::SpecialSections::eep_grooved_convextread:
		return "eep_grooved_convextread";
	case Section::SpecialSections::eep_flatballast:
		return "eep_flatballast";
	case Section::SpecialSections::eepv7_track1000:
		return "eepv7_track1000";
	case Section::SpecialSections::eep_simple2:
		return "eep_simple2";
	case Section::SpecialSections::eep_simple3:
		return "eep_simple3";
	case Section::SpecialSections::eep_flatballast_rods:
		return "eep_flatballast_rods";
	case Section::SpecialSections::square:
		return "square";
	case Section::SpecialSections::hexagon:
		return "hexagon";
	case Section::SpecialSections::pipeline:
		return "pipeline";
	default:
		assert( !"Unknown Section::SpecialSections enumerator!" );
		return "unknown";
	}
}

Section::SpecialSections SpecialSection( const std::string& string ){
	if( string == "standard" )
		return Section::SpecialSections::standard;
	else if( string == "eepv7_grooved_convextread" )
		return Section::SpecialSections::eepv7_grooved_convextread;
	else if( string == "eep_embankment_simple" )
		return Section::SpecialSections::eep_embankment_simple;
	else if( string == "vignol_UIC60" )
		return Section::SpecialSections::vignol_UIC60;
	else if( string == "custom" )
		return Section::SpecialSections::custom;
	else if( string == "empty" )
		return Section::SpecialSections::empty;
	else if( string == "unknown" )
		return Section::SpecialSections::unknown;
	else if( string == "eep_concrete_roadway" )
		return Section::SpecialSections::eep_concrete_roadway;
	else if( string == "eep_rack_railway" )
		return Section::SpecialSections::eep_rack_railway;
	else if( string == "eep7_track1435" )
		return Section::SpecialSections::eep7_track1435;
	else if( string == "eep_simple" )
		return Section::SpecialSections::eep_simple;
	else if( string == "eep_grooved_convextread" )
		return Section::SpecialSections::eep_grooved_convextread;
	else if( string == "eep_flatballast" )
		return Section::SpecialSections::eep_flatballast;
	else if( string == "eepv7_track1000" )
		return Section::SpecialSections::eepv7_track1000;
	else if( string == "eep_simple2" )
		return Section::SpecialSections::eep_simple2;
	else if( string == "eep_simple3" )
		return Section::SpecialSections::eep_simple3;
	else if( string == "eep_flatballast_rods" )
		return Section::SpecialSections::eep_flatballast_rods;
	else if( string == "square" )
		return Section::SpecialSections::square;
	else if( string == "hexagon" )
		return Section::SpecialSections::hexagon;
	else if( string == "pipeline" )
		return Section::SpecialSections::pipeline;

	std::ostringstream stream;
	stream << "Invalid Section::SpecialSections string: " << string << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

void CreateStandardProfile( Section& section ){
	section.SetCntPoints( 15 );

	section.Set( 0, { {-1.800_m,-.900_m}, {0, 1}, 1.0f - 0.0f } );
	section.Set( 1, { {-1.700_m,-.600_m}, {0,1}, 1.0f - 0.01f } );
	section.Set( 2, { {-1.240_m,-.200_m}, {0,1}, 1.0f - 0.3f } );
	section.Set( 3, { {-.800_m,-.150_m}	, {0,1}, 1.0f - 0.515f } );
	section.Set( 4, { {-.780_m,0.0_m}	, {0,1}, 1.0f - 0.58f } );
	section.Set( 5, { {-.700_m, 0.0_m}	, {0,1}, 1.0f - 0.625f }, true );
	section.Set( 6, { {-.680_m, -.150_m}, {0,1}, 1.0f - 0.7f } );
	section.Set( 7, { {0.0_m, -.170_m}	, {0,1}, 1.0f - 1.0f } );
	section.Set( 8, { {.680_m, -.150_m}	, {0,1}, 1.0f - 0.7f } );
	section.Set( 9, { {.700_m, 0.0_m}	, {0,1}, 1.0f - 0.625f }, false, true );
	section.Set( 10, { {.780_m, 0.0_m}	, {0,1}, 1.0f - 0.58f } );
	section.Set( 11, { {.800_m, -.150_m}, {0,1}, 1.0f - 0.515f } );
	section.Set( 12, { {1.240_m, -.200_m}, {0,1}, 1.0f - 0.3f } );
	section.Set( 13, { {1.700_m, -.600_m}, {0,1}, 1.0f - 0.01f } );
	section.Set( 14, { {1.800_m, -.900_m}, {0,1}, 1.0f - 0.0f } );

	section.CalculateNormals();
}

void CreateVignolProfile_UIC60( Section& section ){
	section.SetCntPoints( 35 );

	section.Set( 0, { {-1.800_m, -.900_m}	, {0,1}, 1.0f - 0.0f } );
	section.Set( 1, { {-1.700_m, -.600_m}	, {0,1}, 1.0f - 0.01f } );
	section.Set( 2, { {-1.240_m, -.200_m}	, {0,1}, 1.0f - 0.3f } );

	section.Set( 3, { {-0.8275_m, -0.172_m}	, {0,1}, 1.0f - 0.515f } );
	section.Set( 4, { {-0.7605_m, -0.142_m}	, {0,1}, 1.0f - 0.530f } );
	section.Set( 5, { {-0.7605_m, -0.042_m}	, {0,1}, 1.0f - 0.55f } );
	section.Set( 6, { {-0.7875_m, -0.032_m}	, {0,1}, 1.0f - 0.56f } );
	section.Set( 7, { {-0.7875_m, -0.012_m}	, {0,1}, 1.0f - 0.57f } );
	section.Set( 8, { {-0.7775_m, -0.003_m}	, {0,1}, 1.0f - 0.58f } );
	section.Set( 9, { {-0.7675_m, 0.0_m}	, {0,1}, 1.0f - 0.60f } );
	section.Set( 10, { {-0.7375_m, 0.0_m}	, {0,1}, 1.0f - 0.610f }, true );
	section.Set( 11, { {-0.7275_m, -0.003_m}, {0,1}, 1.0f - 0.615f } );
	section.Set( 12, { {-0.7175_m, -0.012_m}, {0,1}, 1.0f - 0.625f } );
	section.Set( 13, { {-0.7175_m, -0.032_m}, {0,1}, 1.0f - 0.645f } );
	section.Set( 14, { {-0.7445_m, -0.042_m}, {0,1}, 1.0f - 0.655f } );
	section.Set( 15, { {-0.7445_m, -0.142_m}, {0,1}, 1.0f - 0.675f } );
	section.Set( 16, { {-0.6775_m, -0.172_m}, {0,1}, 1.0f - 0.7f } );

	section.Set( 17, { {0.0_m, -.170_m}		, {0,1}, 1.0f - 1.0f } );

	section.Set( 18, { {0.6775_m, -0.172_m}	, {0,1}, 1.0f - 0.7f } );
	section.Set( 19, { {0.7445_m, -0.142_m}	, {0,1}, 1.0f - 0.675f } );
	section.Set( 20, { {0.7445_m, -0.042_m}	, {0,1}, 1.0f - 0.655f } );
	section.Set( 21, { {0.7175_m, -0.032_m}	, {0,1}, 1.0f - 0.645f } );
	section.Set( 22, { {0.7175_m, -0.012_m}	, {0,1}, 1.0f - 0.625f } );
	section.Set( 23, { {0.7275_m, -0.003_m}	, {0,1}, 1.0f - 0.615f } );
	section.Set( 24, { {0.7375_m, 0.0_m}	, {0,1}, 1.0f - 0.610f }, false, true );
	section.Set( 25, { {0.7675_m, 0.0_m}	, {0,1}, 1.0f - 0.60f } );
	section.Set( 26, { {0.7775_m, -0.003_m}	, {0,1}, 1.0f - 0.58f } );
	section.Set( 27, { {0.7875_m, -0.012_m}	, {0,1}, 1.0f - 0.57f } );
	section.Set( 28, { {0.7875_m, -0.032_m}	, {0,1}, 1.0f - 0.56f } );
	section.Set( 29, { {0.7605_m, -0.042_m}	, {0,1}, 1.0f - 0.55f } );
	section.Set( 30, { {0.7605_m, -0.142_m}	, {0,1}, 1.0f - 0.530f } );
	section.Set( 31, { {0.8275_m, -0.172_m}	, {0,1}, 1.0f - 0.515f } );

	section.Set( 32, { {1.240_m, -.200_m}	, {0,1}, 1.0f - 0.3f } );
	section.Set( 33, { {1.700_m, -.600_m}	, {0,1}, 1.0f - 0.01f } );
	section.Set( 34, { {1.800_m, -.900_m}	, {0,1}, 1.0f - 0.0f } );

	section.CalculateNormals();
}

void CreatePipeline( Section& section, int segments ){
	if( segments < 0 )
		throw std::invalid_argument( "Segment count can not be negative!" );

	section.SetCntPoints( segments+1 );

	const Angle angle = 360_deg / segments;
	Vector2D<One> pointer{ 0, -1 };
	for( int i = 0; i <= segments; ++i ){
		section.Set( i, { Origin2D<Length> + 1_m * pointer, pointer, i*1.0f/segments } );
		pointer.Rotate( -angle );
	}
}
///////////////////////////////////////
}