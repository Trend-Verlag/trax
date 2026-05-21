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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "IDType.h"
#include "common/Helpers.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace trax{


	template<class Base>
	class ObjectID_Imp : public virtual Base{
	public:
		using Base::operator=;

		using Base::Reference;

		const char* Reference( const char* name ) const override{
			auto iter = m_References.find( name );
			if( iter == m_References.end() )
				return sm_Empty.c_str();

			return iter->second->c_str();
		}

		void Reference( const char* name, const char* reference ) override{
			if( reference == nullptr || *reference == '\0' )
				m_References.erase( name );
			else{
				m_References[name] = &*sm_InternPool.emplace( reference ).first;
			}
		}

		common::Span<const char*> ReferenceNames( const char* namePart ) const override{
			sm_ReferenceNames.clear();
			for( const auto& pair : m_References )
				if( std::strstr(pair.first.c_str(), namePart) != nullptr )
					sm_ReferenceNames.push_back( pair.first.c_str() );
			return { sm_ReferenceNames.data(), sm_ReferenceNames.size() };
		}

		common::Span<const char*> ReferenceNames() const override{
			sm_ReferenceNames.clear();
			for( const auto& pair : m_References )
				sm_ReferenceNames.push_back( pair.first.c_str() );
			return { sm_ReferenceNames.data(), sm_ReferenceNames.size() };
		}

		const char* EmptyReference() const noexcept override{
			return sm_Empty.c_str();
		}

		IDType ID() const noexcept override{
			return m_ID;
		}

		void ID( IDType id ) noexcept override{
			m_ID = id;
		}
	private:
		std::unordered_map<std::string,const std::string*>	m_References;
		static std::unordered_set<std::string>				sm_InternPool; // global unique values for string comparison by pointers.
		static const std::string							sm_Empty;

		std::string											m_Name;
		IDType												m_ID;
		static std::vector<char const*>						sm_ReferenceNames;
	};


	template<class Base>
	std::unordered_set<std::string> ObjectID_Imp<Base>::sm_InternPool;

	template<class Base>
	const std::string ObjectID_Imp<Base>::sm_Empty;

	template<class Base>
	std::vector<char const *> ObjectID_Imp<Base>::sm_ReferenceNames;
}