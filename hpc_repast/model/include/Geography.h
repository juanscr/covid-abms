/* Geography.h */
#ifndef GEOGRAPHY_H
#define GEOGRAPHY_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include <stdio.h>
#include <vector>

#include "Zone.h"
#include <fstream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

/* Geography */
namespace Geography{
    /**
     * Earth radius (unit: meters). Reference: <pending>
     */
    const double earth_radius = 6378100;

    /**
     * Minimum variation of 1m in latitude
     * unit: sexagesimal degrees.
     * Reference: <pending>
     */
    const double delta_lat = 0.00000898320495133494;

    /**
     * Minimum variation of 1m in latitude
     * unit: sexagesimal degrees.
     * Reference: <pending>
     */
    const double delta_lon = 0.0000363395381030303;

    // Boost geography
    typedef boost::geometry::model::d2::point_xy<double> point;
    typedef boost::geometry::model::polygon<point> polygon;

    /**
     * Calulate distance between two points in meters
     */
    double getDistance(double x1, double y1, double x2, double y2);

    /**
     * Generate a random point given a center and a radius
    */
    void genDistance(double minX, double maxX, double minY, double maxY, double x, double y, double factor, double* newX, double* newY);

    /**
     * Generate a random point given a centar, a radius and a polygon
    */
   void genDistancePoly(polygon p, double x, double y, double factor, double* newX, double* newY);

   /**
    * Get bounds of a process given a border
    */
   void getExternalBorders(int crank, bool clockwise, std::string szp, int nx, int ny, polygon* pol, std::vector<polygon>* ext_borders, double* originX, double* extentX, double* originY, double* extentY, std::vector<Zone*>& zones);

   /**
    * Filter points
   */
   void findIntercepts(bool clockwise, polygon pol, double xm1, double xm2, double ym1, double ym2, std::vector<point>* intercepts, std::vector<int>* S);
   void findInners(std::vector<point> v, std::vector<point>* b, double xm1, double xm2, double ym1, double ym2);

};

#endif