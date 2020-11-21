/*Zone*/
#ifndef ZONE_H
#define ZONE_H

#include <stdio.h>
#include <vector>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

class Border{
    public:
        int core;
        // Polygon definition
        typedef boost::geometry::model::d2::point_xy<double> point;
        typedef boost::geometry::model::polygon<point> polygon;
        typedef boost::geometry::model::multi_polygon<polygon> multipolygon;
        multipolygon border;
        Border(){};
        ~Border();
};

class Zone{
    public:
        // ID in core
        int ID;
        // Name
        std::string label;
        int core;
        std::vector<int> adjacentZonesID;
        std::vector<int> adjacentZonesLabel;
        double walkingAverage;
        // Polygon definition
        typedef boost::geometry::model::d2::point_xy<double> point;
        typedef boost::geometry::model::polygon<point> polygon;
        typedef boost::geometry::model::multi_polygon<polygon> multipolygon;
        std::string wkt;
        multipolygon vertices;
        Zone(){};
        ~Zone();
        void createMultipolygon();
};
#endif