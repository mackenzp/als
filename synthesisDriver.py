# Author:   Mackenzie Peterson
# Contact:  mackenzp@usc.edu
# Purpose:  Drives the sythesisLSA class impementation -- testing purposes only
# Note:
# Example:

import os
from synthesisLSA import synthesisLSA

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



# test driver program to test the synthesisLSA class on a network
command = "benchfolder/ISCAS85/priority.blif"
writeRuntxt(command)
runABC()
command = "python3 blif_to_custom_bench.py > original.bench"
os.system(command)
command = "python3 node_extract.py original.bench"
os.system(command)


user_error_constraint = 0.20

network = synthesisLSA(error_constraint=user_error_constraint)
network.loadLibraryStats()
network.loadNetwork()
network.printGates()
network.printStatus()
#network.testDnn()
network.approxSynth()