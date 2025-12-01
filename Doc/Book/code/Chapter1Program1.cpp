
#include "trax/Units.h"
#include "spat/Dump.h"
#include "common/Helpers.h"

#include <iostream>
#include <string>


/// \brief  A sample program that runs the code from Chapter 1.
int main(){
	using namespace trax;
	assert( CheckUnits() );

	SetDefaultStreamOfLength( _m );


	Mass mass1 = 60.4_kg;
	Mass mass2 = 60_kg + 400_g;

	Mass mass3 = mass1 + mass2;
	assert( common::Equals( mass3, 120.8_kg, 1_g ) );

	One ratio = mass1 / mass2;
	assert( common::Equals( ratio, 1_1, _1(epsilon) ) );

	// Mass mass = 3_kg + 10_cm;  // compiler error, or:
	// Length length = mass1 + mass2; // compiler error

	Length length1 = 7_m;
	Length length2 = length1 + 10_cm + 100_yd - 1_mi;

	// Output:

	std::cout << length2 << std::endl;
	// prints: -1510.80m

	std::cout << as(_km,length2) << ", " << length1 << std::endl;
	// prints: -1.51080km, 7m

	std::cout << _km << length2 << ", " << length1 << std::endl;
	// prints: -1.51080km, 0.00700km
	// but:
	std::ostringstream stream;
	stream << length2;
	assert( stream.str() == "-1510.80m" );

	std::cout << _Length;

	SetDefaultStreamOfLength( _yd );

	std::cout << length2 << ", " << length1 << std::endl;
	// prints: -1652yd -8.5in, 7yd 23.6in

	std::ostringstream stream2;
	stream2 << length2;
	assert( stream2.str() == "-1652yd -8.5in" );


	//Input:
	std::cout << "Type in three meters: ";
	Length length3;
	std::cin >> length3;
	assert( length3 == 3_m );
	std::cout << length3 << std::endl;
	// prints: 3yd 10in
	std::cout << "Type in three meters: ";
	std::cin >> length3;
	assert( length3 == 3_m );
	std::cout << length3 << std::endl;
	// prints: 3yd 10in
	std::cout << "Type in three meters: ";
	std::cin >> length3;
	assert( length3 == 3_m );
	std::cout << length3 << std::endl;
	// prints: 3yd 10in

	std::cout << "Type in several dimensionated values: ";
	std::cin >> std::ws;
	std::string str;
	std::getline( std::cin, str );
	std::istringstream stream3{ str };
	length3 = 0_m;
	for( Length val; stream3 >> val; length3 += val )
			;
	std::cout << length3 << std::endl;
	// prints: 3yd 10in

	// std::cin >> as(_km,length3); // won't compile

	SetDefaultStreamOfLength( _m );
	{
		common::Resetter<StreamInLength> resetter{ DefaultStreamInLength, _km };
		std::cout << "Type in 3: ";
		std::cin >> length3;
		std::cout << length3 << std::endl;
		// prints: 3000m
	}

	Real myLength = 18.f;
	Length length4 = _cm(myLength);
	assert( common::Equals( _m(length4), 0.18f, epsilon ) );

	std::cout << "My length: " << _cm << length4 << std::endl; 
	// prints: My length: 18cm 
}