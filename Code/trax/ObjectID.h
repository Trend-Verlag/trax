// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2015
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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