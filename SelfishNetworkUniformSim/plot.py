from bokeh.plotting import figure, show
import csv 

with open("selfishMiningPlot2.txt", newline='') as f:
    y = []  # profitability
    x_1 = [] # selfish miner data  
    x_2 = [] # honest miner data 
    variables = [y, x_1, x_2] # list of lists 
    reader = csv.DictReader(f)
    columns = len(reader.fieldnames)
    for row in reader:
        for i in range(columns):
            variables[i].append(float(row[reader.fieldnames[i]]))

    p = figure(title="Selfish Mining with Uniformly Distributed Network Delay", x_axis_label='Ratio of Selfish Network Delay to Honest Network Delay', y_axis_label='Selfish Mining Profitability')
    if x_2: # if we are plotting a ratio of selfish to honest data 
        p.circle([i/j for i,j in zip(x_1, x_2)], y, line_width=2)
    else: # if we have only one x value to plot
        p.circle(x_1, y, line_width=2)
        
    show(p)




