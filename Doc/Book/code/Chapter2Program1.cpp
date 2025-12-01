#include <iostream>

#include "trax/Units.h"
#include "spat/Position.h"
#include "spat/Dump.h"


/// \brief  A sample program that runs the code from Chapter 2.
int main(){
	using namespace trax;
	assert( CheckUnits() );

	Length ax = 4_m;
	Length ay = 5_m;
	Length az = 6_m;
	spat::Position<Length> A = {ax,ay,az};

	assert( A.x == ax && A.y == ay && A.z == az );
	std::cout << A << std::endl;


	spat::Position<Length> B = {6_m,7_m,8_m};
	spat::Vector<Length> D = B - A;

	assert( D.dx == B.x - A.x && D.dy == B.y - A.y && D.dz == B.z - A.z );
	std::cout << D << std::endl;

	Length d = D.Length();
	std::cout << d << std::endl;
	// prints:  Vector{ 2m, 2m, 2m }
	//          3.46m


	using namespace spat;

	Position<Length> C = {5_m,9_m,10_m};
	Vector<Length> D1 = B - A;
	Vector<Length> D2 = C - B;
	Vector<Length> D3 = C - A;
	assert( D3 == D1 + D2 );


	Vector<One> U = Ex<One>;
	Length s = 2_m;
	B = A + s * U;
	assert( (B - A).Length() == 2_m );


	Vector<One> V{ 1, 2, 3}, W{ 4, 5, 6 }, R{ 7, 8, 9 }, S{ 10, 11, 12 };
	assert( V % W == - W % V );
	assert( V * (W % R) == W * (R % V) );
	assert( W * (R % V) == R * (V % W) );
	assert( V % (W % R) == (V * R) * W - (V * W) * R );
	assert( (V % W) * (R % S) == (V * R) * (W * S) - (V * S) * (W * R) );


	Vector<Velocity> V0{ 100_mIs, 200_mIs, 0_mIs};
	Vector<Acceleration> G{ 0_mIs2, 0_mIs2, -9.81_mIs2 };
	Time dt = 3_s;

	Vector<Velocity> Vel = V0 + G * dt;
	std::cout << Vel << std::endl;
	// prints: Vector( 360km/h, 720km/h, -105.95km/h )

	Real p = 0.02f;
	Real f = 0.f;
	for( int j = 0; j <= 30; ++j ){
		Real fj = 1.f;
		for( int i = 0; i <= j; ++i )
			fj *= (1+p);
		f+= fj;
	}

	Real T = 15000.f;

	std::cout << T * f << std::endl;
}