/* Geography.h */
#ifndef GEOGRAPHY_H
#define GEOGRAPHY_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include <stdio.h>
#include <vector>

#include "Zone.h"
#include "Agent.h"
#include <fstream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

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
    typedef boost::geometry::model::multi_polygon<polygon> multipolygon;

    /**
     * Calulate distance between two points in meters
     */
    double getDistance(double x1, double y1, double x2, double y2);

    /**
     * Generate a random point given a center and a radius
    */
    void genDistance(int rank, double minX, double maxX, double minY, double maxY, double x, double y, double factor, double* newX, double* newY);

    /**
     * Generate a random point given a centar, a radius and a polygon
    */
   int genDistancePoly(int rank, repast::AgentId id, std::vector<Border*> p, double ax, double ay, double x, double y, double factor, double* newX, double* newY);

   /**
    * Check within to a set of multipolygons
   */
   bool checkWithin(point q, std::vector<Border*> p, int* core);

    /**
    * Get process given a location
    */
   int getProcess(double x, double y, int procsX, int procsY, double originX, double originY, double extentX, double extentY);
};

#endif