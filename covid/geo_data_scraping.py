import geopandas as gpd
import matplotlib.pyplot as plt
import numpy as np

lsoas = gpd.read_file('maps/EOD.shp')
excluded = []

with open('excluded.txt') as f:
    excluded = list(map(lambda x : int(x), f.readline().replace(' ','').split(',')))

lsoas.drop(excluded, axis=0, inplace=True)
lsoas.index = range(lsoas.shape[0])
#lsoas.to_file('maps/EOD_revised.shp', driver='ESRI Shapefile')

# Excluir
for i in list(lsoas.index):
    lsoas.plot(figsize=(10, 10))
    #print(lsoas.iloc[i])
    #lsoas.iloc[i].plot(figsize=(10, 10),color='red')
    #A = lsoas.drop(i, axis=0)
    #A.plot(figsize=(10, 10))
    plt.show(block=False)
    if input('¿Excluir?') == 'y':
        excluded.append(i)
    plt.close()
    print('Index {}'.format(i))

print(excluded)