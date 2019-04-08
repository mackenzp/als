import sys

filename=sys.argv[1]
in1=open(filename,'r')
out1=open('cmd1.txt','w')
line1=''
for line in in1:
    if line == '\n':
        continue
    line1=line1+line.rstrip()+'\n'
in1.close()
out1.write(line1)
out1.close()
in2=open('cmd1.txt','r')
out2=open('type_nodes.txt','w')
input=in2.readlines()

input_line = input[1].split()
output_line = input[2].split()
inv_line = input[3].split()
gates_line = input[4].split()
nodes=list()
gate_type=list()
vertices = int(input_line[1])+int(output_line[1])+int(inv_line[1])+int(gates_line[1])
jump = int(input_line[1])+int(output_line[1])
start_index = 5 + jump
while start_index < len(input):
   edge_list = input[start_index].split()
   to_point = int(edge_list[0])
   
   if to_point not in nodes:
      nodes.append(to_point)
      
   
   if len(edge_list) ==3:
      from_list11 = edge_list[2].split('(')
      from_list1 = from_list11[1].split(')')
      from_point = int(from_list1[0])
      
      if from_point not in nodes:
         nodes.append(from_point)
      gate_type.append(from_list11[0])
      #print(from_point,to_point)
      out2.write(str(from_point) +'\t'+str(to_point)+'\n')
      #g.addEdge(from_point,to_point)
     
   for j in range(2,len(edge_list)-1):
      if j==2:
         
         from_list1 = edge_list[j].split('(')
          
         from_list_final = from_list1[1].split(',')  
         
         from_point = int(from_list_final[0]) 
         if from_point not in nodes:
            nodes.append(from_point)
            
         gate_type.append(from_list1[0])
         #print(from_point)
         #print(from_point,to_point)
         out2.write(str(from_point) +'\t'+str(to_point)+'\n')
         #g.addEdge(from_point,to_point)
         j=j+1
         continue
         
      from_list1 = edge_list[j].split(',')
      from_point = int(from_list1[0])
      #print(from_point)
      #print(from_point,to_point)
      if from_point not in nodes:
         nodes.append(from_point)
         gate_type.append(from_list1[0])
      out2.write(str(from_point) +'\t'+str(to_point)+'\n')
      
      j=j+1   
   if len(edge_list) > 3:
      from_list1 = edge_list[len(edge_list)-1].split(')') 
      from_point = int(from_list1[0])
      #print(from_point,to_point)
      if from_point not in nodes:
         nodes.append(from_point)
      out2.write(str(from_point) +'\t'+str(to_point)+'\n')
      
   start_index= start_index+1

in2.close()
out2.close()
primary_nodes=list()
out3 = open('type.txt','w')
out3.write(str(vertices)+'\n')
for i in range(5,5+jump):
   type_node = input[i].split('(')
   node_number = type_node[1].split(')')
   
   out3.write(type_node[0]+'\t'+node_number[0]+'\n')
   
           
     
   primary_nodes.append(int(node_number[0]))
#print(nodes)  
#print(primary_nodes) 
#print(gate_type)
k=0
for i in range(0,len(nodes)):
   
   if nodes[i] not in primary_nodes:
      
      out3.write(gate_type[k]+'\t'+str(nodes[i])+'\t'+'\n')
      k=k+1
k= int(output_line[1])      
for i in range(int(input_line[1]),int(input_line[1])+int(output_line[1])):
   out3.write(gate_type[len(gate_type)-k]+'\t'+str(primary_nodes[i])+'\t'+'\n')  
   k=k-1          
