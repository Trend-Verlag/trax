// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include <memory>
#include <stdexcept>

namespace common{

	/// \cond
	template<typename Target, typename Source>
	struct NarrowCast_Imp {
		static inline Target narrow_cast( Source v ) {
			const Target r = static_cast<Target>(v);
			if( static_cast<Source>(r) != v )
				throw std::runtime_error("narrow_cast<>() failed");

			return r;
		}
	};

	template<typename SameType>
	struct NarrowCast_Imp<SameType, SameType> {
		static inline SameType narrow_cast( SameType v ) noexcept {
			return v;
		}
	};
	/// \endcond

	///	\brief Safe cast for casting numeric values.
	///
	/// \throws std::runtime_error if the value would be to big
	/// to fit into a smaller type.
	template<typename Target, typename Source>
	inline Target narrow_cast( Source v ){
		return NarrowCast_Imp<Target, Source>::narrow_cast(v);
	}


	template <typename Target, typename Source>
	std::unique_ptr<Target> dynamic_unique_cast(std::unique_ptr<Source>&& source) {
		if( auto* casted = dynamic_cast<Target*>(source.get()) ) {
			source.release();						
			return std::unique_ptr<Target>( casted );
		}

		return nullptr;
	}

}