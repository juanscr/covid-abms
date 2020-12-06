#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import read as rd
import heuristics as H
import sys
import geography

args = sys.argv

# Number of processors
cpx = int(args[1]) # x
cpy = int(args[2]) # y
nc = cpx * cpy

# Number of agents
nag = int(args[3])

# Read data of geography
file_polygons = args[4]

# Read data of municipalities
file_mun = args[5]

# main path
path = args[6]

# EOD Files
eodw_name = 'eod_2017_walks.csv'
eod_name = 'eod_2017.csv'
file_geography = path + 'geography/geoData'
rd.check_path(file_geography)
file_bounds = path +  'geography/bounds'
rd.check_path(file_bounds)
file_agentsData = path +'agents/agentsData'
rd.check_path(file_agentsData)

# Agents disease stages
count_i = 1
count_e = 0
count_s = nag - (count_i + count_e)

# Read data of polygons
mun, data, zc, polygons = rd.polygonData(file_polygons, file_mun)

# Create agents
agents = H.agents_init(nag)

# Create families
agents, uniqueFamilies, families = H.agents_families(agents, nag)

# Get data from municipalities
mun, mid, zpm, zpa = geography.update_mn(data, mun, zc, polygons)

# Get stratums
nHomes, families, agents = H.get_stratum(families, agents, uniqueFamilies, mun, mid)

# Select a sit zone for agents
homes = H.select_sit(families, mid, zpa, zpm)
    
# Locate agents homes
agents, homes = H.agents_home(nc, agents, homes, uniqueFamilies, polygons)

# Create homes and split in cores
xy, agents = geography.split_border(nc, cpy, agents, uniqueFamilies, nHomes, polygons['border'])

# Update polygons
bounds, borders, boundary, polygons = geography. find_borders(xy, polygons)

# Assign polygons to cores
zpp, ids, zd = geography.assign_zones(nc, data, zc, polygons, bounds)

# Read eod
xmin, ymin = boundary[0:2]
eodw, eodw_rows, eodw_cols, walks, averageWalk = rd.eodWalks(eodw_name, xmin, ymin)

# Get walks in SIT zones
zpp, agents = H.get_walks(nc, zpp, eodw_cols, walks, averageWalk, agents)

# Workplaces for agents
eod_rows, eod_rc = rd.eod(eod_name)
agents =  H.workplace_locate(nc, agents, eod_rows, eod_rc, zc, zd, zpp)

# Set disease stages
agents = H.initDisease(count_s, count_e, count_i, agents, nag)

# Plot location
# import matplotlib.pyplot as plt
# x = [agent["hPoint"].x for agent in agents]
# y = [agent["hPoint"].y for agent in agents]
# plt.scatter(x, y, s=1)
# plt.plot(*polygons["border"][0].exterior.coords.xy, color='black')
# plt.show()

# Export data of geography data
geo_data = rd.exportGeography(nc, borders, zpp, ids, file_geography)

# Export data of boundary
geo_bounds = rd.exportBounds(boundary, file_bounds);

# Export data of agents
agents_data = rd.exportAgents(nc, agents, file_agentsData)