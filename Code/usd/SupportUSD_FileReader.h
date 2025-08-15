// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "SupportUSD_Reader.h"

#include <filesystem>

namespace trax{

    class USDFileReader : public USDReader{
    public:
        USDFileReader( const std::filesystem::path& filePath );



    };

}
