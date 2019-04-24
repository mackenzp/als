// Program to print BFS traversal from a given
// source vertex. BFS(int s) traverses vertices 
// reachable from s.
//
// modified MH 28 mar 2019, latest version
// MH: checking error >1 and <0
// set error = 0 if error < 0.001
// a warning is displayed 

#include<iostream>
#include <list>
#include<fstream>
#include<string>

using namespace std;

struct Node{
  //int name;
  string name; // MH: to consider cases with v12.1    
  string type;
  int output_num;
  double gate_error; // case (1-input, 2-input gates): intrinsic Error on gate
  // case (3, 4 input gates): 2nd level Eg 
  double gate_error_1 = 0;// // case (1-input, 2-input): 0
  // case (3, 4 input gates): 1nd level Eg intrinsic Error on gates set to 0. 
  double output_error;
  double prob_1s;  // MH: just one value even for 3, 4 input gates
  //MH: Epsilon inputs i for training
  double e0;
  double e1;
  double e2;
  double e3; 
  //MH: Prob of 1s inputs i for training
  double p0;
  double p1;
  double p2;
  double p3; 
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
  list<string> *adj;
public:
  Graph(int V); // Constructor
  
  // Add data to Nodes
  void editNodes();
  void addAlledges();
  // function to add an edge to graph
  void addEdge(string v, string w);
  
  // prints BFS traversal from a given source s
  void BFS(string s);
  void printGraph();
  void setProb1s();
  void readGateError();
  void calculateError();
  //	double getOutputError(int);
  double getOutputError(string);
  //	double getProb1s(int);
  double getProb1s(string);
  void printFinalError();
};

//MH: adj[i].size() = no. of inputs fanin
void Graph::setProb1s(){
  for (int i=0;i<V;i++){
    int size=static_cast<int>(adj[i].size());
    
    // 2-INPUT AND NOR 0.25      
    if((nodes[i].type=="and2" || nodes[i].type=="nor2") && size==2) {
      nodes[i].prob_1s=0.25;
      //cout << "node i= " << i << " and2/nor2 prob_1s= 0.25\n";
    }
    
    // 2-INPUT OR NAND 0.75          
    if((nodes[i].type=="or2" || nodes[i].type=="nand2")&& size==2) {
      nodes[i].prob_1s=0.75;
      //cout << "node i= " << i << " or2/nand2 prob_1s= 0.75\n";
    }
    
    // 3-INPUT NAND 0.875 (7/8)
    if(nodes[i].type=="nand3"&& size==3)
      nodes[i].prob_1s=0.875;
    
    // 4-INPUT NAND 0.9375 (15/16)
    if(nodes[i].type=="nand4"&& size==4)
      nodes[i].prob_1s=0.9375;
    
    // 3-INPUT NOR 0.125 (1/8)
    if(nodes[i].type=="nor3"&& size==3)
      nodes[i].prob_1s=0.125;
    
    // 4-INPUT NOR 0.0625 (1/16)
    if(nodes[i].type=="nor4"&& size==4)
      nodes[i].prob_1s=0.0625;
    
    // 2-INPUT XOR XNOR 0.5
    if((nodes[i].type=="xor2" || nodes[i].type=="xnor2") && size==2) {
      nodes[i].prob_1s=0.5;
      //cout << "node i= " << i << " xor2/xnor2 prob_1s= 0.5\n";
    }
    // INPUT 0.5         
    if(nodes[i].type=="INPUT" )
      //MH: is going to be covered before getting into this
      //MH: in ABC, if the node is input, node name is na and we should check type 2 input 
      nodes[i].prob_1s=0.5;
    
    // INV BUF 0.5 
    if(nodes[i].type=="inv1" || nodes[i].type=="inv2" || nodes[i].type=="inv3" ||nodes[i].type=="inv4" ||nodes[i].type=="BUF1")
      nodes[i].prob_1s=0.5; 
    
    // Gate zero 0
    if(nodes[i].type=="zero")
      nodes[i].prob_1s=0.0;
    
    // Gate one 1
    if(nodes[i].type=="one")
      nodes[i].prob_1s=1.0;  
    
    // 3-INPUT AOI21 0.375 (3/8) 
    if((nodes[i].type=="aoi21")  && size==3)
      nodes[i].prob_1s=0.375;      
    
    // 4-INPUT AOI22 0.5625 (9/16) 
    if((nodes[i].type=="aoi22")  && size==4)
      nodes[i].prob_1s=0.5625;  
    
    // 3-INPUT OAI21 0.625 (5/8) 
    if((nodes[i].type=="oai21")  && size==3)
      nodes[i].prob_1s=0.625;  
    
    // 4-INPUT OAI22 0.4375 (7/16) 
    if((nodes[i].type=="oai22")  && size==4)
      nodes[i].prob_1s=0.4375;  
    
    
  }
}

// MH: modified, now reads two Epsilon g per node:
// for 1 or 2 inputs, only the first value is valid, the second is 0.
// for 3 and 4 input gates, the second is valid as Epsilon for the first level.
void Graph::readGateError(){
  fstream ip;
  double in;
  ip.open("gate_error.txt");
  if (ip.is_open())
  {
    for(int i=0;i<V;i++){
      if(nodes[i].type != "INPUT" && nodes[i].type != "OUTPUT"){
        //MH: Epsilon gate
        ip >> in;
        nodes[i].gate_error = in;
        // Epsilon gate 1 // set to 0, that is why is commented.
        //    ip >> in;
        //   nodes[i].gate_error_1 = in;
        // e0, e1, e2, e3 for training
        ip >> in;
        //nodes[i].e0 = in;
        ip >> in;
        //nodes[i].e1 = in;
        ip >> in;
        //nodes[i].e2 = in;
        ip >> in;
        //nodes[i].e3 = in;
        // p0, p1, p2, p3 for training 
        ip >> in;
        //nodes[i].p0 = in;
        ip >> in;
        //nodes[i].p1 = in;
        ip >> in;
        //nodes[i].p2 = in;
        ip >> in;
        //nodes[i].p3 = in;
      }
      
      else{
        nodes[i].gate_error=0;
      }
    }
    ip.close();
  } 
  //else //cout << "Unable to open gate_error_out.txt\n";
}




// func calculateError --------------------------------------------------
void Graph::calculateError() {
  double pand2, eand2, por2, eor2, pand21, eand21, por21, eor21;
  double e0, e1, e2, e3, p0, p1, p2, p3;
  
  for (int i = 0; i < V; i++) {
    
    if (nodes[i].type != "INPUT" && nodes[i].type != "OUTPUT") {
      int size = static_cast<int>(adj[i].size());
      double e[adj[i].size()];
      double p[adj[i].size()];
      list<string>::iterator itr;
      int j = 0;
      for (itr = adj[i].begin(); itr != adj[i].end(); itr++) {
        e[j] = getOutputError(*itr);
        p[j] = getProb1s(*itr);
        j++;
      }
      
      // 1- input gates ---------------------------------------------------------------- 
      // BUF1 inv1 inv2 inv3 inv4         
      if ((nodes[i].type == "BUF1" || nodes[i].type == "inv1" || nodes[i].type == "inv2" ||
        nodes[i].type == "inv3" || nodes[i].type == "inv4") && size == 1) {
        
        if (nodes[i].e0 == 0 )  //MH: this node do not have pi, Ei for training
          nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) * e[0];
        else {  // this node do have pi, Ei for training assigned by error_control.py
          e0 = nodes[i].e0;
          nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) * e0;	
        }
        }
        
        // 2-input gates ------------------------------------------------------------------         
        //AND NAND         
        else if((nodes[i].type=="and2" || nodes[i].type=="nand2") && size==2){
          
          
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0)  // this node do not have pi, Ei for training
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (e[0] * p[1] + e[1] * p[0] +  e[0] * e[1] * 
            (1 - 2 * (p[0] + p[1]) + 2 * p[0] * p[1]));
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (e0 * p1 + e1 * p0 +  e0 * e1 * 
            (1 - 2 * (p0 + p1) + 2 * p0 * p1));
          }
        }
        
        //OR NOR 
        else if((nodes[i].type=="or2" || nodes[i].type=="nor2") && size==2){
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 )  // this node do not have pi, Ei for training
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (e[0] * (1 - p[1]) + e[1] * 
            (1 - p[0]) + e[0] * e[1] * (2 * p[0] * p[1] - 1));
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (e0 * (1 - p1) + e1 * 
            (1 - p0) + e0 * e1 * (2 * p0 * p1 - 1));
          }
        }
        
        // XOR XNOR         
        //         if((nodes[i].type=="XOR" || nodes[i].type=="XNOR") && size==2){
        else if ((nodes[i].type == "xor2a" || nodes[i].type == "xor2b" || nodes[i].type == "xnor2a" || nodes[i].type == "xnor2b") && size == 2) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0)  // this node do not have pi, Ei for training
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) * (e[0] + e[1] - 2 * e[0] * e[1]);
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            
            //cout << "e0 " << e0 << endl;
            //cout << "e1 " << e1 << endl;
            //cout << "p0 " << p0 << endl;
            //cout << "p1 " << p1 << endl;
            //cout << "gate error: " << nodes[i].gate_error << endl;
            
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) * (e0 + e1 + 2 * e0 * e1);
          }
        }
        
        // 3- input gates ------------------------------------------------------------------------
        // nand3
        // MH: p of 1: 0.875 (7/8)
        //MH: done by steps: and2 cascading into a nand2
        // MH: we should keep pand2 fixed as the prob_of_1s of the exact node.
        // Mh: p1*E1 prob of 1 on input 1 *error on that input, so for the second level gate
        // MH: it sohuld see p1 (from exact) * E1(BCEC from exact - approx)
        //
        else if (nodes[i].type == "nand3" && size == 3) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0)  {// this node do not have pi, Ei for training
            // 1st level and2
            //cout << "e0 =0 node[" << i << "].e0 = " << nodes[i].e0 << endl;
            pand2 = 0.25;
            eand2 = nodes[i].gate_error_1 + (1 - 2 * nodes[i].gate_error_1) * 
            (e[0] * p[1] + e[1] * p[0] +  e[0] * e[1] * (1 - 2 * (p[0] + p[1]) + 2 * p[0] * p[1]));
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eand2 * p[2] + e[2] * pand2 + eand2 * e[2] * (1 - 2 * (pand2 + p[2]) + 2 * pand2 * p[2]));
          } 
          else {  // this node do have pi, Ei for training assigned by error_control.py
            //cout << "node[" << i << "].e0 = " << nodes[i].e0 << endl;
            //cout << "node[" << i << "].e1 = " << nodes[i].e1 << endl;
            //cout << "node[" << i << "].e2 = " << nodes[i].e2 << endl;
            //cout << "node[" << i << "].e3 = " << nodes[i].e3 << endl;
            //cout << "node[" << i << "].p0 = " << nodes[i].p0 << endl;
            //cout << "node[" << i << "].p1 = " << nodes[i].p1 << endl;
            //cout << "node[" << i << "].p2 = " << nodes[i].p2 << endl;
            //cout << "node[" << i << "].p3 = " << nodes[i].p3 << endl;
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            
            // 1st level and2
            pand2 = 0.25;
            //   pand2 = 0.0;
            eand2 = nodes[i].gate_error_1 + (1 - 2 * nodes[i].gate_error_1) * 
            (e0 * p1 + e1 * p0 +  e0 * e1 * (1 - 2 * (p0 + p1 + 2 * p0 * p1)));
            //cout << "node[" << i << "] eand2 = " << eand2 << endl;
            
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eand2 * p2 + e2 * pand2 + eand2 * e2 * (1 - 2 * (pand2 + p2) + 2 * pand2 * p2));
            
            pand2 = (1 - 2 * nodes[i].gate_error) *
            (eand2 * p2 + e2 * pand2 + eand2 * e2 * (1 - 2 * (pand2 + p2) + 2 * pand2 * p2));
            //*/
            /*
             *		nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error)*
             * ((e0*(1-e1)*(1-e2) * p1*p2) + (e1*(1-e0)*(1-e2)*p0*p2) +
             *                         (e2*(1-e0)*(1-e1)*p0*p1) + (e0*e1*e2*(p0*p1*p2 + (1-p0)*(1-p1)*(1-p2))) +
             *                          (e0*e1*(1-e2)*p2*(p0*p1 + (1-p0)*(1-p1))) + (e0*e2*(1-e1)*p1*(p0*p2 + (1-p0)*(1-p2))) +
             *                         (e1*e2*(1-e0)*p0*(p1*p2 + (1-p1)*(1-p2))));
             * //*/
             
             
             
             //cout << "node[" << i << "]_output_error = " << nodes[i].output_error << endl;
             //cout << "node[" << i << "]_ep eq = " << pand2 << endl;
             
          }
        }
        
        // nor3
        // MH: p of 1: 0.125 (1/8)
        //MH: done by steps: or2 cascading into a nor2
        //
        else if (nodes[i].type == "nor3" && size == 3) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0)  {// this node do not have pi, Ei for training
            // 1st level or2
            por2 = 0.75;
            eor2 = (1 - 2 * nodes[i].gate_error_1) * (e[0] * (1 - p[1]) + e[1] * (1 - p[0]) + e[0] *			 e[1] * (2 * p[0] * p[1] - 1));
            // 2nd level nor2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eor2 * (1 - p[2]) + e[2] * (1 - por2) +  eor2 * e[2] * (2 * por2 * p[2] - 1));
          }
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            
            //cout << "node[" << i << "].e0 = " << nodes[i].e0 << endl;
            //cout << "node[" << i << "].e1 = " << nodes[i].e1 << endl;
            //cout << "node[" << i << "].e2 = " << nodes[i].e2 << endl;
            //cout << "node[" << i << "].e3 = " << nodes[i].e3 << endl;
            //cout << "node[" << i << "].p0 = " << nodes[i].p0 << endl;
            //cout << "node[" << i << "].p1 = " << nodes[i].p1 << endl;
            //cout << "node[" << i << "].p2 = " << nodes[i].p2 << endl;
            //cout << "node[" << i << "].p3 = " << nodes[i].p3 << endl;
            
            //cout << "node[" << i << "].gate_error = " << nodes[i].gate_error << endl;
            //cout << "node[" << i << "].gate_error_1 = " << nodes[i].gate_error_1 << endl;
            
            /*
             * // 1st level or2
             *                por2 = 0.75;
             *                eor2 = (1 - 2 * nodes[i].gate_error_1) * (e0 * (1 - p1) + e1 * (1 - p0) + e[0] *			 e1 * (2 * p0 * p1 - 1));
             * // 2nd level nor2
             *                nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
             *                (eor2 * (1 - p2) + e2 * (1 - por2) +  eor2 * e2 * (2 * por2 * p2 - 1));
             * //*/
             
             nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
             ((e0*(1-e1)*(1-e2)*(1-p1)*(1-p2)) + (e1*(1-e0)*(1-e2)*(1-p0)*(1-p2)) + (e2*(1-e0)*(1-e1)*(1-p0)*(1-p1)) +
             (e0*e1*e2*(p0*p1*p2 + (1-p0)*(1-p1)*(1-p2))) + (e0*e1*(1-e2)*(1-p2)*(p0*p1 + (1-p0)*(1-p1))) +
             (e0*e2*(1-e1)*(1-p1)*(p0*p2 + (1-p0)*(1-p2))) + (e1*e2*(1-e0)*(1-p0)*(p1*p2 + (1-p1)*(1-p2))));
             
             //*/
          }
        }
        
        // aoi21
        // MH: p of 1: 0.375 (3/8)
        //MH: done by steps: or2 cascading into a nor2
        //
        else if (nodes[i].type == "aoi21" && size == 3) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0)  {// this node do not have pi, Ei for training
            // 1st level and2
            pand2 = 0.25;
            eand2 = (1 - 2 * nodes[i].gate_error_1) * (e[0] * p[1] + e[1] * p[0] +  
            e[0] * e[1] * (1 - 2 * (p[0] + p[1]) + 2 * p[0] * p[1]));
            // 2nd level nor2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eand2 * (1 - p[2]) + e[2] * (1 - pand2) +  eand2 * e[2] * (2 * pand2 * p[2] - 1));
          }
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            // 1st level and2
            pand2 = 0.25;
            eand2 = (1 - 2 * nodes[i].gate_error_1) * (e0 * p1 + e1 * p0 +  
            e0 * e1 * (1 - 2 * (p0 + p1) + 2 * p0 * p1));
            // 2nd level nor2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eand2 * (1 - p2 + e2 * (1 - pand2) +  eand2 * e2 * (2 * pand2 * p2 - 1)));
          }
        }
        
        // oai21
        // MH: p of 1: 0.625 (5/8)
        //MH: done by steps: or2 cascading into a nor2
        //
        else if (nodes[i].type == "oai" && size == 3) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0)  {// this node do not have pi, Ei for training
            // 1st level or2
            por2 = 0.75;
            eor2 = (1 - 2 * nodes[i].gate_error_1) *
            (e[0] * (1 - p[1]) + e[1] * (1 - p[0]) + e[0] * e[1] * (2 * p[0] * p[1] - 1));
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eor2 * p[2] + e[2] * por2 + eor2 * e[2] * (1 - 2 * (por2 + p[2]) + 2 * por2 * p[2]));
          }
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            // 1st level or2
            por2 = 0.75;
            eor2 = (1 - 2 * nodes[i].gate_error_1) *
            (e0 * (1 - p1) + e1 * (1 - p0) + e0 * e1 * (2 * p0 * p1 - 1));
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eor2 * p2 + e2 * por2 + eor2 * e2 * (1 - 2 * (por2 + p2) + 2 * por2 * p2));
          }
        }
        
        
        // 4-input gates -------------------------------------------------------------------
        
        // nand4
        // MH: p of 1: 0.9375 (15/16)
        //MH: done by steps: two and2 cascading into a nand2
        
        //
        else if (nodes[i].type == "nand4" && size == 4) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0 && nodes[i].e3 == 0)  {// this node do not have pi, Ei for training
            // 1st level and2
            pand2 = 0.25;
            pand21 = 0.25;
            eand2 = (1 - 2 * nodes[i].gate_error_1) *
            (e[0] * p[1] + e[1] * p[0] + e[0] * e[1] * (1 - 2 * (p[0] + p[1]) + 2 * p[0] * p[1]));
            eand21 = (1 - 2 * nodes[i].gate_error_1) *
            (e[2] * p[3] + e[3] * p[2] + e[2] * e[3] * (1 - 2 * (p[2] + p[3]) + 2 * p[2] * p[3]));
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eand2 * pand21 + eand21 * pand2 + eand2 * eand21 * (1 - 2 * (pand2 + pand21) + 2 * pand2 * pand21));
          }
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p3 = nodes[i].e3;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            p3 = nodes[i].p3;
            // 1st level and2
            pand2 = 0.25;
            pand21 = 0.25;
            eand2 = (1 - 2 * nodes[i].gate_error_1) *
            (e0 * p1 + e1 * p0 + e0 * e1 * (1 - 2 * (p0 + p1) + 2 * p0 * p1));
            eand21 = (1 - 2 * nodes[i].gate_error_1) *
            (e2 * p3 + e3 * p2 + e2 * e3 * (1 - 2 * (p2 + p3) + 2 * p2 * p3));
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eand2 * pand21 + eand21 * pand2 + eand2 * eand21 * 
            (1 - 2 * (pand2 + pand21) + 2 * pand2 * pand21));
          }
        }
        
        // nor4
        // MH: p of 1: 0.0625 (1/16)
        //MH: done by steps: two or2 cascading into a nor2
        
        else if (nodes[i].type == "nor4" && size == 4) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0 && nodes[i].e3 == 0)  {// this node do not have pi, Ei for training
            // 1st level or2
            por2 = 0.75;
            por21 = 0.75;
            eor2 = (1 - 2 * nodes[i].gate_error_1) *
            (e[0] * (1 - p[1]) + e[1] * (1 - p[0]) + e[0] * e[1] * (2 * p[0] * p[1] - 1));
            eor21 = (1 - 2 * nodes[i].gate_error_1) *
            (e[2] * (1 - p[3]) + e[3] * (1 - p[2]) + e[2] * e[3] * (2 * p[2] * p[3] - 1));
            // 2nd level nor2
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (eor2* (1 - por2) + eor21  * (1 - por2) + eor2  * eor21 *
            (2 * por2 * por21  - 1));
          }
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p3 = nodes[i].e3;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            p3 = nodes[i].p3;
            // 1st level or2
            por2 = 0.75;
            por21 = 0.75;
            eor2 = (1 - 2 * nodes[i].gate_error_1) *
            (e0 * (1 - p1) + e1 * (1 - p0) + e0 * e1 * (2 * p0 * p1 - 1));
            eor21 = (1 - 2 * nodes[i].gate_error_1) *
            (e2 * (1 - p3) + e3 * (1 - p2) + e2 * e3 * (2 * p2 * p3 - 1));
            // 2nd level nor2
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (eor2* (1 - por2) + eor21  * 
            (1 - por2) + eor2  * eor21 * (2 * por2 * por21  - 1));
          }
        }
        
        // aoi 22
        // MH: p of 1: 0.5625 (9/16)
        //MH: done by steps: two or2 cascading into a nor2
        
        else if (nodes[i].type == "nor4" && size == 4) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0 && nodes[i].e3 == 0)  {// this node do not have pi, Ei for training
            // 1st level and2
            pand2 = 0.25;
            pand21 = 0.25;
            eand2 = (1 - 2 * nodes[i].gate_error_1) *
            (e[0] * p[1] + e[1] * p[0] + e[0] * e[1] * (1 - 2 * (p[0] + p[1]) + 2 * p[0] * p[1]));
            eand21 = (1 - 2 * nodes[i].gate_error_1) *
            (e[2] * p[3] + e[3] * p[2] + e[2] * e[3] * (1 - 2 * (p[2] + p[3]) + 2 * p[2] * p[3]));
            // 2nd level nor2
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (eand2* (1 - pand2) + eand21  * 
            (1 - pand2) + eand2 * eand21 * (2 * pand2 * pand21  - 1)); 
          }
          
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p3 = nodes[i].e3;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            p3 = nodes[i].p3;
            // 1st level and2
            pand2 = 0.25;
            pand21 = 0.25;
            eand2 = (1 - 2 * nodes[i].gate_error_1) *
            (e0 * p1 + e1 * p0 + e0 * e1 * (1 - 2 * (p0 + p1) + 2 * p0 * p1));
            eand21 = (1 - 2 * nodes[i].gate_error_1) *
            (e2 * p3 + e3 * p2 + e2 * e3 * (1 - 2 * (p2 + p3) + 2 * p2 * p3));
            // 2nd level nor2
            nodes[i].output_error = nodes[i].gate_error + 
            (1 - 2 * nodes[i].gate_error) * (eand2* (1 - pand2) + eand21  * 
            (1 - pand2) + eand2 * eand21 * (2 * pand2 * pand21  - 1)); 
          }
        }
        
        // oai 22
        // MH: p of 1: 0.5625 (9/16)
        //MH: done by steps: two or2 cascading into a nor2
        
        else if (nodes[i].type == "nor4" && size == 4) {
          if (nodes[i].e0 == 0 && nodes[i].e1 == 0 && nodes[i].e2 == 0 && nodes[i].e3 == 0)  {// this node do not have pi, Ei for training
            // 1st level or2
            por2 = 0.75;
            por21 = 0.75;
            eor2 = (1 - 2 * nodes[i].gate_error_1) *
            (e[0] * (1 - p[1]) + e[1] * (1 - p[0]) + e[0] * e[1] * (2 * p[0] * p[1] - 1));
            eor21 = (1 - 2 * nodes[i].gate_error_1) *
            (e[2] * (1 - p[3]) + e[3] * (1 - p[2]) + e[2] * e[3] * (2 * p[2] * p[3] - 1));
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eor2 * por21 + eor21 * por2 + eor2 * eor21 * (1 - 2 * (por2 + por21) + 
            2 * por2 * por21));
          }
          else {  // this node do have pi, Ei for training assigned by error_control.py
            e0 = nodes[i].e0;
            e1 = nodes[i].e1;
            e2 = nodes[i].e2;
            p3 = nodes[i].e3;
            p0 = nodes[i].p0;
            p1 = nodes[i].p1;
            p2 = nodes[i].p2;
            p3 = nodes[i].p3;
            // 1st level or2
            por2 = 0.75;
            por21 = 0.75;
            eor2 = (1 - 2 * nodes[i].gate_error_1) *
            (e0 * (1 - p1) + e1 * (1 - p0) + e0 * e1 * (2 * p0 * p1 - 1));
            eor21 = (1 - 2 * nodes[i].gate_error_1) *
            (e2 * (1 - p3) + e3 * (1 - p2) + e2 * e3 * (2 * p2 * p3 - 1));
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eor2 * por21 + eor21 * por2 + eor2 * eor21 * (1 - 2 * (por2 + por21) + 
            2 * por2 * por21));
          }
        }
        
        
        // GATE zero one set to 0 by default
        else if ((nodes[i].type == "zero" || nodes[i].type == "one")) {
          nodes[i].output_error = nodes[i].gate_error;
        }
        
        else {
          nodes[i].output_error = 0;
        }
        
        for (int i = 0; i < V; i++) {
          if (nodes[i].type == "OUTPUT")
            nodes[i].output_error = getOutputError(nodes[i].name);
        }
        
    }
  }
}
// ----------------------------------------------------------------------------





//Graph::Graph(int V) {
Graph::Graph(int V) {
  this->V = V;
  adj = new list<string>[V];
  nodes= new Node[V];
}

void Graph::addAlledges(){
  fstream ip;
  string u,v;
  //   ip.open("type_nodes.txt");
  ip.open("node_edges.txt");
  if (ip.is_open())
  {
    
    while(ip>>u>>v){
      if (u != "-1") // to handle case zero gate 
        addEdge(u,v);
    }
    ip.close();
  } 
  //else //cout << "Unable to open node_edges.txt\n";
}

double Graph::getOutputError(string n)
{
  for(int i=0;i<V;i++){
    if(nodes[i].name == n && nodes[i].type != "OUTPUT")
      return nodes[i].output_error;
  }
}

double Graph::getProb1s(string n)
{
  for(int i=0;i<V;i++){
    if(nodes[i].name == n)
      return nodes[i].prob_1s;
  }
}

void Graph::addEdge(string u, string v)
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
  string name;
  //   ip.open("type.txt");
  ip.open("node_types.txt");
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
  //else //cout << "Unable to open node_types.txt\n";
}

void Graph::printGraph(){
  for (int v = 0; v < V; ++v)
  {
    //cout << "\n Adjacency list of vertex "<< v << "\n head ";
    //list<int>::iterator itr;
    //for (itr=adj[v].begin();itr!=adj[v].end();itr++)
    //   //cout << "-> " << *itr;
    for (int i = 0; i < adj[v].size(); i++){
      //cout << "-> " << &adj[v];
    }
    printf("\n");
    //cout<<"\n Node details:"<<"Name: "<<nodes[v].name<<"Type: "<<nodes[v].type<<endl;
  }
}

void Graph::printFinalError(){
  double final_error=0;
  int num_outputs = 0;
  double max_error_output = 0;
  //cout << "\n";
  for (int v = 0; v < V; ++v)
  {
    if(nodes[v].type == "OUTPUT"){
      num_outputs++;
      //cout << "\nOUTPUT node :"<< nodes[v].name;
      ////cout << ", name: " << nodes[v].name;
      ////cout << ", gate_error: " << nodes[v].gate_error;
      if (nodes[v].output_error < 0.001) {
        //cout << "Warning: ouput error rounded from " << nodes[v].output_error << " to 0.0" << endl;
        nodes[v].output_error = 0;
      }
      //cout << ", output_error: " << nodes[v].output_error << endl <<endl;
    }
    else {
      
      //cout << "node " << v << " type: " << nodes[v].type;
      //cout << ", name: " << nodes[v].name;
      //cout << ", gate_error: " << nodes[v].gate_error;
      //cout << ", output_error: " << nodes[v].output_error << endl;
    }
    
    if(nodes[v].type=="OUTPUT" && nodes[v].output_error!=0){
      final_error+=nodes[v].output_error;
      if (max_error_output < nodes[v].output_error)
        max_error_output = nodes[v].output_error;
      
    }
    
    ////cout<<"\nNode: "<<v<<"\t"<<nodes[v].output_error;
  }
  
  final_error = final_error/num_outputs;
  //cout<<"max_error_output = "<<max_error_output<<endl;  
  //cout<<"Global MHD normalized = "<<final_error<<endl;
  fstream op;
  op.open("final_error.txt",ios::out);
  if (op.is_open())
  {
    op<<final_error << " " << max_error_output << endl;
    op.close();
  }
  //else //cout << "Unable to open final_error.txt\n";
}


// Driver program to test methods of graph class
int main() {
  // Create a graph given in the above diagram
  fstream ip;
  int num;
  ip.open("node_types.txt");
  if (ip.is_open()) {
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
  else //cout << "Unable to open node_types.txt\n";
  
  
  return 0;
}

