
# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose:  Acts as a class container for approximate logic synthesis
# Note:     Utilizes DNN as a fast verification method to communicate to the synthesis
#           algorithm that the proposed change is within the allowed error threshold.
#           Goal is to design the DNN to be able to be utilized with many
#           different synthesis strategies or techniques.
# Example:

import os
import copy
import sys
import random
from operator import itemgetter

# for the dnn
import numpy as np
import logging
logging.getLogger('tensorflow').setLevel(logging.ERROR)
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'  # or any {'0', '1', '2'}
from keras.models import Sequential
from keras.models import load_model


# class container for network approximation ------------------------------------------------
class synthesisLSA(object):
    # -- Constructors -------------------------------------------------------------------------
    def __init__(self, error_constraint):
        # user constraints
        self.error_constraint = error_constraint

        # optimization features
        self.current_avg_error = 0.0
        self.current_max_error = 0.0
        self.current_area = 0.0
        self.current_delay = 0.0

        # techlib specific features
        self.lib_dict = {}

        # network specific features
        self.num_nodes = 0
        self.all_features = []
        self.all_nodes = []
        self.all_vdd_gnd = []
        self.all_primary = []
        self.node_by_level = []
        self.gate_error = []
        self.network_lookup = {}
        self.connections_dict = {}
        self.curr_delay_dict = {}
        self.curr_gate_dict = {}

        # insert a gate_features dictionary / key = node name / value = list[all features of the node]
        self.gate_features = {}
        # utilize the connections_dict dictionary for the key = node name / value = list[outputs of node]
        # consider output_connections and input_connections dictionaries for easier traversal during feature generation
        self.output_connections = {}
        self.input_connections = {}


        self.nodes_changed = []
        self.crit_output = ""
        self.crit_path = []
        self.truthTable = {}
        self.numInputs = {}

        # dnn initialization
        self.model = Sequential()
        self.model = load_model('model_data_error_train.h5')

    # --------------------------------------------------------------------------------------

    # Class methods ------------------------------------------------------------------------
    # -- Simple print status ---------------------------------------------------
    def printStatus(self):
        self.calcOutputError()
        self.calcDelay(1)
        self.calcArea(1)
        print("\n")
        print("Current Status: ------------")
        print("error_constraint: ", self.error_constraint)
        print("current_avg_error: ", self.current_avg_error)
        print("current_max_error: ", self.current_max_error)
        print("current_area: ", self.current_area)
        print("current_delay: ", self.current_delay)
        print("----------------------------")



    # -- load the contents of the technology library ---------------------------
    def loadLibraryStats(self):
        library = [line.rstrip("\n") for line in open("mcnc.genlib")]
        lib_list = []
        for item in library:
            temp_list = item.replace("\t", " ").split(" ")
            while('' in temp_list):
                temp_list.remove('')
            lib_list.append(temp_list)

        for gate in lib_list:
            if("zero" not in gate[1] and "DFF" not in gate[1] and "one" not in gate[1]):
                temp_name = gate[1]
                temp_area = gate[2]
                temp_input_load = gate[7]
                temp_delay = gate[9]
                self.lib_dict[temp_name] = {}
                self.lib_dict[temp_name]['area'] = temp_area
                self.lib_dict[temp_name]["input_load"] = temp_input_load
                self.lib_dict[temp_name]["delay"] = temp_delay
            elif("zero" in gate[1] or "one" in gate[1]):
                temp_name = gate[1]
                self.lib_dict[temp_name] = {}
                self.lib_dict[temp_name]['area'] = 0
                self.lib_dict[temp_name]["input_load"] = 0
                self.lib_dict[temp_name]["delay"] = 0

        self.truthTable = {"inv1": "10", "inv2": "10", "inv3": "10", "inv4": "10", \
                           "nand2": "1110", "nand3": "11111110", "nand4": "1111111111111110", \
                           "nor2": "1000", "nor3": "10000000", "nor4": "1000000000000000", \
                           "and2": "0001", "or2": "0111", "xor2a": "0110", "xor2b": "0110", \
                           "xnor2a": "1001", "xnor2b": "1001", "aoi21": "10101000", "aoi22": "1110111011100000", \
                           "oai21": "11101010", "oai22": "1111100010001000", "BUF1": "01", "DFF": "01", \
                           "zero": "0", "one": "1"}

        self.numInputs = {"inv1": "1", "inv2": "1", "inv3": "1", "inv4": "1", \
                          "nand2": "2", "nand3": "3", "nand4": "4", \
                          "nor2": "2", "nor3": "3", "nor4": "4", \
                          "and2": "2", "or2": "2", "xor2a": "2", "xor2b": "2", \
                          "xnor2a": "2", "xnor2b": "2", "aoi21": "3", "aoi22": "4", \
                          "oai21": "3", "oai22": "4", "BUF1": "1", \
                          "zero": "0", "one": "0"}

    def reset(self):
        # techlib specific features
        self.lib_dict = {}

        # network specific features
        self.num_nodes = 0
        self.all_features = []
        self.all_nodes = []
        self.all_vdd_gnd = []
        self.all_primary = []
        self.node_by_level = []
        self.gate_error = []
        self.network_lookup = {}
        self.connections_dict = {}
        self.curr_delay_dict = {}
        self.curr_gate_dict = {}

        self.gate_features = {}
        self.output_connections = {}
        self.input_connections = {}

        self.normalize_list = []
        self.nodes_changed = []
        self.crit_output = ""
        self.crit_path = []
        self.truthTable = {}
        self.numInputs = {}



    # -- load the exact mapped network -----------------------------------------
    def loadNetwork(self):
        # get the network from node_types.txt
        temp_nodes_types = [line.rstrip("\n") for line in open("node_types.txt")]

        self.num_nodes = len(temp_nodes_types)-1

        # filter out anything with INPUT or OUTPUT
        nodes_types = []
        primary_types = []
        temp_nodes_types.pop(0)
        for item in temp_nodes_types:
            if("INPUT" not in item and "OUTPUT" not in item):
                nodes_types.append(item)
            else:
                primary_types.append(item)


        # append 0.0 for gate error
        for i in range(0, len(nodes_types)):
            self.gate_error.append(0.0)

        temp_input_list = []
        for item in primary_types:
            if("\t" in item):
                item = item.replace("\t", " ")
            temp_input_list.append(item.split(" "))

        self.all_primary = copy.deepcopy(temp_input_list)

        # split list into name and output name of node
        temp_count = 0
        for item in nodes_types:
            temp_list = item.split("\t")
            if("" in temp_list):
                temp_list.remove("")
            nodes_types[temp_count] = temp_list
            temp_count = temp_count + 1

        for item in nodes_types:
            if(item[0] not in self.curr_gate_dict):
                self.curr_gate_dict[item[0]] = 1
            else:
                self.curr_gate_dict[item[0]] = self.curr_gate_dict[item[0]] + 1

        # get node features provided by ABC synthesis tool on initial mapping
        node_features = [line.rstrip("\n") for line in open("train_data.txt")]

        # create list of node levels // as provided by initial ABC mapping
        temp_list = []
        level_list = []
        for item in node_features:
            temp_list = item.split(" ")
            self.all_features.append(temp_list)
            level_list.append(temp_list[4])

        # append the level onto the node_type list of lists
        temp_count = 0
        for item in nodes_types:
            item.append(level_list[temp_count])
            item.append(temp_count)
            temp_count = temp_count + 1

        self.all_nodes = copy.deepcopy(nodes_types)

        self.node_by_level = []
        temp_count = 1
        for node in range(0,len(nodes_types)):
            temp_list = []
            for item in nodes_types:
                if (item[2] == str(temp_count)):
                    temp_list.append(item)
                elif (item[2] == "0" and item not in self.all_vdd_gnd):
                    self.all_vdd_gnd.append(item)
            if(temp_list):
                self.node_by_level.append(temp_list)
            temp_count = temp_count + 1

        for item in self.all_vdd_gnd:
            name = item[1]
            item[1] = item[2]
            item[2] = item[3]
            item[3] = name
            item.append([])


        # create dictionary of output names and the inputs
        temp_nodes_edges = [line.rstrip("\n") for line in open("node_edges.txt")]
        node_edges = []
        for item in temp_nodes_edges:
            temp = (item.split("\t"))
            if ("" in temp):
                temp.remove("")
            temp.reverse()
            node_edges.append(temp)

        for item in node_edges:
            if (item[0] not in self.connections_dict):
                self.connections_dict[item[0]] = []
                self.connections_dict[item[0]].append(item[1])
                self.curr_delay_dict[item[0]] = 0
            else:
                self.connections_dict[item[0]].append(item[1])
                self.curr_delay_dict[item[0]] = 0

            if (item[0] not in self.output_connections):
                self.output_connections[item[0]] = []
                for other in node_edges:
                    if (item[0] == other[1]):
                        self.output_connections[item[0]].append(other[0])



        # move the output name of the node to the back
        for level in self.node_by_level:
            for node in level:
                temp = node.pop(1)
                node.append(temp)
                node.append(self.connections_dict[node[3]])

        # create a network lookup
        for level in range(0,len(self.node_by_level)):
            for node in range(0, len(self.node_by_level[level])):
                name = self.node_by_level[level][node][3]
                index = []
                index.append(level)
                index.append(node)
                self.network_lookup[name] = index

        self.levelToFlat()
        self.loadFeatures()

    # initializes every node's features for feature injection into the DNN during error prediction
    def loadFeatures(self):
        # initialize input connections for each node
        for gate in self.all_nodes:
            self.input_connections[gate[3]] = gate[4]
        features = [line.rstrip("\n") for line in open("train_data.txt")]
        gate_count = 0
        for item in features:
            feat_list = item.split(" ")
            splice_list = feat_list[0:6]
            self.gate_features[self.all_nodes[gate_count][3]] = splice_list
            gate_count = gate_count + 1

        # load in the normalized features
        temp_normalize_list = [line.rstrip("\n") for line in open("td_normalization_values.txt")]
        self.normalize_list = []
        for item in temp_normalize_list:
            temp_float = []
            temp = item.split(" ")
            temp_float.append(float(temp[0]))
            temp_float.append(float(temp[1]))
            self.normalize_list.append(temp_float)




    def gateNumLookup(self, name):
        if (name == "inv1"):
            return 0
        elif (name == "inv2"):
            return 1
        elif (name == "inv3"):
            return 2
        elif (name == "inv4"):
            return 3
        elif (name == "nand2"):
            return 4
        elif (name == "nand3"):
            return 5
        elif (name == "nand4"):
            return 6
        elif (name == "nor2"):
            return 7
        elif (name == "nor3"):
            return 8
        elif (name == "nor4"):
            return 9
        elif (name == "and2"):
            return 10
        elif (name == "or2"):
            return 11
        elif (name == "xor2a"):
            return 12
        elif (name == "xor2b"):
            return 13
        elif (name == "xnor2a"):
            return 14
        elif (name == "xnor2b"):
            return 15
        elif (name == "aoi21"):
            return 16
        elif (name == "aoi22"):
            return 17
        elif (name == "oai21"):
            return 18
        elif (name == "oai22"):
            return 19
        elif (name == "BUF1"):
            return 20
        elif (name == "DFF"):
            return 21
        elif (name == "zero"):
            return 22
        elif (name == "one"):
            return 23
        else:
            print("Error: There has been a problem with gate naming internally.")
            exit(1)

    def updateFeature(self, node, replacement):
        if (len(node) <= 2):
            self.gate_features[self.node_by_level[node[0]][node[1]][3]][3] = self.gateNumLookup(replacement)
        else:
            self.gate_features[node[3]][3] = self.gateNumLookup(replacement)

    def genFeature(self, node, replacement):
        # replicate the feature generation of the training model based on the current network
        # direct node features
        feature = []
        for feat in self.gate_features[node[3]]:
            feature.append(feat)
        # node fanin features
        count = 0
        for in_gate in self.input_connections[node[3]]:
            if (in_gate in self.gate_features and count < 4):
                for feat in self.gate_features[in_gate]:
                    feature.append(feat)
                count = count + 1
        while(count < 4):
            for i in range(0, 6):
                feature.append("0")
            count = count + 1
        # node fanout features
        count = 0
        for out_gate in self.output_connections[node[3]]:
            if (out_gate in self.gate_features and count < 10):
                for feat in self.gate_features[out_gate]:
                    feature.append(feat)
                count = count + 1
        while (count < 10):
            for i in range(0, 6):
                feature.append("0")
            count = count + 1

        # append on local error, current_max_error, current_avg_error
        feature.append(self.getIntrinsic(node[0], replacement))
        feature.append(self.current_avg_error)
        feature.append(self.current_max_error)
        # normalize the feature based on training data
        for i in range(0, 93):
            temp = feature[i]
            feature[i] = (float(feature[i]) - self.normalize_list[i][0]) / (self.normalize_list[i][1] - self.normalize_list[i][0])
            # handle circuits we haven't seen before // extreme cases
            if (feature[i] < 0):
                feature[i] = 0
            elif (feature[i] > 1):
                feature[i] = 1

        return feature


    # converts the network stored by level and converts to a flattened list of all nodes
    def levelToFlat(self):
        temp_all_nodes = []
        for level in self.node_by_level:
            for gate in level:
                temp_all_nodes.append(gate)
        for gate in self.all_vdd_gnd:
            temp_all_nodes.append(gate)
        self.all_nodes = sorted(temp_all_nodes, key=itemgetter(2))


    # -- Calculate the Total Area of the network --------------------------------
    def calcArea(self, set):
        total_area = 0
        for level in self.node_by_level:
            for gate in level:
                if (gate[0] in self.lib_dict):
                    total_area = total_area + float(self.lib_dict[gate[0]]['area'])
        if(set != 0):
            self.current_area = total_area
        return total_area


    def getAvgError(self):
        return self.current_avg_error

    def getMaxError(self):
        return self.current_max_error


    def printGates(self):
        self.calcArea(1)
        print("\nNetwork Gates: --------------------------")
        for i in self.curr_gate_dict:
            percent_error = str(round((100*self.curr_gate_dict[i]*float(self.lib_dict[i]['area']))/self.current_area,1))
            print('{:<6}{:<3}{:>11}{:>4}{:>4}{:>7}{:>6}'.format(i, " | ", "Instance =", self.curr_gate_dict[i] \
                                                 , " | ", "Area =", percent_error+"%"))
        print("-----------------------------------------")


    # -- Calculate the Delay of the network, updates a dictionary of delays as well as
    # sets the current max delay of the network. Starting at the first level and progressing to
    # the last, it propagates the delay of each gate and updates a delay dictionary with
    # each gate's respective delay in a circuit.
    def calcDelay(self, set):
        max_of_network = 0.0
        for level in self.node_by_level:
            for gate in level:
                max_of_gate_inputs = 0.0
                if (gate[0] != "one"):
                    for gate_input in gate[4]:
                        if(gate_input in self.curr_delay_dict):
                            if(self.curr_delay_dict[gate_input] > max_of_gate_inputs):
                                max_of_gate_inputs = self.curr_delay_dict[gate_input]
                    # round to tenths place
                    self.curr_delay_dict[gate[3]] = round(max_of_gate_inputs + float(self.lib_dict[gate[0]]['delay']), 1)
                else:
                    self.curr_delay_dict[gate[3]] = 0.0
                if (self.curr_delay_dict[gate[3]] > max_of_network):
                    max_of_network = self.curr_delay_dict[gate[3]]
                    self.crit_output = gate[3]
        if(set != 0):
            self.current_delay = max_of_network
        return max_of_network


    def getCritPath(self):
        self.calcDelay(1)
        self.crit_path = []
        gate = self.crit_output
        while(self.curr_delay_dict[gate]):
            max = -1
            name_delay = []
            name_delay.append(gate)
            name_delay.append(self.curr_delay_dict[gate])
            if(name_delay not in self.crit_path):
                self.crit_path.append(name_delay)
            else:
                break
            for in_gate in self.connections_dict[gate]:
                if(in_gate in self.curr_delay_dict):
                    if(self.curr_delay_dict[in_gate] > max):
                        max = self.curr_delay_dict[in_gate]
                        gate = in_gate
        return self.crit_path


    def printCritPath(self):
        print("\nCritical Path: ----")
        print('{:<7}{:<3}{:>7}'.format("Node: ", "| ", "Delay(ns)"))
        print("-------------------")
        for item in self.crit_path:
            print('{:<6}{:<3}{:>6}'.format(item[0], " | ", item[1]))
        print("-------------------")


    # -- Prints the delay and the max delay of each level
    def printDelay(self):
        self.calcDelay(1)
        print("\nLevel Num ", " | Max Node | ", "Delay(ns)", sep="")
        print("---------------------------------", sep="")

        level_count = 1
        for level in self.node_by_level:
            max_delay_gate = 0
            for gate in level:
                if (self.curr_delay_dict[gate[3]] > max_delay_gate):
                    max_delay_gate = self.curr_delay_dict[gate[3]]
                    name = gate[3]
            print('{:<6}{:<3}{:<5}{:<6}{:<5}{:<5}'.format("Level: ", level_count, " | ", name, " | ", max_delay_gate, sep=""))
            level_count = level_count + 1

        print("\nCritical Delay: ", self.current_delay,sep="")
        print("---------------------------------")

    # -- Write the current node structure to node_types.txt // used for absolute error propagation
    def writeNodeTypes(self):
        self.levelToFlat()
        file = open("node_types.txt", "w")
        file.write(str(self.num_nodes))
        file.write("\n")
        for item in self.all_primary:
            file.write(item[0] + "\t" + item[1] + "\n")
        for item in self.all_nodes:
            file.write(item[0] + "\t" + item[3] + "\n")
        file.close()

    # -- Write the current local error of each node to gate_error.txt // used for absolute error propagation
    def writeGateError(self):
        file = open("gate_error.txt", "w")
        for error in self.gate_error:
            file.write(str(error))
            file.write(" 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0")
            file.write("\n")
        file.close()

    # -- Calculate absolute error of the current network
    def calcOutputError(self):
        self.writeNodeTypes()
        self.writeGateError()
        command = "./error > /dev/null"
        os.system(command)
        final_errors_temp = [line.rstrip("\n") for line in open("final_error.txt")]
        final_errors = final_errors_temp[0].split(" ")
        self.current_avg_error = final_errors[0]
        self.current_max_error = final_errors[1]


    def testDnn(self):

        feature = [6, 1, 5, 0, 1, 0.166667, 6, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 3, 1, 18, 2, 0.333333, 6, 2, 1, 4, 3, 0.500000, 6, 2, 1, 7, 2, 0.333333, 6, 3, 1, 5, 2, 0.333333, 6, 3, 1, 5, 2, 0.333333, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.25, 0.0388167, 0.280609]

        print(len(feature))
        for i in range(0, 93):
            temp = feature[i]
            feature[i] = (float(feature[i]) - self.normalize_list[i][0]) / (self.normalize_list[i][1] - self.normalize_list[i][0])
            # handle circuits we haven't seen before // extreme cases
            if (feature[i] < 0):
                feature[i] = 0
            elif (feature[i] > 1):
                feature[i] = 1


        nf_temp = np.array(feature)
        nf = nf_temp.reshape(1,93)
        ypred = self.model.predict(nf, batch_size=1, verbose=0)
        print(ypred.shape, np.argmax(ypred))
        if (np.argmax(ypred) < 41):
            dnn_error = np.argmax(ypred[0]) * 0.0125  # (0.5/40 = 0.0125)
        else:
            dnn_error = ((np.argmax(ypred[0]) - 40) * 0.05) + 0.5  # (0.5/10)=0.05)

        print(dnn_error)

    # gets the error from the dnn
    # every n times runs self.dnnCheckError() and checks how off it is
    def dnnGetError(self, node, replacement):
        # have some sort of count
        # normalized feature is a list of float values of length 93
        normalized_feature = self.genFeature(node, replacement)
        nf_temp = np.array(normalized_feature)
        nf = nf_temp.reshape(1,93)
        ypred = self.model.predict(nf, batch_size=1, verbose=0)
        #print(ypred.shape, np.argmax(ypred))
        if (np.argmax(ypred) < 41):
            dnn_error = np.argmax(ypred[0]) * 0.0125  # (0.5/40 = 0.0125)
        else:
            dnn_error = ((np.argmax(ypred[0]) - 40) * 0.05) + 0.5  # (0.5/10)=0.05)


        # calculating error now just for testing
        self.calcOutputError()

        #print(dnn_error, self.current_avg_error)


    # -- Returns the absolute error of the current network
    def getOutputError(self):
        self.calcOutputError()
        return self.current_avg_error

    # -- Returns another random gate of similar input number
    def randReplacementGate(self, gate):
            list_of_possible = []
            if ("inv" in gate):
                return "BUF1"
            for key in self.numInputs:
                if (self.numInputs[gate] == self.numInputs[key] and key != gate):
                    list_of_possible.append(key)
            return list_of_possible[random.randint(0, len(list_of_possible) - 1)]

    def getIntrinsic(self, gate1, gate2):
        count = 0
        if ("one" not in gate1 and "one" not in gate2 and "zero" not in gate1 and "zero" not in gate2):
            for i in range(len(self.truthTable[gate1])):
                if (self.truthTable[gate1][i] != self.truthTable[gate2][i]):
                    count = count + 1
            error = count / len(self.truthTable[gate1])
            return error
        elif ("one" in gate1+gate2):
            if("one" in gate1):
                for i in range(len(self.truthTable[gate2])):
                    if (self.truthTable[gate2][i] != "1"):
                        count = count + 1
                error = count / len(self.truthTable[gate2])
                return error
            else:
                for i in range(len(self.truthTable[gate1])):
                    if (self.truthTable[gate1][i] != "1"):
                        count = count + 1
                error = count / len(self.truthTable[gate1])
                return error
        else:
            if("zero" in gate1):
                for i in range(len(self.truthTable[gate2])):
                    if (self.truthTable[gate2][i] != "0"):
                        count = count + 1
                error = count / len(self.truthTable[gate2])
                return error
            else:
                for i in range(len(self.truthTable[gate1])):
                    if (self.truthTable[gate1][i] != "0"):
                        count = count + 1
                error = count / len(self.truthTable[gate1])
                return error


    def fastestNode(self, gate):
        num_inputs = self.numInputs[gate]
        if(num_inputs == "1"):
            return ("inv1")
        elif(num_inputs == "2"):
            return ("nand2")
        elif(num_inputs == "3"):
            return ("nand3")
        elif(num_inputs == "4"):
            return ("nand4")
        else:
            print("This library only has 4 inputs")

    def getNode(self, node):
        if (len(node) <= 2):
            return self.node_by_level[node[0]][node[1]]
        else:
            return node

    def approxSynth(self):

        print("Optmizing Delay...")
        continue_to_area_opt = 0

        # Critical Path Optimization (begin) -------------------------------------------------------
        temp = []
        count = 0
        # break will end the loop
        while(1):
            # prints the loading bar
            sys.stdout.write("\r" + "Trying: " + str(count) + " | " + "Error: " + str(self.current_avg_error) + "     ")
            # previous change // used for stopping condition
            last_temp = temp
            # cp is the list of the critical path
            cp = self.getCritPath()
            # get the first gate that hasnt been changed on the critical path
            if(cp):
                firstgate = cp.pop(-1)
            while(firstgate[0] in self.nodes_changed):
                if(cp):
                    firstgate = cp.pop(-1)
                else:
                    break
            # get the index of the gate in the network // temp is a list
            temp = self.network_lookup[firstgate[0]]
            # make a copy of the gate in case the error constraint is violated
            orig_gate = copy.deepcopy(self.node_by_level[temp[0]][temp[1]])
            # replace the gate in the network with the fastest gate
            fast_gate = self.fastestNode(orig_gate[0])


            if (fast_gate != orig_gate[0]):

                self.node_by_level[temp[0]][temp[1]][0] = fast_gate
                # if the gate has not been changed
                if(firstgate[0] not in self.nodes_changed):
                    # note the gate has been changed
                    self.nodes_changed.append(firstgate[0])
                    index = self.node_by_level[temp[0]][temp[1]][2]
                    # add the error of the replacement to the gate_error list // later printed to calc error
                    self.gate_error[index] = self.getIntrinsic(fast_gate, orig_gate[0])
                    self.updateFeature(temp, fast_gate)
                    #self.calcOutputError()
                    self.dnnGetError(self.getNode(orig_gate), fast_gate)
                    count = count + 1
                    # if error contraint violated, replace the node with the original and set local error to 0
                    if (float(self.current_avg_error) > self.error_constraint):
                        self.nodes_changed.remove(firstgate[0])
                        self.node_by_level[temp[0]][temp[1]][0] = orig_gate[0]
                        self.gate_error[index] = 0
                        self.updateFeature(temp, orig_gate[0])
                        self.calcOutputError()
                        break
                # if the node has been changed, change the node back to the original gate
                else:
                    self.node_by_level[temp[0]][temp[1]][0] = orig_gate
            else:
                self.nodes_changed.append(firstgate[0])
            # stopping condition // if there is no option for further replacement to make the critical path better
            if (last_temp == temp):
                self.calcOutputError()
                break

        while (1):
            # prints the loading bar
            sys.stdout.write("\r" + "Trying: " + str(count) + " | " + "Error: " + str(self.current_avg_error) + "     ")
            # previous change // used for stopping condition
            last_temp = temp
            # cp is the list of the critical path
            cp = self.getCritPath()
            # get the first gate that hasnt been changed on the critical path
            if (cp):
                firstgate = cp.pop(0)
            while ("nand" not in firstgate[0]):
                if (cp):
                    firstgate = cp.pop(0)
                else:
                    break
            #print(firstgate)
            # get the index of the gate in the network // temp is a list
            temp = self.network_lookup[firstgate[0]]
            # make a copy of the gate in case the error constraint is violated
            orig_gate = copy.deepcopy(self.node_by_level[temp[0]][temp[1]])
            # replace the gate in the network with the fastest gate
            if (self.getIntrinsic("one", orig_gate[0]) <= self.getIntrinsic("zero", orig_gate[0])):
                fast_gate = "one"
            else:
                fast_gate = "zero"

            if (fast_gate != orig_gate[0]):

                self.node_by_level[temp[0]][temp[1]][0] = fast_gate
                # if the gate has not been changed
                if (firstgate[0] is not "one" and firstgate is not "zero"):
                    # note the gate has been changed
                    #self.nodes_changed.append(firstgate[0])
                    index = self.node_by_level[temp[0]][temp[1]][2]
                    # add the error of the replacement to the gate_error list // later printed to calc error
                    self.gate_error[index] = self.getIntrinsic(fast_gate, orig_gate[0])
                    #print(fast_gate, orig_gate[0], self.gate_error[index], "\n")
                    self.updateFeature(temp, fast_gate)
                    self.levelToFlat()
                    self.calcOutputError()
                    #self.dnnGetError(self.getNode(orig_gate), fast_gate)
                    count = count + 1
                    # if error contraint violated, replace the node with the original and set local error to 0
                    if (float(self.current_avg_error) > self.error_constraint):
                        self.node_by_level[temp[0]][temp[1]][0] = orig_gate[0]
                        self.gate_error[index] = 0
                        self.updateFeature(temp, orig_gate[0])
                        self.calcOutputError()
                        break
                # if the node has been changed, change the node back to the original gate
                else:
                    self.node_by_level[temp[0]][temp[1]][0] = orig_gate
            else:
                self.nodes_changed.append(firstgate[0])
            # stopping condition // if there is no option for further replacement to make the critical path better
            if (last_temp == temp):
                self.calcOutputError()
                continue_to_area_opt = 1
                break


        # Critical Path Optimization (end) --------------------------------------------------------

    def areaClean(self):

        # Area Optimization // if allowed (begin) -------------------------------------------------
        # Only does area optimization if there is left over error constraint
        # This happens after minimizing the critical path
        continue_to_area_opt = 1
        count = 0
        # iteration version
        if(continue_to_area_opt):
            print("\n\nOptimizing area with left over error constraint...")
            cont_break = 0
            for level in range(0, len(self.node_by_level)-1):
                for i in range(0,len(self.node_by_level[level])-1):
                    gate = self.node_by_level[level][i]
                    if (gate[3] not in self.nodes_changed and "one" not in gate[3] and "zero" not in gate[3]):
                        orig_gate = copy.deepcopy(gate)
                        faster_gate = self.fastestNode(orig_gate[0])
                        if (orig_gate[0] != faster_gate):
                            self.node_by_level[level][i][0] = faster_gate
                            self.nodes_changed.append(gate[3])
                            self.gate_error[gate[2]] = self.getIntrinsic(orig_gate[0], faster_gate)
                            self.updateFeature(gate, faster_gate)
                            # self.calcOutputError()
                            self.dnnGetError(self.getNode(orig_gate), faster_gate)
                            count = count + 1
                            if (float(self.current_avg_error) > self.error_constraint):
                                self.nodes_changed.remove(gate[3])
                                gate[0] = orig_gate[0]
                                self.node_by_level[level][i][0] = orig_gate[0]
                                self.gate_error[gate[2]] = 0
                                self.updateFeature(gate, orig_gate[0])
                                self.calcOutputError()
                                count = count - 1
                                cont_break = 1
                                break
                    self.calcArea(1)
                    sys.stdout.write("\r" + "Trying: " + str(count) + " | " \
                                    + "Error: " + str(self.current_avg_error) + "      ")
                if (cont_break):
                    break




