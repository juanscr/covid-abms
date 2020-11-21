#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import numpy as np
import random
from multiprocessing import Pool
from itertools import repeat
from geography import get_random_pip

def get_walks(nc, zpp, eodw_cols, walks, averageWalk, agents):
    for core in range(nc):
        for key in zpp[core].keys():
            if key in eodw_cols: 
                zpp[core][key]["walk"] = walks[key]  
            else:
                zpp[core][key]["walk"] = averageWalk
                
    for i in range(len(agents)):
        core = agents[i]["core"]
        zone = agents[i]["hZone"]
        
        agents[i]["walk"] = zpp[core][zone]["walk"]
    
    return zpp, agents

def agents_init(nag):
    agents = []
    ages = [getRandomAge() for i in range(nag)]
    for i in range(nag):
        agents.append({"name": i,
                       "age": ages[i],
                       "family": []
                       }
                      )
    return agents

def agents_families(agents, nag):
    # Get child agents
    agentsChild = [agent["name"] for agent in agents if agent["age"] < 18]
    agentsAdult = [agent["name"] for agent in agents if agent["age"] > 17]
    
    # Probabilities of families
    familiySizes = [1, 2, 3, 4, 5, 6]
    familyProbs = [19, 23, 24, 19, 8, 7]
    
    # Create families 
    families = []
    families2 = []
    nag_r = 0
    fid = 0
    
    # Create enough families
    while nag_r <= nag:
    
        size = random.choices(familiySizes, weights = familyProbs, k=1)[0]
        
        if nag_r + size > nag:
            size = nag - nag_r
        
        if size <= 0:
            break
        
        families.append({"id": fid,
                         "size": size,
                         "members": []})
        
        nag_r += size
        
        if size > 1:
            families2.append(fid)
        
        fid += 1
    
    # Assign the first adult per family
    nfamilies = len(families)
    
    # Shuffle adults
    random.shuffle(agentsAdult)
    
    uniqueFamilies = [] 
    for i in range(nfamilies):
        families[i]["members"].append(agentsAdult[i])
        uniqueFamilies.append(agentsAdult[i])
         
    agentsAdult = agentsAdult[nfamilies:]
    
    # Shuffle agents without home
    agentsR = agentsAdult + agentsChild
    random.shuffle(agentsR)
    
    # Assign adult and childs to families
    i = 0
    for j in range(len(families2)):
        
        f = families2[j]
    
        size = families[f]["size"] - 1
        
        members = agentsR[i : i+size]
        
        # Update family
        families[f]["members"] += members
    
        i += size

    for family in families:
        for member in family["members"]:
            agents[member]["family"] = family["members"]
            agents[member]["familyID"] = family["id"]

    return agents, uniqueFamilies

def agents_spaces(nc, zc, ids, zpp):
    # Areas per process
    zonesCores = {}
    for z in zc:   
        cores = []
        
        # Processes where a zone is
        for i in range(nc):
            if z in ids[i]:
                cores.append(i)
        
        areas = []
        for core in cores:
            areas.append(zpp[core][z]["polygon"].area)
            
        areas = np.cumsum(areas) / sum(areas)
                
        # Polygons for the zone
        zonesCores[z] = {"ncores": len(cores),
                         "cores": cores,
                         "areas": areas}
    return zonesCores

def agents_sit(uniqueFamilies, zc):
    homes = []
    nHomes = len(uniqueFamilies)
    
    # Choose a SIT Zone
    zhomes = random.choices(zc, k=nHomes)
    random.shuffle(zhomes)
    
    for i in range(nHomes):
        homes.append({'zone':zhomes[i],
                      'point':0})
    
    return homes, nHomes

def agents_home(nc, agents, homes, uniqueFamilies, polygons):
    
    pol = [polygons[home["zone"]] for home in homes]
    
    if __name__ == "heuristics" :
        with Pool(nc) as p:    
            homes = p.starmap(random_point, zip(homes, pol))
            
    for i in range(len(uniqueFamilies)):
        family = agents[uniqueFamilies[i]]["family"]    
        for m in family:
            agents[m]["hPoint"] = homes[i]["point"]
            agents[m]["hZone"] = homes[i]["zone"]
    
    return agents, homes

def random_point(house, pol):
    point = get_random_pip(pol)
    
    house["point"] = point
    
    return house
    

def agents_houses(uniqueFamilies, zc, zonesCores):
    nHomes = len(uniqueFamilies)
    houses = {}
    
    # Choose a SIT Zone
    zhomes = random.choices(zc, k=nHomes)
    random.shuffle(zhomes)
    rs = np.random.rand(nHomes)
    
    # Process for the homeplace
    for i in range(nHomes):
        # zone
        zhome = zhomes[i]
        
        # processes
        zonecore = zonesCores[zhome]
        ncores = zonecore["ncores"]
        
        r = rs[i]
        
        for j in range(ncores):
            if r < zonecore["areas"][j]:
                core = zonecore["cores"][j]
                break;
        
        point = 0
    
        house = {'core': core,
                  'zoneID': zhome,
                  'point': point
                  }
    
        houses[i] = house
    
    h = [houses[i] for i in range(nHomes)]
    
    return h, nHomes

def get_random_point(house, zpp):
    
    core = house["core"]
    
    zoneIndex = house["zoneID"]
    
    pol = zpp[core][zoneIndex]["polygon"]
    
    point = get_random_pip(pol)
    
    house["point"] = point
    
    return house

def houses_locate(nc, zpp, h):
    
    if __name__ == "heuristics" :
        with Pool(nc) as p:    
            hresults = p.starmap(get_random_point, zip(h, repeat(zpp)) )
        
    return hresults

def agents_locate(agents, nc, zpp, h, nHomes, uniqueFamilies):
    
    hresults = houses_locate(nc, zpp, h)
    
    for i in range(nHomes):
        house = hresults[i]
        
        core = house["core"]
        zoneID = house["zoneID"]
        point = house["point"]
        
        family = agents[uniqueFamilies[i]]["family"]
        
         
        for m in family:
            agents[m]["coreHome"] = core
            agents[m]["zhName"] = zoneID
            agents[m]["zhID"] = zpp[core][zoneID]["zid"]
            agents[m]["xhome"] = point.x
            agents[m]["yhome"] = point.y
            agents[m]["walk"] = zpp[core][zoneID]["walk"]
        
    return agents, hresults

def agent_workplace(agent, eod_rows, eod_rc, zc, zd, zpp):
    # Find a workplace
    zoneID = int(agent["hZone"])
    
    if zoneID in eod_rows:
    
        travels = eod_rc[zoneID]
    
        player1 = random.randint(0, travels.shape[0]-1)
    
        for k in range(0, 10):
            
            player2 = random.randint(0, travels.shape[0]-1)
        
            decision = np.random.rand()
        
            tr1 = travels.iloc[player1]
            tr2 = travels.iloc[player2]
        
            suma = tr1 + tr2
            
            if tr1 < tr2:
                temp = player1
                player1 = player2
                player2 = temp
                
            if decision >= tr1 / suma:
                player1 = player2
                
        pid = travels.index[player1]
        
        if pid in zc:
            
            zone = zd[pid]
            zone_nc = zone["nCores"]
            
            if zone_nc == 1:
                fcore = zone["cores"][0]
                
            else:
                r = np.random.rand()
                
                for j in range (zone_nc):
                    if r < zone["areas"][j]:
                        fcore  = zone["cores"][j]
                        break
            
            point = get_random_pip(zpp[fcore][pid]["polygon"])
            
            agent["wZone"] = pid
            agent["wCore"] = fcore
            agent["zwID"] = zpp[fcore][pid]["zid"]
            agent["xwork"] = point.x
            agent["ywork"] = point.y

            return agent
    
    fcore = agent["core"]
    pid = agent["hZone"]
    point = get_random_pip(zpp[fcore][pid]["polygon"])
    
    agent["wCore"] = fcore
    agent["wZone"] = pid
    agent["zwID"] = zpp[fcore][pid]["zid"]
    agent["xwork"] = point.x
    agent["ywork"] = point.y
    
    return agent

def workplace_locate(nc, agents, eod_rows, eod_rc, zc, zd, zpp):
    # Assign workplaces
    if __name__ == "heuristics" :
        with Pool(nc) as p:    
            agents = p.starmap(agent_workplace, zip(agents, repeat(eod_rows),
                                                    repeat(eod_rc),
                                                    repeat(zc),
                                                    repeat(zd),
                                                    repeat(zpp)))
        
    return agents

def initDisease(count_s, count_e, count_i, agents, nag):
    count = [count_s, count_e, count_i]
    stage = [0, 1, 2]
    aid = [*range(0, nag)]
    random.shuffle(aid)
    
    # Count of agents
    i = 0;
    while sum(count) > 0:
        
        # Choice random stage
        diseaseStage = random.choices(stage, count, k=1)[0]
        
        # Choice random agent
        agentID = aid[i]
        
        if diseaseStage == 2 and agents[agentID]["age"] < 18:
            continue
        
        # Set a disease stage
        agents[agentID]["diseaseStage"] = diseaseStage
        
        # Update count
        count[diseaseStage] -= 1
        
        # Update agents
        i += 1
        
    return agents

def getRandomAge():
    age_ranges = [[0,9],
                  [10, 19],
                  [20, 29],
                  [30, 39],
                  [40, 49],
                  [50, 59],
                  [60, 69],
                  [70, 79],
                  [80, 121]]

    age_probabilities = [0.1443, 0.1690, 0.1728, 0.1487, 0.1221,
                         0.1104, 0.0728, 0.0393, 0.0206]
    
    n = len(age_probabilities)
    c_ap = np.cumsum(age_probabilities)
    r = np.random.rand()
    
    for i in range(n):   
        if r < c_ap[i]:
            a = age_ranges[i][0]
            b = age_ranges[i][1]
            
            return np.random.randint(a, b)