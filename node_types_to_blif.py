# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose: This file acts as a translator for converting the approximated network to a blif file
# Note: This is called by als.py and output redirected to original.bench


temp_orig_bench = [line.rstrip("\n") for line in open("original.bench")]
temp_node_types = [line.rstrip("\n") for line in open("node_types.txt")]

temp_node_types.pop(0)
node_types = []
node_dict = {}
for item in temp_node_types:
    if("INPUT" not in item and "OUTPUT" not in item):
        item = item.split("\t")
        item.reverse()
        node_types.append(item)
        node_dict[item[0]] = item[1]


orig_bench_prim = []
orig_bench_gates = []
for item in temp_orig_bench:
    if ("=" in item):
        orig_bench_gates.append(item)
    else:
        orig_bench_prim.append(item)

split_orig_bench_gates = []
for item in orig_bench_gates:
    temp_list = item.split(" = ")
    temp_list2 = temp_list[1].split("(")
    temp_list.pop(-1)
    for sub_item in temp_list2:
        temp_list.append(sub_item)
    split_orig_bench_gates.append(temp_list)

combined_orig_bench_gates = []
for item in split_orig_bench_gates:
    name = item[0]
    item[1] = node_dict[item[0]]
    temp = item[0] + " = " + item[1] + "("
    for i in range(2,len(item)):
        temp = temp + item[i]
    combined_orig_bench_gates.append(temp)

file = open("original.bench", "w")

for item in orig_bench_prim:
    file.write(item)
    file.write("\n")

for item in combined_orig_bench_gates:
    file.write(item)
    file.write("\n")

file.close()

