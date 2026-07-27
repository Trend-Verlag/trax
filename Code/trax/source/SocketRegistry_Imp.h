//	trax track library
//	AD 2014 
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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "trax/SocketRegistry.h"
#include "trax/IDType.h"
#include "trax/Plug.h"

#include <map>
#include <vector>

namespace trax{

	class SocketRegistry_Imp : public virtual SocketRegistry
	{
	public:
		void ReservePlugIDs( IDType upToID ) override;

		void RegisterPlug( Plug& plug ) override;

		void RegisterPlug( MultiPlug& plug ) override;

		void UnRegisterPlug( const Plug& plug ) override;

		void UnRegisterPlug( const MultiPlug& plug ) override;

		void UnregisterUnconnectedPlugs() override;

		Plug* GetPlug( IDType id ) const override;

		IDType MaxValidPlugID() const noexcept override;

		void ConnectJack( Jack& jack ) override;

		void RemoveJack( Jack& jack ) noexcept override;

		void ClearRegistry() noexcept override;
	private:

		/// \brief Container for identified trax objects
		template<typename T> 
		class KeyedContainer{
			typedef T mapped_type;
			typedef IDType key_type;
			typedef std::map<key_type,T> ContainerType;
			typedef typename std::map<key_type,T>::iterator iterator;
			typedef typename std::map<key_type,T>::const_iterator const_iterator;

			ContainerType m_Container;
			IDType m_ReservedUpToID = 0;
		public:
			void reserve( IDType upToID ){
				m_ReservedUpToID = upToID;
			}

			void insert( const mapped_type& pelement ){
				if( pelement->ID() ){
					if( !IsFree( pelement->ID() ) ){
						if( const_iterator citer = find( pelement->ID() ); citer != end() && citer->second == pelement )
							return; // already in container, do nothing
						
						std::ostringstream stream;
						stream << "An objects ID is not unique: " << pelement->ID() << " value type: " << typeid( mapped_type ).name();
						throw std::logic_error( stream.str() );		
					}
				}
				else
					pelement->ID( GetFree() );

				m_Container.insert( std::make_pair( pelement->ID(), pelement ) );
			}

			void erase( const key_type& key ){
				 auto iter = m_Container.find( key );
				 if( iter != m_Container.end() )
					 m_Container.erase(iter);
			}

			iterator erase( const_iterator pos ){
				return m_Container.erase( pos );
			}

			void clear() noexcept{
				m_Container.clear();
			}

			iterator find( const key_type& key ){
				return m_Container.find(key);
			}

			const_iterator find( const key_type& key ) const{
				return m_Container.find(key);
			}

			iterator begin() noexcept{
				return m_Container.begin();
			}

			const_iterator begin() const noexcept{
				return m_Container.begin();
			}

			iterator end() noexcept{
				return m_Container.end();
			}

			const_iterator end() const noexcept{
				return m_Container.end();
			}

			bool IsFree( key_type id ) const{
				return m_Container.find( id ) == m_Container.end();
			}

			key_type GetFree() const{
				key_type x = m_ReservedUpToID + 1;		
				for( auto it = m_Container.lower_bound( x ); 
					 it != m_Container.end() && it->first == x; 
					 ++it, ++x )
				{}

				return x;
			}

			key_type GetMaxValid() const noexcept{
				if( m_Container.empty() )
					return 0u;

				return m_Container.crbegin()->first;
			}
		};

		KeyedContainer<Plug*>	m_Plugs;
		std::vector<Jack*>		m_Jacks;	// these Jacks know their plugs 
											// ids and stay in the list
											// until they can get connected.
		void Connect( Plug& plug );

	};

} // namespace trax
