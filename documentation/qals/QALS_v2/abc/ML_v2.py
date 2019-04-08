from subprocess import *;
from random import *;
import os;

filename="c17.bench"
episodes=1000

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

# Create and initialize Q list according to the maximum permissible bit Error = 2^5 = 32 and +1 for exact matching
max_bit_error=32
perm_error=0.1
# use_exact=0 means mapping will be done using only approx gates
# use_exact=1 means mapping will be done using approx and exact gates
use_exact=1;
Q=list()
for i in range(int(nodes[0])):
   temp=[None]*(max_bit_error+1)
   Q.append(temp)
print(Q)

# Set Q list according to maximum permissible bit Error for nodes in this network
for i in range(int(nodes[0])):
   for j in range(int(nodes[i+1])+1): # +1 for exact matching
      if (j >= (max_bit_error+1)):
         break
      Q[i][j]=0
print(Q)

# If use_exact=0 make first index of every node to None
if(use_exact==0):
   for i in range(int(nodes[0])):
      if(Q[i][1] is not None):
         Q[i][0]=None

# Create list of permissible path between nodes
permissible_path=list();
for i in range(len(Q)):
   temp=list()
   for j in range(len(Q[i])):
      if Q[i][j] is not None:
         temp.append(j)
   permissible_path.append(temp)
print(permissible_path)

# Generate required conf file for error calculation
call(['python3','graph.py',filename])

for num in range(episodes):
   # Choose random path between nodes
   random_path=list()
   for i in range(len(permissible_path)):
      random_path.append(randint(min(permissible_path[i]),max(permissible_path[i])))
   print(random_path)
   
   # Write random path to a file
   fp = open('generated_result.txt', 'w')
   fp.write("%s\n" % len(random_path))
   for item in random_path:
      fp.write("%s\n" % item)
   fp.close()

   # Create dummy script file to perform synthesis using chosen bit Errors
   fp = open('dummy', 'w')
   fp.write("read mcnc.genlib\n")
   fp.write("read_bench "+filename+"\n")
   fp.write("map -l ")
   fp.write("%s " % len(random_path))
   for item in random_path:
      fp.write("%s " % item)
   fp.write("\nprint_stats -a")
   fp.close()
   fp = open("dummy.dump", "w")
   call(['./abc','-f','dummy'],stdout=fp)

   # Check if error has occured
   error_flag=0;
   area_old=100000000;
   if 'Error: Mapping has failed.' in open('dummy.dump').read():
      error_flag=1
   else:
      with open("dummy.dump") as fp:
         for line in fp:
            if "area=" in line:
               temp=line[5:-3]
               area_new=int(temp)
      # Error calculation
      call(['./error'])
      # Read Error from file
      with open('final_error.txt') as f:
         error = f.read().splitlines()
         error=float(error[0])

   # Update Q list according to the rewards achieved
   # Error in Mapping -10
   # Mapping successful +10
   # Mapping successful but area is more than previous one -3
   # Mapping successful and area is less than previous one +3
   # Error less than permissible error +10
   # Error more than permissible error -10
   alpha = 0.8;
   for i,j in zip(range(len(Q)),random_path):
         if (error_flag==0):
            if (error < perm_error):
               if (area_new < area_old):
                  area_old=area_new
                  Q[i][j]=Q[i][j]+alpha*(10+3+10)
               else:
                  Q[i][j]=Q[i][j]+alpha*(10-3+10)
            else:
               if (area_new < area_old):
                  area_old=area_new
                  Q[i][j]=Q[i][j]+alpha*(10+3-10)
               else:
                  Q[i][j]=Q[i][j]+alpha*(10-3-10)
         else:
            Q[i][j]=Q[i][j]+alpha*(-10)
print(Q)

# Final path from Q list
final_path=list()
for i in range(len(Q)):
   if(use_exact==0):
      if (Q[i][1] is not None):
         maxi=Q[i][1]
      else:
         maxi=Q[i][0]
   else:
      maxi=Q[i][0]
   for j in range(len(Q[i])):
      if (Q[i][j] is not None):
         if(Q[i][j] > maxi):
            maxi=Q[i][j]
   final_path.append(Q[i].index(maxi))
      
print(final_path)      

# Write final path to a file
fp = open('generated_result.txt', 'w')
fp.write("%s\n" % len(final_path))
for item in final_path:
   fp.write("%s\n" % item)
fp.close()

# Testing using final path 
fp = open('dummy', 'w')
fp.write("read mcnc.genlib\n")
fp.write("read_bench "+filename+"\n")
fp.write("map -l 0")
#fp.write("%s " % len(final_path))
#for item in final_path:
#   fp.write("%s " % item)
fp.write("\nprint_stats")
fp.close()
call(['./abc','-f','dummy'])
# Error calculation
call(['./error'])
# Read Error from file
with open('final_error.txt') as f:
   error = f.read().splitlines()
   error=float(error[0]) 
print('Final error = '+str(error))
