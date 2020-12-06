/* Reader.h */
#ifndef READER_H
#define READER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

#include "Zone.h"
#include "Agent.h"
#include "DiseaseStage.h"
#include "Policy.h"
#include "Geography.h"

// Short alias for the namespace
namespace pt = boost::property_tree;

// Redefine boost geometries
typedef boost::geometry::model::d2::point_xy<double> point;
typedef boost::geometry::model::polygon<point> polygon;
typedef boost::geometry::model::multi_polygon<polygon> multipolygon;

namespace Reader{
    // Read boundary
    void getBounds(std::string path, std::string filename, double* xmin, double* extX, double* ymin, double* extY);

    // Read border and SIT Zones
    void getBorders(std::string path, std::string filename, int rank, std::vector<Zone*>& zones, std::vector<Border*>& border);

    // Read agents file
    void getAgents(std::string path, std::string filename, int rank, int* n, std::vector<RepastHPCAgent*>& agents);

    // Check disease stage
    DiseaseStage checkDiseaseStage(int d);

    // Read policies
    void getPolicies(int crank, std::string path, std::string filename, int stopAt, std::vector<policy>& policies);

    // Check policy type
    Policy checkPolicyType(int d);

    // Check if file exists
    bool fileExists(std::string& fileName);
}

#endif