//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



#include "IndicatorCollection_Imp.h"

namespace trax{
////////////////////////////////////////
std::unique_ptr<IndicatorCollection> IndicatorCollection::Make() noexcept{
	return std::make_unique<IndicatorCollection_Imp>();
}
////////////////////////////////////////
IndicatorCollection_Imp::IndicatorCollection_Imp() noexcept
{}

const char* IndicatorCollection_Imp::TypeName() const{
	return "IndicatorCollection";
}

void IndicatorCollection_Imp::Update(){
}

IDType IndicatorCollection_Imp::Add( std::shared_ptr<Indicator> pIndicator ){
	if( IDType retval = IndicatorCollection_Base::Add( pIndicator ) )
	{
		return retval;
	}

	return 0;
}

bool IndicatorCollection_Imp::Remove( Indicator* pIndicator, bool zeroIDs ){
	if( IndicatorCollection_Base::Remove( pIndicator, zeroIDs ) )
	{	
		return true;
	}

	return false;
}
//////////////////////////////////////
}
