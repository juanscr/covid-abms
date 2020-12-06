#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import json
from shapely import wkt
import pandas as pd
import numpy as np
from geopy.distance import geodesic as distance
import os
import errno

def polygonData(filename, filename2):
    
    with open(filename) as f:
        data = json.load(f)
        
    with open(filename2) as f:
        mun = json.load(f)

    # Codes of borders and zones
    codes = list(data.keys())
    
    # Codes of zones
    zc = codes[0:-1]
    
    # Create polygons
    polygons = {}
    
    for key in codes:
        polygons[key] = wkt.loads(data[key]["polygon"])
        
    border = polygons["border"]
    for z in zc:
        polygons[z] = polygons[z].intersection(border)
    
    return mun, data, zc, polygons

def eodWalks(eodw_name, xmin, ymin):
    # Read file
    eodw = pd.read_csv(eodw_name, sep=",", decimal=".", index_col=0)
    eodw_rows = list(eodw.index)
    eodw_cols = eodw.columns.tolist()
    
    averageWalks = np.diag(eodw)
    averageWalks = averageWalks / max(averageWalks)
    
    # Units in meters
    max_movement = 50
    dis_factor = 0.0001
    
    averageWalks *= (max_movement)*(dis_factor)
    
    # Average displacement
    dis_0 = list(zip(averageWalks*0 + xmin, averageWalks*0 + ymin))
    dis_1 = list(zip(averageWalks + xmin, averageWalks + ymin))
    
    dis = [*map(distance, dis_0, dis_1)]
    dis = [d.meters for d in dis]
    
    averageWalk = np.mean(dis)
    walks = {}
    
    for i in range(len(dis)):
        if dis[i] == 0:
            dis[i] = averageWalk  
        name = eodw_cols[i]
        walks[name] = dis[i]
        
    return eodw, eodw_rows, eodw_cols, walks, averageWalk

def eod(eod_name):
    # Read EOD file
    eod = pd.read_csv(eod_name, sep=",", decimal=",", index_col=0)
    eod_rows = list(eod.index)
    
    eod_rc = {}
    
    for row in eod_rows:
        r = eod.loc[row]
        r = r[r != 0]        
        eod_rc[row] = r
        
    return eod_rows, eod_rc

def exportGeography(nc, borders, zpp, ids, filename):
    geography_data = {}

    for i in range(nc):
        # Create dictioary
        geography_data[i] = {}
        
        # Border
        geography_data[i]["border"] = {}
        geography_data[i]["border"]["polygon"] = borders[i].wkt
        geography_data[i]["border"]["n"] = 0
        geography_data[i]["len"] = 0
        geography_data[i]["cores"] = [i]
        
        # Find neighbor borders
        k = 0
        for j in range(nc):
            
            if i != j:
                
                if borders[i].intersects(borders[j]):
                    
                    geography_data[i]["border"]["n"] += 1
                    geography_data[i]["border"]["n" + str(k)] = borders[j].wkt
                    geography_data[i]["cores"].append(j)
                    
                    k += 1
        
        if geography_data[i]["border"]["polygon"][0] == 'P':
            geography_data[i]["border"]["polygonType"] = 'pol'
        else:
            geography_data[i]["border"]["polygonType"] = 'multi'
            
        for j in range(len(zpp[i])):
            
            geography_data[i][j] = {}
            k = ids[i][j]
            
            geography_data[i][j]["id"] = zpp[i][k]["name"]
            geography_data[i][j]["polygon"] = zpp[i][k]["coords"]
            geography_data[i][j]["walk"] = zpp[i][k]["walk"]
            geography_data[i][j]["polygonType"] = zpp[i][k]["type"]
            geography_data[i][j]["neighbors"] = zpp[i][k]["neighbors"]
            geography_data[i][j]["neighborsCore"] = zpp[i][k]["cneighbors"]
        
        geography_data[i]["len"] = len(zpp[i])

        file = filename + '_' + str(i) + '.json'
        with open(file, "w") as f:
            json.dump(geography_data[i], f, indent=4)
    
    return geography_data

def exportBounds(boundary, filename):
    bounds = {}
    
    bounds["xmin"] = boundary[0]
    bounds["xmax"] = boundary[1]
    bounds["xext"] = boundary[1] - boundary[0]
    bounds["ymin"] = boundary[2]
    bounds["ymax"] = boundary[3]
    bounds["yext"] = boundary[3] - boundary[2]
    
    file = filename + '.json'
    
    with open(file, "w") as f:
            json.dump(bounds, f, indent=4)
    
    return bounds

def exportAgents(nc, agents, filename):
    agents_data = {}

    for i in range(nc):
        # Define empty dictionary
        agents_data[i] = {}
        agents_data[i]["len"] = 0
        
        agents_i = [agent for agent in agents if agent["core"] == i]
        
        j = 0
        
        for agent in agents_i:
            agents_data[i][j] = {}
            
            # Personal attributes
            agents_data[i][j]["idName"] = agent["name"]
            agents_data[i][j]["age"] = agent["age"]
            agents_data[i][j]["family"] = agent["familyID"]
            # Homeplace attribures
            agents_data[i][j]["homeCore"] = agent["core"]
            agents_data[i][j]["homeZoneName"] = agent["hZone"]
            agents_data[i][j]["homeX"] = agent["hPoint"].x
            agents_data[i][j]["homeY"] = agent["hPoint"].y
            agents_data[i][j]["homeWalk"] = agent["walk"]
            # Workplace attribures
            agents_data[i][j]["workCore"] = agent["wCore"]
            agents_data[i][j]["workZoneName"] = agent["wZone"]
            agents_data[i][j]["workX"] = agent["xwork"]
            agents_data[i][j]["workY"] = agent["ywork"]
            # Disease Stage attribures
            agents_data[i][j]["diseaseStage"] = agent["diseaseStage"]
            # Stratum
            agents_data[i][j]['stratum'] = agent['stratum']
            
            j += 1
            
        agents_data[i]["len"] = len(agents_data[i]) - 1
        
        file = filename + '_' + str(i) + '.json'
        with open(file, "w") as f:
            json.dump(agents_data[i], f, indent=4)
        
    return agents_data

def check_path(filename):
    if not os.path.exists(os.path.dirname(filename)):
        try:
            os.makedirs(os.path.dirname(filename))
        except OSError as exc: # Guard against race condition
            if exc.errno != errno.EEXIST:
                raise