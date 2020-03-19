import pandas
import os
import matplotlib.pyplot as plt
from read_data import DataAnalysis


class Data:
    def __init__(self, line):
        self.channel = line[2]
        self.order_type = ""
        self.obtain_time(line[3])

        self.span = 0
        self.calculate_span(line[4], line[5])

        self.item_type = line[6]
        self.sku = line[7]
        self.quantity = line[8]

    def obtain_time(self, order_type):
        if "Future" in order_type:
            self.order_type = "Future"
        else:
            self.order_type = "At Once"

    def calculate_span(self, start, finish):
        if self.order_type == "Future":
            finish_dt = pandas.to_datetime(finish)
            start_dt = pandas.to_datetime(start)
            self.span = (finish_dt - start_dt).days

    def __str__(self):
        string = "From channel: " + str(self.channel)
        string += "\nOrder type: " + str(self.order_type)
        string += "\nTime to deliver: " + str(self.span)
        string += "\nItem type: " + str(self.item_type)
        string += "\nQuantity: " + str(self.quantity)
        return string


class DataSet:
    def __init__(self):
        self.set = []
        self.process_file()

        cwd = os.getcwd()
        self.dist_dir = os.path.join(cwd, "distributions")

    def process_file(self):
        cwd = os.getcwd()
        os.chdir("..")
        path = os.path.join(os.getcwd(), "problem/data-set.xlsx")
        file = pandas.read_excel(path).to_numpy()
        os.chdir(cwd)
        for i in range(file.shape[0]):
            self.set.append(Data(file[i, :]))

    def channel_probability(self):
        # Probability calculation
        channels = {}
        total = 0
        for data in self.set:
            if data.channel in channels:
                channels[data.channel] += 1
            else:
                channels[data.channel] = 1
            total += 1

        for key in channels:
            channels[key] /= total

        # File creation
        file = open(os.path.join(self.dist_dir, "channels.prob"), "w+")
        for key in channels:
            file.write(str(key) + ": " + str(channels[key]) + "\n")
        file.close()

    def order_type_probability(self):
        # Probability calculation
        order_type = {}
        total_channels = {}
        for data in self.set:
            if data.channel in order_type:
                if data.order_type in order_type[data.channel]:
                    order_type[data.channel][data.order_type] += 1
                else:
                    order_type[data.channel][data.order_type] = 1
            else:
                order_type[data.channel] = {}
                order_type[data.channel][data.order_type] = 1

            if data.channel in total_channels:
                total_channels[data.channel] += 1
            else:
                total_channels[data.channel] = 1

        for key in order_type:
            for key0 in order_type[key]:
                order_type[key][key0] /= total_channels[key]

        # File creation
        file = open(os.path.join(self.dist_dir, "order_type.prob"), "w+")
        for key in order_type:
            file.write(str(key) + "=\n")
            for key0 in order_type[key]:
                file.write("\t" + str(key0) + ": " + str(order_type[key][key0]) + "\n")
        file.close()

    def span_probability(self):
        # Probability
        span = list(filter(lambda x: x.span > 0, self.set))
        spans_count = {}
        total_channels = {}
        for data in span:
            if data.channel in spans_count:
                if data.span in spans_count[data.channel]:
                    spans_count[data.channel][data.span] += 1
                else:
                    spans_count[data.channel][data.span] = 1
            else:
                spans_count[data.channel] = {}
                spans_count[data.channel][data.span] = 1

            if data.channel in total_channels:
                total_channels[data.channel] += 1
            else:
                total_channels[data.channel] = 1

        for key in spans_count:
            for key0 in spans_count[key]:
                spans_count[key][key0] /= total_channels[key]

        # File creation
        file = open(os.path.join(self.dist_dir, "span.prob"), "w+")
        for key in spans_count:
            file.write(key + "=\n")
            for key0 in spans_count[key]:
                file.write("\t" + str(key0) + ": " + str(spans_count[key][key0]) + "\n")
        file.close()


DataSet().span_probability()
