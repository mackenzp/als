//============================================================================//
//                           16-bit Kogge-Stone Adder                         //
//                                                                            //
//                       Naveen Katam and Massoud Pedram                      //
//     SPORT Lab, University of Southern California, Los Angeles, CA 90089    //
//                          http://sportlab.usc.edu/                          //
//                                                                            //
// For licensing, please refer to the LICENSE file in the main directory.     //
//============================================================================//

module KSA16 (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, cin, 
sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, sum8, sum9, sum10, sum11, sum12, sum13, sum14, sum15, cout);

input a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, cin;
output sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, sum8, sum9, sum10, sum11, sum12, sum13, sum14, sum15, cout;

wire p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 
	 g0, g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12, g13, g14, g15, 
	 cp0, cp1, cp2, cp3, cp4, cp5, cp6, cp7, cp8, cp9, cp10, cp11, cp12, cp13, cp14, cp15, 
	 cg0, cg1, cg2, cg3, cg4, cg5, cg6, cg7, cg8, cg9, cg10, cg11, cg12, cg13, cg14, cg15, 
	 ccp0, ccp1, ccp2, ccp3, ccp4, ccp5, ccp6, ccp7, ccp8, ccp9, ccp10, ccp11, ccp12, ccp13, ccp14, ccp15,
	 ccg0, ccg1, ccg2, ccg3, ccg4, ccg5, ccg6, ccg7, ccg8, ccg9, ccg10, ccg11, ccg12, ccg13, ccg14, ccg15, 
	 cccp0, cccp1, cccp2, cccp3, cccp4, cccp5, cccp6, cccp7, cccp8, cccp9, cccp10, cccp11, cccp12, cccp13, cccp14, cccp15, 
	 cccg0, cccg1, cccg2, cccg3, cccg4, cccg5, cccg6, cccg7, cccg8, cccg9, cccg10, cccg11, cccg12, cccg13, cccg14, cccg15, 
	 ccccg0, ccccg1, ccccg2, ccccg3, ccccg4, ccccg5, ccccg6, ccccg7, ccccg8, ccccg9, ccccg10, ccccg11, ccccg12, ccccg13, ccccg14, ccccg15, 
	 ccccp0, ccccp1, ccccp2, ccccp3, ccccp4, ccccp5, ccccp6, ccccp7, ccccp8, ccccp9, ccccp10, ccccp11, ccccp12, ccccp13, ccccp14, ccccp15, 
	 c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15;
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

assign p8 = a8 ^ b8;
assign g8 = a8 & b8;

assign p9 = a9 ^ b9;
assign g9 = a9 & b9;

assign p10 = a10 ^ b10;
assign g10 = a10 & b10;

assign p11 = a11 ^ b11;
assign g11 = a11 & b11;

assign p12 = a12 ^ b12;
assign g12 = a12 & b12;

assign p13 = a13 ^ b13;
assign g13 = a13 & b13;

assign p14 = a14 ^ b14;
assign g14 = a14 & b14;

assign p15 = a15 ^ b15;
assign g15 = a15 & b15;
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

assign cg8 = (p8 & g7) | g8;
assign cp8 = p8 & p7;

assign cg9 = (p9 & g8) | g9;
assign cp9 = p9 & p8;

assign cg10 = (p10 & g9) | g10;
assign cp10 = p10 & p9;

assign cg11 = (p11 & g10) | g11;
assign cp11 = p11 & p10;      
                                  
assign cg12 = (p12 & g11) | g12;
assign cp12 = p12 & p11;      
                                  
assign cg13 = (p13 & g12) | g13;
assign cp13 = p13 & p12;      
                                  
assign cg14 = (p14 & g13) | g14;
assign cp14 = p14 & p13;

assign cg15 = (p15 & g14) | g15;
assign cp15 = p15 & p14;
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

assign ccg8 = (cp8 & cg6) | cg8;
assign ccp8 = cp8 & cp6;

assign ccg9 = (cp9 & cg7) | cg9;
assign ccp9 = cp9 & cp7;

assign ccg10 = (cp10 & cg8) | cg10;
assign ccp10 = cp10 & cp8;

assign ccg11 = (cp11 & cg9) | cg11;
assign ccp11 = cp11 & cp9;

assign ccg12 = (cp12 & cg10) | cg12;
assign ccp12 = cp12 & cp10;

assign ccg13 = (cp13 & cg11) | cg13;
assign ccp13 = cp13 & cp11;

assign ccg14 = (cp14 & cg12) | cg14;
assign ccp14 = cp14 & cp12;

assign ccg15 = (cp15 & cg13) | cg15;
assign ccp15 = cp15 & cp13;
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

assign cccg8 = (ccp8 & ccg4) | ccg8;
assign cccp8 = ccp8 & ccp4;

assign cccg9 = (ccp9 & ccg5) | ccg9;
assign cccp9 = ccp9 & ccp5;

assign cccg10 = (ccp10 & ccg6) | ccg10;
assign cccp10 = ccp10 & ccp6;

assign cccg11 = (ccp11 & ccg7) | ccg11;
assign cccp11 = ccp11 & ccp7;

assign cccg12 = (ccp12 & ccg8) | ccg12;
assign cccp12 = ccp12 & ccp8;

assign cccg13 = (ccp13 & ccg9) | ccg13;
assign cccp13 = ccp13 & ccp9;

assign cccg14 = (ccp14 & ccg10) | ccg14;
assign cccp14 = ccp14 & ccp10;

assign cccg15 = (ccp15 & ccg11) | ccg15;
assign cccp15 = ccp15 & ccp11;
//--------------------------------------------------//
assign ccccg0 = cccg0;
assign ccccp0 = cccp0;

assign ccccg1 = cccg1;
assign ccccp1 = cccp1;

assign ccccg2 = cccg2;
assign ccccp2 = cccp2;

assign ccccg3 = cccg3;
assign ccccp3 = cccp3;

assign ccccg4 = cccg4;
assign ccccp4 = cccp4;

assign ccccg5 = cccg5;
assign ccccp5 = cccp5;

assign ccccg6 = cccg6;
assign ccccp6 = cccp6;

assign ccccg7 = (cccp7 & cin) | cccg7;
assign ccccp7 = cccp7;

assign ccccg8 = (cccp8 & cccg0) | cccg8;
assign ccccp8 = cccp8 & cccp0;

assign ccccg9 = (cccp9 & cccg1) | cccg9;
assign ccccp9 = cccp9 & cccp1;

assign ccccg10 = (cccp10 & cccg2) | cccg10;
assign ccccp10 = cccp10 & cccp2;

assign ccccg11 = (cccp11 & cccg3) | cccg11;
assign ccccp11 = cccp11 & cccp3;

assign ccccg12 = (cccp12 & cccg4) | cccg12;
assign ccccp12 = cccp12 & cccp4;

assign ccccg13 = (cccp13 & cccg5) | cccg13;
assign ccccp13 = cccp13 & cccp5;

assign ccccg14 = (cccp14 & cccg6) | cccg14;
assign ccccp14 = cccp14 & cccp6;

assign ccccg15 = (cccp15 & cccg7) | cccg15;
assign ccccp15 = cccp15 & cccp7;
//--------------------------------------------------//
assign c0 = ccccg0;
assign c1 = ccccg1;
assign c2 = ccccg2;
assign c3 = ccccg3;
assign c4 = ccccg4;
assign c5 = ccccg5;
assign c6 = ccccg6;
assign c7 = ccccg7;
assign c8 = ccccg8;
assign c9 = ccccg9;
assign c10 = ccccg10;
assign c11 = ccccg11;
assign c12 = ccccg12;
assign c13 = ccccg13;
assign c14 = ccccg14;
assign c15 = ccccg15;

assign sum0  = p0  ^ cin;
assign sum1  = p1  ^ c0;
assign sum2  = p2  ^ c1;
assign sum3  = p3  ^ c2;
assign sum4  = p4  ^ c3;
assign sum5  = p5  ^ c4;
assign sum6  = p6  ^ c5;
assign sum7  = p7  ^ c6;
assign sum8  = p8  ^ c7;
assign sum9  = p9  ^ c8;
assign sum10 = p10 ^ c9;
assign sum11 = p11 ^ c10;
assign sum12 = p12 ^ c11;
assign sum13 = p13 ^ c12;
assign sum14 = p14 ^ c13;
assign sum15 = p15 ^ c14;
assign cout = (ccccp15 & cin) | ccccg15;
//--------------------------------------------------//
endmodule
