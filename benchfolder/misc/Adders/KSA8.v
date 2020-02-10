//============================================================================//
//                           8-bit Kogge-Stone Adder                          //
//                                                                            //
//                       Naveen Katam and Massoud Pedram                      //
//     SPORT Lab, University of Southern California, Los Angeles, CA 90089    //
//                          http://sportlab.usc.edu/                          //
//                                                                            //
// For licensing, please refer to the LICENSE file in the main directory.     //
//============================================================================//

module KSA8_new (a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7, cin, sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, cout);

input a0, a1, a2, a3, a4, a5, a6, a7, b0, b1, b2, b3, b4, b5, b6, b7, cin;
output sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, cout;

wire p0, p1, p2, p3, p4, p5, p6, p7, g0, g1, g2, g3, g4, g5, g6, g7, 
	 cp0, cp1, cp2, cp3, cp4, cp5, cp6, cp7, cg0, cg1, cg2, cg3, cg4, cg5, cg6, cg7, 
	 ccp0, ccp1, ccp2, ccp3, ccp4, ccp5, ccp6, ccp7, ccg0, ccg1, ccg2, ccg3, ccg4, ccg5, ccg6, ccg7,
	 cccp0, cccp1, cccp2, cccp3, cccp4, cccp5, cccp6, cccp7, cccg0, cccg1, cccg2, cccg3, cccg4, cccg5, cccg6, cccg7,
	 c0, c1, c2, c3, c4, c5, c6, c7;

//--------------------------------------------------//
assign p0 = a0 ^ b0;
assign g0 = a0 & b0;

assign p1 = a1 ^ b1;
assign g1 = a1 & b1;

assign p2 = a2 ^ b2;
assign g2 = a2 & b2;

assign p3 = a3 ^ b3;
assign g3 = a3 & b3;

assign p4 = a4 ^ b4;
assign g4 = a4 & b4;

assign p5 = a5 ^ b5;
assign g5 = a5 & b5;

assign p6 = a6 ^ b6;
assign g6 = a6 & b6;

assign p7 = a7 ^ b7;
assign g7 = a7 & b7;
//--------------------------------------------------//
assign cg0 = (p0 & cin) | g0;
assign cp0 = p0;

assign cg1 = (p1 & g0) | g1;
assign cp1 = p1 & p0;

assign cg2 = (p2 & g1) | g2;
assign cp2 = p2 & p1;

assign cg3 = (p3 & g2) | g3;
assign cp3 = p3 & p2;

assign cg4 = (p4 & g3) | g4;
assign cp4 = p4 & p3;

assign cg5 = (p5 & g4) | g5;
assign cp5 = p5 & p4;

assign cg6 = (p6 & g5) | g6;
assign cp6 = p6 & p5;

assign cg7 = (p7 & g6) | g7;
assign cp7 = p7 & p6;
//--------------------------------------------------//
assign ccg0 = cg0;
assign ccp0 = cp0;

assign ccg1 = (cp1 & cin) | cg1;
assign ccp1 = cp1;

assign ccg2 = (cp2 & cg0) | cg2;
assign ccp2 = cp2 & cp0;

assign ccg3 = (cp3 & cg1) | cg3;
assign ccp3 = cp3 & cp1;

assign ccg4 = (cp4 & cg2) | cg4;
assign ccp4 = cp4 & cp2;

assign ccg5 = (cp5 & cg3) | cg5;
assign ccp5 = cp5 & cp3;

assign ccg6 = (cp6 & cg4) | cg6;
assign ccp6 = cp6 & cp4;

assign ccg7 = (cp7 & cg5) | cg7;
assign ccp7 = cp7 & cp5;
//--------------------------------------------------//
assign cccg0 = ccg0;
assign cccp0 = ccp0;

assign cccg1 = ccg1;
assign cccp1 = ccp1;

assign cccg2 = ccg2;
assign cccp2 = ccp2;

assign cccg3 = (ccp3 & cin) | ccg3;
assign cccp3 = ccp3;

assign cccg4 = (ccp4 & ccg0) | ccg4;
assign cccp4 = ccp4 & ccp0;

assign cccg5 = (ccp5 & ccg1) | ccg5;
assign cccp5 = ccp5 & ccp1;

assign cccg6 = (ccp6 & ccg2) | ccg6;
assign cccp6 = ccp6 & ccp2;

assign cccg7 = (ccp7 & ccg3) | ccg7;
assign cccp7 = ccp7 & ccp3;
//--------------------------------------------------//
assign c0 = cccg0;
assign c1 = cccg1;
assign c2 = cccg2;
assign c3 = cccg3;
assign c4 = cccg4;
assign c5 = cccg5;
assign c6 = cccg6;
assign c7 = cccg7;

assign sum0 = p0 ^ cin;
assign sum1 = p1 ^ c0;
assign sum2 = p2 ^ c1;
assign sum3 = p3 ^ c2;
assign sum4 = p4 ^ c3;
assign sum5 = p5 ^ c4;
assign sum6 = p6 ^ c5;
assign sum7 = p7 ^ c6;
assign cout = (cccp7 & cin) | cccg7;
//--------------------------------------------------//
endmodule
