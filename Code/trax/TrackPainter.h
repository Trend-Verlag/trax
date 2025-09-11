//	trax track library
//	AD 2013 
//
//  "I'm not the kind of girl who gives up just like that."
//
//								Blondie
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

#include "SectionTrack.h"
#include "Section.h"

#include "common/NarrowCast.h"
#include "common/Interval.h"
#include "spat/Position.h"
#include "spat/VectorBundle.h"
#include "spat/VectorBundle2.h"
#include "spat/Frame.h"

#include <vector>

namespace trax{

	/// \brief Base class for painting tracks as a series of short straight pieces.
	class TrackPainter{
	public:
		enum Mode{
			mode_none						= 0x00,
			mode_leftHandedFaces			= 0x01, ///< Produces triangles according to the left hand rule. Default it is the right hand rule.
			mode_constantSegmentLength		= 0x02, ///< Uses segmentLimits.Far() as segment length.
			mode_SegmentLength				= 0x04, ///< Checks each segment at its starting point. The default.
			mode_checkedSegmentLength		= 0x04,	///< Checks the end point of the segment, shrinks if necessary. Use it if the curvature changes much inside the segmentLimits.Far() distance.
			mode_totallycheckedSegmentLength= 0x08, ///< Checks every point of the segment with respect to the segmentLimits.Near() grid. Use it if sudden curves might occure inside the segmentLimits.Far() distance.
			mode_ignoreCuvesTorsion			= 0x10, ///< Ignores the curves torsion (but not the track's twist) for segment calculations.
			mode_localFrame					= 0x20, ///< Paints the track in it's local frame of reference. To get global coordinates, the tracks's transformations have to be applied to the geometric data.
			mode_startFrame					= 0x40,	///< Paints the track around it's start frame. The first segment will start at {0,0,0} with it's tangent heading towards {1,0,0}.
			mode_default					= mode_none,
		};


		/// \name Construction
		/// \brief Creates the track painter.
		///
		/// To make for a constant segment length, have segmentLimits.Near() == segmentLimits.Far(), this will save
		/// some computations, but will probabably not give the most efficent mesh to render.
		/// \param segmentLimits The minimum and maximum segment length to deliver. The last segment returned might be 
		/// shorter, to go to the track's end.
		/// \param e Distance failure that is assumed not to matter visibly.
		/// \throws std::invalid_argument
		/// \see Segment, Segment_Checked
		///@{
		dclspc TrackPainter() noexcept;
		dclspc TrackPainter( int mode, common::Interval<Length> segmentLimits = {epsilon__length,plausible_maximum_length}, Length e = epsilon__length );
		dclspc TrackPainter( const TrackPainter& ) = default;
		dclspc TrackPainter( TrackPainter&& ) = default;
		///@}


		dclspc virtual ~TrackPainter() = default;

		TrackPainter& operator=( const TrackPainter& ) = default;
		TrackPainter& operator=( TrackPainter&& ) = default;

		inline TrackPainter& operator()( const SectionTrack& track ){
			if( track.IsValid() )
				Paint( track, *track.GetSection() );
			return *this;
		}

		/// \returns The mode setting adjusted on constructor call.
		inline int GetMode() const noexcept{
			return m_Mode;
		}

		/// \returns The number of segments from the track last painted.
		inline int GetCountSegments() const noexcept{
			return m_CountSegmentsLastPaint;
		}

		/// \brief Paints the track.
		///
		/// For reaching the track's end, the last segment might be shorter than what would serve the limits given.
		/// \param track The track to paint.
		/// \param section The section to be used for painting.
		/// \param offset If connected tracks are painted, it can be usefull to supply the return value
		/// of a connected track's painting call here. This can be used to adjust texture coordinates for example.
		/// \returns track.GetLength() + offset
		///@{
		dclspc Length Paint( const TrackBuilder& track, const Section& section, Length offset = 0_m );
		///@}	
	protected:
		/// \brief Gets called once on rendering startup before all PaintSegment() calls.
		virtual void StartPaint( const spat::Frame<Length,One>& /*rFrame*/, Length /*offset*/, const Section& /*section*/ ){}

		/// \brief This gets called once per segment to paint a segment of the track.
		/// \param rFrame Position and orientation at the end of the segment to paint.
		/// \param segmentLength Length of the segment. This value migh vary from call to call.
		virtual void PaintSegment( const spat::Frame<Length,One>& rFrame, Length segmentLength, const Section& /*section*/ ) = 0;

		/// \brief Gets called once on rendering end after all PaintSegment() calls.
		virtual void EndPaint( const spat::Frame<Length,One>& /*rFrame*/ ){}
	private:
		int m_Mode;
		Length m_Epsilon;
		common::Interval<Length> m_SegmentLimits;

		int m_CountSegmentsLastPaint = 0;
		spat::Frame<Length,One> m_Transformation;
	};


	/// \brief Paints a track by taking a cross section as a profile.
	class SectionPainter : public TrackPainter{
	public:
		dclspc SectionPainter() noexcept;
		dclspc SectionPainter( int mode, common::Interval<Length> segmentLimits = {epsilon__length,plausible_maximum_length}, Length e = epsilon__length );
	protected:
		void dclspc StartPaint( const spat::Frame<Length,One>& rFrame, Length offset, const Section& section ) override;
		void dclspc PaintSegment( const spat::Frame<Length,One>& rFrame, Length segmentLength, const Section& section ) override;
		virtual void AddVertex( const spat::Frame<Length,One>& rFrame, const Section::SectionPoint& pt, const spat::Position2D<One>& textcoord ) = 0;
		virtual void AddTriangle( unsigned int i0, unsigned int i1, unsigned int i2 ) = 0;
	private:
		void AddVertexRowAt( const spat::Frame<Length,One>& rFrame, const Section& section );
		int							m_NextIndex;
		Value<Dimension<-1,0,0>>	m_TexScaleU;
		One							m_TexCoordU;
	};

	/// \brief Paints data to buffers
	template<typename PointType_>
	class BufferedPainter_Base : public SectionPainter{
	public:
		using PointType = PointType_;

		BufferedPainter_Base(	std::vector<PointType>& points, 
								std::vector<int>& indices, 
								int mode = TrackPainter::Mode::mode_default, 
								common::Interval<Length> segmentLimits = {epsilon__length,plausible_maximum_length}, 
								Length e = epsilon__length )
		:	SectionPainter	{mode,segmentLimits,e},
			m_Points		{points},
			m_Indices		{indices},
			m_IndexOffset	{common::narrow_cast<int>(m_Points.size())}
		{
		}

	protected:
		void StartPaint( const spat::Frame<Length,One>& rFrame, Length offset, const Section& section ) override{
			m_IndexOffset = common::narrow_cast<int>(m_Points.size());
			SectionPainter::StartPaint( rFrame, offset, section );
		}
		
		void AddTriangle( unsigned int i0, unsigned int i1, unsigned int i2 ) override{
			m_Indices.push_back( m_IndexOffset + i0 );
			m_Indices.push_back( m_IndexOffset + i1 );
			m_Indices.push_back( m_IndexOffset + i2 );
		}

		std::vector<PointType>& m_Points;
	private:
		std::vector<int>&	m_Indices;
		int					m_IndexOffset;
	};


	template<typename PointType>
	class BufferedPainter : public BufferedPainter_Base<PointType>{
	public:
		BufferedPainter(	std::vector<PointType>& points, 
							std::vector<int>& indices, 
							int mode = TrackPainter::Mode::mode_default, 
							common::Interval<Length> segmentLimits = {epsilon__length,plausible_maximum_length}, 
							Length e = epsilon__length )
		:	BufferedPainter_Base<PointType>{points,indices,mode,segmentLimits,e}
		{
		}

	protected:
	};


	template<>
	class BufferedPainter<spat::Position<Length>> : public BufferedPainter_Base<spat::Position<Length>>{
	public:
		BufferedPainter(	std::vector<PointType>& points, 
							std::vector<int>& indices, 
							int mode = TrackPainter::Mode::mode_default, 
							common::Interval<Length> segmentLimits = {epsilon__length,plausible_maximum_length}, 
							Length e = epsilon__length )
		:	BufferedPainter_Base<PointType>{points,indices,mode,segmentLimits,e}
		{
		}

	protected:
		void AddVertex( const spat::Frame<Length,One>& rFrame, const Section::SectionPoint& pt, const spat::Position2D<One>& /*textcoord*/ ) override{
			m_Points.push_back( rFrame.P + pt.p.x * rFrame.N + pt.p.y * rFrame.B );
		}
	};

	template<>
	class BufferedPainter<std::pair<spat::VectorBundle<Length,One>,spat::Position2D<One>>> : public BufferedPainter_Base<std::pair<spat::VectorBundle<Length,One>,spat::Position2D<One>>>{
	public:
		BufferedPainter(	std::vector<PointType>& points, 
							std::vector<int>& indices, 
							int mode = TrackPainter::Mode::mode_default, 
							common::Interval<Length> segmentLimits = {epsilon__length,plausible_maximum_length}, 
							Length e = epsilon__length )
		:	BufferedPainter_Base<PointType>{points,indices,mode,segmentLimits,e}
		{
		}

	protected:
		void AddVertex( const spat::Frame<Length,One>& rFrame, const Section::SectionPoint& pt, const spat::Position2D<One>& textcoord ) override{
			m_Points.push_back( {{	rFrame.P + pt.p.x * rFrame.N + pt.p.y * rFrame.B, 
									pt.n.dx * rFrame.N + pt.n.dy * rFrame.B },
									textcoord} );
		}
	};

	template<>
	class BufferedPainter<std::pair<spat::VectorBundle2<Length,One>,spat::Position2D<One>>> : public BufferedPainter_Base<std::pair<spat::VectorBundle2<Length,One>,spat::Position2D<One>>>{
	public:
		BufferedPainter(	std::vector<PointType>& points, 
							std::vector<int>& indices, 
							int mode = TrackPainter::Mode::mode_default, 
							common::Interval<Length> segmentLimits = {epsilon__length,plausible_maximum_length}, 
							Length e = epsilon__length )
		:	BufferedPainter_Base<PointType>{points,indices,mode,segmentLimits,e}
		{
		}

	protected:
		void AddVertex( const spat::Frame<Length,One>& rFrame, const Section::SectionPoint& pt, const spat::Position2D<One>& textcoord ) override{
			m_Points.push_back( {{	rFrame.P + pt.p.x * rFrame.N + pt.p.y * rFrame.B, 
									rFrame.T,
									pt.n.dx * rFrame.N + pt.n.dy * rFrame.B },
									textcoord} );
		}
	};
}