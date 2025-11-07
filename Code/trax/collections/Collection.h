// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2016
//
//  "If I don't go crazy, I'll loose my mind"
//
//								Death In Vegas

#pragma once

#include "ObjectIDDecorator.h"

#include <map>

namespace trax{

	template<class Collection_Type,class Value_Type>
	struct Collection{

		/// \name Type information for decorator implementation
		///@{
		using collection_type = Collection_Type;
		using value_type = Value_Type;
		///@}


		template<typename _value_type, typename _std_iterator_type>
		class iterator_imp{
		public:
			/// \name Type Definitions
			/// \brief Type definitions as expected from an iterator by std.
			///@{
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = _value_type;
			using difference_type = int;
			using distance_type = int;
			using pointer = std::shared_ptr<_value_type>;
			using reference = _value_type&;
			///@}


			inline iterator_imp( const _std_iterator_type& iter ) noexcept
				: m_Current{iter}
			{}


			/// \name Iterator Operators
			///@{
			inline bool operator==( const iterator_imp& iter ) const noexcept{
				return m_Current == iter.m_Current;
			}

			inline bool operator!=( const iterator_imp& iter ) const noexcept{
				return m_Current != iter.m_Current;
			}

			inline iterator_imp& operator++() noexcept{
				++m_Current;
				return *this;
			}

			inline iterator_imp operator++(int) noexcept{
				iterator_imp retval( *this );
				++m_Current;
				return retval;
			}

			inline iterator_imp& operator--() noexcept{		
				--m_Current;
				return *this;
			}

			inline iterator_imp operator--(int) noexcept{
				iterator_imp retval( *this );
				--m_Current;
				return retval;
			}

			inline reference operator*() const noexcept{
				return *m_Current->second;
			}

			inline pointer operator->() const noexcept{
				return m_Current->second;
			}
			///@}
		private:
			_std_iterator_type m_Current;
		};

		using iterator = iterator_imp<value_type,typename std::map<IDType,std::shared_ptr<value_type>>::const_iterator>;

		using const_iterator = iterator_imp<const value_type,typename std::map<IDType,std::shared_ptr<value_type>>::const_iterator>;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Checks whether the elements in this collection are valid 
		/// \param bSilent If false there will be diagnostic output to the console.
		/// \returns true if all the elements are valid.
		virtual bool IsValid() const noexcept = 0;


		/// \brief Adds an element to the container and returns its index in the container.
		///
		/// If the element carries an id, it is used as long as it is unique. If the id is 0 
		/// the object is assigned one valid id. If it is not zero but already occupied,
		/// an exception is thrown.
		/// \param pValue_Type Object to get added to the fleet.
		/// \throws std::invalid_argument if the object is invalid or the objects id is 
		/// already occupied.
		virtual IDType Add( std::shared_ptr<Value_Type> pValue_Type ) = 0;


		/// \brief Adds an element to the container and returns its index in the container. Does not throw.
		///
		/// While the Collection::Add will throw an exception if an element
		/// carries an id wich is already occupied, this method will supply the 
		/// element with a free one. This might lead to other ids becoming occupied without
		/// explicitely requested.
		virtual IDType AddRelaxed( std::shared_ptr<Value_Type> pTraxType ) = 0;


		/// \brief Removes an element from this container.
		///
		/// \param pValue_Type Pointer to object to remove.
		/// \param zeroIDs If true sets the IDs of any removed object to 0. If false
		/// the ids are freed for the collection, but the objects keep their id values.
		/// \returns true if the object was found and successfully removed; false otherwise.
		virtual bool Remove( Value_Type* pValue_Type, bool zeroIDs = false ) = 0;


		/// \brief Moves all the items from another container to this container.
		///
		/// The ids of the items might get changed thereby.
		/// \param collection_Type Container from wich to take over the elements.
		/// \returns The offset by which the elements of the collection were 
		/// offset to maintain unambiguity of ids.
		virtual int Take( Collection_Type& collection_Type ) = 0;


		/// \brief Removes all the elements from this container.
		virtual void Clear() = 0;


		/// \returns the number of elements in this container.
		virtual int Count() const = 0;


		/// \returns An iterator to the first element in this container.
		virtual iterator begin() = 0;


		/// \returns An const_iterator to the first element in this container.
		virtual const_iterator begin() const = 0;
		
		virtual const_iterator cbegin() const = 0;


		/// \returns An iterator to one past the last element in this container.
		virtual iterator end() = 0;


		/// \returns An const_iterator to one past the last element in this container.
		virtual const_iterator end() const = 0;

		virtual const_iterator cend() const = 0;


		/// \returns A pointer to the first element in this container.
		virtual std::shared_ptr<Value_Type> GetFirst() const = 0;


		/// \returns A pointer to the next element in this container.
		///
		/// \param pValue_Type The previous element to get the next for.
		virtual std::shared_ptr<Value_Type> GetNext( const std::shared_ptr<Value_Type>& pValue_Type ) const = 0;


		/// \returns A pointer to the last element in this container.
		virtual std::shared_ptr<Value_Type> GetLast() const = 0;


		/// \returns A pointer to the previous element in this container.
		///
		/// \param pValue_Type The element to get the previous for.
		virtual std::shared_ptr<Value_Type> GetPrevious( const std::shared_ptr<Value_Type>& pValue_Type ) const = 0;


		/// \name Get
		/// \brief Gets an element from the container.
		/// \param id Identifier for the object to retreive.
		/// \param name Name to get the object for.
		/// \returns A pointer to the requested element.
		///@{
		virtual std::shared_ptr<Value_Type> Get( IDType id ) const = 0;

		virtual std::shared_ptr<Value_Type> Get( const std::string& name ) const = 0;
		///@}


		/// \brief Pushes an element on the activety stack.
		///
		/// The activity stack stores member elements for later retrieval. 
		/// If an element is added to the container, it automatically is pushed
		/// on the activity stack.
		/// \param id for the element to push.
		virtual	void PushActive( IDType id ) = 0;


		/// \brief Pops an element from the activity stack.
		/// \throws std::underflow_error if the activity stack is empty.
		virtual	void PopActive() = 0;


		/// \returns The topmost element on the activity stack.
		virtual std::shared_ptr<Value_Type> GetActive() const = 0;


		/// \name IsMember
		/// \returns true if the element is a member of the collection; 
		/// false otherwise.
		/// \param item reference to element to test for membership.
		/// \param id Unique id to test wether it is already in use.
		///@{
		virtual bool IsMember( const value_type& item ) const = 0;

		virtual bool IsMember( IDType id ) const = 0;
		///@}


		/// \brief Adds an offset to all ids.
		///
		/// If offset is not zero, this will invalidate all ids received
		/// prior to the shift.
		/// \param offset Distance to move the id space.
		/// \throws std::out_of_range if by this operation one id would become <= 0.
		virtual void ShiftIDs( int offset ) = 0;


		/// \returns The maximum valid id for an element of the collection. If no 
		/// element is in the collection, the result will be zero.
		virtual IDType MaxID() const = 0;


		/// \returns The minimum valid id for an element of the collection. If no 
		/// element is in the collection, the result will be zero.
		virtual IDType MinID() const = 0;


		virtual ~Collection() = default;
		Collection( const Collection& ) = delete;
		Collection( Collection&& ) = delete;
		Collection& operator=( const Collection& ) = delete;
		Collection& operator=( Collection&& ) = delete;
	protected:
		Collection() = default;

		friend Decorator<Collection_Type>;
		virtual void SetDecorator( Collection_Type* pCollection_TypeDecorator ) = 0;
	};

}
