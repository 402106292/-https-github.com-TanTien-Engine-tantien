// Copyright (c) 2018  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.3.1/Intersections_3/include/CGAL/Intersections_3/Line_3_Point_3.h $
// $Id: Line_3_Point_3.h 0f3305f 2020-01-16T17:20:13+01:00 Maxime Gimeno
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Maxime Gimeno

#ifndef CGAL_INTERSECTIONS_3_LINE_3_POINT_3_H
#define CGAL_INTERSECTIONS_3_LINE_3_POINT_3_H

#include <CGAL/Line_3.h>
#include <CGAL/Point_3.h>
#include <CGAL/Intersection_traits_3.h>

namespace CGAL {

namespace Intersections {

namespace internal {

template <class K>
inline bool
do_intersect(const typename K::Point_3 &pt,
             const typename K::Line_3 &line,
             const K& k)
{
  return k.has_on_3_object()(line,pt);
}

template <class K>
inline bool
do_intersect(const typename K::Line_3 &line,
             const typename K::Point_3 &pt,
             const K& k)
{
  return k.has_on_3_object()(line, pt);
}

template <class K>
typename CGAL::Intersection_traits
<K, typename K::Point_3, typename K::Line_3>::result_type
intersection(const typename K::Point_3 &pt,
             const typename K::Line_3 &line,
             const K& k)
{
  if (do_intersect(pt,line,k))
    return intersection_return<typename K::Intersect_3, typename K::Point_3, typename K::Line_3>(pt);
  return intersection_return<typename K::Intersect_3, typename K::Point_3, typename K::Line_3>();
}

template <class K>
typename CGAL::Intersection_traits
<K, typename K::Line_3, typename K::Point_3>::result_type
intersection(const typename K::Line_3 &line,
             const typename K::Point_3 &pt,
             const K& k)
{
  return internal::intersection(pt, line, k);
}

} // namespace internal
} // namespace Intersections

CGAL_INTERSECTION_FUNCTION(Point_3, Line_3, 3)
CGAL_DO_INTERSECT_FUNCTION(Point_3, Line_3, 3)

} //namespace CGAL

#endif // CGAL_INTERSECTIONS_3_LINE_3_POINT_3_H
