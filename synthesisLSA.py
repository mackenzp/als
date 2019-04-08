
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
        self.all_primary = []
        self.node_by_level = []
        self.gate_error = []
        self.network_lookup = {}
        self.connections_dict = {}
        self.curr_delay_dict = {}
        self.curr_gate_dict = {}

        self.nodes_changed = []
        self.crit_output = ""
        self.crit_path = []
        self.truthTable = {}
        self.numInputs = {}
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
            if(temp_list):
                self.node_by_level.append(temp_list)
            temp_count = temp_count + 1

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

    # converts the network stored by level and converts to a flattened list of all nodes
    def levelToFlat(self):
        temp_all_nodes = []
        for level in self.node_by_level:
            for gate in level:
                temp_all_nodes.append(gate)
        self.all_nodes = sorted(temp_all_nodes, key=itemgetter(2))


    # -- Calculate the Total Area of the network --------------------------------
    def calcArea(self, set):
        total_area = 0
        for node in self.all_nodes:
            if (node[0] in self.lib_dict):
                total_area = total_area + float(self.lib_dict[node[0]]['area'])
        if(set != 0):
            self.current_area = total_area
        return total_area

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
                for gate_input in gate[4]:
                    if(gate_input in self.curr_delay_dict):
                        if(self.curr_delay_dict[gate_input] > max_of_gate_inputs):
                            max_of_gate_inputs = self.curr_delay_dict[gate_input]
                # round to tenths place
                self.curr_delay_dict[gate[3]] = round(max_of_gate_inputs + float(self.lib_dict[gate[0]]['delay']), 1)
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


    # gets the error from the dnn
    def dnnGetError():
        pass


    # periodically check delta error between actual and dnn predicted
    def dnnPredictError():
        # call dnnPredictError()
        pass


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
        for i in range(len(self.truthTable[gate1])):
            if (self.truthTable[gate1][i] != self.truthTable[gate2][i]):
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

    def approxSynth(self):

        print("Running Synthesis...")
        continue_to_area_opt = 0
        
        # Critical Path Optimization (begin) -------------------------------------------------------
        temp = []
        count = 0
        # break will end the loop
        while(1):
            # prints the loading bar
            sys.stdout.write("\r" + "Num Replaced: " + str(count) + " | " + "Error: " + str(self.current_avg_error) + " | " \
                             + "Delay: " + str(self.current_delay))
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
            orig_gate = copy.deepcopy(self.node_by_level[temp[0]][temp[1]][0])
            # replace the gate in the network with the fastest gate
            fast_gate = self.fastestNode(orig_gate)
            self.node_by_level[temp[0]][temp[1]][0] = fast_gate
            # if the gate has not been changed
            if(firstgate[0] not in self.nodes_changed):
                # note the gate has been changed
                self.nodes_changed.append(firstgate[0])
                index = self.node_by_level[temp[0]][temp[1]][2]
                # add the error of the replacement to the gate_error list // later printed to calc error
                self.gate_error[index] = self.getIntrinsic(fast_gate, orig_gate)
                self.calcOutputError()
                count = count + 1
                # if error contraint violated, replace the node with the original and set local error to 0
                if (float(self.current_avg_error) > self.error_constraint):
                    self.nodes_changed.remove(firstgate[0])
                    self.node_by_level[temp[0]][temp[1]][0] = orig_gate
                    self.gate_error[index] = 0
                    self.calcOutputError()
                    break
            # if the node has been changed, change the node back to the original gate
            else:
                self.node_by_level[temp[0]][temp[1]][0] = orig_gate
            
            # stopping condition // if there is no option for further replacement to make the critical path better
            if (last_temp == temp):
                self.calcOutputError()
                continue_to_area_opt = 1
                break

        # Critical Path Optimization (end) --------------------------------------------------------


        # Area Optimization // if allowed (begin) -------------------------------------------------
        # Only does area optimization if there is left over error contraint
        # This happeins after minimizing the critical path
        '''
        if(continue_to_area_opt):
            orig_gate = ""
            fast_gate = ""
            print("\nOptimizing area with left over error constraint...") 
            for level in self.node_by_level:    
                for gate in level:
                    if (gate[3] not in self.nodes_changed):
                        orig_gate = copy.deepcopy(gate[0])
                        faster_gate = self.fastestNode(orig_gate)
                        if (orig_gate != faster_gate):
                            gate[0] == faster_name
                            self.nodes_changed.append(gate[3])
                            self.gate_error[gate[2]] = self.getIntrinsic(name, faster_name)
                            self.calcOutputError()
                            count = count + 1
                            if (float(self.current_avg_error) > self.error_constraint):
                               self.nodes_changed.remove(gate[3])
                               gate[0] = orig_gate
                               self.gate_error[gate[2]] = 0
                               self.calcOutputError()
                               break
        '''






        # Area Optimization // if allowed (end) -------------------------------------------------










        '''
        ntk = []
        for level in self.node_by_level:
            Tk = self.Tk
            while(Tk > self.Tf):
                gate_num = random.randint(0, len(level)-1)
                orig = copy.deepcopy(level[gate_num][0])
                level[gate_num][0] = self.randReplacementGate(level[gate_num][0])
                self.gate_error[level[gate_num][2]] = self.getIntrinsic(orig, level[gate_num][0])
                self.calcOutputError()
                if (float(self.current_avg_error) < self.error_constraint):
                    area = self.calcArea(0)
                    delay = self.calcDelay(0)
                    if(delay < self.current_delay):
                        print("found", delay)
                        print("current", self.current_delay)
                        ntk.append(copy.deepcopy(self.node_by_level))
                        self.calcDelay(1)
                    elif(area < self.current_area):
                        self.calcArea(1)
                        Tk = Tk*self.B
                    else:
                        level[gate_num][0] = orig
                        self.gate_error[level[gate_num][2]] = 0.0
                        Tk = Tk*self.B
                else:
                    level[gate_num][0] = orig
                    self.gate_error[level[gate_num][2]] = 0.0
                    Tk = Tk * self.B

        fast = self.node_by_level
        for item in ntk:
            min = 1000000
            self.node_by_level = copy.deepcopy(item)
            delay = self.calcDelay(1)
            print(delay)
            if (delay < min):
                fast = self.node_by_level
        self.node_by_level = copy.deepcopy(fast)
        self.levelToFlat()

        print("Approximate Network: ")
        for level in self.node_by_level:
            print(level)
        '''


