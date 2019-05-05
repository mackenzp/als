#!/usr/bin/python3

# Author(s): Mackenzie Peterson
# Description:  This file acts as the user control for DNN training, approximate synthesis,
#               exact synthesis, and file I/O.

import os
import copy
import time
from synthesisEngine import synthesisEngine

# adds tab autocomplete capability ---------------------------------------------------------
try:
	import readline
	readline.set_completer_delims(' \t\n;')
	readline.parse_and_bind("tab: complete")
except ImportError:
	print("Note: Unable to import packages for tab autocomplete. This may happen with OS X\n")

# ------------------------------------------------------------------------------------------


# prints the welcome message when opened ---------------------------------------------------
def printInit():
    print("USC Approximate Logic Synthesis Suite v1.0")
    print("For a list of commands, please type \"help\"")

# gets the user command from the terminal --------------------------------------------------
def getCommand():
    command = input("als > ")
    return command

# ------------------------------------------------------------------------------------------
def writeRuntxt(command):
    file = open("run.txt", "w")
    file.write("read_library mcnc.genlib")
    file.write("\n")
    file.write("read ")
    file.write(command)
    file.write("\n")
    file.write("map")
    file.write("\n")
    file.write("write_blif original.blif")
    file.write("\n")
    file.write("show")
    file.close()

# ------------------------------------------------------------------------------------------
def runABC():
    command = "./abc -f run.txt > abc.log"
    os.system(command)

def checkABCError(filename):
    lines = [line.rstrip("\n") for line in open("abc.log")]
    for line in lines:
        if ("Error" in line):
            print("Error: Filename ",filename," not found", sep="")
            print("\n")
            return 1
    return 0

def is_float(s_in):
    s = copy.deepcopy(s_in)
    result = False
    if s.count(".") == 1:
        if s.replace(".", "").isdigit():
            result = True
    return result

# prints the possible commands that can be run for the user --------------------------------
def printHelp(): 
    print("\tSynthesize Command(s):")
    print("\t map_approx    <file_path (.blif or .bench)>   <error constraint (0 - 1.0)>")
    print("\t map_exact     <file_path (.blif or .bench)>")
    print("\n")
    print("\tWrite Command(s):")
    print("\t write_blif    <filename (.blif)>")
    print("\t print_error")
    print("\n")
    print("\tTrain Command(s):")
    print("\t train_dnn")

def writeBlif(command):
    # ensure the argument is valid
    command_list = command.split(" ")
    if(len(command_list)!=2):
        print("ERROR: write_blif should have one argument\n")
        return
    for item in command_list:
        if("write_blif" in item):
            command_list.remove(item)
    command = command_list[0]
    if (".blif" not in command or len(command)<=6):
        print("ERROR: invalid filetype for write_blif\n")
        return

    system_call = "python3 node_types_to_blif.py"
    os.system(system_call)
    system_call = "python3 custom_bench_to_blif.py original.bench >" + command
    os.system(system_call)
    print("Successfully wrote mapped network to", command)
    print("\n")

# utilizes the DNN to do approximate synthesis ----------------------------------------------
def mapApprox(command):

    util_dnn = True
    init_command = copy.deepcopy(command)
    # ensure the argument is valid
    command_list = command.split(" ")
    if(len(command_list) == 4 and "-nodnn" in command_list):
        print("\nNot Utilizing DNN")
        util_dnn = False
    elif(len(command_list)!=3):
        print("ERROR: map_approx should have two arguments")
        print("\t map_approx    <file_path (.blif or .bench)>   <error constraint (0 - 1.0)>")
        return

    for item in command_list:
        if("map_approx" in item):
            command_list.remove(item)
    command = command_list[0]

    # checks that the user_error_constraint is of type float before continuing
    if(is_float(command_list[1])):
        user_error_constraint = float(command_list[1])
        '''
        if (user_error_constraint <= 0.001):
            temp = init_command.split(" ")
            temp.pop(0)
            temp.pop(-1)
            new_command = ""
            for item in temp:
                new_command = new_command + item + " "
            print("\nError Rate must be above 0.001, mapping exact network..")
            mapExact(new_command)
            return
        '''
    else:
        print("ERROR: map_approx should have a valid error constraint between 0.0 and 1.0")
        print("\t map_approx    <file_path (.blif or .bench)>   <error constraint (0.0 - 1.0)>")
        return

    if ((".bench" not in command and ".blif" not in command) or len(command)<=6):
        print("ERROR: invalid filetype for map_approx\n")
        return
    writeRuntxt(command)
    map_start = time.time()
    runABC()
    map_end = time.time()

    # extracts information about the nodes for approximate synthesis
    extract_command = "python3 blif_to_custom_bench.py > original.bench"
    os.system(extract_command)
    extract_command = "python3 node_extract.py original.bench"
    os.system(extract_command)

    # checks that the filename is correct and that ABC was able to successfully map
    if(not checkABCError(command)):

        print("\nExact Network:")
        network = synthesisEngine(error_constraint=user_error_constraint)
        network.loadLibraryStats()
        network.loadNetwork()
        network.printGates()
        network.getCritPath()
        network.printCritPath()
        init_delay = network.calcDelay(1)
        init_area = network.calcArea(1)
        start = time.time()
        network.approxSynth(dnn=util_dnn)
        network.areaClean(dnn=util_dnn)
        end = time.time()
        error = network.calcOutputError()
        repl_delay = network.calcDelay(1)
        repl_area = network.calcArea(1)
        #network.getCritPath()
        network.writeNodeTypes()

        # get the error at all outputs
        command = "./error -all"
        os.system(command)

        #start = time.time()
        system_call = "python3 node_types_to_blif.py"
        os.system(system_call)
        system_call = "python3 custom_bench_to_blif.py original.bench > temp.blif"
        os.system(system_call)
        writeRuntxt("temp.blif")
        runABC()
        extract_command = "python3 blif_to_custom_bench.py > original.bench"
        os.system(extract_command)
        extract_command = "python3 node_extract.py original.bench"
        os.system(extract_command)
        os.system("rm temp.blif")
        #end = time.time()


        print("\n\nApproximated Network:")
        network.reset()
        network.loadLibraryStats()
        network.loadNetwork()
        network.calcArea(1)
        network.printGates()
        network.getCritPath()
        network.printCritPath()
        final_delay = network.calcDelay(1)
        final_area = network.calcArea(1)

        print("\nSynthesis Time (seconds):", round(end - start,6))

        print("\nResults:")
        print("----------------------------------")
        print("Initial Critical Delay: | ", init_delay)
        print("Initial Area:           | ", init_area)
        print("----------------------------------")
        print("PreMap Critical Delay:  | ", repl_delay)
        print("PreMap Area:            | ", repl_area)
        print("----------------------------------")
        print("Final Critical Delay:   | ", final_delay)
        print("Final Area:             | ", final_area)
        print("----------------------------------")
        print("Error Rate:             | ", error)

        print("\n")

        # unnecessary file removal
        os.system("rm *.dot > /dev/null")
        command = "ls .model* > /dev/null 2>&1"
        if (os.system(command) == 0):
            os.system("rm .model* > /dev/null")

# map the exact network using abc -----------------------------------------------------------
def mapExact(command):
    # ensure the argument is valid
    command_list = command.split(" ")
    if(len(command_list)!=2):
        print("ERROR: map_approx should have one argument\n")
        return
    for item in command_list:
        if("map_exact" in item):
            command_list.remove(item)
    command = command_list[0]
    if ((".bench" not in command and ".blif" not in command) or len(command)<=6):
        print("ERROR: invalid filetype for map_exact\n")
        return
    writeRuntxt(command)
    runABC()
    # write to original.bench in case user wants to write_blif
    os.system("python3 blif_to_custom_bench.py > original.bench")
    extract_command = "python3 node_extract.py original.bench"
    os.system(extract_command)

    print("\nExact Network:")
    network = synthesisEngine(error_constraint=0)
    network.loadLibraryStats()
    network.loadNetwork()
    network.printGates()
    network.getCritPath()
    network.printCritPath()
    network.calcOutputError()
    command = "./error -all"
    os.system(command)
    print("\nNetwork has been mapped with:\nAverage error:\t0%\n")

    # unnecessary file removal
    os.system("rm *.dot > /dev/null")
    command = "ls .model* > /dev/null 2>&1"
    if (os.system(command) == 0):
        os.system("rm .model* > /dev/null")


def printError():
    all_final_errors_temp = [line.rstrip("\n") for line in open("final_error_all_outputs.txt")]
    print("\nNode Name: -> Error")
    print("--------------------")
    for item in all_final_errors_temp:
        print(item)
    print("\n")

# executes the training flow for the DNN -----------------------------------------------------
def trainDNN():
    print("Please ensure that the intended training files are in \"/benchfolder/training_folder\"")
    train = input("Continue? [Y/N]: ")
    print("\n")
    if(train == "Y" or train == "y"):
        # execute training process
        extract_data = "python3 extract_all_features.py"
        os.system(extract_data)
        convert_to_npz = "python3 data_to_npz.py"
        os.system(convert_to_npz)
        train_dnn = "python3 error_training_DNN.py"
        os.system(train_dnn)
        return
    else:
        return

# handles and distributes the commands from the user to their respective functions -----------
def commandHandler(command):
    exit_list = ["quit", "exit", "q"]
    if (command == ""):
        return
    elif ("help" in command or command == "h"):
        printHelp()
    elif ("map_approx" in command):
        mapApprox(command)
    elif ("map_exact" in command):
        mapExact(command)
    elif ("write_blif" in command):
        writeBlif(command)
    elif ("print_error" in command):
        printError()
    elif ("train_dnn" in command):
        trainDNN()
    elif (command not in exit_list):
        print("Command not recognized, type \"help\" for a list of commands\n")

# ------------------------------------------------------------------------------------------
def main():
    printInit()
    command = ""
    exit_list = ["quit", "exit", "q"]
    while(command not in exit_list):
        command = getCommand()
        commandHandler(command)
    


if __name__ == "__main__":
    main()
