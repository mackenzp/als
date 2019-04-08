#!/usr/bin/python3

# Author(s): Mackenzie Peterson, Moises Herrera.
# Description:  This file acts as the user control for DNN training, approximate synthesis,
#               exact synthesis, and file I/O.




import os
from synthesisLSA import synthesisLSA

# ------------------------------------------------------------------------------------------
class training():
    def __init__(self):
        temp = "test"

# prints the welcome message when opened ---------------------------------------------------
def printInit():
    print("USC Approximate Logic Synthesis Suite v1.0")
    print("For a list of commands, please type \"help\"")

# gets the user command from the terminal --------------------------------------------------
def getCommand():
    print("als > ",end="")
    command = input()
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
    file.write("show -g")
    file.close()

# ------------------------------------------------------------------------------------------
def runABC():
    command = "./abc -f run.txt"
    os.system(command)

# prints the possible commands that can be run for the user --------------------------------
def printHelp(): 
    print("\tSynthesize Command(s):")
    print("\t map_approx <.bench file>")
    print("\t map_exact <.bench file>")
    print("\n")
    print("\tWrite Command(s):")
    print("\t write_blif <.blif file>")
    print("\n")
    print("\tTrain Command(s):")
    print("\t train_dnn")

# utilizes the DNN to do approximate synthesis ----------------------------------------------
def mapApprox(command):
    # ensure the argument is valid
    command_list = command.split(" ")
    if(len(command_list)!=2):
        print("ERROR: map_approx should have one argument\n")
        return
    for item in command_list:
        if("map_approx" in item):
            command_list.remove(item)
    command = command_list[0]
    if (".bench" not in command or len(command)<=6):
        print("ERROR: invalid filetype for map_approx\n")
        return
    writeRuntxt(command)
    runABC()

    network = synthesisLSA()
    network.printStatus()


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
    if (".bench" not in command or len(command)<=6):
        print("ERROR: invalid filetype for map_exact\n")
        return
    writeRuntxt(command)
    runABC()
    # write to original.bench in case user wants to write_blif
    os.system("python3 blif_to_custom_bench.py > original.bench")
    print("\nNetwork has been mapped with:\nAverage error:\t0%\nMax error:\t0%\n")


def writeBlif(command):
    # ensure the argument is valid
    command_list = command.split(" ")
    if(len(command_list)!=2):
        print("ERROR: write_bench should have one argument\n")
        return
    for item in command_list:
        if("write_blif" in item):
            command_list.remove(item)
    command = command_list[0]
    if (".blif" not in command or len(command)<=6):
        print("ERROR: invalid filetype for write_blif\n")
        return

    system_call = "python3 custom_bench_to_blif.py original.bench >" + command
    os.system(system_call)
    print("Successfully wrote mapped network to", command)
    print("\n")

# executes the training flow for the DNN -- later should specify the directory of bench files-
def trainDNN():
    train_script = "python3 extract_all_features.py"
    os.system(train_script)

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
    elif ("train_dnn" in command):
        trainDNN()
    elif (command not in exit_list):
        print("Command not recognized\n")

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
