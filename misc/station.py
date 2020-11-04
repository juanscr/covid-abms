#!/usr/bin/env python3

import holidays_co
import matplotlib.pyplot as plt
import numpy as np
import warnings
warnings.filterwarnings("ignore")

from datetime import datetime
from read_data import DataAnalysis

class Station:
    class DataDay:
        def __init__(self, line, date):
            self.date = date
            self.traffic = line[4:]

    def __init__(self, name, dataset, fixed_year=None):
        self.name = name
        data = []
        for i in range(dataset.shape[0]):
            if dataset[i, 2] == name:
                date = datetime.strptime(dataset[i, 1], "%d.%m.%Y")
                if fixed_year is not None and date.year == fixed_year:
                    data.append(Station.DataDay(dataset[i, :], date))
        self.data = data
        self.traffics = np.array(list(map(lambda x: x.traffic, data)))

    @staticmethod
    def boxplot_dic(data_dic, prefix, save=False, name="test.pdf"):
        xticks = []
        data_list = []
        for key in sorted(data_dic):
            data_list.append(data_dic[key])
            xticks.append(prefix + str(key))

        fig = plt.figure(1, figsize=(10, 8))
        ax = fig.add_subplot(111)
        ax.boxplot(data_list)
        ax.set_xticklabels(xticks)
        if save:
            plt.savefig(name, bbox_inches='tight')
        plt.show()

    def get_data_time(self, get_time):
        data_per_time = {}
        for data in self.data:
            if get_time(data.date) in data_per_time:
                data_per_time[get_time(data.date)].append(data.traffic.mean())
            else:
                data_per_time[get_time(data.date)] = [data.traffic.mean()]
        return data_per_time

    def separate_data(self):
        data_nh = []
        data_sat = []
        data_sun = []
        for data in self.data:
            if data.date.weekday() == 5:
                data_sat.append(data.traffic[:-2])
            elif data.date.weekday() == 6:
                data_sun.append(data.traffic[:-2])
            elif not holidays_co.is_holiday_date(data.date):
                data_nh.append(data.traffic[:-2])

        data_nh = np.array(data_nh, dtype=float)
        data_sat = np.array(data_sat, dtype=float)
        data_sun = np.array(data_sun, dtype=float)
        return data_nh, data_sat, data_sun

    def create_fitting(self):
        da = DataAnalysis()
        def fit_data(data_aux, name_file):
            for i in range(data_aux.shape[1]):
                da.set_data(data_aux[:, i])
                da.fitting_test(write=True, title="Hour {}".format(i + 4),
                                name_file=name_file, rank=5)
        data_nh, data_sat, data_sun = self.separate_data()
        name_gen = lambda x: "fittings/" + self.name + "-fitting" + x + ".txt"

        fit_data(data_nh, name_gen(""))
        fit_data(data_sat, name_gen("sat"))
        fit_data(data_sun, name_gen("sun"))

    def __str__(self):
        return self.name
