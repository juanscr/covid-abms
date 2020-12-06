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

void Geography::genDistance(repast::Random* r, int rank,double minX, double maxX, double minY, double maxY, double x, double y, double factor, double* newX, double* newY){

    // Random for latitude displacement
    double rand_lat = r->nextDouble();
    double rand_lon = sqrt(1 - pow(rand_lat, 2));

    // Calculate change in latitude and longitude
    double dlat = rand_lat * factor * Geography::delta_lat;
    double dlon = rand_lon * factor * Geography::delta_lon;

    // Get direction of latitude in longitude
    double rlat = r->nextDouble();
    double rlon = r->nextDouble();

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

int Geography::genDistancePoly(repast::Random* r, int rank, repast::AgentId id, std::vector<Border*> p, double x, double y, double factor, double* newX, double* newY){
    // Point for new coordinate
    point newPoint;

    // Random for latitude displacement
    double rand_lat;
    double rand_lon;

    // Calculate change in latitude and longitude
    double dlat;
    double dlon;

    // Get direction of latitude in longitude
    double rlat;
    double rlon;
    int cont = 0;

    // Criteria and core
    int core = -1;
    bool cr = false;

    do{
        rand_lat = r->nextDouble();
        rand_lon = sqrt(1 - pow(rand_lat, 2));

        dlat = rand_lat * factor * Geography::delta_lat;
        dlon = rand_lon * factor * Geography::delta_lon;

        rlat = r->nextDouble();
        rlon = r->nextDouble();

        if (rlat > 0.5){
            *newX = x + dlat;
        }else{
            *newX = x - dlat;
        }

        if (rlon > 0.5){
            *newY = y + dlat;
        }else{
            *newY = y - dlat;
        }

        newPoint.set<0>(*newX);
        newPoint.set<1>(*newY);

        cont++;
        if (cont >= 15){
            std::cout << "failed at: r " << rank << " id " << id << " x " << x << " y " << y << " f " << factor << std::endl;
            break;
        }
        factor = factor * 0.5;
        cr = checkWithin(newPoint, p, &core);
    }while(!cr);

    return core;
}

bool Geography::checkWithin(point q, std::vector<Border*> p, int* core){
    for (Border* b : p){
        if (boost::geometry::within(q, b->border)) {
            *core = b->core;
            return true;
        }
    }
    return false;
}

int Geography::getProcess(double x, double y, int procsX, int procsY, double originX, double originY, double extentX, double extentY){
    int pX;
	int pY;

	for (int i = 0; i<procsX; i++){
		if ( originX + (extentX*i)/procsX <= x && x <= originX + (extentX*(i+1))/procsX ){
			pX = i;
			break;
		}
	}
	for (int i = 0; i<procsY; i++){
		if ( originY + (extentY*i)/procsY <= y && y<= originY + (extentY*(i+1))/procsY){
			pY = i;
			break;
		}
	}
	return procsY*pX + pY;
}