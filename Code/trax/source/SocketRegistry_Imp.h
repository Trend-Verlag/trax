// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

		void RegisterPlug( Plug& plug ) override;

		void UnRegisterPlug( const Plug& plug ) override;

		Plug* GetPlug( IDType id ) const override;

		IDType MaxValidPlugID() const noexcept override;

		void ConnectJack( Jack& jack ) override;

		void RemoveJack( Jack& jack ) noexcept override;

		void Clear() noexcept override;
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
		public:
			void insert( const mapped_type& pelement ){
				if( pelement->ID() ){
					if( !IsFree( pelement->ID() ) ){
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

			bool IsFree( IDType id ) const{
				return m_Container.find( id ) == m_Container.end();
			}

			IDType GetFree() const{
				IDType x = 1u;
				for( auto citer = m_Container.begin();
					citer != m_Container.end(); ++x, ++citer )
				{
					if( x == (*citer).first )
						continue;

					return x;
				}

				return x;
			}

			IDType GetMaxValid() const noexcept{
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
