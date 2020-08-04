/* Geography.cpp */

#include "Geography.h"

double Geography::getDistance(double x1, double y1, double x2, double y2){

    // Convert degress to radians
    double lat1 = repast::toRadians(x1);
    double lon1 = repast::toRadians(y1);
    double lat2 = repast::toRadians(x2);
    double lon2 = repast::toRadians(y2);
    // Difference between latitude and longitude
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = pow(sin(dlat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon/2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    double distance = Geography::earth_radius*c;

    return distance;
}

void Geography::genDistance(double minX, double maxX, double minY, double maxY, double x, double y, double factor, double* newX, double* newY){

    // Random for latitude displacement
    double rand_lat = repast::Random::instance()->nextDouble();
    double rand_lon = sqrt(1 - pow(rand_lat, 2));

    // Calculate change in latitude and longitude
    double dlat = rand_lat * factor * Geography::delta_lat;
    double dlon = rand_lon * factor * Geography::delta_lon;

    // Get direction of latitude in longitude
    double rlat = repast::Random::instance()->nextDouble();
    double rlon = repast::Random::instance()->nextDouble();

    // Generate new position
    if (rlat > 0.5){
        *newX = min(max(x + dlat, minX), maxX);
    }else{
        *newX = min(max(x - dlat, minX), maxX);
    }

    if (rlon > 0.5){
        *newY = min(max(y + dlon, minY), maxY);
    }else{
        *newY = min(max(y - dlon, minY), maxY);
    }

}

Geography::Geography(void){}