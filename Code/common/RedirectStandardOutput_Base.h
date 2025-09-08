//	trax track library
//	AD 2020 
//
//  "God is a concept by which we measure our pain"
//
//							John Lennon
//
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include <sstream>

/// \brief Take a stream buffer like std::cout and redirects its output to another
/// buffer, specified by the template parameter.
template<class buffer>
class RedirectStandardOut{
public:
	typedef buffer buffer_type;
	typedef typename buffer_type::char_type char_type;
	typedef typename buffer_type::traits_type traits_type;

	RedirectStandardOut( std::basic_ostream<char_type,traits_type>& out, bool bReportReconnection = true )
		:	m_Out					{out},
			m_pOriginalBuffer		{out.rdbuf(&m_DebugBuffer) },
			m_bReportReconnection	{bReportReconnection}
	{
		if( m_bReportReconnection )
			m_Out << "Connecting output redirection." << std::endl;
	}

	~RedirectStandardOut(){
		if( m_bReportReconnection )
			m_Out << "Disconnecting output redirection." << std::endl;

		m_Out.rdbuf(m_pOriginalBuffer);
	}
private:
	std::basic_ostream<char_type,traits_type>& m_Out;
	std::basic_streambuf<char_type,traits_type>* const m_pOriginalBuffer;
	buffer_type m_DebugBuffer;
	bool m_bReportReconnection = true;
};


/// \brief Different method: provide debug buffer to replace with the std::cout rbuffer.
template <class CharT, class TraitsT = std::char_traits<CharT> >
class debugbuffer : public std::basic_stringbuf<CharT, TraitsT>
{
public:

    virtual ~debugbuffer()
    {
        sync();
    }

	using std::basic_stringbuf<CharT, TraitsT>::str;

protected:
	/// \brief Redirects the output of std::basic_stringbuf
    int sync() override
    {
        output_debug_string(str().c_str());
		str({});    // Clear the string buffer

        return 0;
    }

	/// \brief Method to be specialized for specific output destination.
    virtual void output_debug_string( const std::basic_string<CharT>& /*text*/ ) {} //specialized for output
};
