// Program to print BFS traversal from a given
// source vertex. BFS(int s) traverses vertices 
// reachable from s.
#include<iostream>
#include <list>
#include<fstream>

using namespace std;

struct Node{
    int name;
    string type;
    int output_num;
    double gate_error;
    double output_error;
    double prob_1s;
};

typedef struct Node Node;

// This class represents a directed graph using
// adjacency list representation
class Graph
{
	int V; // No. of vertices
    Node *nodes;
	// Pointer to an array containing adjacency
	// lists
	list<int> *adj; 
public:
	Graph(int V); // Constructor
    
    // Add data to Nodes
    void editNodes();
    void addAlledges();
	// function to add an edge to graph
	void addEdge(int v, int w); 

	// prints BFS traversal from a given source s
	void BFS(int s); 
	void printGraph();
	void setProb1s();
	void readGateError();
	void calculateError();
	double getOutputError(int);
	double getProb1s(int);
	void printFinalError();
};

void Graph::setProb1s(){
   for (int i=0;i<V;i++){
      int size=static_cast<int>(adj[i].size());
      if((nodes[i].type=="AND" || nodes[i].type=="NOR") && size==2)
         nodes[i].prob_1s=0.25; 
      if((nodes[i].type=="OR" || nodes[i].type=="NAND")&& size==2)
         nodes[i].prob_1s=0.75;
      if((nodes[i].type=="XOR" || nodes[i].type=="XNOR") && size==2)
         nodes[i].prob_1s=0.5;
      if(nodes[i].type=="INPUT")
         nodes[i].prob_1s=0.5;  
         
   }
}

void Graph::readGateError(){
   fstream ip;
   double error;
   ip.open("gate_error.txt");
   if (ip.is_open())
   {
     for(int i=0;i<V;i++){
         if(nodes[i].type!="INPUT" && nodes[i].type!="OUTPUT"){
            ip>>error;
            nodes[i].gate_error=error;
         }
         else{
            nodes[i].gate_error=0;
         }
     }
     ip.close();
   } 
   else cout << "Unable to open file";
}

void Graph::calculateError(){
   for(int i=0;i<V;i++){
      if(nodes[i].type!="INPUT" && nodes[i].type!="OUTPUT"){
         int size=static_cast<int>(adj[i].size());
         double e[adj[i].size()];
         double p[adj[i].size()];
         list<int>::iterator itr;
         int j=0;
         for(itr=adj[i].begin();itr!=adj[i].end();itr++){
            e[j]=getOutputError(*itr);
            p[j]=getProb1s(*itr);
            j++;
         }
         if((nodes[i].type=="AND" || nodes[i].type=="NAND") && size==2){
            nodes[i].output_error=nodes[i].gate_error+(1-2*nodes[i].gate_error)*(e[0]*p[1]+e[1]*p[0]+e[0]*e[1]*(1-2*(p[0]+p[1])+2*p[0]*p[1]));
         }
         if((nodes[i].type=="OR" || nodes[i].type=="NOR") && size==2){
            nodes[i].output_error=nodes[i].gate_error+(1-2*nodes[i].gate_error)*(e[0]*(1-p[1])+e[1]*(1-p[0])+e[0]*e[1]*(2*p[0]*p[1]-1));
         }
         if((nodes[i].type=="XOR" || nodes[i].type=="XNOR") && size==2){
            nodes[i].output_error=nodes[i].gate_error+(1-2*nodes[i].gate_error)*(e[0]+e[1]+2*e[0]*e[1]);
         }
         if(size>2){
            nodes[i].output_error=nodes[i].gate_error;
            for(int j=0;j<size;j++){
               if (e[j]>nodes[i].output_error)
                  nodes[i].output_error=e[j];
            }
         }
      }
      else{
         nodes[i].output_error=0;
      }
   }
   
   for(int i=0;i<V;i++){
      if(nodes[i].type=="OUTPUT")
         nodes[i].output_error=getOutputError(nodes[i].name);
   }
}

Graph::Graph(int V)
{
	this->V = V;
	adj = new list<int>[V];
	nodes= new Node[V];
}

void Graph::addAlledges(){
   fstream ip;
   int u,v;
   ip.open("type_nodes.txt");
   if (ip.is_open())
   {
     while(ip>>u>>v){
         addEdge(u,v);
     }
     ip.close();
   } 
   else cout << "Unable to open file";
}

double Graph::getOutputError(int n)
{
   for(int i=0;i<V;i++){
      if(nodes[i].name == n && nodes[i].type != "OUTPUT")
	      return nodes[i].output_error;
	}
}

double Graph::getProb1s(int n)
{
   for(int i=0;i<V;i++){
      if(nodes[i].name == n)
	      return nodes[i].prob_1s;
	}
}

void Graph::addEdge(int u, int v)
{
   for(int i=0;i<V;i++){
      if(nodes[i].name == v)
	      adj[i].push_back(u); // Add v to u’s list
	}
}

void Graph::editNodes()
{
   fstream ip;
   string type;
   int name;
   ip.open("type.txt");
   if (ip.is_open())
   {
     ip>>name;
     for(int i=0;i < V;i++){
      ip>>type;
      nodes[i].type=type;
      ip>>name;
      nodes[i].name=name;
     }
     ip.close();
   } 
   else cout << "Unable to open file";
}

void Graph::printGraph(){
   for (int v = 0; v < V; ++v)
    {
        cout << "\n Adjacency list of vertex "<< v << "\n head ";
        list<int>::iterator itr;
        for (itr=adj[v].begin();itr!=adj[v].end();itr++)
           cout << "-> " << *itr;
        printf("\n");
        cout<<"\n Node details:"<<"Name: "<<nodes[v].name<<"Type: "<<nodes[v].type<<endl;
    }
}

void Graph::printFinalError(){
    double final_error=1;
    for (int v = 0; v < V; ++v)
    {
        if(nodes[v].type=="OUTPUT" && nodes[v].output_error!=0){
            final_error*=nodes[v].output_error;
        }
        //cout<<"\nNode: "<<v<<"\t"<<nodes[v].output_error;
    }
    if(final_error == 1)
      final_error = 0;
    
    cout<<"Final error = "<<final_error<<endl;
    fstream op;
    op.open("final_error.txt",ios::out);
   if (op.is_open())
   {
     op<<final_error;
     op.close();
   }
   else cout << "Unable to open file";
}

void Graph::BFS(int s)
{
	// Mark all the vertices as not visited
	bool *visited = new bool[V];
	for(int i = 0; i < V; i++)
		visited[i] = false;

	// Create a queue for BFS
	list<int> queue;

	// Mark the current node as visited and enqueue it
	visited[s] = true;
	queue.push_back(s);

	// 'i' will be used to get all adjacent
	// vertices of a vertex
	list<int>::iterator i;

	while(!queue.empty())
	{
		// Dequeue a vertex from queue and print it
		s = queue.front();
		cout << s << " ";
		queue.pop_front();

		// Get all adjacent vertices of the dequeued
		// vertex s. If a adjacent has not been visited, 
		// then mark it visited and enqueue it
		for (i = adj[s].begin(); i != adj[s].end(); ++i)
		{
			if (!visited[*i])
			{
				visited[*i] = true;
				queue.push_back(*i);
			}
		}
	}
}

// Driver program to test methods of graph class
int main()
{
	// Create a graph given in the above diagram
	fstream ip;
	int num;
   ip.open("type.txt");
   if (ip.is_open())
   {
     ip>>num;
     Graph g(num);
     g.editNodes();
	g.addAlledges();
	g.readGateError();
	g.setProb1s();
	g.calculateError();
    // g.printGraph();
     g.printFinalError();
     ip.close();
   } 
   else cout << "Unable to open file";
	//Graph g(13);
	/*g.addEdge(0, 1);
	g.addEdge(0, 2);
	g.addEdge(1, 2);
	g.addEdge(2, 0);
	g.addEdge(2, 3);
	g.addEdge(3, 3);*/
	
	//cout << "Following is Breadth First Traversal "
		//<< "(starting from vertex 2) \n";
	//g.BFS(2);

	return 0;
}

