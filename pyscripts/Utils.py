import os
import copy
import time
import shutil
import pathlib
from os.path import join
from os import listdir, rmdir
from shutil import move
#===========================================================================================
# prints the welcome message when opened ---------------------------------------------------
def printInit():
    print("USC Approximate Logic Synthesis Suite v1.0")
    print("For a list of commands, please type \"help\"")

# initialized files when als is opened -----------------------------------------------------
def initFiles():
    file = open("final_error_all_outputs.txt","w")
    file.write("\nNo network has been approximated yet\n")
    file.close()

# gets the user command from the terminal --------------------------------------------------
def getCommand():
    command = input("als > ")
    # removes extra spaces
    command = " ".join(command.split())
    return command

# ------------------------------------------------------------------------------------------
def writeRuntxt(command):
    file = open("run.txt", "w")
    file.write("read_library tech_lib/techlib.genlib")
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
def writeRuntxt_power(command):
    file = open("run.txt", "w")
    file.write("read_library tech_lib/techlib.genlib")
    file.write("\n")
    file.write("read ")
    file.write(command)
    file.write("\n")
    file.write("amap; map -a")
    file.write("\n")
    file.write("write_blif original.blif")
    file.write("\n")
    file.write("show")
    file.close()

# ------------------------------------------------------------------------------------------
def runABC():
    command = "abc/abc -f run.txt > abc.log"
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

def copyDirectory(src, dest):
    try:
        shutil.copytree(src, dest)
    # Directories are the same
    except shutil.Error as e:
        print('Directory not copied. Error: %s' % e)
    # Any error saying that the directory doesn't exist
    except OSError as e:
        print('Directory not copied. Error: %s' % e)

def del_unnecessary_files():
    os.system("rm abc.rc")
    os.system("rm *.dot > /dev/null")
    os.system("rm *.txt > /dev/null")
    os.system("rm *.log > /dev/null")
    os.system("rm *.bench > /dev/null")
    os.system("rm *.blif > /dev/null")
    command = "ls .model* > /dev/null 2>&1"
    if (os.system(command) == 0):
        os.system("rm .model* > /dev/null")
    #returning .blif file back:
    current_path = os.getcwd()
    path = join(current_path, "temp_blif")
    if os.path.exists(path):
        #copyDirectory(path, current_path)
        for filename in listdir(path):
            move(join(path, filename), join(current_path, filename))
        rmdir(path)

# prints the possible commands that can be run for the user --------------------------------
def printHelp(): 
    print("\tSynthesize Command(s):")
    print("\t map_approx    <file_path (.blif or .bench)>   <error constraint (0 - 1.0)>")
    print("\t map_exact     <file_path (.blif or .bench)>")
    print("\t read_library  <library name (.genlib)>")
    print("\n")
    print("\tWrite Command(s):")
    print("\t write_blif    <filename (.blif)>")
    print("\t print_error")
    print("\n")
    print("\tTrain Command(s):")
    print("\t train_dnn")
    print("\n")
    print("\t show -g")

def writeBlif(command, verbose, model_name):
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
    
    system_call = "python3 pyscripts/node_types_to_blif.py"
    os.system(system_call)
    system_call = "python3 pyscripts/custom_bench_to_blif.py original.bench >" + command
    os.system(system_call)
    if(verbose):
        print("Successfully wrote mapped network to", command)

    #fixing .model name:
    tmp = open("temp__blif.blif", "w")
    with open(command, "r") as fp:
        for line in fp:
            if(line.startswith(".model")):
                tmp.write(".model  " + model_name + "\n")
            else:
                tmp.write(line)
    tmp.close()
    #os.system("cp " + tmp + " " + command + "\n")
    tmp = open("temp__blif.blif", "r")
    with open(command, "w") as fp:
        for line in tmp:
            fp.write(line)
    tmp.close()


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

def setLib(command):
    # ensure the argument is valid
    command_list = command.split(" ")
    if(len(command_list)!=2):
        print("ERROR: read_library should have one argument")
        return
    for item in command_list:
        if("read_library" in item):
            command_list.remove(item)
    command = command_list[0]
    if ("45nm.genlib" not in command and "mcnc.genlib" not in command):
        print("ERROR: invalid filetype for set_lib. Options include \"45nm.genlib\" or \"mcnc.genlib\"""\n")
        return
    if ("45nm.genlib" in command):
        print("Setting technology library to 45nm.genlib...")
        command = "cp tech_lib/45nm.genlib tech_lib/techlib.genlib"
        os.system(command)
    if ("mcnc.genlib" in command):
        print("Setting technology library to mcnc.genlib...")
        command = "cp tech_lib/mcnc.genlib tech_lib/techlib.genlib"
        os.system(command)

