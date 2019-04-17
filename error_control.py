# Author:   Mackenzie Peterson, Moises Herrera
# Contact:  mackenzp@usc.edu
# Purpose: This file will act as the control for generating the local and global error for training the DNN

# MH: this file takes the output from node_extract.py

import os
import sys
import random
import copy
from operator import itemgetter

truthTable = {"inv1": "10", "inv2": "10", "inv3": "10", "inv4": "10", \
              "nand2": "1110", "nand3": "11111110", "nand4": "1111111111111110", \
              "nor2": "1000", "nor3": "10000000", "nor4": "1000000000000000", \
              "and2": "0001", "or2": "0111", "xor2a": "0110", "xor2b": "0110", \
              "xnor2a": "1001", "xnor2b": "1001", "aoi21": "10101000", "aoi22": "1110111011100000", \
              "oai21": "11101010", "oai22": "1111100010001000", "BUF1": "01", "DFF": "01", \
              "zero": "0", "one": "1"}

numInputs = {"inv1": "1", "inv2": "1", "inv3": "1", "inv4": "1", \
             "nand2": "2", "nand3": "3", "nand4": "4", \
             "nor2": "2", "nor3": "3", "nor4": "4", \
             "and2": "2", "or2": "2", "xor2a": "2", "xor2b": "2", \
             "xnor2a": "2", "xnor2b": "2", "aoi21": "3", "aoi22": "4", \
             "oai21": "3", "oai22": "4", "BUF1": "1", "DFF": "1", \
             "zero": "0", "one": "0"}



# ----------------------------------------------------------------------------------------
def getIntrinsic(gate1, gate2):
    count = 0
    for i in range(len(truthTable[gate1])):
        if (truthTable[gate1][i] != truthTable[gate2][i]):
            count = count + 1
    error = count / len(truthTable[gate1])
    return error


# ----------------------------------------------------------------------------------------
def allowCompare(gate1, gate2):
    if (numInputs[gate1] == numInputs[gate2]):
        return 1
    return 0


def randReplacementGate(in_gate):
    list_of_possible = []
    if("inv" in in_gate):
        return "BUF1"
    for key in numInputs:
        if(numInputs[in_gate] == numInputs[key] and key != in_gate):
            list_of_possible.append(key)
    return list_of_possible[random.randint(0, len(list_of_possible)-1)]


def initializeIntrinsicError(length):
    temp_list = []
    for i in range(0, length):
        temp_list.append([0.0, " 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0"])
    return temp_list


# ----------------------------------------------------------------------------------------

# used as progress bar

def loadingBar(count, total, size):
    percent = float(count)/float(total)*100
    sys.stdout.write("\r" + "sim: " + str(int(count)).rjust(3,'0')+"/"+str(int(total)).rjust(3,'0') + " " + str(round(count*100/total, 1)) + "%" + ' [' + '='*int(percent/10)*size + ' '*(10-int(percent/10))*size + ']')



# beginning of main -----------------------------------------------------------------

master_list = []
duplicate_input_list = []
duplicate_output_list = []

# get node_types.txt
original_types_temp = [line.rstrip("\n") for line in open("node_types.txt")]

# get train_data feature vectors from train_data.txt
train_data = [line.rstrip("\n") for line in open("train_data.txt")]

# get the total number of nodes (including inputs and outputs)
num_nodes = original_types_temp[0]

# create good list of original_types
original_types = []
for item in original_types_temp:
    original_types.append(item.split("\t"))
original_types.pop(0)


# get the level of each node from train_data (5th item in feature vector)
temp_list = []
level_list = []
max_level = 0
for item in train_data:
    temp_list = item.split(" ")
    level_list.append(temp_list[4])
    if(max_level < int(temp_list[4])):
        max_level = int(temp_list[4])

primary_nodes = []
gate_nodes = []
for node in original_types:
    if (node[0] == "INPUT" or node[0] == "OUTPUT"):
        primary_nodes.append(node)
    else:
        gate_nodes.append(node)

approx_nodes = copy.deepcopy(gate_nodes)

# append on the level to each node. Used for randomization by level
level_count = 0
for node in approx_nodes:
    node.append(level_list[level_count])
    level_count = level_count + 1

# organizes all nodes into a list of its level, then places that list into a network list of all levels
# deep copy of approx_nodes
temp_approx_nodes = copy.deepcopy(approx_nodes)
for item in range(0, len(temp_approx_nodes)):
    temp_approx_nodes[item].append(item)


temp_split_by_level = []
network_by_level = []
# iterate per level
for num in range(1, max_level + 1):
    # iterate through all nodes
    temp_split_by_level = []
    for item in temp_approx_nodes:
        # if current level, place in temp_split_by_level
        if (item[2] == str(num)):
            temp_split_by_level.append(item)
    # append the list containing all nodes in a level to the network_by_level (now index is level)
    network_by_level.append(temp_split_by_level)


gate_error = initializeIntrinsicError(len(gate_nodes))

#print(gate_error[0][0])


# for each level, randomize and propagate
rand_partition = [1, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1]
count_nodes = 0
curr_node = 0
iter = 0

loadingBar(curr_node, len(gate_nodes)*10, 3)

for level in range(0, max_level):
    # generate a random circuit with restriction of only changing prior levels
    # count_nodes = the accumulated number of nodes up to the current level
    count_nodes = count_nodes + len(network_by_level[level])
    approx_network = copy.deepcopy(network_by_level)
    # randomize network prior and including current level // must ignore current node
    for node in approx_network[level]:
        #print(node, "----------------------------------")
        for ratio in rand_partition:
            iter = iter+1
            # get golden network
            approx_network = copy.deepcopy(network_by_level)
            do_rand = []
            # create list of 1 and 0, 1 means to replace the gate
            for i in range(1, count_nodes+1):
                if(i <= (ratio*count_nodes)):
                    do_rand.append(1)
                else:
                    do_rand.append(0)
            # randomize the 1's and 0's and make sure the current node is never switched during intitial network setup
            random.shuffle(do_rand)
            do_rand[curr_node] = 0

            #print(do_rand)
            # for each level, insert the random gate replacement into the approximate network (if 1)
            line_count = 0
            for line in approx_network:
                if (line_count <= level):
                    for gate in line:
                        if(do_rand):
                            if(do_rand[0] == 1):
                                temp = gate[0]
                                gate[0] = randReplacementGate(gate[0])
                                gate_error[gate[3]][0] = getIntrinsic(temp, gate[0])
                            do_rand.pop(0)
                        else:
                            break
                line_count = line_count + 1

            #print(approx_network)
            #for item in gate_error:
                #print(item)
            #print("\n")
            # calculate the initial error ---------------------------
            # write node_types and gate_error
            file = open("gate_error.txt", "w")
            for each in gate_error:
                file.write(str(each[0]))
                file.write(each[1])
                file.write("\n")
            file.close()

            file = open("node_types.txt", "w")
            file.write(num_nodes)
            file.write("\n")
            for item in primary_nodes:
                file.write(item[0] + "\t" + item[1] + "\n")
            # flatten to a single list of nodes
            flatten_nodes = []
            for temp_level in approx_network:
                for temp_gate in temp_level:
                    flatten_nodes.append(temp_gate)
            # sort from level back to index
            flatten_nodes = sorted(flatten_nodes, key=itemgetter(3))
            for item in flatten_nodes:
                file.write(item[0] + "\t" + item[1] + "\n")
            file.close()

            # run .error to get initial error
            command = "./error"
            os.system(command)
            initial_errors_temp = [line.rstrip("\n") for line in open("final_error.txt")]
            initial_errors = initial_errors_temp[0]

            # now approximate the current node M times (where M is all other possible similar input number gates)
            node_index = node[3]
            M_approx_flattened_nodes = copy.deepcopy(flatten_nodes)
            M_node = M_approx_flattened_nodes[node_index]
            temp_node = copy.deepcopy(M_node)


            if(temp_node[0] == "one" or temp_node[0] == "zero" or temp_node[0] == "DFF"):
                continue

            elif("inv" in temp_node[0] or "BUF" in temp_node[0]):
                if("inv" in temp_node[0]):
                    M_node[0] = "BUF1"
                else:
                    M_node[0] = "inv1"

                # aggregate lists and #print to file
                file = open("node_types.txt", "w")
                file.write(num_nodes)
                file.write("\n")
                for item in primary_nodes:
                    file.write(item[0] + "\t" + item[1] + "\n")
                for item in M_approx_flattened_nodes:
                    file.write(item[0] + "\t" + item[1] + "\n")
                file.close()
                # node_types.txt is changed

                # write gate error file
                M_error = getIntrinsic(temp_node[0], M_node[0])
                gate_error[node_index][0] = M_error
                file = open("gate_error.txt", "w")
                for each in gate_error:
                    file.write(str(each[0]))
                    file.write(each[1])
                    file.write("\n")
                file.close()

                # run error
                command = "./error"
                os.system(command)

                final_errors_temp = [line.rstrip("\n") for line in open("final_error.txt")]
                final_errors = final_errors_temp[0]

                vector_string = train_data[curr_node] + " " + str(M_error) + " " + initial_errors + " " + final_errors

                # remove double space if generated (easier to remove here before appending)
                if ("  " in vector_string):
                    vector_string = vector_string.replace("  ", " ")

                #print("ORIGINAL: ", temp_node)
                #print("APPROX: ", M_node)
                #for item in flatten_nodes:
                #   print(item)
                #print("\n")
                #for item in M_approx_flattened_nodes:
                #   print(item)
                #for item in gate_error:
                #   print(item)
                #print("\n")
                #print("\n\n\n")

                # append error
                #print(temp_node[0], M_node[0], str(node_index), str(M_error), initial_errors, final_errors)
                master_list.append(vector_string)

                # reset node to original
                M_node[0] = temp_node[0]


            else:
                for key in numInputs:
                    if ((numInputs[temp_node[0]] == numInputs[key]) and (key != temp_node[0])):
                        if("xnor2" in key and "xnor2" in temp_node[0]):
                            continue
                        elif("xor2" in key and "xor2" in temp_node[0]):
                            continue
                        else:
                            # swap with allowed approximate gate
                            M_node[0] = key

                            # aggregate lists and #print to file
                            file = open("node_types.txt", "w")
                            file.write(num_nodes)
                            file.write("\n")
                            for item in primary_nodes:
                                file.write(item[0] + "\t" + item[1] + "\n")
                            for item in M_approx_flattened_nodes:
                                file.write(item[0] + "\t" + item[1] + "\n")
                            file.close()
                            # node_types.txt is changed

                            # write gate error file
                            M_error = getIntrinsic(temp_node[0], M_node[0])
                            gate_error[node_index][0] = M_error
                            file = open("gate_error.txt", "w")
                            for each in gate_error:
                                file.write(str(each[0]))
                                file.write(each[1])
                                file.write("\n")
                            file.close()

                            # run error
                            command = "./error"
                            os.system(command)

                            final_errors_temp = [line.rstrip("\n") for line in open("final_error.txt")]
                            final_errors = final_errors_temp[0]

                            #file_test = open("error_test.txt", "a+")
                            #file_test.write(final_errors)
                            #file_test.write("\n")
                            #file_test.close()

                            vector_string = train_data[curr_node] + " " + str(M_error) + " " + initial_errors + " " + final_errors

                            #duplicate_check_input = train_data[curr_node] + str(M_error) + " " + initial_errors
                            #duplicate_check_output = final_errors

                            # remove double space if generated (easier to remove here before appending)
                            if ("  " in vector_string):
                                vector_string = vector_string.replace("  ", " ")

                            temp_error_list = final_errors.split(" ")


                            if (float(temp_error_list[0]) > 1.0 or float(temp_error_list[1]) > 1.0):
                                print("ERROR: There has been a problem in the error propagation network (error.cpp)")
                                print("       An error is out of range")
                                exit(1)

                            #print("ORIGINAL: ", temp_node)
                            #print("APPROX: ", M_node)
                            #for item in flatten_nodes:
                            #   print(item)
                            #print("\n")
                            #for item in M_approx_flattened_nodes:
                            #   print(item)
                            #for item in gate_error:
                            #   print(item)
                            #print("\n")
                            #print("\n\n\n")

                            # append error
                            #print(temp_node[0], M_node[0], str(node_index), str(M_error), initial_errors, final_errors)
                            master_list.append(vector_string)

                            #duplicate_input_list.append(duplicate_check_input)
                            #duplicate_output_list.append(duplicate_check_output)

                            # reset node to original
                            M_node[0] = temp_node[0]

            # sets gate_error back to all 0.0 for all lines
            gate_error = initializeIntrinsicError(len(gate_nodes))


            loadingBar(iter, len(flatten_nodes)*10, 3)
        
        curr_node = curr_node + 1

# reset node_types.txt and gate_error.txt files to their original
file = open("node_types.txt", "w")
file.write(num_nodes)
file.write("\n")
for item in primary_nodes:
    file.write(item[0] + "\t" + item[1] + "\n")
for item in flatten_nodes:
    file.write(item[0] + "\t" + item[1] + "\n")
file.close()
# write gate error file
M_error = getIntrinsic(temp_node[0], M_node[0])
gate_error[node_index][0] = M_error
file = open("gate_error.txt", "w")
for each in gate_error:
    file.write(str(each[0]))
    file.write(each[1])
    file.write("\n")
file.close()

# remove any duplicates feature vectors in the training data
master_set = set(master_list)
master_list = list(master_set)


# append to train_dnn.txt file
file = open("train_dnn.txt", "a+")
for item in master_list:
    file.write(item)
    file.write("\n")
file.close()


