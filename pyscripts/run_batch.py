#!/usr/bin/python3.6
#
# run_batch.py
#
# Description:  This file runs all the files inside the selected folder and runs map_approx with the parameters provided.
# saves all execution output in different text files inside an existent folder. 
#
# fixes/improvements:
#herrerab@usc.edu 
# initial version February 19 2020. 
# added folder checking and creation March 7 2020.
# adding a short version, log version -l March 9 2020.
# updated to changes June 20,2020.
#
import os, sys, glob
from Utils import printInit
#printInit()
print("\nALS suite.\n\nrun_bach script.\n\nRuns an ALS command on all files inside the selected folder (.bench and .blif).\nSaves execution output in different text files in an existent folder.\n\n")
#
if (len(sys.argv) > 1):
 print("Executing long version \n")
 l = 1
else:
 l =0
 library= "mcnc.genlib"
 folder_to_write = "results/test/"
 command_p = "map_approx"
 parameters = "-r 0.05 -a -p -i 10"
 print("-----------------------\nshort version \n-----------------------\nRunning Defaults: \n")
 print("Library: " + library + "\nFolder to write results: " + folder_to_write + " (Note: Previous results will be erased!)")
 print("Command to be run: " + command_p + parameters + " \n")
 #
working_directory= str(os.getcwd())
if (l == 1):
 library = input("Library (e.g. 45nm.genlib): ")
print("Executing ...")
command= "./als 'read_library " + str(library) +"'"
print(command)
os.system(command)
cont = 0
while (cont ==0):
 folder_to_read= input("Folder path to read (Example: benchmarks): ")
 if (folder_to_read[-1] != "/"):
  folder_to_read = folder_to_read+"/"
 print("folder to read files is: "+folder_to_read)
 print("working directory is: "+working_directory)
 if (os.path.isdir(working_directory+"/"+folder_to_read) == True):
  folder_to_read = working_directory+"/"+folder_to_read 
  print("folder to read files is: "+folder_to_read)
  files_bench=glob.glob(folder_to_read + "*.bench",recursive=True)
  files_blif=glob.glob(folder_to_read + "*.blif",recursive=True)
  file_list_bench={x.replace(folder_to_read, '') for x in files_bench}
  file_list_blif={x.replace(folder_to_read, '') for x in files_blif}
#  print(file_list_bench)
#  print(file_list_blif)
  #total_files=len(file_list)
  total_files_bench=len(file_list_bench)
  total_files_blif=len(file_list_blif)  
  #print("Inside "+folder_to_read+": "+ str(total_files)+" files with extension "+file_extension)
  print("Inside "+folder_to_read+":\n"+ str(total_files_bench)+" files with extension bench")
  print(str(total_files_blif)+" files with extension blif")
 if (l == 1):  
  cont =int(input("that is correct? (1) try again(0): "))
  #
 else: 
  cont = 1
#
cont = 0
while (cont == 0):
 if (l == 1):
  command_p = input("command to execute on files (e.g. map_approx): ")
  parameters = input("command parameters (e.g. -r 0.05 -p): ")
  cont = int(input ("Command: " + command_p + " " + parameters + ": Correct? (1) Try again(0) "))
 else:
  cont = 1 
cont = 0
while (cont == 0):
 if (l == 1):
  folder_to_write= input("Path to results (Example: results/ISCAS85/): ")
  if (folder_to_write[-1] != "/"):
   folder_to_write=folder_to_write+"/"
 else:
  cont = 1 
#
#checking the destination folder
#
 dest_dir = working_directory+"/"+folder_to_write
 if (os.path.isdir(dest_dir) == True):
  print(dest_dir+" exists, saving results inside.")
  cont = 1
  if (len(os.listdir(dest_dir)) !=0):
   print(" Warning:destination directory is not empty.")
   if (l == 1):
    cont = int(input("Erase previous files: proceed(1) or try another path(0)? "))
   else:
    cont = 1
   if (cont == 1):
# erasing files inside destination folder   
    filestoremove = [os.path.join(dest_dir,fil) for fil in os.listdir(dest_dir)]
    for fil in filestoremove:
     os.remove(fil)   
 else:
  print (working_directory+"/"+folder_to_write+" does not exist, creating the folder...")
  os.makedirs(working_directory+"/"+folder_to_write)
  if (os.path.isdir(working_directory+"/"+folder_to_write) == True):
   print("created, saving results inside.")
   cont = 1
  else:
   print (working_directory+"/"+folder_to_write+" creation unsuccesful. Please fix this and try again.")
   cont = 0
file_index = 0
#
files = files_bench + files_blif
#print(files)
file_list={x.replace(folder_to_read, '') for x in files}
#
#print(file_list)
total_files=len(file_list)
#
print("Files to be processed: "+str(total_files)+"\n")
#
for file_name in file_list:
 file_index = file_index + 1
 print("\nExecuting file "+ str(file_index) + " out of " + str(total_files))
 print("Checking "+ str(file_name))
 if (str(file_name) == "c17.bench"):
  print ("c17.bench skipped, it is a very small circuit!")
 else:
  print("Working on ..."+ str(file_name))
  temp = file_name.split(".")
  command = "./als '" + str(command_p) + " " + folder_to_read + file_name + " " + str(parameters) + "' >> " + str(folder_to_write) + temp[0] + "_out.txt"
  print(command)
  os.system(command)
print("done")
exit()
