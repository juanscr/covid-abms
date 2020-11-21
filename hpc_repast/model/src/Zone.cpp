/*Zone.cpp*/
#include "Zone.h"
Zone::~Zone(){};

void Zone::createMultipolygon(){
    boost::geometry::read_wkt(wkt, vertices);
}

Border::~Border(){};