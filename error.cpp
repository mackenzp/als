// error.cpp
//
// modified MH 28 mar 2019, latest version implemented.
// MH: error formulas checked.
// MH: checking error >1 and <0
// set error = 0 if error < 0.001
// a warning is displayed 
// 
// Version 1.0:
// EE599 requirements reviewed and commented 
//


#include "error.h"

using namespace std;

//MH: adj[i].size() = no. of inputs fanin
void Netlist::setProb1s() {
  for (int i=0;i<V;i++) {
    int size=static_cast<int>(adj[i].size());    
    // 2-INPUT AND NOR 0.25      
    if((nodes[i].type=="and2" || nodes[i].type=="nor2") && size==2) {
      nodes[i].prob_1s=0.25;
      #ifdef DEBUG
      cout << "node i= " << i << " and2/nor2 prob_1s= 0.25\n";
      #endif
    }    
    // 2-INPUT OR NAND 0.75          
    if((nodes[i].type=="or2" || nodes[i].type=="nand2")&& size==2) {
      nodes[i].prob_1s=0.75;
      #ifdef DEBUG
      cout << "node i= " << i << " or2/nand2 prob_1s= 0.75\n";
      #endif
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
      #ifdef DEBUG
      cout << "node i= " << i << " xor2/xnor2 prob_1s= 0.5\n";
      #endif
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

// reads two Epsilon g per node:
// for 1 or 2 inputs, only the first value is valid, the second is 0.
// for 3 and 4 input gates, the second is valid as Epsilon for the first level.
void Netlist::readGateError(){
  fstream ip;
  double in;
  // Reads text file with gate error provided from the wrapper
  ip.open("gate_error.txt");
  if (ip.is_open()) {
    for(int i=0;i<V;i++){
      if(nodes[i].type != "INPUT" && nodes[i].type != "OUTPUT"){
        // Epsilon gate
        ip >> in;
        nodes[i].gate_error = in;
        // Epsilon gate 1 // set to 0, that is why next lines are commented.
        //    ip >> in;
        //   nodes[i].gate_error_1 = in;
        // e0, e1, e2, e3 for training
        // values are 0 0 0 0  0 0 0 0
        // if needed, uncomment and load e and p values
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
  else {
    #ifdef DEBUG  
    cout << "Unable to open gate_error.txt\n";
    #endif
  }
}

void Netlist::calculateError() {
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
        
        // 2-input gates        
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
            // comment out if arbitrary e and p are used
            //#ifdef DEBUG
            //cout << "e0 " << e0 << endl;
            //cout << "e1 " << e1 << endl;
            //cout << "p0 " << p0 << endl;
            //cout << "p1 " << p1 << endl;
            //cout << "gate error: " << nodes[i].gate_error << endl;
            
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) * (e0 + e1 + 2 * e0 * e1);
          }
        }
        
        // 3- input gates
        // nand3
        // MH: p of 1: 0.875 (7/8)
        //MH: done by steps: and2 cascading into a nand2
        // MH: we should keep pand2 fixed as the prob_of_1s of the exact node.
        // Mh: p1*E1 prob of 1 on input 1 *error on that input, so for the second level gate
        // MH: it should see p1 (from exact) * E1(BCEC from exact - approx)
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
            #ifdef DEBUG
            cout << "node[" << i << "].e0 = " << nodes[i].e0 << endl;
            cout << "node[" << i << "].e1 = " << nodes[i].e1 << endl;
            cout << "node[" << i << "].e2 = " << nodes[i].e2 << endl;
            cout << "node[" << i << "].e3 = " << nodes[i].e3 << endl;
            cout << "node[" << i << "].p0 = " << nodes[i].p0 << endl;
            cout << "node[" << i << "].p1 = " << nodes[i].p1 << endl;
            cout << "node[" << i << "].p2 = " << nodes[i].p2 << endl;
            cout << "node[" << i << "].p3 = " << nodes[i].p3 << endl;
            #endif
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
            #ifdef DEBUG
            cout << "node[" << i << "] eand2 = " << eand2 << endl;
            #endif
            // 2nd level nand2
            nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error) *
            (eand2 * p2 + e2 * pand2 + eand2 * e2 * (1 - 2 * (pand2 + p2) + 2 * pand2 * p2));
            
            pand2 = (1 - 2 * nodes[i].gate_error) *
            (eand2 * p2 + e2 * pand2 + eand2 * e2 * (1 - 2 * (pand2 + p2) + 2 * pand2 * p2));
            
            // comment out if arbitrary e and p are used
            /*
             *		nodes[i].output_error = nodes[i].gate_error + (1 - 2 * nodes[i].gate_error)*
             * ((e0*(1-e1)*(1-e2) * p1*p2) + (e1*(1-e0)*(1-e2)*p0*p2) +
             *                         (e2*(1-e0)*(1-e1)*p0*p1) + (e0*e1*e2*(p0*p1*p2 + (1-p0)*(1-p1)*(1-p2))) +
             *                          (e0*e1*(1-e2)*p2*(p0*p1 + (1-p0)*(1-p1))) + (e0*e2*(1-e1)*p1*(p0*p2 + (1-p0)*(1-p2))) +
             *                         (e1*e2*(1-e0)*p0*(p1*p2 + (1-p1)*(1-p2))));
             * //*/
             
             
             #ifdef DEBUG
             cout << "node[" << i << "]_output_error = " << nodes[i].output_error << endl;
             
             // comment out if arbitrary e and p are used
             //cout << "node[" << i << "]_ep eq = " << pand2 << endl;
             #endif            
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
            
            #ifdef DEBUG
            cout << "node[" << i << "].e0 = " << nodes[i].e0 << endl;
            cout << "node[" << i << "].e1 = " << nodes[i].e1 << endl;
            cout << "node[" << i << "].e2 = " << nodes[i].e2 << endl;
            cout << "node[" << i << "].e3 = " << nodes[i].e3 << endl;
            cout << "node[" << i << "].p0 = " << nodes[i].p0 << endl;
            cout << "node[" << i << "].p1 = " << nodes[i].p1 << endl;
            cout << "node[" << i << "].p2 = " << nodes[i].p2 << endl;
            cout << "node[" << i << "].p3 = " << nodes[i].p3 << endl;
            // comment out if arbitrary e and p are used
            //cout << "node[" << i << "].gate_error = " << nodes[i].gate_error << endl;
            //cout << "node[" << i << "].gate_error_1 = " << nodes[i].gate_error_1 << endl;
            #endif
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
        
        // 4-input gates     
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

Netlist::Netlist(int V) {
  this->V = V;
  adj = new list<string>[V];
  nodes= new Node[V];
}

void Netlist::addAlledges(){
  fstream ip;
  string u,v;
  ip.open("node_edges.txt");
  if (ip.is_open())
  {
    // to handle case zero gate 
    while(ip>>u>>v){
      if (u != "-1") 
        addEdge(u,v);
    }
    ip.close();
  } 
  else {
    #ifdef DEBUG
    cout << "Unable to open node_edges.txt\n";
    #endif
  }
}

double Netlist::getOutputError(string n) {
  for(int i=0;i<V;i++){
    if(nodes[i].name == n && nodes[i].type != "OUTPUT")
      return nodes[i].output_error;
  }
}

double Netlist::getProb1s(string n) {
  for(int i=0;i<V;i++){
    if(nodes[i].name == n)
      return nodes[i].prob_1s;
  }
}

void Netlist::addEdge(string u, string v) {
  for(int i=0;i<V;i++){
    if(nodes[i].name == v)
      adj[i].push_back(u); // Add v to u’s list
  }
}

void Netlist::editNodes() {
  fstream ip;
  string type;
  string name;
  //   ip.open("type.txt");
  ip.open("node_types.txt");
  if (ip.is_open()) {
    ip>>name;
    for(int i=0;i < V;i++){
      ip>>type;
      nodes[i].type=type;
      ip>>name;
      nodes[i].name=name;
    }
    ip.close();
  } 
  else { 
    #ifdef DEBUG
    cout << "Unable to open node_types.txt\n";
    #endif
  }
}

//friend function
void fprintNetlist(Netlist N){
//cout << "friend function" << endl;
N.printNetlist();
}

void Netlist::printNetlist(){
  for (int v = 0; v < V; ++v) {
    cout<<"\n Node details:"<<"Name: "<<nodes[v].name<<" Type: "<<nodes[v].type<<endl;
      cout << " node " << v << " type: " << nodes[v].type;
      cout << ", name: " << nodes[v].name;
      cout << ", gate_error: " << nodes[v].gate_error;
      cout << ", output_error: " << nodes[v].output_error << endl;
    }
    printf("\n");
  }

void Netlist::printFinalError(){
  double final_error=0;
  int num_outputs = 0;
  double max_error_output = 0;
  //cout << "\n";
  for (int v = 0; v < V; ++v)
  {
    if(nodes[v].type == "OUTPUT"){
      num_outputs++;
      #ifdef DEBUG
      cout << "\nOUTPUT node :"<< nodes[v].name;
      cout << ", name: " << nodes[v].name;
      cout << ", gate_error: " << nodes[v].gate_error;
      #endif
      if (nodes[v].output_error < 0.001) {
        #ifdef DEBUG
        cout << "Warning: ouput error rounded from " << nodes[v].output_error << " to 0.0" << endl;
        #endif
        nodes[v].output_error = 0;
      }
      #ifdef DEBUG
      cout << ", output_error: " << nodes[v].output_error << endl <<endl;
      #endif
    }
    else {
      #ifdef DEBUG
      cout << "node " << v << " type: " << nodes[v].type;
      cout << ", name: " << nodes[v].name;
      cout << ", gate_error: " << nodes[v].gate_error;
      cout << ", output_error: " << nodes[v].output_error << endl;
      #endif
    }
    
    if(nodes[v].type=="OUTPUT" && nodes[v].output_error!=0){
      final_error+=nodes[v].output_error;
      if (max_error_output < nodes[v].output_error)
        max_error_output = nodes[v].output_error;
      
    }
    #ifdef DEBUG
    cout<<"\nNode: "<<v<<"\t"<<nodes[v].output_error;
    #endif
  }
  
  final_error = final_error/num_outputs;
  #ifdef DEBUG
  cout<<"max_error_output = "<<max_error_output<<endl;  
  cout<<"Global MHD normalized = "<<final_error<<endl;
  #endif
  fstream op;
  op.open("final_error.txt",ios::out);
  if (op.is_open())
  {
    op<<final_error << " " << max_error_output << endl;
    op.close();
  }
  else {
    #ifdef DEBUG
    cout << "Unable to open final_error.txt\n";
    #endif
  }
}

//Netlist_Output::Netlist_Output(int V) {
//  this->V = V;
//  adj = new list<string>[V];
//  nodes= new Node[V];
//}

void Netlist::Output_Error(){
  fstream op1;
  op1.open("final_error_all_outputs.txt",ios::out);
  #ifdef DEBUG
  cout << "Output_Error: V : " << V << endl;
  #endif
  for (int v = 0; v < V; ++v) {
    if(nodes[v].type == "OUTPUT"){
      #ifdef DEBUG
      cout << "\nOUTPUT node :"<< nodes[v].name;
      cout << ", name: " << nodes[v].name;
      cout << ", gate_error: " << nodes[v].gate_error;
      cout << ", output_error: " << nodes[v].output_error << endl;
      #endif
      if (nodes[v].output_error < 0.001) {
        #ifdef DEBUG
        cout << "Warning: ouput error rounded from " << nodes[v].output_error << " to 0.0" << endl;
        #endif
        nodes[v].output_error = 0;
      }
      if (op1.is_open()) {
        op1<< "Node: " << nodes[v].name << " -> " << nodes[v].output_error << endl;
        
      } else {
        #ifdef DEBUG
        cout << "Unable to open final_error.txt\n";
        #endif
      }
    } 
  }
  op1.close();
}

// Driver program
int main(int argc, char** argv) {
  fstream ip;
  int num;
  int& r = num;
  bool print_all = 0;
  if (argc > 1)
    if(strcmp(argv[1],"-all") == 0)
      print_all = 1;
    ip.open("node_types.txt");
  if (ip.is_open()) {
      ip>>r;
    Netlist g(r);
    g.editNodes();
    g.addAlledges();
    g.readGateError();
    g.setProb1s();
    g.calculateError();
    #ifdef DEBUG
    fprintNetlist(g); //friend function
    //g.printNetlist();
    #endif
    if (print_all == 1)
      g.Output_Error();
    g.printFinalError();
    ip.close();
  } 
  else {
    #ifdef DEBUG
    cout << "Unable to open node_types.txt\n";
    #endif
    
  }
  return 0;
}

