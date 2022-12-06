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

    for count, (i,j) in enumerate(zip(reader1, reader2)):
        if count ==0:
            continue
        f1_selfish_profit.append(float(i[0]))
        f2_selfish_profit.append(float(j[0]))

    # print(f1_selfish_profit)
    
    mean_1 = np.mean(f1_selfish_profit)
    mean_2 = np.mean(f2_selfish_profit)
    median_1 = np.median(f1_selfish_profit)
    median_2 = np.median(f2_selfish_profit)
    stddev_1 = np.std(f1_selfish_profit)
    stddev_2 = np.std(f2_selfish_profit)

    components = name1.split('_')
    components[3] = components[3].replace(".txt", "")
    print (f"for alpha = {components[2]}")
    print (f"{('baseline') if components[3] == 1 else ('with delay')} mean = {mean_1 * 100}, median = {median_1 * 100},  stddev={stddev_1 * 100}")
    print (f"{('with delay') if components[3] == 1 else ('baseline')} mean = {mean_2 * 100}, median = {median_2 * 100},  stddev={stddev_2 * 100}")
    print("Difference of means:", np.abs(mean_1-mean_2)*100)

