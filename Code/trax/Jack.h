//	trax track library
//	AD 2014 
//
// "Mr. Spock, sometimes I think if I hear that word 'frequency' once more, I'll cry."
//
//							Lieutenant Uhura
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

// The signal is the message.

#pragma once

#include "Configuration.h"
#include "Identified.h"
#include "ObjectID.h"

#include <cassert>

namespace trax{

	struct Plug;
	struct JackEnumerator;

	/// \brief A jack a plug can get connected with.
	struct Jack : Identified<Jack>
	{
		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const = 0;


		/// \returns The Plugs id if connected or an plug id for reconnecting
		virtual IDType RefPlugID() const noexcept = 0;


		/// \brief Sets the Plug id for reconnecting.
		virtual void RefPlugID( IDType id ) noexcept = 0;


		/// \brief This function gets called from the owner 
		/// of a Jack on a certain event.
		virtual void Pulse () noexcept = 0; // no parameters


		/// \brief Connects together a Plug with a Jack.
		///
		/// To disconnect call with nullptr.
		virtual void Insert( Plug* pPlug ) noexcept = 0;


		/// \brief Inserts the plug at this Jack if
		/// no plug is already inserted. Inserts it at the
		/// tail of this plug chain otherwise.
		virtual void InsertAtTail( Plug* pPlug ) = 0;


		/// \brief Inserts the plug at this Jack. If a plug is already
		/// inserted this will be appended into pPlug's Jack.
		virtual void InsertAndAppend( Plug* pPlug ) noexcept = 0;


		/// \brief Gets a connected plug if any.
		/// \returns Pointer to connected plug, nullptr if none.
		virtual Plug* GetPlug() const noexcept = 0;


		/// \brief Is a Plug already plugged into this jack?
		virtual bool Plugged() const noexcept = 0;


		/// \brief Clear the Jack from any plug
		virtual void Clear() noexcept = 0;
	};


	class Jack_Imp : public ObjectID_Imp<Jack> {
	public:
		Jack_Imp() = delete;
		Jack_Imp(const Jack_Imp& jack) = delete;
		dclspc Jack_Imp(Jack_Imp&& jack) noexcept;
		explicit dclspc Jack_Imp(const std::string& name);

		Jack_Imp& operator=(const Jack_Imp&) = delete;
		Jack_Imp& operator=(Jack_Imp&& jack) = delete;

		dclspc ~Jack_Imp() noexcept;

		dclspc const char* TypeName() const override;

		IDType dclspc RefPlugID() const noexcept override;

		void dclspc RefPlugID(IDType id) noexcept override;

		void dclspc Pulse() noexcept override;

		void dclspc Insert(Plug* pPlug) noexcept override;

		void dclspc InsertAtTail(Plug* pPlug) override;

		void dclspc InsertAndAppend(Plug* pPlug) noexcept override;

		dclspc Plug* GetPlug() const noexcept override;

		bool dclspc Plugged() const noexcept override;

		void dclspc Clear() noexcept override;
	private:
		Plug* m_pPlug;
		bool		m_bPulsing;
		IDType		m_RefPlugID;

		void DoClear() noexcept;
	};


	/// \brief Interface for enumerating the Jacks an object provides.
	///
	/// The interface can be retrieved by dynamic_cast for every object that provides Jacks.
	/// There is a begin/end pair of functions defined, that can get used for iterating, e.g.
	/// in a range-for loop.
	struct JackEnumerator{

		/// \name Type information for iterator implementation
		///@{		
		typedef JackEnumerator	collection_type;
		typedef Jack			value_type;
		///@}


		/// \name GetJack
		/// \brief Get one of the object's Jacks.
		/// \param name idx The zero based index of the Jack.
		/// \throws std::range_error if idx is not in [0,SizeJacks()(
		///@{
		/// Use only for iterating over the jacks.
		Jack& GetJack( int idx )
		{
			return const_cast<Jack&>(_GetJack( idx ));
		}

		const Jack& GetJack( int idx ) const{
			return _GetJack( idx );
		}
		///@}


		/// \returns The total number of jacks available through this interface.
		virtual int CountJacks() const = 0;


		virtual ~JackEnumerator() = default;
	protected:
		JackEnumerator( const JackEnumerator& ) = default;
		JackEnumerator( JackEnumerator&& ) = default;
		JackEnumerator& operator=( const JackEnumerator& ) = default;
		JackEnumerator& operator=( JackEnumerator&& ) = default;
		JackEnumerator() = default;

		virtual const Jack& _GetJack( int idx ) const = 0; // must be const; no forewarding of Jacks ...
	};


	template<	class EnumeratorType,	
				class PointerType = Jack*,
				class ReferenceType = Jack&>
	class JackIterator{// : public std::iterator<std::random_access_iterator_tag,Jack,ptrdiff_t, PointerType, ReferenceType>{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = Jack;
		using difference_type = ptrdiff_t;
		using pointer = PointerType;
		using reference = ReferenceType;

		/// \brief Construction.
		JackIterator( EnumeratorType& Container, bool bEnd = false )
			:	m_Container( Container ),
				m_IndexCurrent( bEnd ? Container.CountJacks() : 0 )
		{}

		/// \name Iterator Operators
		///@{
		JackIterator& operator++() noexcept{
			++m_IndexCurrent;
			return *this;
		}

		JackIterator operator++( int ){
			JackIterator retval( *this );
			operator++();
			return retval;
		}

		ReferenceType operator*() const{
			return m_Container.GetJack( m_IndexCurrent );
		}

		JackIterator& operator--(){
			--m_IndexCurrent;
			return *this;
		}

		JackIterator operator--( int ){
			JackIterator retval( *this );
			operator--();
			return retval;
		}

		ReferenceType operator[]( int n ) const{
			return m_Container.GetJack( m_IndexCurrent + n );
		}

		ReferenceType operator->() const{
			return m_Container.GetJack( m_IndexCurrent );
		}

		bool operator==( const JackIterator& iter ) const{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent == iter.m_IndexCurrent;
		}

		bool operator!=( const JackIterator& iter ) const noexcept{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent != iter.m_IndexCurrent;
		}

		bool operator<( const JackIterator& iter ) const{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent < iter.m_IndexCurrent;
		}

		bool operator<=( const JackIterator& iter ) const{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent <= iter.m_IndexCurrent;
		}

		bool operator>( const JackIterator& iter ) const{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent > iter.m_IndexCurrent;
		}

		bool operator>=( const JackIterator& iter ) const{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent >= iter.m_IndexCurrent;
		}

		void operator+=( int n ){
			m_IndexCurrent += n;
		}

		void operator-=( int n ){
			m_IndexCurrent -= n;
		}

		JackIterator operator+( int n ) const{
			JackIterator iter( *this );
			iter.m_IndexCurrent += n;
		}

		JackIterator operator-( int n ) const{
			JackIterator iter( *this );
			iter.m_IndexCurrent -= n;
		}
		///@}
	private:
		EnumeratorType& m_Container;
		int m_IndexCurrent;
	};

	inline JackIterator<JackEnumerator> begin( JackEnumerator& jackEnumerator ){
		return JackIterator<JackEnumerator>( jackEnumerator );
	}

	inline JackIterator<const JackEnumerator,const Jack*, const Jack&> begin( const JackEnumerator& jackEnumerator ){
		return JackIterator<const JackEnumerator, const Jack*, const Jack&>( jackEnumerator );
	}

	inline JackIterator<JackEnumerator> end( JackEnumerator& jackEnumerator ){
		return JackIterator<JackEnumerator>( jackEnumerator, true );
	}

	inline JackIterator<const JackEnumerator, const Jack*, const Jack&> end( const JackEnumerator& jackEnumerator ){
		return JackIterator<const JackEnumerator, const Jack*, const Jack&>( jackEnumerator, true );
	}
}