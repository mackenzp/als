# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose:  Drives the sythesisLSA class impementation -- testing purposes only
# Note:
# Example:

import os
from synthesisEngine import synthesisEngine

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

def runABC():
    command = "./abc -f run.txt"
    os.system(command)



# test driver program to test the synthesisEngine class on a network
command = "benchfolder/ISCAS85/c880.bench"
writeRuntxt(command)
runABC()
command = "python3 blif_to_custom_bench.py > original.bench"
os.system(command)
command = "python3 node_extract.py original.bench"
os.system(command)


user_error_constraint = 0.05

network = synthesisEngine(error_constraint=user_error_constraint)
network.loadLibraryStats()
network.loadNetwork()
network.printGates()
network.printStatus()
network.approxSynth(dnn=True)
network.areaClean(dnn=True)
network.printStatus()
