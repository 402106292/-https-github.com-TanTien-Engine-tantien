// Copyright (c) 2008  INRIA Sophia-Antipolis (France), ETH Zurich (Switzerland).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.3.1/Intersections_3/include/CGAL/Intersections_3/internal/Bbox_3_Sphere_3_do_intersect.h $
// $Id: Bbox_3_Sphere_3_do_intersect.h 7785a9c 2021-03-03T15:25:30+00:00 Andreas Fabri
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Camille Wormser, Jane Tournois, Pierre Alliez


#ifndef CGAL_INTERNAL_INTERSECTIONS_3_BBOX_3_SPHERE_3_DO_INTERSECT_H
#define CGAL_INTERNAL_INTERSECTIONS_3_BBOX_3_SPHERE_3_DO_INTERSECT_H

#include <CGAL/Sphere_3.h>
#include <CGAL/Bbox_3.h>

#include <CGAL/number_utils.h>


namespace CGAL {

namespace Intersections {

namespace internal {

  template <class K, class Box3>
    bool do_intersect_sphere_box_3(const typename K::Sphere_3& sphere,
                                   const Box3& bbox,
                                   const K&)
    {
        typedef typename K::FT FT;
        typedef typename K::Point_3 Point;
        FT d = FT(0);
        FT distance = FT(0);
        FT sr = sphere.squared_radius();

                Point center = sphere.center();

                if(center.x() < (FT)bbox.xmin())
                {
                        d = (FT)bbox.xmin() - center.x();
                        d = square(d);
                        if(certainly(d > sr)){
                          return false;
                        }
                        distance = d;
                }
                else if(center.x() > (FT)bbox.xmax())
                {
                        d = center.x() - (FT)bbox.xmax();
                        d = square(d);
                        if(certainly(d > sr)){
                          return false;
                        }
                        distance = d;
                }

                if(center.y() < (FT)bbox.ymin())
                {
                        d = (FT)bbox.ymin() - center.y();
                        d = square(d);
                        if(certainly(d > sr)){
                          return false;
                        }
                        distance += d ;
                }
                else if(center.y() > (FT)bbox.ymax())
                {
                        d = center.y() - (FT)bbox.ymax();
                        d = square(d);
                        if(certainly(d > sr)){
                          return false;
                        }
                        distance += d;
                }

                if(center.z() < (FT)bbox.zmin())
                {
                        d = (FT)bbox.zmin() - center.z();
                        d = square(d);
                        distance += d;
                }
                else if(center.z() > (FT)bbox.zmax())
                {
                        d = center.z() - (FT)bbox.zmax();
                        d = square(d);
                        distance += d;
                }

                // For unknown reason this causes a syntax error on VC2005
                // but compiles fine on Linux and MAC
                //int i;
                //for(i = 0; i < 3; ++i)
                //{
                //        if(center[i] < (FT)bbox.min(i))
                //        {
                //                d = (FT)bbox.min(i) - center[i];
                //                distance += d * d;
                //        }
                //        else if(center[i] > (FT)bbox.max(i))
                //        {
                //                d = center[i] - (FT)bbox.max(i);
                //                distance += d * d;
                //        }
                //}

                return distance <= sr;
    }

    template <class K>
    bool do_intersect(const CGAL::Bbox_3& bbox,
                      const typename K::Sphere_3& sphere,
                      const K&)
    {
      return do_intersect_sphere_box_3(sphere, bbox, K());
    }


    template <class K>
    bool do_intersect(const typename K::Sphere_3& sphere,
                      const CGAL::Bbox_3& bbox,
                      const K&)
    {
          return do_intersect_sphere_box_3(sphere, bbox, K());
    }

    template <class K>
    bool do_intersect(const typename K::Iso_cuboid_3& bbox,
                      const typename K::Sphere_3& sphere,
                      const K&)
    {
      return do_intersect_sphere_box_3(sphere, bbox, K());
    }


    template <class K>
    bool do_intersect(const typename K::Sphere_3& sphere,
                      const typename K::Iso_cuboid_3& bbox,
                      const K&)
    {
          return do_intersect_sphere_box_3(sphere, bbox, K());
    }

} // namespace internal
} // namespace Intersections
} //namespace CGAL

#endif  // CGAL_INTERNAL_INTERSECTIONS_3_BBOX_3_SPHERE_3_DO_INTERSECT_H
