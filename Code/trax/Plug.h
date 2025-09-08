//	trax track library
//	AD 2014 
//
//  "If you have a message - go to the post office."
//
//								Alfred Hitchcock
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

#include "Identified.h"
#include "Jack.h"
#include "Units.h"

#include <vector>
#include <algorithm>
#include <sstream>

#if defined(_MSC_VER)
#	pragma warning(disable: 4250) // virtual inheritance of methods via dominance
#endif

namespace trax{

	struct PlugEnumerator;
	struct SocketRegistry;

	/// \brief A Plug of some object can get plugged into a jack of some object, 
	/// specific to a certain event.
	///
	///	The plug will receive a Pulse() call if that event occures. The pulse
	/// is always going from jack to plug. The plugs are provided by objects, 
	/// too. These would take a pulse, received from a jack of another object
	/// as a trigger to perform a certain action.
	/// The Pulse() - method has no parameters! There is no meaning
	/// to a pulse and no message forewarded. All the 'meaning' of a pulse is laying in the
	/// connection of a specific plug to a specific jack.
	struct Plug : Identified<Plug>
	{
		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const = 0;


		/// \brief Is this Plug already plugged into a jack?
		/// \returns A pointer to the Jack this Plug is plugged to.
		virtual Jack* Plugged() const noexcept = 0;


		/// \returns true if it happens that the plug's jacks are 
		/// connected in a cyclic manner, false otherwise.
		virtual bool IsCyclic() const noexcept = 0;


		/// \brief receives the tail (last element) of a Plug chain.
		///
		/// Plugs provide themselves a Jack so they can 
		/// get cascaded. The function receives the last element
		/// in this chain.
		/// \returns A Plug that is directly or indirectly inserted in
		/// this plug's jack and provides an empty Jack.
		/// \throws std::logic_error if the connection happens to be cyclic, 
		/// i.e., there is no empty jack.
		virtual const Plug& Tail() const = 0;


		/// \brief receives the tail (last element) of a Plug chain.
		///
		/// Plugs provide themselves a Jack so they can 
		/// get cascaded. The function receives the last element
		/// in this chain.
		/// \returns A Plug that is directly or indirectly inserted in
		/// this plug's jack and provides an empty Jack.
		/// \throws std::logic_error if the connection happens to be cyclic, 
		/// i.e., there is no empty jack.
		virtual Plug& Tail() = 0;


		/// \brief Get a Jack that triggers a pulse if the Plug receives a pulse.
		///
		/// This can be used to cascade several Plugs to receive the pulse from one
		/// single Jack.
		virtual Jack& JackOnPulse() noexcept = 0;


		/// \brief Get a Jack that triggers a pulse if the Plug receives a pulse.
		virtual const Jack& JackOnPulse() const noexcept = 0;


		/// \returns a pointer to the owner of this plug.
		///
		/// Every object that provides plugs will implement this interface.
		virtual PlugEnumerator& Parent() const noexcept = 0;


		///// \brief Releases this Socket from a chain of Sockets.
		virtual void Remove() noexcept = 0;
	protected:
		friend class Jack_Imp;

		virtual void Pulse( bool /*bForewardToJack*/ ) noexcept{}

		virtual void Release() noexcept{}

		virtual void ConnectTo( Jack& /*jack*/ ) noexcept{}
	};

	dclspc const Plug* GetFirstNonZeroIDPlugInChain( const Plug& plug );


	/// \brief Interface for enumerating the Plugs an object provides.
	///
	/// The interface can be retrieved by dynamic_cast for every object that provides Plugs.
	/// There is a begin/end pair of functions defined, that can get used for iterating, e.g.
	/// in a range-for loop.
	struct PlugEnumerator{

		/// \name Type information for iterator implementation
		///@{		
		typedef PlugEnumerator	collection_type;
		typedef Plug			value_type;
		///@}


		/// \name GetPlug
		/// \brief Get one of the object's Plugs.
		/// \param name idx The zero based index of the Plug.
		/// \throws std::range_error if idx is not in [0,SizePlugs()(
		///@{		
		Plug& GetPlug( int idx ){
			return const_cast<Plug&>(_GetPlug( idx ));
		}

		const Plug& GetPlug( int idx ) const{
			return _GetPlug( idx );
		}
		///@}


		/// \returns The total number of plugs available through this interface.
		virtual int CountPlugs() const = 0;

		virtual ~PlugEnumerator() = default;
	protected:
		PlugEnumerator( const PlugEnumerator& ) = default;
		PlugEnumerator( PlugEnumerator&& ) = default;
		PlugEnumerator& operator=( const PlugEnumerator& ) = default;
		PlugEnumerator& operator=( PlugEnumerator&& ) = default;
		PlugEnumerator() = default;


		/// \brief to be implemented for an object that provides plugs.
		///
		/// Do not rely on constness of the returned plug. The alternative
		/// would have been to implement all the GetPlug a second time
		/// for constness.
		/// \returns a plug by index.
		virtual const Plug& _GetPlug( int idx ) const = 0;
	};


	template<	class EnumeratorType,
				class PointerType = Plug*,
				class ReferenceType = Plug&>
	class PlugIterator{// : public std::iterator<std::random_access_iterator_tag, Plug, ptrdiff_t, PointerType, ReferenceType>{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = Plug;
		using difference_type = ptrdiff_t;
		using pointer = PointerType;
		using reference = ReferenceType;

		/// \brief Construction.
		PlugIterator( EnumeratorType& Container, bool bEnd = false )
			:	m_Container( Container ),
				m_IndexCurrent( bEnd ? Container.CountPlugs() : 0 )
		{}

		/// \name Iterator Operators
		///@{
		PlugIterator& operator++() noexcept{
			++m_IndexCurrent;
			return *this;
		}

		PlugIterator operator++( int ) noexcept{
			PlugIterator retval( *this );
			operator++();
			return retval;
		}

		ReferenceType operator*() const{
			return m_Container.GetPlug( m_IndexCurrent );
		}

		PlugIterator& operator--() noexcept{
			--m_IndexCurrent;
			return *this;
		}

		PlugIterator operator--( int ) noexcept{
			PlugIterator retval( *this );
			operator--();
			return retval;
		}

		ReferenceType operator[]( int n ) const{
			return m_Container.GetPlug( m_IndexCurrent + n );
		}

		ReferenceType operator->() const{
			return m_Container.GetPlug( m_IndexCurrent );
		}

		bool operator==( const PlugIterator& iter ) const noexcept{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent == iter.m_IndexCurrent;
		}

		bool operator!=( const PlugIterator& iter ) const noexcept{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent != iter.m_IndexCurrent;
		}

		bool operator<( const PlugIterator& iter ) const noexcept{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent < iter.m_IndexCurrent;
		}

		bool operator<=( const PlugIterator& iter ) const noexcept{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent <= iter.m_IndexCurrent;
		}

		bool operator>( const PlugIterator& iter ) const noexcept{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent > iter.m_IndexCurrent;
		}

		bool operator>=( const PlugIterator& iter ) const noexcept{
			assert( &m_Container == &iter.m_Container );
			return m_IndexCurrent >= iter.m_IndexCurrent;
		}

		void operator+=( int n ) noexcept{
			m_IndexCurrent += n;
		}

		void operator-=( int n ) noexcept{
			m_IndexCurrent -= n;
		}

		PlugIterator operator+( int n ) const noexcept{
			PlugIterator iter( *this );
			iter.m_IndexCurrent += n;
		}

		PlugIterator operator-( int n ) const noexcept{
			PlugIterator iter( *this );
			iter.m_IndexCurrent -= n;
		}
		///@}
	private:
		EnumeratorType& m_Container;
		int m_IndexCurrent;
	};

	inline PlugIterator<PlugEnumerator> begin( PlugEnumerator& plugEnumerator ){
		return PlugIterator<PlugEnumerator>( plugEnumerator );
	}

	inline PlugIterator<const PlugEnumerator, const Plug*, const Plug&> begin( const PlugEnumerator& plugEnumerator ){
		return PlugIterator<const PlugEnumerator, const Plug*, const Plug&>( plugEnumerator );
	}

	inline PlugIterator<PlugEnumerator> end( PlugEnumerator& plugEnumerator ){
		return PlugIterator<PlugEnumerator>( plugEnumerator, true );
	}

	inline PlugIterator<const PlugEnumerator, const Plug*, const Plug&> end( const PlugEnumerator& plugEnumerator ){
		return PlugIterator<const PlugEnumerator, const Plug*, const Plug&>( plugEnumerator, true );
	}


	/// \brief A MultiPlug provides a variable number of actual Plugs to one input slot.
	///
	/// If a clone plug gets pulsed it will pulse the base plug and thereby the target object.
	struct MultiPlug :	virtual Plug,
						PlugEnumerator
	{
		using Plug::ID;

		using Plug::Reference;

		/// \brief Creates a clone for the plug
		/// \param pRegistry If not nullptr, the function registers a plug clone
		/// \returns A reference to the newly created plug.
		virtual Plug& Make( SocketRegistry* pRegistry = nullptr ) = 0;


		/// \returns The base plug if it is free, otherwise a clone is created and returned.
		/// \param pRegistry If not nullptr, the function registers a plug clone
		virtual Plug& Unplugged( SocketRegistry* pRegistry = nullptr ) = 0;


		/// \brief Tells the MultiPlug that the plug clone is no longer needed.
		///
		/// This will invalidate the index assignment to all plug clones.
		virtual void Release( Plug& plug ) = 0;


		/// \brief Releases all the clone plugs.
		virtual void Clear() = 0;
	protected:
		using Plug::Release;
	};	


	class Plug_Imp : public ObjectID_Imp<Plug>,
					 public JackEnumerator {
	public:
		Plug_Imp() = delete;
		Plug_Imp(const Plug_Imp& plug) = delete;
		dclspc Plug_Imp(Plug_Imp&& plug) noexcept;
		dclspc Plug_Imp(PlugEnumerator& parent);
		dclspc Plug_Imp(PlugEnumerator& parent, Plug_Imp&& plug);

		dclspc ~Plug_Imp() noexcept;	// TODO: if the user defines his own Plug_Imp derived Plug
										// and assigns a string by Reference() without using Reference 
										// again to delete it, the destructor will crash on trying to
										// delete the string in a different module.
										// User framework should be supplied independently from trax 
										// library implementation.	

		//~Plug_Imp() noexcept{
		//	Remove();
		//}


		Plug_Imp& operator=(Plug_Imp&) = delete;
		Plug_Imp& operator=(Plug_Imp&& plug) = delete;

		const char*	TypeName() const noexcept override {
			return "Plug";
		}

		using ObjectID_Imp<Plug>::Reference;

		const std::string& Reference(const std::string& name) const override {
			return ObjectID_Imp<Plug>::Reference(name);
		}

		using ObjectID_Imp<Plug>::ID;

		IDType ID() const noexcept override {
			return ObjectID_Imp<Plug>::ID();
		}

		dclspc Jack* Plugged() const noexcept override;

		dclspc bool IsCyclic() const noexcept override;

		dclspc const Plug& Tail() const override;

		dclspc Plug& Tail() override;

		dclspc Jack& JackOnPulse() noexcept override;

		dclspc const Jack& JackOnPulse() const noexcept override;

		dclspc PlugEnumerator& Parent() const noexcept override;

		dclspc void Remove() noexcept;

		dclspc int CountJacks() const noexcept override;
	protected:
		/// \brief Triggers a pulse for the plug.
		/// \param bForewardToJack true if the pulse should be 
		/// sent to a plug that's plugged in the plug's
		/// jack.
		dclspc void Pulse(bool bForewardToJack) noexcept override;

		dclspc const Jack& _GetJack(int idx) const override;
	private:
		friend class Jack_Imp;
		// Plug_Imp:
		dclspc void Release() noexcept override;

		dclspc void ConnectTo(Jack& jack) noexcept override;

		Jack* m_pPlugged;
		Jack_Imp m_JackOnPulse{ "JackOnPulse" };
		PlugEnumerator& m_Parent;
	};


	template<class ParentType>
	class Plug_Imp_Parent : public Plug_Imp {
	protected:
		ParentType& m_Parent;
	public:
		Plug_Imp_Parent() = delete;
		Plug_Imp_Parent(const Plug_Imp_Parent& plug) = delete;
		Plug_Imp_Parent(Plug_Imp_Parent&& plug) = default;
		Plug_Imp_Parent(ParentType& parent)
			: Plug_Imp{ parent },
			m_Parent{ parent }
		{}
		Plug_Imp_Parent(ParentType& parent, Plug_Imp_Parent&& plug)
			: Plug_Imp{ parent,std::move(plug) },
			m_Parent{ parent }
		{}
	};


	/// \brief Implementation for a plug that calls an objects member function on pulse.
	///
	/// \param ParentType PlugEnumerator derived owner of the plug.
	template<class ParentType>
	class Plug_Imp_ParentPointer : public Plug_Imp_Parent<ParentType> {
#if defined(_MSC_VER) && !defined(_WIN64)
#	pragma pack(push, 4) // alignment of a member was sensitive to packing
#endif
		void (ParentType::* m_pPulseFunction)() = nullptr;
#if defined(_MSC_VER) && !defined(_WIN64)
#	pragma pack(pop)
#endif
	public:
		using TargetType = ParentType;

		Plug_Imp_ParentPointer() = delete;
		Plug_Imp_ParentPointer(const Plug_Imp_ParentPointer& plug) = delete;
		Plug_Imp_ParentPointer(Plug_Imp_ParentPointer&& plug) = default;

		Plug_Imp_ParentPointer(ParentType& parent, void (ParentType::* pPulseFunction)())
			: Plug_Imp_Parent<ParentType>{ parent },
			m_pPulseFunction{ pPulseFunction }
		{
			//static_assert( std::is_convertible<Plug_Imp_ParentPointer*,PlugEnumerator*>::value );
			assert(m_pPulseFunction);
		}
		Plug_Imp_ParentPointer(ParentType& parent, void (ParentType::* pPulseFunction)(), Plug_Imp_ParentPointer&& plug)
			: Plug_Imp_Parent<ParentType>{ parent, std::move(plug) },
			m_pPulseFunction{ pPulseFunction }
		{
			//static_assert( std::is_convertible<Plug_Imp_ParentPointer*,PlugEnumerator*>::value );
			assert(m_pPulseFunction);
		}

		Plug_Imp_ParentPointer& operator=(const Plug_Imp_ParentPointer&) = delete;
		Plug_Imp_ParentPointer& operator=(Plug_Imp_ParentPointer&&) = delete;

		void Pulse(bool bForewardToJack) noexcept override {
			(Plug_Imp_Parent<ParentType>::m_Parent.*m_pPulseFunction)();
			Plug_Imp_Parent<ParentType>::Pulse(bForewardToJack);
		}
	};


}