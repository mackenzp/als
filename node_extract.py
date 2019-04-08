# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose:  Takes a mapped "custom" .bench file and extacts the nodes and edges for error.cpp
# Note:     
# Example:  

import sys
filename = sys.argv[1]

temp_lines = [line.rstrip("\n") for line in open(filename)]

filtered_lines = []
filter_by = ["INPUT", "OUTPUT", "="]
for line in temp_lines:
    if (filter_by[0] in line or filter_by[1] in line or filter_by[2] in line):
        filtered_lines.append(line)

input_nodes = []
output_nodes = []
gate_nodes = []

for line in filtered_lines:
    if ("INPUT" in line):
        temp = line.split("(")
        temp.pop(0)
        temp = temp[0]
        temp = temp.split(")")
        temp = temp[0]
        input_nodes.append(temp)

    elif ("OUTPUT" in line):
        temp = line.split("(")
        temp.pop(0)
        temp = temp[0]
        temp = temp.split(")")
        temp = temp[0]
        output_nodes.append(temp)
    elif ("=" in line):
        gate_nodes.append(line)


# create node associations for the type_nodes.txt file ------------------------------------
all_node_associations = []
for item in input_nodes:
    node_association = []
    node_association.append("INPUT")
    node_association.append(item)
    all_node_associations.append(node_association)

for item in output_nodes:
    node_association = []
    node_association.append("OUTPUT")
    node_association.append(item)
    all_node_associations.append(node_association)

for item in gate_nodes:
    #print (item)
    temp = item.split(" = ")
    node_association = []
    node_association.append(temp[0])
    temp = temp[1]
    temp = temp.split("(")
    temp = temp[0]
    node_association.append(temp)
    node_association.reverse()
    all_node_associations.append(node_association)

# print to node_type.txt
node_type = open("node_types.txt", "w")
node_type.write(str(len(all_node_associations)) + "\n")
for item in all_node_associations:
    node_type.write(item[0] + "\t" + item[1] + "\n")
node_type.close()


# create node vertices for the type.txt file ---------------------------------------------
all_node_edges = []
for item in gate_nodes:
    node_edges = []
    temp = item.split(" = ")
    node_edges.append(temp[0])

    temp = temp[1]
    temp = temp.split("(")
    temp.pop(0)
    temp = temp[0]
    temp = temp[:-1]
    temp = temp.split(", ")
    node_edges.append(temp)
    node_edges.reverse()
    all_node_edges.append(node_edges)

node_edges = open("node_edges.txt", "w")
for item in all_node_edges:
    for i in range(len(item[0])):
        #print(item[0][i] + "\t\t" + item[1])
        node_edges.write(item[0][i] + "\t\t" + item[1] + "\n")
node_edges.close()
