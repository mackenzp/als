#!/usr/bin/python3
#
# run_bach.py
#
# Description:  This file runs all the files inside the selected folder and runs map_approx with the parameters provided.
# saves all execution output in different text files inside an existent folder. 
#
#herrerab@usc.edu feb 19 2020.
#
import os, sys, glob
from Utils import printInit
#printInit()
print("\nALS suite.\n\nrun_bach script.\n\nRuns an ALS command on all files inside the selected folder.\nSaves execution output in different text files in an existent folder.\n\n")
library = input("Library (e.g. 45nm.genlib): ")
print("Executing ...")
command= "./als 'read_library " + str(library) +"'"
print(command)
os.system(command)
folder_to_read= input("Folder path to read(e.g. benchfolder/ISCAS85/): ")
file_extension= input("files extension (e.g. bench): ")
command_p = input("command to execute on files (e.g. map_approx): ")
parameters = input("command parameters (e.g. 0.05 -p): ")
folder_to_write= input("path to output folder (e.g. results/ISCAS85/): ")
#command="./als 'map_approx benchfolder/ISCAS85/c880.bench 0.05 -p' >> test_1.txt"
file_index = 0
#working_directory= str(os.getcwd())
files=glob.glob(folder_to_read + "*." + file_extension,recursive=True)
file_list={x.replace(folder_to_read, '') for x in files}
#print(file_list)
total_files=len(file_list)
for file_name in file_list:
 file_index = file_index + 1
 print("Executing file "+ str(file_index) + " out of " + str(total_files))
 print("Working on ..."+ str(file_name))
 command = "./als '" + str(command_p) + " " + folder_to_read + file_name + " " + str(parameters) + "' >> " + str(folder_to_write) + file_name + "_out.txt"
 print(command)
 os.system(command)
print("done")
exit()
