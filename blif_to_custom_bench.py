# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose:  Converts .blif to "custom mapped" .bench file. Used by node_extract.py for node/edge structure
# Note:     Please output redirect to original.bench for use-case application
# Example:  python3 blif_to_custom_bench.py > original.bench

library = {"inv1":"1", "inv2":"1", "inv3":"1", "inv4":"1",\
           "nand2":"2", "nand3":"3", "nand4":"4",\
           "nor2":"2", "nor3":"3", "nor4":"4",\
           "and2":"2",  "or2":"2", "xor2a":"2", "xor2b":"2", \
           "xnor2a": "2", "xnor2b":"2", "aoi21":"3", "aoi22":"4", \
           "oai21": "3", "oai22":"4", "BUF1":"1", "DFF":"1", \
           "zero": "0", "one":"0"}


temp_lines = [line.rstrip("\n") for line in open("original.blif")]
lines = []


#(begin - multi-line parse)-------------------------------------------------------------------
# This parses through the code and handles multi-line inputs and outputs
# It prepares the lines for the rest of the code to easily read in the inputs and outputs
all_piece = []
for line in temp_lines:
    piece = line.split(" ")
    while ("" in piece):
        piece.remove("")
    all_piece.append(piece)

i = 0
while i < len(all_piece):
    if (all_piece[i][0] == ".inputs" and all_piece[i][-1] == "\\"):
        while(all_piece[i+1][-1] == "\\"):
            temp = all_piece.pop(i+1)
            for val in temp:
                all_piece[i].append(val)
        temp = all_piece.pop(i + 1)
        for val in temp:
            all_piece[i].append(val)
    i = i + 1

i = 0
while i < len(all_piece):
    if (all_piece[i][0] == ".outputs" and all_piece[i][-1] == "\\"):
        while (all_piece[i + 1][-1] == "\\"):
            temp = all_piece.pop(i + 1)
            for val in temp:
                all_piece[i].append(val)
        temp = all_piece.pop(i + 1)
        for val in temp:
            all_piece[i].append(val)
    i = i + 1


for i in range(len(all_piece)):
    while ("\\" in all_piece[i]):
        all_piece[i].remove("\\")
    while ("" in all_piece[i]):
        all_piece[i].remove("")

temp_lines = []
for i in range(len(all_piece)):
    temp = ""
    for k in range(len(all_piece[i])):
        temp = temp + all_piece[i][k] + " "
    temp_lines.append(temp)

#(end - multi-line parse)---------------------------------------------------------------------

lines = temp_lines
initial = lines

# initial pass through file for header information
num_inverter = 0
num_gates = 0

for item in initial:
    if(".model" in item):
        print("# ", end="")
        temp_model = item.split("/")
        model = temp_model[-1]
        print (model)

    elif (".inputs" in item):
        init_inputs = item.split(" ")
        while("" in init_inputs):
            init_inputs.remove("")
        init_inputs.pop(0)
        num_init_inputs = len(init_inputs)
        print ("# " + str(num_init_inputs) + " inputs")

    elif (".outputs" in item):
        init_outputs = item.split(" ")
        while("" in init_outputs):
            init_outputs.remove("")
        init_outputs.pop(0)
        num_init_outputs = len(init_outputs)
        print("# " + str(num_init_outputs) + " outputs")

    elif (".gate" in item):
        gate = item.split(" ")
        gate.pop(0)
        gate.remove("")
        name = gate.pop(0)
        if ("inv" in name):
            num_inverter = num_inverter + 1
        num_gates = num_gates + 1
print("# " + str(num_inverter) + " inverter")
print("# " + str(num_gates) + " gates")
print("\n")

# convert the mean of the blif file to a custom bench file
for item in lines:
    if(".model" in item):
        continue

    # input parsing
    elif (".inputs" in item):
        inputs = item.split(" ")
        if (".inputs" in inputs):
            inputs.remove(".inputs")
        while ("" in inputs):
            inputs.remove("")
        while ("\\" in inputs):
            inputs.remove("\\")
        for val in inputs:
            print("INPUT(" + val + ")")
        print("\n")

    # output parsing
    elif (".outputs" in item):
        outputs = item.split(" ")
        if (".outputs" in outputs):
            outputs.remove(".outputs")
        while ("" in outputs):
            outputs.remove("")
        while ("\\" in outputs):
            outputs.remove("\\")
        for val in outputs:
            print("OUTPUT(" + val + ")")
        print("\n")

    # gate parsing
    elif (".gate" in item):
        gate = item.split(" ")
        gate.pop(0)

        # remove all "" that ABC adds to line
        while("" in gate):
            gate.remove("")


        # print output node of gate
        temp_item = item.split(" ")
        while ("" in temp_item):
            temp_item.remove("")

        last = temp_item[-1]
        outnode_temp = last.split("O=")
        outnode = outnode_temp[-1]
        print (outnode + " = ", end = "")
        del gate[-1]

        # print the gate type and parenthesis
        gate_type = gate.pop(0)

        print (gate_type + "(", end = "")
        num_inputs = library[gate_type]
        for i in range(int(num_inputs)-1):
            temp_in = gate.pop(0)
            temp_in_list = temp_in.split("=")
            gate_input = temp_in_list[-1]
            print(gate_input + ", ", end="")

        if (gate):
            temp_in = gate.pop(0)
            temp_in_list = temp_in.split("=")
            gate_input = temp_in_list[-1]
            print(gate_input + ")")
        else:
            print("-1)")


    elif (".end" in item):
        break

