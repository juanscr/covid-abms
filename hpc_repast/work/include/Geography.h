/* Geography.h */
#ifndef GEOGRAPHY_H
#define GEOGRAPHY_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include <stdio.h>
#include <vector>

/* Geography */
namespace Geography{


    /**
     * PI
    */
    const double pi = std::acos(-1);

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

    /**
     * Calulate distance between two points in meters
     */
    double getDistance(double x1, double y1, double x2, double y2);

    /**
     * Generate a random point given a center and a radius
    */
    void genDistance(double minX, double maxX, double minY, double maxY, double x, double y, double factor, double* newX, double* newY);

};

#endif