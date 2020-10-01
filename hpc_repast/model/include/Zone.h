/*Zone*/
#ifndef ZONE_H
#define ZONE_H

#include <stdio.h>
#include <vector>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

class Zone{
    public:
        int ID;
        int core;
        std::vector<int> adjacentZones;
        // Polygon definition
        typedef boost::geometry::model::d2::point_xy<double> point;
        typedef boost::geometry::model::polygon<point> polygon;
        polygon vertices;

        Zone(){};
        ~Zone();
};

#endif