// error.h
// header for error.cpp
// MH april 25 2019
//
// version 1.0
// EE599 requirements reviewed and commented
//

#ifndef ERROR_H
#define ERROR_H

#include<iostream>
#include <list>
#include<fstream>
#include<string.h>

using namespace std;

class Node {
public:
  string name;   
  string type;
  int output_num;
  // case (1-input, 2-input gates): intrinsic Error on gate
  double gate_error; 
  // case (3, 4 input gates): 2nd level Eg 
  double gate_error_1 = 0;// // case (1-input, 2-input): 0
  // case (3, 4 input gates): 1nd level Eg intrinsic Error on gates set to 0. 
  double output_error;
  
  // MH: just one value even for 3, 4 input gates  
  double prob_1s;
  
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

class Graph {
public:
  virtual void editNodes() = 0;
  virtual void addAlledges() = 0;
  virtual void printNetlist() = 0;
  virtual void addEdge(string v, string w) = 0;
  virtual void setProb1s() = 0;
  virtual void readGateError() = 0;
  virtual void calculateError() = 0;
  virtual double getOutputError(string) = 0; 
  virtual double getProb1s(string) = 0;
  virtual void printFinalError() = 0;
  virtual void Output_Error() = 0;
};

// Netlist class
// This class represents a directed Netlist using
// adjacency list representation
class Netlist : public Graph{
public:
  int V;
  // composition: class Netlist has Nodes
  Node *nodes;
  // Pointer to an array containing adjacency lists
  list<string> *adj;
  
  Netlist(int V);
  void editNodes();
  void addAlledges();
  int ReturnV();
// friend function
  friend void fprintNetlist(Netlist);
  void printNetlist();
  void addEdge(string v, string w);
  void setProb1s();
  void readGateError();
  void calculateError();
  double getOutputError(string); 
  double getProb1s(string);
  inline void printFinalError();
  inline void Output_Error();
  //virtual void Output_Error() = 0;
};

class Node_Output : public Node {
public:
  //virtual void Output_Error() = 0;
};


#endif
