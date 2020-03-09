#!/usr/bin/python3
#
# run_bach.py
#
# Description:  This file runs all the files inside the selected folder and runs map_approx with the parameters provided.
# saves all execution output in different text files inside an existent folder. 
#
# fixes/improvements:
#herrerab@usc.edu 
# initial version feb 19 2020. 
# added folder checking and creation march 7 2020.
#
import os, sys, glob
from Utils import printInit
#printInit()
print("\nALS suite.\n\nrun_bach script.\n\nRuns an ALS command on all files inside the selected folder.\nSaves execution output in different text files in an existent folder.\n\n")
#
working_directory= str(os.getcwd())
library = input("Library (e.g. 45nm.genlib): ")
print("Executing ...")
command= "./als 'read_library " + str(library) +"'"
print(command)
os.system(command)
cont = 0
while (cont ==0):
 folder_to_read= input("Folder path to read(e.g. inside benchfolder type: ISCAS85/): ")
 if (os.path.isdir(working_directory+"/benchfolder/"+folder_to_read) == True):
  cont =1
  folder_to_read = "benchfolder/"+folder_to_read
 else: 
  print ("path not found, try again")
file_extension= input("files extension (e.g. bench): ")
command_p = input("command to execute on files (e.g. map_approx): ")
parameters = input("command parameters (e.g. -r 0.05 -p): ")
cont = 0
while (cont == 0):
 folder_to_write= input("path to output folder (e.g. results/ISCAS85/): ")
#
#checking the destination folder
#
#print(os.path.isdir(working_directory+"/"+folder_to_write))
 if (os.path.isdir(working_directory+"/"+folder_to_write) == True):
  print(working_directory+"/"+folder_to_write+" exists, saving results inside.")
#  cont = 1
  if (len(os.listdir(working_directory+"/"+folder_to_write)) !=0):
   print(" Warning:destination directory is not empty. Some files can be overwrite.")
   cont = int(input("proceed(1) or try another path(0)? "))
 else:
  print (working_directory+"/"+folder_to_write+" does not exist, creating the folder...")
  os.makedirs(working_directory+"/"+folder_to_write)
  if (os.path.isdir(working_directory+"/"+folder_to_write) == True):
   print("created, saving results inside.")
   cont = 1
  else:
   print (working_directory+"/"+folder_to_write+" creation unsuccesful. Please fix this and try again.")
   cont = 0
#command="./als 'map_approx benchfolder/ISCAS85/c880.bench 0.05 -p' >> test_1.txt"
#exit()
file_index = 0
#
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
