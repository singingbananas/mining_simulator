import csv 
import sys
import numpy as np


if len(sys.argv) > 2:
    name1 = sys.argv[1]    # first file
    name2 = sys.argv[2]    # second file
else:
    print("Must specify two file names to parse")

with open(name1, newline='') as f1, open(name2, newline='') as f2:
    f1_selfish_profit = []  # profitability from file 1
    f2_selfish_profit = []  # profitability from file 2

    reader1 = csv.reader(f1)
    reader2 = csv.reader(f2)

    for i,j in zip(reader1, reader2):
        f1_selfish_profit.append(float(i[0]))
        f2_selfish_profit.append(float(j[0]))

    # print(f1_selfish_profit)
    
    mean1 = np.mean(f1_selfish_profit)
    mean2 = np.mean(f2_selfish_profit)
    print("Mean of selfish profitability in", name1, ":", mean1)
    print("Mean of selfish profitability in", name2, ":", mean2)
    print("Difference of means:", np.abs(mean1-mean2))

