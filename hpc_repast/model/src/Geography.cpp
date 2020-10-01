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

void Geography::genDistancePoly(polygon p, double x, double y, double factor, double* newX, double* newY){
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
    do{
        rand_lat = repast::Random::instance()->nextDouble();
        rand_lon = sqrt(1 - pow(rand_lat, 2));

        dlat = rand_lat * factor * Geography::delta_lat;
        dlon = rand_lon * factor * Geography::delta_lon;

        rlat = repast::Random::instance()->nextDouble();
        rlon = repast::Random::instance()->nextDouble();

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

    }while(! boost::geometry::within(newPoint, p));
}

void Geography::getExternalBorders(int crank, bool clockwise, std::string szp, int nx, int ny, polygon* pol, std::vector<polygon>* ext_borders, double* originX, double* extentX, double* originY, double* extentY, std::vector<Zone*>& zones){
    // Read file
	std::ifstream eod(szp + "eod.map");
    // Read first line of polygon
	std::string line;
	// Set first line to string
	std::getline(eod, line);
    // Read WKT
    boost::geometry::read_wkt(line, *pol);
    // Extract points from polygon
	std::vector<point> points = (*pol).outer();
    // Number of points
    int N = points.size();
    // Auxiliary variables
    int i, j, k, l;
    double m, xm1, xm2, ym1, ym2;
    // Auxiliary polygons
    polygon pl;
    // Auxiliary variable for adjacent zones
    std::vector<int> adjacentZones;
    std::vector<std::string> az;

    // Extreme points
    double xmin = points.at(0).x();
    double xmax = points.at(0).x();
    double ymin = points.at(0).y();
    double ymax = points.at(0).y();

    for (auto point : points){
        if (point.x() < xmin){
            xmin = point.x();
        } else if (point.x() > xmax){
            xmax = point.x();
        }
        if (point.y() < ymin){
            ymin = point.y();
        } else if (point.y() > ymax){
            ymax = point.y();
        }
    }

    // Check if there is only one process
    if(nx*ny == 1){
        ext_borders->push_back(*pol);
        return; // Corregir
    }else{
        // If there are more processes
        // Partitioning x axis
        double dx = xmax - xmin;
        std::vector<double> xi;
        xi.push_back(xmin);
        for (i = 0; i<nx-1; i++){
            xi.push_back(xmin + dx*(i+1)/nx);
        }
        xi.push_back(xmax);
        int nxi = xi.size();

        // Partitioning y axis
        double dy = ymax - ymin;
        std::vector<double> yi;
        yi.push_back(ymin);
        for (i = 0; i < ny-1; i++){
            yi.push_back(ymin + dy*(i+1)/ny);
        }
        yi.push_back(ymax);
        int nyi = yi.size();

        *originX = xmin;
        *originY = ymin;
        *extentX = dx;
        *extentY = dy;

        // Add middle points
        std::vector<point> vals;
        double x1, x2, y1, y2;
        for(i = 0; i < N-1; i++){
            // Add current point
            vals.push_back(points.at(i));
            // Sort point in x
            if (points.at(i).x() < points.at(i+1).x()){
                x1 = points.at(i).x();
                y1 = points.at(i).y();
                x2 = points.at(i+1).x();
                y2 = points.at(i+1).y();
            }else{
                x1 = points.at(i+1).x();
                y1 = points.at(i+1).y();
                x2 = points.at(i).x();
                y2 = points.at(i).y();
            }
            // Add points between xi
            for (j = 0; j < nxi-1; j++){
                if(x1 <= xi.at(j) && xi.at(j)<=x2 && x1 != x2){
                    m = (y2-y1)/(x2-x1);
                    vals.push_back(point(xi.at(j), y1+m*(xi.at(j)-x1)));
                }
            }

            // Sort point in y
            if (points.at(i).y() < points.at(i+1).y()){
                x1 = points.at(i).x();
                y1 = points.at(i).y();
                x2 = points.at(i+1).x();
                y2 = points.at(i+1).y();
            }else{
                x1 = points.at(i+1).x();
                y1 = points.at(i+1).y();
                x2 = points.at(i).x();
                y2 = points.at(i).y();
            }
            // Add points between yi
            for (j = 0; j < nyi-1; j++){
                if(y1 <= yi.at(j) && yi.at(j)<=y2 && y1 != y2){
                    m = (y2-y1)/(x2-x1);
                    vals.push_back(point(x1 + (yi.at(j)-y1)/m, yi.at(j)));
                }
            }
        }
        // Add the last point
        vals.push_back(points.at(N-1));
        // Update polygon
        (*pol).clear();
        boost::geometry::assign_points(*pol, vals);
        std::vector<point> ext_border;
        std::vector<point> intercepts;
        std::vector<int> S;
        for(i = 1; i < nxi; i++){
            xm1 = xi.at(i-1);
            xm2 = xi.at(i);
            for(j = 1; j < nyi; j++){
                pl.clear();
                ym1 = yi.at(j-1);
                ym2 = yi.at(j);
                findIntercepts(clockwise, *pol, xm1, xm2, ym1, ym2, &intercepts, &S);
                findInners(vals, &ext_border, xm1, xm2, ym1, ym2);
                for(k = 0; k < intercepts.size(); k++){
                    switch (S.at(i)){
                    case 0:
                        ext_border.insert(ext_border.begin(), intercepts.at(k));
                        break;
                    case 1:
                        for(l = 0; l < ext_border.size(); l++){
                            if(ext_border.at(l).x() == intercepts.at(k).x()){
                                break;
                            }
                            ext_border.insert(ext_border.begin()+(l+1), intercepts.at(k));
                        }
                        break;
                    case 2:
                        ext_border.push_back(intercepts.at(k));
                        break;
                    case 3:
                        ext_border.push_back(intercepts.at(k));
                        break;
                    default:
                        break;
                    }
                }
                if(ext_border.size()>0){
                    x1 = ext_border.at(0).x();
                    x1 = ext_border.at(0).y();
                    x2 = ext_border.back().x();
                    y2 = ext_border.back().y();
                    if (x1 != x2 || y1 != y2){
                        ext_border.push_back(ext_border.at(0));
                    }
                }
                boost::geometry::assign_points(pl, ext_border);
                ext_borders->push_back(pl);
            }
        }
        }
    // Read zones
    std::string delimiter = "|";
    size_t pos;
    std::string token;
    k = 0;
    while(std::getline(eod, line)){
        // Polygon of zone
        az.clear();
        adjacentZones.clear();
        pl.clear();
        i = 0;
        k++;
        pos = 0;
        // Read information
        while((pos = line.find(delimiter)) != std::string::npos){
            i++;
            token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            if(i==1){
                boost::geometry::read_wkt(token, pl);
            }else{
                boost::trim(token);
                j = repast::strToInt(token);
            }
        }
        boost::trim(line);
        boost::erase_all(line, " ");
        boost::split(az, line, boost::is_any_of(","), boost::token_compress_off);
        for (auto item : az){
            adjacentZones.push_back(repast::strToInt(item));
        }
        Zone* zone = new Zone();
        zone->ID = j;
        zone->core = crank;
        zone->adjacentZones = adjacentZones;
        zone->vertices = pl;
        zones.push_back(zone);
    }
}

void Geography::findIntercepts(bool clockwise, polygon pol, double xm1, double xm2, double ym1, double ym2, std::vector<point>* intercepts, std::vector<int>* S){
    // Clear vectors
    (*S).clear();
    (*intercepts).clear();
    if (clockwise){
        if(boost::geometry::within(point(xm2, ym2), pol)){
            (*S).push_back(0);
            (*intercepts).push_back(point(xm2, ym2));
        }
        if(boost::geometry::within(point(xm2, ym1), pol)){
            (*S).push_back(1);
            (*intercepts).push_back(point(xm2, ym1));
        }
        if(boost::geometry::within(point(xm1, ym1), pol)){
            (*S).push_back(2);
            (*intercepts).push_back(point(xm1, ym1));
        }
        if(boost::geometry::within(point(xm1, ym2), pol)){
            (*S).push_back(3);
            (*intercepts).push_back(point(xm1, ym2));
        }
    }else{
        if(boost::geometry::within(point(xm2, ym2), pol)){
            (*S).push_back(3);
            (*intercepts).push_back(point(xm2, ym2));
        }
        if(boost::geometry::within(point(xm1, ym2), pol)){
            (*S).push_back(2);
            (*intercepts).push_back(point(xm1, ym2));
        }
        if(boost::geometry::within(point(xm1, ym1), pol)){
            (*S).push_back(1);
            (*intercepts).push_back(point(xm1, ym1));
        }
        if(boost::geometry::within(point(xm2, ym1), pol)){
            (*S).push_back(0);
            (*intercepts).push_back(point(xm2, ym1));
        }
    }
}

void Geography::findInners(std::vector<point> v, std::vector<point>* b, double xm1, double xm2, double ym1, double ym2){
    // Clear border
    (*b).clear();
    // Update border
    for(auto p : v){
        if (p.x()>=xm1 && p.x()<=xm2 && p.y()>=ym1 && p.y()<=ym2){
            (*b).push_back(p);
        }
    }
}