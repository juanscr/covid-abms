#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import read as rd
import heuristics as H
from time import time
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

# main path
path = args[5]

# EOD Files
eodw_name = '../../sit-zone-information/eod_2017_walks.csv'
eod_name = '../../sit-zone-information/eod_2017.csv'

# Path to save data
file_geography = path + 'geography/geoData'
rd.check_path(file_geography)
file_bounds = path +  'geography/bounds'
rd.check_path(file_bounds)
file_agentsData = path +'agents/agentsData'
rd.check_path(file_agentsData)

# Agents disease stages
count_i = 10
count_e = 0
count_s = nag - (count_i + count_e)

# Read data of polygons
data, zc, polygons = rd.polygonData(file_polygons)

# Create agents
agents = H.agents_init(nag)

# Create families
agents, uniqueFamilies = H.agents_families(agents, nag)

# Select a SIT Zone for the agents
homes, nHomes = H.agents_sit(uniqueFamilies, zc)

# Locate agents homes
agents, homes = H.agents_home(nc, agents, homes, uniqueFamilies, polygons)

t0 = time()
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

# Export data of geography data
geo_data = rd.exportGeography(nc, borders, zpp, ids, file_geography)

# Export data of boundary
geo_bounds = rd.exportBounds(boundary, file_bounds);

# Export data of agents
agents_data = rd.exportAgents(nc, agents, file_agentsData)
