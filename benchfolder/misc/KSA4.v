//============================================================================//
//                           4-bit Kogge-Stone Adder                          //
//                                                                            //
//                       Naveen Katam and Massoud Pedram                      //
//     SPORT Lab, University of Southern California, Los Angeles, CA 90089    //
//                          http://sportlab.usc.edu/                          //
//                                                                            //
// For licensing, please refer to the LICENSE file in the main directory.     //
//============================================================================//

module KSA4_new (a0, a1, a2, a3, b0, b1, b2, b3, cin, sum0, sum1, sum2, sum3, cout);

input a0, a1, a2, a3, b0, b1, b2, b3, cin;
output sum0, sum1, sum2, sum3, cout;

wire p0, p1, p2, p3, g0, g1, g2, g3, cp0, cp1, cp2, cp3, cg0, cg1, cg2, cg3, ccg0, ccg1, ccg2, ccg3, ccp0, ccp1, ccp2, ccp3, c0, c1, c2, c3;
//--------------------------------------------------//
assign p0 = a0 ^ b0;
assign g0 = a0 & b0;

assign p1 = a1 ^ b1;
assign g1 = a1 & b1;

assign p2 = a2 ^ b2;
assign g2 = a2 & b2;

assign p3 = a3 ^ b3;
assign g3 = a3 & b3;
//--------------------------------------------------//
assign cg0 = (p0 & cin) | g0;
assign cp0 = p0;

assign cg1 = (p1 & g0) | g1;
assign cp1 = p1 & p0;

assign cg2 = (p2 & g1) | g2;
assign cp2 = p2 & p1;

assign cg3 = (p3 & g2) | g3;
assign cp3 = p3 & p2;
//--------------------------------------------------//
assign ccg0 = cg0;
assign ccp0 = cp0;

assign ccg1 = (cp1 & cin) | cg1;
assign ccp1 = cp1;

assign ccg2 = (cp2 & cg0) | cg2;
assign ccp2 = cp2 & cp0;

assign ccg3 = (cp3 & cg1) | cg3;
assign ccp3 = cp3 & cp1;
//--------------------------------------------------//
assign c0 = ccg0;
assign c1 = ccg1;
assign c2 = ccg2;
assign c3 = ccg3;

assign sum0 = p0 ^ cin;
assign sum1 = p1 ^ c0;
assign sum2 = p2 ^ c1;
assign sum3 = p3 ^ c2;
assign cout = (ccp3 & cin) | ccg3;
//--------------------------------------------------//
endmodule
