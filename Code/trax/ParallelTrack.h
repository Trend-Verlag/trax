// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2019 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Track.h"

namespace trax{

	/// \brief Track that can be the original to parallel tracks.
	struct ParallelizableTrack : virtual TrackBuilder
	{
		/// \brief Makes a ParallelizableTrack object.
		static dclspc std::shared_ptr<ParallelizableTrack> Make() noexcept;


		struct Data{
			spat::Vector2D<Length>	shift{ 0_m, 0_m };
			bool					bTrackRelative{ true };
		};

		/// \brief Adds a parallel to this track.
		/// \param shift the displacement of the parallel track.
		/// \param bTrackRelative Specifies whether the shift is meant relative to the track's
		/// twisted frame, or to a frame relative to the world's up direction.
		/// \returns A shared pointer to the newly created track
		virtual std::shared_ptr<TrackBuilder> AddParallel( const spat::Vector2D<Length>& shift, bool bTrackRelative ) = 0;


		/// \brief Removes the parallel track with index idx.
		/// 
		/// The call will also remove the track from any TrackCollection.
		/// It also invalidates all track indices bigger than the one for parallelTrack.
		virtual void RemoveParallel( const TrackBuilder& parallelTrack ) = 0;


		/// \returns true if the track is a parallel to this one. false if not.
		virtual bool IsParallel( const TrackBuilder& parallelTrack ) const noexcept = 0;


		/// \brief Removes all parallel tracks.
		/// 
		/// The call will also remove the tracks from any TrackCollection.
		virtual void Clear() = 0;


		/// \returns the number of parallel tracks.
		virtual int CountTracks() const = 0;


		/// \returns the index if the track is a parallel to this one. -1 if not.
		virtual int ParallelIdx( const TrackBuilder& parallelTrack ) const = 0;


		/// \returns the parallel track with index idx or nullptr if idx is invalid.
		virtual std::shared_ptr<TrackBuilder> GetParallel( int idx ) const = 0;


		/// \returns a reference to the parallel's data for index idx.
		/// \throws std::invalid_argument if idx is out of range.
		virtual const Data& GetParallelData( int idx ) const = 0;


		/// \returns The parameter of the point on the parallel track, that corresponds
		/// with the point at sOriginalParameter on the original track.
		/// \throws std::invalid_argument if idx is out of range.
		virtual Length ParallelParameterFrom( int idx, Length sOriginalParameter ) const = 0;


		/// \returns The parameter of the point on the original track, that corresponds
		/// with the point at sParallelParameter on the parallel track.
		/// \throws std::invalid_argument if idx is out of range.
		virtual Length OriginalParameterFrom( int idx, Length sParallelParameter ) const = 0;
	};

} // namespace trax