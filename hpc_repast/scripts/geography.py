#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from shapely.geometry import Polygon, Point, MultiPolygon
import shapely
import random
import numpy as np
from shapely import wkt

def update_mn(data, mun, zc, polygons):
    mid  = list(mun.keys())
    zpm = {}
    for ml in mid:
        zpm[ml] = []
        
        # Create polygon of municipality
        mun[ml]['pol'] = wkt.loads(mun[ml]['wkt'])
        
        # Generate cummulative probabilities for stratums
        mun[ml]['stratum'] = np.cumsum(mun[ml]['stratum']) / sum(mun[ml]['stratum'])
    
    for z in zc:
        item =  data[z]
        ml = str(item['loc'])
        zpm[ml].append(z)
        
    # Determine proportion of areas
    zpa = {}
    for ml in mid:
        zpa[ml] = []
        
        # Check if based on density
        if mun[ml]['density'] == False:
            # Number of sit zones
            nz = len(zpm[ml])
            # Probabilities of sit zones
            pz = np.ones(nz)
        else:
            pz = []
            for zone in zpm[ml]:
                pz.append(polygons[zone].area)
        
        # Get cummulative probabilities
        pz = np.cumsum(pz) / sum(pz)
        
        zpa[ml] = pz
    
    return mun, mid, zpm, zpa
    

def get_random_pip(poly):
    minx, miny, maxx, maxy = poly.bounds
    while True:
        p = Point(random.uniform(minx, maxx), random.uniform(miny, maxy))
        if poly.contains(p):
            return p

def type_pol(pol):
    # Valid geometry types
    type_m = shapely.geometry.multipolygon.MultiPolygon
    type_p = shapely.geometry.polygon.Polygon
    
    type_pol = type(pol)
        
    if type_pol == type_m:
        return "multi"
    elif type_pol == type_p:
        return "pol"
    else:
        raise Exception("Unknown geometry type")
        
def split_border(nc, cpy, agents, uniqueFamilies, nHomes, border):
    fj = np.array([i for i in range(nHomes)])
    fx = np.array([agents[u]["hPoint"].x for u in uniqueFamilies])
    fy = np.array([agents[u]["hPoint"].y for u in uniqueFamilies])
    fw = np.array([len(agents[u]["family"]) for u in uniqueFamilies])
    # plt.scatter(fx,fy,s=2)
    
    fs = np.argsort(fx).tolist()
    fx = fx[fs]
    fy = fy[fs]
    fw = fw[fs]
    fj = fj[fs]
    
    # Sort position
    fs = np.argsort(fy).tolist()
    fx = fx[fs]
    fy = fy[fs]
    fw = fw[fs]
    fj = fj[fs]
    
    inds, a = equisum_partition(fw, nc)
    
    xmin, ymin, xmax, ymax = border.bounds
    xs = [xmin, xmax]
    ys = [ymin]
    inds = list(inds)
    inds = [0] + inds + [nHomes]
        
    for j in range(1, cpy):
        i = inds[j]
        y1 = (fy[i] + fy[i+1]) / 2
        ys.append(y1)
    
    
    ys.append(ymax)   
    
    nxs = len(xs)
    nys = len(ys)
    
    # Create uniqueFamilies
    for j in range(cpy):
        
        t0 = inds[j]
        t1 = inds[j+1]
        
        for k in fj[t0:t1]:
            
            family = uniqueFamilies[k]
            
            members =  agents[family]["family"]
            
            for m in members:
                agents[m]["core"] = j
    
    return [xs, nxs, ys, nys], agents

def equisum_partition(arr, p):
    ac = arr.cumsum()

    #sum of the entire array
    partsum = ac[-1]//p

    #generates the cumulative sums of each part
    cumpartsums = np.array(range(1, p))*partsum

    #finds the indices where the cumulative sums are sandwiched
    inds = np.searchsorted(ac,cumpartsums)

    #split into approximately equal-sum arrays
    parts = np.split(arr, inds)

    return inds, parts

def find_borders(xy, polygons):
    # Polygons per process
    xs, nxs, ys, nys = xy
    bounds = []
    for i in range(nxs - 1):
        x0 = xs[i]
        x1 = xs[i+1]
        for j in range(nys - 1):
            y0 = ys[j]
            y1 = ys[j+1]
            bound = Polygon([(x0, y0),
                              (x0, y1),
                              (x1, y1),
                              (x1, y0)])
            bounds.append(bound)
    
    # Intersections of borders
    border = polygons['border']
    
    borders = []
    
    for bound in bounds:
        pol = bound.intersection(border)
        if type_pol(pol) == 'pol':
            pol = MultiPolygon([pol])
        borders.append(pol)
        
    # boundary
    boundary = polygons['border'].bounds
    
    # Intersect polygons with border
    for key in polygons.keys():
        polygons[key] = polygons[key].intersection(border)
        
        if type_pol(polygons[key])  == 'pol':
            polygons[key] = MultiPolygon([polygons[key]])
    
    return bounds, borders, boundary, polygons

def assign_zones(nc, data, zc, polygons, bounds):
    # SIT zones per process
    zpp = []
    ids = []
    zd = {}
    nzones = len(zc)
    
    for z in zc:
        zd[z] = {}
        zd[z]["cores"] = []
        zd[z]["areas"] = []
        zd[z]['nCores'] = 0

    # Polygons per process
    for bound in bounds:
        zpp.append({})
        ids.append([])
        nzones = 0
        
        for z in zc: 
            pol = polygons[z].intersection(bound)

    
            # If intersection is not empty
            if not pol.is_empty:
                if type_pol(pol) == 'pol':
                    pol = MultiPolygon([pol])
                
                ids[-1].append(z)
                zpp[-1][z] = {"name": z,
                              "zid": nzones,
                              "coords": pol.wkt,
                              "type": type_pol(pol),
                              "neighbors": [],
                              "cneighbors": [],
                              "polygon": pol}
            
            nzones += 1
 
    # Neighbors per process      
    for i in range(nc):
        zid = ids[i]
        for z in zid:
            neighbors = data[z]['neighbors']
            for neig in neighbors:
                sneig = str(neig)
                if sneig in ids[i]:
                    zpp[i][z]['neighbors'].append(neig)
                    zpp[i][z]['cneighbors'].append(ids[i].index(sneig))
    
    for z in zc:
        for i in range(nc):
            if z in ids[i]:
                zd[z]["nCores"] += 1
                zd[z]["cores"].append(i)
                zd[z]["areas"].append(zpp[i][z]["polygon"].area)
                
        zd[z]["areas"] = np.cumsum(zd[z]["areas"]) / sum(zd[z]["areas"])
    
    return zpp, ids, zd