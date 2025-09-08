//	trax track library
//	AD 2015
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "IDType.h"

#include <vector>
#include <unordered_map>

namespace trax{


	template<class Base>
	class ObjectID_Imp : public virtual Base{
	public:
		using Base::operator=;

		const std::string& Reference( const std::string& name ) const override{
			auto iter = m_References.find( name );
			if( iter == m_References.end() )
				return m_Empty;

			return iter->second;
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			if( reference.empty() )
				m_References.erase( name );
			else
				m_References[name] = reference;
		}

		const std::vector<char const *>& ReferenceNames( const std::string& namePart ) const override{
			sm_ReferenceNames.clear();
			for( const auto& pair : m_References )
				if( pair.first.find( namePart ) != std::string::npos )
					sm_ReferenceNames.push_back( pair.first.c_str() );
			return sm_ReferenceNames;
		}

		IDType ID() const noexcept override{
			return m_ID;
		}

		void ID( IDType id ) noexcept override{
			m_ID = id;
		}

	private:
		std::unordered_map<std::string, std::string> m_References;
		static const std::string m_Empty;
		std::string		m_Name;
		IDType			m_ID;
		static std::vector<char const *> sm_ReferenceNames;
	};

	template<class Base>
	std::vector<char const *> ObjectID_Imp<Base>::sm_ReferenceNames;

	template<class Base>
	const std::string ObjectID_Imp<Base>::m_Empty;
}