import copy

# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose:  Converts "custom mapped" .bench to .blif file.
# Note:     Please output redirect to (custom_name).blif for use-case application
# Example:  python3 custom_bench_to_blif.py > (custom_name).blif

library = {"inv1":"1", "inv2":"1", "inv3":"1", "inv4":"1",\
           "nand2":"2", "nand3":"3", "nand4":"4",\
           "nor2":"2", "nor3":"3", "nor4":"4",\
           "and2":"2",  "or2":"2", "xor2a":"2", "xor2b":"2", \
           "xnor2a": "2", "xnor2b":"2", "aoi21":"3", "aoi22":"4", \
           "oai21": "3", "oai22":"4", "BUF1":"1", "DFF":"1", \
           "zero": "0", "one":"0"}


temp_lines = [line.rstrip("\n") for line in open("original.bench")]
lines = []

# get header information
header_list_temp = []
for item in temp_lines:
    if ("#" in item):
        header_list_temp.append(item)
header_list = []
for item in header_list_temp:
    if ("#" in item):
        temp = item.replace("#", "")
    if (" " in item):
        temp = item.replace(" ", "")
    header_list.append(temp)
model = header_list[0]
if ("#" in model):
    model = model.replace("#", "")
num_inputs = header_list[1]
if ("#" in num_inputs):
    num_inputs = num_inputs.replace("#", "")
if ("inputs" in num_inputs):
    num_inputs = num_inputs.replace("inputs", "")
num_outputs = header_list[2]
if ("#" in num_outputs):
    num_outputs = num_outputs.replace("#", "")
if ("outputs" in num_outputs):
    num_outputs = num_outputs.replace("outputs", "")

print(".model ", model)

lines = copy.deepcopy(temp_lines)
lines[:] = (value for value in lines if(value != "" and "#" not in value))

input_list = []
for item in lines:
    if("INPUT(" in item):
        temp = item.replace("INPUT(" , "")
        temp = temp.replace(")", "")
        input_list.append(temp)

output_list = []
for item in lines:
    if("OUTPUT(" in item):
        temp = item.replace("OUTPUT(" , "")
        temp = temp.replace(")", "")
        output_list.append(temp)

# write input line
print(".inputs ",end="")
input_count = 1
max_line_length = 23
for item in input_list:
    if (input_count == len(input_list)):
        print(item)
    elif (input_count % max_line_length != 0):
        print(item, "",end="")
    else:
        print(item,"\\")
        print(" ", end="")
    input_count = input_count + 1

# write output line
print(".outputs ",end="")
output_count = 1
max_line_length = 23
for item in output_list:
    if (output_count == len(output_list)):
        print(item)
    elif (output_count % max_line_length != 0):
        print(item, "",end="")
    else:
        print(item,"\\")
        print(" ", end="")
    output_count = output_count + 1


lines[:] = (value for value in lines if("=" in value))

# separate node features
out_node_list = []
gate_list = []
connection_list = []
temp = ""
input_label = ["a=", "b=", "c=", "d="]
temp_list = []
for item in lines:
    temp_list = item.split(" = ")
    out_node_list.append(temp_list[0])
    temp = temp_list[1]
    temp_list = temp.split("(")
    gate_list.append(temp_list[0])
    temp = temp_list[1]
    if ("," in temp):
        temp = temp.replace(",", "")
    if (")" in temp):
        temp = temp.replace(")", "")
    temp_list = temp.split(" ")
    connection_list.append(temp_list)

max_gate_name_len = 0
for item in gate_list:
    if (len(item) > max_gate_name_len):
        max_gate_name_len = len(item)

gate_count = 0
for gate in gate_list:
    print(".gate ", end = "")
    print(gate, end = "")
    for i in range(0, max_gate_name_len-len(gate)+2):
        print(" ", end="")
    if(gate != "zero" and gate != "one"):
        for i in range(len(connection_list[gate_count])):
            print(input_label[i], connection_list[gate_count][i], " ", end="", sep="")
    print("O=", out_node_list[gate_count], sep="")
    gate_count = gate_count + 1

print(".end")
