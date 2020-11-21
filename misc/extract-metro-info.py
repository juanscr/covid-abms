#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import statsmodels.api as sm

from station import Station

# Global functions
save_fig = lambda x: plt.savefig("figs/" + x, bbox_inches='tight')

# Data creation
data = pd.read_excel("metro-data.xlsx")
dataset = data.to_numpy()
dataset = dataset[1:, :]
for i in range(dataset.shape[0]):
    for j in range(dataset.shape[1]):
        if type(dataset[i, j]) != str and np.isnan(dataset[i, j]):
            dataset[i, j] = 0

# Station processing
all_stations = ["ACE", "ALP", "AYU", "BAN", "BEO", "BER", "CAR", "CAT", "CIS",
                "ENV", "EST", "FLO", "HOS", "IND", "ITA", "JAV", "LUC", "MAD",
                "NIQ", "POB", "PRA", "SAA", "SAM", "STA", "TRI", "UNI", "XPO"]
directory = lambda x: "figs/" + x
lambdas_file = "lambda.csv"

for station_name in all_stations:
    station = Station(station_name, dataset, fixed_year=2019)
    # data_dic = station.get_data_time(lambda x: x.month)
    # data_dic1 = station.get_data_time(lambda x: x.weekday())
    station.calculate_lambda(lambdas_file)
    # Station.boxplot_dic(data_dic, "m = ", save=True, name=directory(station_name + "-month-boxplot.pdf"))
    # Station.boxplot_dic(data_dic1, "d = ", save=True, name=directory(station_name + "-day-boxplot.pdf"))
    # station.create_fitting()
