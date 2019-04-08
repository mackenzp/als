from subprocess import *;
from random import *;
import os;

filename="Networks/c17.bench"

# Get Number of Nodes and maximum bit difference for each node
fp = open('dummy', 'w')
temp=["read mcnc.genlib","read_bench "+filename,"map"]
for item in temp:
   fp.write("%s\n" % item)
fp.close()
call(['./abc','-f','dummy'])

# Read file nodes.txt into a list
with open('nodes.txt') as f:
   nodes = f.read().splitlines()

# Read file final_result.txt into a list
with open('final_result.txt') as f:
   final_path = f.read().splitlines()

final_path=final_path[:int(nodes[0])]

# Testing using final path 
fp = open('dummy', 'w')
fp.write("read mcnc.genlib\n")
fp.write("read_bench "+filename+"\n")
fp.write("map -l ")
fp.write("%s " % len(final_path))
for item in final_path:
   fp.write("%s " % item)
fp.write("\nprint_stats")
fp.close()
call(['./abc','-f','dummy']) 
   

