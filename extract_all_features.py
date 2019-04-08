import os

# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose: This file will act as the control and setup flow for error_control.py for all benchfiles
# Note: This is the top level wrapper for generating all training vectors for the DNN
print("\n")
command = "ls train_dnn.txt > /dev/null 2>&1"
if(os.system(command) == 0):
    print("NOTE: Moved train_dnn.txt to train_dnn.bak")
    command = "mv train_dnn.txt train_dnn.bak"
    os.system(command)
else:
    print("NOTE: There is no train_dnn.txt present. Will create one.")

print("NOTE: This will take a while. Ctrl-Z to suspend")

command = "ls benchfolder/ISCAS85 > listOfISCAS85.txt"
os.system(command)

file = open("listOfISCAS85.txt", "r")
temp_names = file.readlines()
benchfilenames = []

# remove \n
for all in temp_names:
  benchfilenames.append(all.rstrip())

file.close()

filecount = 1
# iterate over all benchfiles given // here only for ISCAS85
for benchFile in benchfilenames:
    print("Starting Extraction on File (" + str(filecount) + " of " + str(len(benchfilenames)) + ") => " + benchFile)

    # generate the run.txt file as a buffer for commands for abc to execute
    file = open("run.txt", "w")
    file.write("read_library mcnc.genlib")
    file.write("\n")

    file.write("read benchfolder/ISCAS85/")
    file.write(benchFile)
    file.write("\n")

    file.write("map")
    file.write("\n")

    file.write("write_blif original.blif")
    file.write("\n")

    file.write("show -g")
    file.write("\n")
    file.close()

    # - send the result of abc to a log file for the respective benchfile
    #   + also generates train_data.txt
    #       - please note that a line in train_data.txt is an input vector of features to the DNN
    #       - please note that the order of nodes in train_data.txt is the same order
    #         as the original.blif file that is generated
    logname = benchFile
    if(".bench" in logname):
        new_log = logname.replace(".bench", ".log")
    elif (".blif" in logname):
        new_log = logname.replace(".blif", ".log")
    command = "./abc -f run.txt > console_output/" + new_log
    os.system(command)

    # - run the python script blif_to_custom_bench.py
    #   to convert the mapped blif file to a custom "mapped" bench file
    command = "python3 blif_to_custom_bench.py > original.bench"
    os.system(command)

    # - run the python script node_extract.py with the original.bench
    #   this extracts nodes and edges for error traversal
    #   +   writes to node_edges.txt and node_type.txt
    command = "python3 node_extract.py original.bench"
    os.system(command)

    # - run error_control
    command = "python3 error_control.py"
    os.system(command)
    filecount = filecount + 1
    print("\n")

os.system("rm *.dot")
