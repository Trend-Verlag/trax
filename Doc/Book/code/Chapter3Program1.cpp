
#include "trax/Units.h"
#include "spat/Position.h"


int main(){
	using namespace trax;
	assert( CheckUnits() );

	Length x, y, z;
	spat::Position<Length> C = { x, y, z };


}