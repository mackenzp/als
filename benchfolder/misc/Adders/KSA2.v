//============================================================================//
//                           2-bit Kogge-Stone Adder                          //
//                                                                            //
//                       Naveen Katam and Massoud Pedram                      //
//     SPORT Lab, University of Southern California, Los Angeles, CA 90089    //
//                          http://sportlab.usc.edu/                          //
//                                                                            //
// For licensing, please refer to the LICENSE file in the main directory.     //
//============================================================================//

module KS2_new (a0, a1, b0, b1, cin, sum0, sum1, cout);

input a0, a1, b0, b1, cin;
output sum0, sum1, cout;

wire p0, p1, g0, g1, cp0, cp1, cg0, cg1, ccg1, c0, c1;
//--------------------------------------------------//
assign p0 = a0 ^ b0;
assign g0 = a0 & b0;

assign p1 = a1 ^ b1;
assign g1 = a1 & b1;

//--------------------------------------------------//
assign cg0 = (p0 & cin) | g0;
assign cp0 = p0;

assign cg1 = (p1 & g0) | g1;
assign cp1 = p1 & p0;

//-----------------------------//

assign ccg1 = (cp1 & cin) | cg1;

//--------------------------------------------------//
assign c0 = cg0;
assign c1 = ccg1;

assign sum0 = p0 ^ cin;
assign sum1 = p1 ^ c0;

assign cout = (cp1 & cin) | ccg1;
//--------------------------------------------------//
endmodule
