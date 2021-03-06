// Copyright (c) 2018  GeometryFactory Sarl (France).
// All rights reserved.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.3.1/Stream_support/include/CGAL/IO/WKT/traits_multipoint.h $
// $Id: traits_multipoint.h 490a726 2020-01-20T10:42:46+01:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Maxime Gimeno

#ifndef CGAL_IO_WKT_TRAITS_MULTIPOINT_H
#define CGAL_IO_WKT_TRAITS_MULTIPOINT_H

#if BOOST_VERSION >= 105600 && (! defined(BOOST_GCC) || BOOST_GCC >= 40500)

#include <CGAL/internal/Geometry_container.h>

#include <boost/geometry/io/wkt/write.hpp>
#include <boost/geometry/io/wkt/read.hpp>

namespace boost {
namespace geometry {
namespace traits {

// WKT traits for MultiPoint
template< typename R >
struct tag<CGAL::internal::Geometry_container<R, multi_point_tag > >
{
  typedef multi_point_tag type;
};

} // namespace traits
} // namespace geometry
} // namespace boost

#endif // BOOST VERSION CHECKS
#endif // CGAL_IO_WKT_TRAITS_MULTIPOINT_H
