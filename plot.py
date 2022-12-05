from bokeh.plotting import figure, show
from bokeh.io import export_png
import csv 
import sys

colours= {"12": "red", "25" : "brown", "38" : "green" , "40" : "blue", "50" : "purple" , "baseline": "black"}
colours_2 = {"127551": "red", "6377550": "green", "25000000" : "purple"}

def file_read(file1):
    with open(file1, newline='') as f1:
        y = []  # profitability
        x_1 = [] # selfish miner data  
        x_2 = [] # honest miner data 
        x_3 = [] 
        x_4 = []
        variables = [y, x_1, x_2, x_3, x_4] # list of lists 
        reader = csv.DictReader(f1)
        columns = len(reader.fieldnames)
        for row in reader:
            for i in range(columns):
                variables[i].append(float(row[reader.fieldnames[i]]))
    
    if not x_2:
        return file1, [y, x_1] # we obtained just x_1 values 
    elif not x_3: 
        return file1, [y, x_1, x_2] # we obtained both x_1 and x_2 values
    else:
        return file1, [y, x_1, x_2, x_3, x_4]

# Uniform and Poisson Network Delay Data --> plot ratio of selfish to honest network delay
def plot_uni_poiss_network_data(graph_title, x_label, y_label, data_dict): 
    p = figure(title=graph_title, x_axis_label=x_label, y_axis_label=y_label)
    for filename, value in data_dict.items(): # plot a line for each file in the data_dict. we may want to plot multiple lines ...
        components = filename.split("_")
        if len(value) == 3: # if we are plotting a ratio of selfish to honest data aka we have x_1 AND x_2 values
            plot = p.circle if components[3] == "0.txt" else p.cross # visual distinction between baseline network delay and modified network delay. 1 == baseline aka zero network delay
            print(components)
            # plot([i/j if i and j else 1 for i,j in zip(value[1], value[2])], value[0], line_width= 1, legend_label=f"α={components[2]}% {'(baseline)' if components[3] == '1.txt' else '' }", color=colours[components[2]] ) # get ratio of x_1 to x_2 values
            # baseline ratio is always 1 ^

            plot([i-j for i,j in zip(value[1], value[2])], [x*100 for x in value[0]], line_width= 1, legend_label=f"α={components[2]}% {'(baseline)' if components[3] == '1.txt' else '' }", color=colours[components[2] if components[3] == "0.txt" else "baseline"] ) # get ratio of x_1 to x_2 values
            # plot the difference (for both baseline and modified data!): selfish delay - honest delay 
    show(p)
    export_png(p, filename=f"graphs/{graph_title}.png")
    
# Linear and Exponential Network Delay Data 
def plot_lin_exp_network_data(graph_title, x_label, y_label, data_dict): 
    plot = None
    if "Exponential" in graph_title:
        p = figure(title=graph_title, x_axis_label=x_label, y_axis_label=y_label, x_axis_type="log")
        plot = p.line
    else:
        p = figure(title=graph_title, x_axis_label=x_label, y_axis_label=y_label)
        plot = p.circle

    for filename, value in data_dict.items(): # plot a line for each file in the data_dict. we may want to plot multiple lines ...
        components = filename.split("_")
        if len(value) == 3: # we have x_1 and x_2, but note that x_2 = 0 since we fix honest delay to be 0
            plot(value[1], [x*100 for x in value[0]], line_width=1, legend_label=f"α={components[2].replace('.txt', '')}%", color=colours[components[2].replace(".txt", "")]) # just plot selfish delay

    show(p)
    export_png(p, filename=f"graphs/{graph_title}.png")

# Block Value Data 
def plot_block_value_data(graph_title, x_label, y_label, data_dict): # we have only one x value to plot
    p = figure(title=graph_title, x_axis_label=x_label, y_axis_label=y_label)
    for filename, value in data_dict.items(): # plot a line for each file in the data_dict. we may want to plot multiple lines ...
        components = filename.split("_")
        p.circle([x/100000000 for x in value[1]], [x*100 for x in value[0]], line_width=2, legend_label=f"α={components[2]}, λ={int(components[3].replace('.txt', ''))/1000000}" , color=colours[components[2]]) # note that value[0] = y values and value[1] = x values 
        # recall that lambda here = mean of poisson distribution for block value (lambda could be 0.127551, 6.37755, 25)
    show(p)
    export_png(p, filename=f"graphs/{graph_title}.png")

# Cost of Mining Data 
def plot_cost_data(graph_title, x_label, y_label, data_dict): 
    p = figure(title=graph_title, x_axis_label=x_label, y_axis_label=y_label)   
    for filename, value in data_dict.items(): # plot a line for each file in the data_dict. we may want to plot multiple lines ...
        components = filename.split("_")
        p.line([x/100000000 for x in value[1]], [x*100 for x in value[0]], line_width=2, legend_label=f"α={components[2].replace('.txt', '')}%", color=colours[components[2].replace(".txt", "")]) # we are plotting cost/sec agaisnt profitability. (we are ignoring the x_2 values!)

    show(p)
    export_png(p, filename=f"graphs/{graph_title}.png")

def plot_classic_vs_clever(graph_title, x_label, y_label, data_dict):    
    p = figure(title=graph_title, x_axis_label=x_label, y_axis_label=y_label)   
    values = list(data_dict.values())[0] # extract [Classic Selfish Miner Profit, Classic Selfish Miner Alpha, Clever Selfish Miner Profit, Clever Selfish Miner Alpha] and put it into list 
    classic_ys, classic_xs, clever_ys, clever_xs, honest_ys = tuple(values) # convert that list into a tuple and unpack the tuple 
    p.line(list(map (lambda x: 100*x,classic_xs)), list(map (lambda x: 100*x,classic_ys)), legend_label="classic selfish miner" , color="red") # plot a line for classic selfish miner: CLASSIC alpha vs CLASSIC profit
    p.line(list(map (lambda x: 50-100*x,clever_xs)), list(map (lambda x: 100*x,clever_ys)), legend_label="clever selfish miner" , color="blue") # plot a line for clever selfish miner: CLEVER alpha vs CLEVER profit
    p.line(list(map (lambda x: 100*x,classic_xs)), list(map (lambda x: 100*x,honest_ys)), legend_label="honest miner" , color="green") # plot a line for honest miner: CLASSIC alpha vs HONEST profit
    show(p)
    export_png(p, filename=f"graphs/{graph_title}.png")

if __name__ == "__main__":
    data_dict = {}     # create dictionary of filename:datalist pairs 
    for arg in sys.argv[3:]:
        key, value = file_read(arg) # parse the data. file_read returns the filename, list of lists containing actual data values
        data_dict[key] = value   # add the parsed data to the data dictionary 
    
    
    graph_title = sys.argv[1]
    x_label = sys.argv[2]
    y_label = "% Selfish Miner Profitability"

    # parse the file name and determine which plotting function to call 
    file_name = sys.argv[3].split("_")[0] # grab the first component of the file name
    plot_data = None
    if file_name in ["selfish-nuni", "selfish-npoiss"]:
        plot_data = plot_uni_poiss_network_data
    elif file_name in ["selfish-nlin", "selfish-nexp" ]:
        plot_data = plot_lin_exp_network_data
    elif file_name in ["selfish-cm"]:
        plot_data = plot_cost_data
    elif file_name in ["selfish-bv"]:
        plot_data = plot_block_value_data
    elif file_name in ["selfish-comp"]:
        plot_data = plot_classic_vs_clever
    else:
        print("Invalid file name")
    plot_data(graph_title, x_label, y_label, data_dict) 









