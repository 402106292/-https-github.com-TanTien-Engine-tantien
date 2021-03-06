// Copyright (c) 1998-2004
// Utrecht University (The Netherlands),
// ETH Zurich (Switzerland),
// INRIA Sophia-Antipolis (France),
// Max-Planck-Institute Saarbruecken (Germany),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.3.1/Distance_3/include/CGAL/squared_distance_3_2.h $
// $Id: squared_distance_3_2.h 2820a94 2021-05-25T10:46:29+02:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Geert-Jan Giezeman, Andreas Fabri


#ifndef CGAL_DISTANCE_3_2_H
#define CGAL_DISTANCE_3_2_H

#include <CGAL/squared_distance_3_0.h>
#include <CGAL/squared_distance_3_1.h>
#include <CGAL/wmult.h>

#include <CGAL/Point_3.h>
#include <CGAL/Segment_3.h>
#include <CGAL/Line_3.h>
#include <CGAL/Ray_3.h>
#include <CGAL/Triangle_3.h>
#include <CGAL/Plane_3.h>

namespace CGAL {

namespace internal {

template <class K>
bool
contains_vector(const typename K::Plane_3 &pl,
                const typename K::Vector_3 &vec,
                const K&)
{
  typedef typename K::RT RT;
  return pl.a()*vec.hx() + pl.b()*vec.hy() + pl.c() * vec.hz() == RT(0);
}


template <class K>
inline typename K::FT
squared_distance(
    const typename K::Point_3 & pt,
    const typename K::Plane_3 & plane,
    const K& k)
{
  typename K::Construct_vector_3 construct_vector;
  typedef typename K::Vector_3 Vector_3;
  Vector_3 diff = construct_vector(plane.point(), pt);
  return squared_distance_to_plane(plane.orthogonal_vector(), diff, k);
}



template <class K>
inline typename K::FT
squared_distance(
    const typename K::Plane_3 & plane,
    const typename K::Point_3 & pt,
    const K& k)
{
    return squared_distance(pt, plane, k);
}

template <class K>
typename K::FT
squared_distance(
    const typename K::Line_3 &line,
    const typename K::Plane_3 &plane,
    const K& k)
{
    typedef typename K::FT FT;
    if (contains_vector(plane, line.direction().vector(), k))
        return squared_distance(plane, line.point(), k);
    return FT(0);
}


template <class K>
inline typename K::FT
squared_distance(
    const typename K::Plane_3 & p,
    const typename K::Line_3 & line,
    const K& k)
{
    return squared_distance(line, p, k);
}

template <class K>
typename K::FT
squared_distance(
    const typename K::Ray_3 &ray,
    const typename K::Plane_3 &plane,
    const K& k)
{
    typename K::Construct_vector_3 construct_vector;
    typedef typename K::Point_3 Point_3;
    typedef typename K::Vector_3 Vector_3;
    typedef typename K::RT RT;
    typedef typename K::FT FT;
    const Point_3 &start = ray.start();
    const Point_3 &planepoint = plane.point();
    Vector_3 start_min_pp = construct_vector(planepoint, start);
    Vector_3 end_min_pp = ray.direction().vector();
    const Vector_3 &normal = plane.orthogonal_vector();
    RT sdm_rs2pp = wdot(normal, start_min_pp, k);
    RT sdm_re2pp = wdot(normal, end_min_pp, k);
    switch (CGAL_NTS sign(sdm_rs2pp)) {
    case -1:
        if (sdm_re2pp > RT(0))
            return FT(0);
        return squared_distance_to_plane(normal, start_min_pp, k);
    case 0:
    default:
        return FT(0);
    case 1:
        if (sdm_re2pp < RT(0))
            return FT(0);
        return squared_distance_to_plane(normal, start_min_pp, k);
    }
}


template <class K>
inline typename K::FT
squared_distance(
    const typename K::Plane_3 & plane,
    const typename K::Ray_3 & ray,
    const K& k)
{
    return squared_distance(ray, plane, k);
}

template <class K>
typename K::FT
squared_distance(
    const typename K::Segment_3 &seg,
    const typename K::Plane_3 &plane,
    const K& k)
{
    typename K::Construct_vector_3 construct_vector;
    typedef typename K::Point_3 Point_3;
    typedef typename K::Vector_3 Vector_3;
    typedef typename K::RT RT;
    typedef typename K::FT FT;
    const Point_3 &start = seg.start();
    const Point_3 &end = seg.end();
    if (start == end)
        return squared_distance(start, plane, k);
    const Point_3 &planepoint = plane.point();
    Vector_3 start_min_pp = construct_vector(planepoint, start);
    Vector_3 end_min_pp = construct_vector(planepoint, end);
    const Vector_3 &normal = plane.orthogonal_vector();
    RT sdm_ss2pp = wdot(normal, start_min_pp, k);
    RT sdm_se2pp = wdot(normal, end_min_pp, k);
    switch (CGAL_NTS sign(sdm_ss2pp)) {
    case -1:
        if (sdm_se2pp >= RT(0))
            return FT(0);
        if (sdm_ss2pp * end_min_pp.hw() >= sdm_se2pp * start_min_pp.hw())
            return squared_distance_to_plane(normal, start_min_pp, k);
        else
            return squared_distance_to_plane(normal, end_min_pp, k);
    case 0:
    default:
        return FT(0);
    case 1:
        if (sdm_se2pp <= RT(0))
            return FT(0);
        if (sdm_ss2pp  * end_min_pp.hw() <= sdm_se2pp * start_min_pp.hw())
            return squared_distance_to_plane(normal, start_min_pp, k);
        else
            return squared_distance_to_plane(normal, end_min_pp, k);
    }
}


template <class K>
inline typename K::FT
squared_distance(
    const typename K::Plane_3 & plane,
    const typename K::Segment_3 & seg,
    const K& k)
{
    return squared_distance(seg, plane, k);
}


template <class K>
inline bool
on_left_of_triangle_edge(const typename K::Point_3 & pt,
                         const typename K::Vector_3 & normal,
                         const typename K::Point_3 & ep0,
                         const typename K::Point_3 & ep1,
                         const K& k)
{
  // return true iff pt is on the negative side of the plane defined
  // by (ep0, ep1) and normal
  typename K::Construct_vector_3 vector;
  typename K::Vector_3 edge = vector(ep0, ep1);
  typename K::Vector_3 diff = vector(ep0, pt);

  typedef typename K::RT RT;

  const bool result =
    RT(wdot(wcross(edge,
                   normal,
                   k),
            diff,
            k)) <= RT(0);
  return result;
}

template <class K>
inline void
squared_distance_to_triangle_RT(
    const typename K::Point_3 & pt,
    const typename K::Point_3 & t0,
    const typename K::Point_3 & t1,
    const typename K::Point_3 & t2,
    bool & inside,
    typename K::RT& num,
    typename K::RT& den,
    const K& k)
{
  typename K::Construct_vector_3 vector;
  typedef typename K::Vector_3 Vector_3;
  const Vector_3 e1 = vector(t0, t1);
  const Vector_3 oe3 = vector(t0, t2);
  const Vector_3 normal = wcross(e1, oe3, k);

  if(normal == NULL_VECTOR) {
    // The case normal==NULL_VECTOR covers the case when the triangle
    // is colinear, or even more degenerate. In that case, we can
    // simply take also the distance to the three segments.
    squared_distance_RT(pt, typename K::Segment_3(t2, t0), num, den, k);

    typename K::RT num2, den2;
    squared_distance_RT(pt, typename K::Segment_3(t1, t2), num2, den2, k);
    if(compare_quotients(num2,den2, num,den) == SMALLER) {
      num = num2;
      den = den2;
    }

    // should not be needed since at most 2 edges cover the full triangle in the degenerate case
    squared_distance_RT(pt, typename K::Segment_3(t0, t1), num2, den2, k);
    if(compare_quotients(num2,den2, num,den) == SMALLER){
      num = num2;
      den = den2;
    }

    return;
  }

  const bool b01 = on_left_of_triangle_edge(pt, normal, t0, t1, k);
  if(!b01) {
    squared_distance_RT(pt, typename K::Segment_3(t0, t1), num, den, k);
    return;
  }

  const bool b12 = on_left_of_triangle_edge(pt, normal, t1, t2, k);
  if(!b12) {
    squared_distance_RT(pt, typename K::Segment_3(t1, t2), num, den, k);
    return;
  }

  const bool b20 = on_left_of_triangle_edge(pt, normal, t2, t0, k);
  if(!b20) {
    squared_distance_RT(pt, typename K::Segment_3(t2, t0), num, den, k);
    return;
  }

  // The projection of pt is inside the triangle
  inside = true;
  squared_distance_to_plane_RT(normal, vector(t0, pt), num, den, k);
}

template <class K>
void
squared_distance_RT(
    const typename K::Point_3 & pt,
    const typename K::Triangle_3 & t,
    typename K::RT& num,
    typename K::RT& den,
    const K& k)
{
  typename K::Construct_vertex_3 vertex;
  bool inside = false;
  squared_distance_to_triangle_RT(pt,
                                  vertex(t, 0),
                                  vertex(t, 1),
                                  vertex(t, 2),
                                  inside,
                                  num,
                                  den,
                                  k);
}

template <class K>
inline typename K::FT
squared_distance_to_triangle(
    const typename K::Point_3 & pt,
    const typename K::Point_3 & t0,
    const typename K::Point_3 & t1,
    const typename K::Point_3 & t2,
    bool & inside,
    const K& k)
{
  typename K::Construct_vector_3 vector;
  typedef typename K::Vector_3 Vector_3;
  const Vector_3 e1 = vector(t0, t1);
  const Vector_3 oe3 = vector(t0, t2);
  const Vector_3 normal = wcross(e1, oe3, k);

  if(normal == NULL_VECTOR) {
    // The case normal==NULL_VECTOR covers the case when the triangle
    // is colinear, or even more degenerate. In that case, we can
    // simply take also the distance to the three segments.
    typename K::FT d1 = squared_distance(pt, typename K::Segment_3(t2, t0), k);
    typename K::FT d2 = squared_distance(pt, typename K::Segment_3(t1, t2), k);

    // should not be needed since at most 2 edges cover the full triangle in the degenerate case
    typename K::FT d3 = squared_distance(pt, typename K::Segment_3(t0, t1), k);

    return (std::min)( (std::min)(d1, d2), d3);
  }

  const bool b01 = on_left_of_triangle_edge(pt, normal, t0, t1, k);
  if(!b01) {
    return internal::squared_distance(pt, typename K::Segment_3(t0, t1), k);
  }

  const bool b12 = on_left_of_triangle_edge(pt, normal, t1, t2, k);
  if(!b12) {
    return internal::squared_distance(pt, typename K::Segment_3(t1, t2), k);
  }

  const bool b20 = on_left_of_triangle_edge(pt, normal, t2, t0, k);
  if(!b20) {
    return internal::squared_distance(pt, typename K::Segment_3(t2, t0), k);
  }

  // The projection of pt is inside the triangle
  inside = true;
  return squared_distance_to_plane(normal, vector(t0, pt), k);
}

template <class K>
inline typename K::FT
squared_distance(
    const typename K::Point_3 & pt,
    const typename K::Triangle_3 & t,
    const K& k)
{
  typename K::Construct_vertex_3 vertex = k.construct_vertex_3_object();
  bool inside = false;
  return squared_distance_to_triangle(pt,
                                      vertex(t, 0),
                                      vertex(t, 1),
                                      vertex(t, 2),
                                      inside,
                                      k);
}

} // namespace internal


template <class K>
bool
contains_vector(const Plane_3<K> &pl, const Vector_3<K> &vec)
{
  return internal::contains_vector(pl,vec, K());
}


template <class K>
inline
typename K::FT
squared_distance(
    const Point_3<K> & pt,
    const Plane_3<K> & plane)
{
  return internal::squared_distance(pt, plane, K());
}



template <class K>
inline
typename K::FT
squared_distance(
    const Plane_3<K> & plane,
    const Point_3<K> & pt)
{
    return internal::squared_distance(pt, plane, K());
}

template <class K>
inline
typename K::FT
squared_distance(
    const Line_3<K> &line,
    const Plane_3<K> &plane)
{
    return internal::squared_distance(line, plane, K());
}


template <class K>
inline
typename K::FT
squared_distance(
    const Plane_3<K> & p,
    const Line_3<K> & line)
{
    return internal::squared_distance(line, p, K());
}

template <class K>
inline
typename K::FT
squared_distance(
    const Ray_3<K> &ray,
    const Plane_3<K> &plane)
{
  return internal::squared_distance(ray, plane, K());
}



template <class K>
inline
typename K::FT
squared_distance(
    const Plane_3<K> & plane,
    const Ray_3<K> & ray)
{
    return internal::squared_distance(ray, plane, K());
}

template <class K>
inline
typename K::FT
squared_distance(
    const Segment_3<K> &seg,
    const Plane_3<K> &plane)
{
  return internal::squared_distance(seg, plane, K());

}


template <class K>
inline
typename K::FT
squared_distance(
    const Plane_3<K> & plane,
    const Segment_3<K> & seg)
{
    return internal::squared_distance(seg, plane, K());
}

template <class K>
inline
typename K::FT
squared_distance(const Point_3<K> & pt,
                 const Triangle_3<K> & t) {
  return internal::squared_distance(pt, t, K());
}


template <class K>
inline
typename K::FT
squared_distance(const Triangle_3<K> & t,
                 const Point_3<K> & pt) {
  return internal::squared_distance(pt, t, K());
}


template <class K>
inline
typename K::FT
squared_distance(const Plane_3<K> & p1,
                 const Plane_3<K> & p2) {
  K k;
  typename K::Construct_orthogonal_vector_3 ortho_vec =
      k.construct_orthogonal_vector_3_object();
  if (!internal::is_null(internal::wcross(ortho_vec(p1), ortho_vec(p2), k), k))
    return typename K::FT(0);
  else
    return internal::squared_distance(p1.point(), p2, k);
}

} //namespace CGAL


#endif
