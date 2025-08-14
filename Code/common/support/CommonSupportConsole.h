#pragma once

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   define VC_EXTRALEAN
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   include <Windows.h> // for displaying colors
#endif // Windows

#include <iostream>
#include <string>

namespace common{

enum class TextColor {
	black       = 0,
	dark_blue   = 1,
	dark_green  = 2,
	light_blue  = 3,
	dark_red    = 4,
	magenta     = 5,
	orange      = 6,
	light_gray  = 7,
	gray        = 8,
	blue        = 9,
	green       = 10,
	cyan        = 11,
	red         = 12,
	pink        = 13,
	yellow      = 14,
	white       = 15,

    reset       = light_gray,
};

enum class BackgroundColor {
	black       = 0,
	dark_blue   = 1,
	dark_green  = 2,
	light_blue  = 3,
	dark_red    = 4,
	magenta     = 5,
	orange      = 6,
	light_gray  = 7,
	gray        = 8,
	blue        = 9,
	green       = 10,
	cyan        = 11,
	red         = 12,
	pink        = 13,
	yellow      = 14,
	white       = 15,

    reset       = black,
};

#if defined(__linux__)

inline std::string get_textcolor_code( TextColor textcolor ) {
    switch(textcolor)
    {
        case TextColor::black: return "30";
        case TextColor::dark_blue: return "34";
        case TextColor::dark_green: return "32";
        case TextColor::light_blue: return "36";
        case TextColor::dark_red: return "31";
        case TextColor::magenta: return "35";
        case TextColor::orange: return "33";
        case TextColor::light_gray: return "37";
        case TextColor::gray: return "90";
        case TextColor::blue: return "94";
        case TextColor::green: return "92";
        case TextColor::cyan: return "96";
        case TextColor::red: return "91";
        case TextColor::pink: return "95";
        case TextColor::yellow: return "93";
        case TextColor::white: return "97";
        default: return "37";
    }
}

inline std::string get_backgroundcolor_code( BackgroundColor backgroundcolor) {
    switch( backgroundcolor ) 
    {
        case BackgroundColor::black: return "40";
        case BackgroundColor::dark_blue: return "44";
        case BackgroundColor::dark_green: return "42";
        case BackgroundColor::light_blue: return "46";
        case BackgroundColor::dark_red: return "41";
        case BackgroundColor::magenta: return "45";
        case BackgroundColor::orange: return "43";
        case BackgroundColor::light_gray: return "47";
        case BackgroundColor::gray: return "100";
        case BackgroundColor::blue: return "104";
        case BackgroundColor::green: return "102";
        case BackgroundColor::cyan: return "106";
        case BackgroundColor::red: return "101";
        case BackgroundColor::pink: return "105";
        case BackgroundColor::yellow: return "103";
        case BackgroundColor::white: return "107";
        default: return "40";
    }
}
#endif


inline std::ostream& operator<<( std::ostream& stream, TextColor color ) {
#if defined(_WIN32)
    ::SetConsoleTextAttribute( ::GetStdHandle(STD_OUTPUT_HANDLE), static_cast<const int>(color) );
#elif defined(__linux__)
    stream << "\033["+get_textcolor_code(color)+"m";
#endif // Windows/Linux
	return stream;
}

inline std::ostream& operator<<( std::ostream& stream, BackgroundColor color ) {
#if defined(_WIN32)
    ::SetConsoleTextAttribute( ::GetStdHandle(STD_OUTPUT_HANDLE), static_cast<const int>(color) << 4 );
#elif defined(__linux__)
    stream << "\033["+get_backgroundcolor_code(color)+"m";
#endif // Windows/Linux
	return stream;
}

inline std::ostream& rendl( std::ostream& stream ) {
	stream << TextColor::reset << std::endl;
    return stream;
}

} // namespace common