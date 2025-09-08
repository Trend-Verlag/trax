//	trax track library
//	AD 2014 
//
//  "You are too clever to be mentally ill."
//
//						The Indelicates
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

#include "common/Helpers.h"
#include <boost/property_tree/ptree.hpp>


namespace spat{

	template<typename> struct Box;
	template<typename,typename> struct Frame;
	template<typename> struct Position;
	template<typename> struct Position2D;
	template<typename> struct Rect;
	template<typename> struct Vector;
	template<typename,typename> struct VectorBundle;
	template<typename,typename> struct VectorBundle2;

	/// \brief ptree operator support
	///
	/// If you want to use these in a namespace that itself declares overloads
	/// of these operators, use using declarations to bring them into scope 
	/// (e.g.: using spat::ptreesupport::operator<<;); otherwise they will be
	/// hidden by the namespace's own overloads.
	namespace ptreesupport{

		/// \name Property Tree Streaming Support for spat classes
		/// \brief The operators will write the objects with their XML names and syntax.
		///@{
		template<typename Valtype>
		boost::property_tree::ptree&	operator << ( boost::property_tree::ptree& opt, const Position<Valtype>& p );
		template<typename Valtype>
		boost::property_tree::ptree&	operator >> ( boost::property_tree::ptree& ipt, Position<Valtype>& p );
		template<typename Valtype>
		boost::property_tree::ptree&	operator << ( boost::property_tree::ptree& opt, const Position2D<Valtype>& p );
		template<typename Valtype>
		boost::property_tree::ptree&	operator >> ( boost::property_tree::ptree& ipt, Position2D<Valtype>& p );
		template<typename Valtype>
		boost::property_tree::ptree&	operator << ( boost::property_tree::ptree& opt, const Vector<Valtype>& v );
		template<typename Valtype>
		boost::property_tree::ptree&	operator >> ( boost::property_tree::ptree& ipt, Vector<Valtype>& v );
		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree&	operator << ( boost::property_tree::ptree& opt, const VectorBundle<Valtype,ValtypeT>& vb );
		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree&	operator >> ( boost::property_tree::ptree& ipt, VectorBundle<Valtype,ValtypeT>& vb );
		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree&	operator << ( boost::property_tree::ptree& opt, const VectorBundle2<Valtype,ValtypeT>& vb2 );
		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree&	operator >> ( boost::property_tree::ptree& ipt, VectorBundle2<Valtype,ValtypeT>& vb2 );
		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree&	operator << ( boost::property_tree::ptree& opt, const Frame<Valtype,ValtypeT>& f );
		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree&	operator >> ( boost::property_tree::ptree& ipt, Frame<Valtype,ValtypeT>& f );
		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Box<Valtype>& section );
		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Rect<Valtype>& rect );
		///@}


		/// \name Property Tree read/write support for spat classes
		/// \brief The methods will expect the XML names and syntax.
		/// 
		/// These read methods will leave the property tree unchanged while
		/// the operator versions will remove the read tags.
		///@{
		template<typename Valtype> inline
		void ReadPosition( const boost::property_tree::ptree& pt, Position<Valtype>& position ){
			position.x = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.x", Valtype{0} ));
			position.y = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.y", Valtype{0} ));
			position.z = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.z", Valtype{0} ));
		}

		template<typename Valtype> inline
		bool FindPosition( const boost::property_tree::ptree& pt, Position<Valtype>& position ){
			auto iter = pt.find( "Position" );
			if( iter == pt.not_found() )
				return false;

			ReadPosition( iter->second, position );
			return true;
		}

		template<typename Valtype> inline
		void ReadPosition2D( const boost::property_tree::ptree& pt, Position2D<Valtype>& position ){
			position.x = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.x", Valtype{0} ));
			position.y = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.y", Valtype{0} ));
		}

		template<typename Valtype> inline
		void ReadVector( const boost::property_tree::ptree& pt, Vector<Valtype>& vector ){
			vector.dx = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.dx", Valtype{1} ));
			vector.dy = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.dy", Valtype{0} ));
			vector.dz = common::DealDenormalizedNumbers(pt.get( "<xmlattr>.dz", Valtype{0} ));
		}

		template<typename Valtype> inline
		bool FindVector( const boost::property_tree::ptree& pt, Vector<Valtype>& vector ){
			const auto iter = pt.find( "Vector" );
			if( iter == pt.not_found() )
				return false;

			ReadVector( iter->second, vector );
			return true;
		}

		template<typename Valtype,typename ValtypeT> inline
		void ReadFrame( const boost::property_tree::ptree& pt, Frame<Valtype,ValtypeT>& frame ){
			frame.Init();

			auto iter = pt.begin();

			if( iter == pt.end() ||
				iter->first != "Position" )
				return;
			ReadPosition( iter->second, frame.P );

			if( ++iter == pt.end() ||
				iter->first != "Vector" )
				return;
			ReadVector( iter->second, frame.T );

			if( ++iter == pt.end() ||
				iter->first != "Vector" )
				return;
			ReadVector( iter->second, frame.N );

			if( ++iter == pt.end() ||
				iter->first != "Vector" )
				return;
			ReadVector( iter->second, frame.B );
		}

		template<typename Valtype,typename ValtypeT> inline
		bool FindFrame( const boost::property_tree::ptree& pt, Frame<Valtype,ValtypeT>& frame ){
			const auto iter = pt.find( "Frame" );
			if( iter == pt.not_found() )
				return false;

			ReadFrame( iter->second, frame );
			return true;
		}

		template<typename Valtype,typename ValtypeT> inline
		void ReadVectorBundle( const boost::property_tree::ptree& pt, VectorBundle<Valtype,ValtypeT>& VectorBundle ){
			VectorBundle.Init();

			auto iter = pt.begin();

			if( iter == pt.end() ||
				iter->first != "Position" )
				return;
			ReadPosition( iter->second, VectorBundle.P );

			if( ++iter == pt.end() ||
				iter->first != "Vector" )
				return;
			ReadVector( iter->second, VectorBundle.T );
		}

		template<typename Valtype,typename ValtypeT> inline
		void ReadVectorBundle2( const boost::property_tree::ptree& pt, VectorBundle2<Valtype,ValtypeT>& VectorBundle2 ){
			VectorBundle2.Init();

			auto iter = pt.begin();

			if( iter == pt.end() ||
				iter->first != "Position" )
				return;
			ReadPosition( iter->second, VectorBundle2.P );

			if( ++iter == pt.end() ||
				iter->first != "Vector" )
				return;
			ReadVector( iter->second, VectorBundle2.T );

			if( ++iter == pt.end() ||
				iter->first != "Vector" )
				return;
			ReadVector( iter->second, VectorBundle2.N );
		}

		template<typename Valtype> inline
		void ReadRect( const boost::property_tree::ptree& pt, Rect<Valtype>& rect ){
			rect.Left( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.left", Valtype{0} )) );
			rect.Top( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.top", Valtype{0} )) );
			rect.Right( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.right", Valtype{0} )) );
			rect.Bottom( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.bottom", Valtype{0} )) );
		}

		template<typename Valtype> inline
		void ReadArea( const boost::property_tree::ptree& pt, Box<Valtype>& area ){
			area.NearX( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.xnear", Valtype{0} )) );
			area.NearY( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.ynear", Valtype{0} )) );
			area.NearZ( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.znear", Valtype{0} )) );
			area.FarX( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.xfar", Valtype{0} )) );
			area.FarY( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.yfar", Valtype{0} )) );
			area.FarZ( common::DealDenormalizedNumbers(pt.get( "<xmlattr>.zfar", Valtype{0} )) );
		}

		template<typename Valtype> inline
		bool FindArea( const boost::property_tree::ptree& pt, Box<Valtype>& area ){
			auto iter = pt.find( "Area" );
			if( iter == pt.not_found() )
				return false;

			ReadArea( iter->second, area );
			return true;
		}
		///@}

		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const Position<Valtype>& p ){
			boost::property_tree::ptree ptPosition;
			ptPosition.add( "<xmlattr>.x", p.x );
			ptPosition.add( "<xmlattr>.y", p.y );
			ptPosition.add( "<xmlattr>.z", p.z );
			opt.add_child( "Position", ptPosition );
			return opt;
		}

		template<typename Valtype>
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, Position<Valtype>& p ){
			for( auto iter = ipt.begin(); iter != ipt.end(); ++iter ){
				if( (*iter).first == "Position" ){
					p.x = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.x", Valtype{0} ));
					p.y = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.y", Valtype{0} ));
					p.z = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.z", Valtype{0} ));

					ipt.erase( iter );
					return ipt;
				}
			}

			assert( !"Syntax error: no Position tag!" );
			return ipt;
		}

		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const Position2D<Valtype>& p ){
			boost::property_tree::ptree ptPosition;
			ptPosition.add( "<xmlattr>.x", p.x );
			ptPosition.add( "<xmlattr>.y", p.y );
			opt.add_child( "Position2D", ptPosition );
			return opt;
		}

		template<typename Valtype>
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, Position2D<Valtype>& p ){
			for( auto iter = ipt.begin(); iter != ipt.end(); ++iter ){
				if( (*iter).first == "Position2D" ){
					p.x = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.x", Valtype{0} ));
					p.y = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.y", Valtype{0} ));

					ipt.erase( iter );
					return ipt;
				}
			}

			assert( !"Syntax error: no Position tag!" );
			return ipt;
		}

		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const Vector<Valtype>& v ){
			boost::property_tree::ptree ptVector;
			ptVector.add( "<xmlattr>.dx", v.dx );
			ptVector.add( "<xmlattr>.dy", v.dy );
			ptVector.add( "<xmlattr>.dz", v.dz );
			opt.add_child( "Vector", ptVector );
			return opt;
		}

		template<typename Valtype>
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, Vector<Valtype>& v ){
			for( auto iter = ipt.begin(); iter != ipt.end(); ++iter ){
				if( (*iter).first == "Vector" ){
					v.dx = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.dx", Valtype{1} ));
					v.dy = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.dy", Valtype{0} ));
					v.dz = common::DealDenormalizedNumbers((*iter).second.get( "<xmlattr>.dz", Valtype{0} ));

					ipt.erase( iter );
					return ipt;
				}
			}

			assert( !"Syntax error: no Vector tag!" );
			return ipt;
		}

		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const VectorBundle<Valtype,ValtypeT>& vb ){
			boost::property_tree::ptree ptVectorBundle;
			ptVectorBundle << vb.P << vb.T;
			opt.add_child( "VectorBundle", ptVectorBundle );
			return opt;
		}

		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, VectorBundle<Valtype,ValtypeT>& vb ){
			for( auto iter = ipt.begin(); iter != ipt.end(); ++iter ){
				if( (*iter).first == "VectorBundle" )
				{
					(*iter).second >> vb.P >> vb.T;
					ipt.erase( iter );
					return ipt;
				}
			}

			assert( !"Syntax error: no VectorBundle tag!" );
			return ipt;
		}

		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const VectorBundle2<Valtype,ValtypeT>& vb2 ){
			boost::property_tree::ptree ptVectorBundle2;
			ptVectorBundle2 << vb2.P << vb2.T << vb2.N;
			opt.add_child( "VectorBundle2", ptVectorBundle2 );
			return opt;
		}

		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, VectorBundle2<Valtype,ValtypeT>& vb2 ){
			for( auto iter = ipt.begin(); iter != ipt.end(); ++iter ){
				if( (*iter).first == "VectorBundle2" )
				{
					(*iter).second >> vb2.P >> vb2.T >> vb2.N;
					ipt.erase( iter );
					return ipt;
				}
			}

			assert( !"Syntax error: no VectorBundle2 tag!" );
			return ipt;
		}

		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const Frame<Valtype,ValtypeT>& f ){
			boost::property_tree::ptree ptFrame;
			ptFrame << f.P << f.T << f.N << f.B;
			opt.add_child( "Frame", ptFrame );
			return opt;
		}

		template<typename Valtype,typename ValtypeT>
		boost::property_tree::ptree& operator >> ( boost::property_tree::ptree& ipt, Frame<Valtype,ValtypeT>& f ){
			for( auto iter = ipt.begin(); iter != ipt.end(); ++iter ){
				if( (*iter).first == "Frame" )
				{
					(*iter).second >> f.P >> f.T >> f.N >> f.B;
					ipt.erase( iter );
					return ipt;
				}
			}

			assert( !"Syntax error: no Frame tag!" );
			return ipt;
		}

		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Box<Valtype>& area ){
			boost::property_tree::ptree ptArea;

			ptArea.add( "<xmlattr>.xnear", area.NearX() );
			ptArea.add( "<xmlattr>.xfar", area.FarX() );
			ptArea.add( "<xmlattr>.ynear", area.NearY() );
			ptArea.add( "<xmlattr>.yfar", area.FarY() );
			ptArea.add( "<xmlattr>.znear", area.NearZ() );
			ptArea.add( "<xmlattr>.zfar", area.FarZ() );

			pt.add_child( "Area", ptArea );
			return pt;
		}

		template<typename Valtype>
		boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Rect<Valtype>& rect ){
			boost::property_tree::ptree ptRect;

			ptRect.add( "<xmlattr>.left", rect.Left() );
			ptRect.add( "<xmlattr>.top", rect.Top() );
			ptRect.add( "<xmlattr>.right", rect.Right() );
			ptRect.add( "<xmlattr>.bottom", rect.Bottom() );

			pt.add_child( "Rect", ptRect );
			return pt;
		}

	} // namespace ptreesupport
}

