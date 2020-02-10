//============================================================================//
//                           32-bit Kogge-Stone Adder                         //
//                                                                            //
//                       Naveen Katam and Massoud Pedram                      //
//     SPORT Lab, University of Southern California, Los Angeles, CA 90089    //
//                          http://sportlab.usc.edu/                          //
//                                                                            //
// For licensing, please refer to the LICENSE file in the main directory.     //
//============================================================================//

module KSA32(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17, b18, b19, b20, b21, b22, b23, b24, b25, b26, b27, b28, b29, b30, b31, 
cin,sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, sum8, sum9, sum10, sum11, sum12, sum13, sum14, sum15, sum16, sum17, sum18, sum19, sum20, sum21, sum22, sum23, sum24, sum25, sum26, sum27, sum28, sum29, sum30, sum31, cout);

input  a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17, b18, b19, b20, b21, b22, b23, b24, b25, b26, b27, b28, b29, b30, b31, cin;
output  sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, sum8, sum9, sum10, sum11, sum12, sum13, sum14, sum15, sum16, sum17, sum18, sum19, sum20, sum21, sum22, sum23, sum24, sum25, sum26, sum27, sum28, sum29, sum30, sum31, cout;

wire p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, 
	g0, g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12, g13, g14, g15, g16, g17, g18, g19, g20, g21, g22, g23, g24, g25, g26, g27, g28, g29, g30, g31, 
	cp0, cp1, cp2, cp3, cp4, cp5, cp6, cp7, cp8, cp9, cp10, cp11, cp12, cp13, cp14, cp15, cp16, cp17, cp18, cp19, cp20, cp21, cp22, cp23, cp24, cp25, cp26, cp27, cp28, cp29, cp30, cp31, 
	cg0, cg1, cg2, cg3, cg4, cg5, cg6, cg7, cg8, cg9, cg10, cg11, cg12, cg13, cg14, cg15, cg16, cg17, cg18, cg19, cg20, cg21, cg22, cg23, cg24, cg25, cg26, cg27, cg28, cg29, cg30, cg31, 
	ccg0, ccg1, ccg2, ccg3, ccg4, ccg5, ccg6, ccg7, ccg8, ccg9, ccg10, ccg11, ccg12, ccg13, ccg14, ccg15, ccg16, ccg17, ccg18, ccg19, ccg20, ccg21, ccg22, ccg23, ccg24, ccg25, ccg26, ccg27, ccg28, ccg29, ccg30, ccg31, 
	ccp0, ccp1, ccp2, ccp3, ccp4, ccp5, ccp6, ccp7, ccp8, ccp9, ccp10, ccp11, ccp12, ccp13, ccp14, ccp15, ccp16, ccp17, ccp18, ccp19, ccp20, ccp21, ccp22, ccp23, ccp24, ccp25, ccp26, ccp27, ccp28, ccp29, ccp30, ccp31, 
	cccg0, cccg1, cccg2, cccg3, cccg4, cccg5, cccg6, cccg7, cccg8, cccg9, cccg10, cccg11, cccg12, cccg13, cccg14, cccg15, cccg16, cccg17, cccg18, cccg19, cccg20, cccg21, cccg22, cccg23, cccg24, cccg25, cccg26, cccg27, cccg28, cccg29, cccg30, cccg31, 
	cccp0, cccp1, cccp2, cccp3, cccp4, cccp5, cccp6, cccp7, cccp8, cccp9, cccp10, cccp11, cccp12, cccp13, cccp14, cccp15, cccp16, cccp17, cccp18, cccp19, cccp20, cccp21, cccp22, cccp23, cccp24, cccp25, cccp26, cccp27, cccp28, cccp29, cccp30, cccp31, 
	ccccg0, ccccg1, ccccg2, ccccg3, ccccg4, ccccg5, ccccg6, ccccg7, ccccg8, ccccg9, ccccg10, ccccg11, ccccg12, ccccg13, ccccg14, ccccg15, ccccg16, ccccg17, ccccg18, ccccg19, ccccg20, ccccg21, ccccg22, ccccg23, ccccg24, ccccg25, ccccg26, ccccg27, ccccg28, ccccg29, ccccg30, ccccg31, 
	ccccp0, ccccp1, ccccp2, ccccp3, ccccp4, ccccp5, ccccp6, ccccp7, ccccp8, ccccp9, ccccp10, ccccp11, ccccp12, ccccp13, ccccp14, ccccp15, ccccp16, ccccp17, ccccp18, ccccp19, ccccp20, ccccp21, ccccp22, ccccp23, ccccp24, ccccp25, ccccp26, ccccp27, ccccp28, ccccp29, ccccp30, ccccp31, 
	cccccg0, cccccg1, cccccg2, cccccg3, cccccg4, cccccg5, cccccg6, cccccg7, cccccg8, cccccg9, cccccg10, cccccg11, cccccg12, cccccg13, cccccg14, cccccg15, cccccg16, cccccg17, cccccg18, cccccg19, cccccg20, cccccg21, cccccg22, cccccg23, cccccg24, cccccg25, cccccg26, cccccg27, cccccg28, cccccg29, cccccg30, cccccg31, 
	cccccp0, cccccp1, cccccp2, cccccp3, cccccp4, cccccp5, cccccp6, cccccp7, cccccp8, cccccp9, cccccp10, cccccp11, cccccp12, cccccp13, cccccp14, cccccp15, cccccp16, cccccp17, cccccp18, cccccp19, cccccp20, cccccp21, cccccp22, cccccp23, cccccp24, cccccp25, cccccp26, cccccp27, cccccp28, cccccp29, cccccp30, cccccp31, 
	c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20, c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, c31;
	
//--------------------------------------------------//
assign p0 = a0 ^ b0;
assign p1 = a1 ^ b1;
assign p2 = a2 ^ b2;
assign p3 = a3 ^ b3;
assign p4 = a4 ^ b4;
assign p5 = a5 ^ b5;
assign p6 = a6 ^ b6;
assign p7 = a7 ^ b7;
assign p8 = a8 ^ b8;
assign p9 = a9 ^ b9;
assign p10 = a10 ^ b10;
assign p11 = a11 ^ b11;
assign p12 = a12 ^ b12;
assign p13 = a13 ^ b13;
assign p14 = a14 ^ b14;
assign p15 = a15 ^ b15;
assign p16 = a16 ^ b16;
assign p17 = a17 ^ b17;
assign p18 = a18 ^ b18;
assign p19 = a19 ^ b19;
assign p20 = a20 ^ b20;
assign p21 = a21 ^ b21;
assign p22 = a22 ^ b22;
assign p23 = a23 ^ b23;
assign p24 = a24 ^ b24;
assign p25 = a25 ^ b25;
assign p26 = a26 ^ b26;
assign p27 = a27 ^ b27;
assign p28 = a28 ^ b28;
assign p29 = a29 ^ b29;
assign p30 = a30 ^ b30;
assign p31 = a31 ^ b31;
assign g0 = a0 & b0;
assign g1 = a1 & b1;
assign g2 = a2 & b2;
assign g3 = a3 & b3;
assign g4 = a4 & b4;
assign g5 = a5 & b5;
assign g6 = a6 & b6;
assign g7 = a7 & b7;
assign g8 = a8 & b8;
assign g9 = a9 & b9;
assign g10 = a10 & b10;
assign g11 = a11 & b11;
assign g12 = a12 & b12;
assign g13 = a13 & b13;
assign g14 = a14 & b14;
assign g15 = a15 & b15;
assign g16 = a16 & b16;
assign g17 = a17 & b17;
assign g18 = a18 & b18;
assign g19 = a19 & b19;
assign g20 = a20 & b20;
assign g21 = a21 & b21;
assign g22 = a22 & b22;
assign g23 = a23 & b23;
assign g24 = a24 & b24;
assign g25 = a25 & b25;
assign g26 = a26 & b26;
assign g27 = a27 & b27;
assign g28 = a28 & b28;
assign g29 = a29 & b29;
assign g30 = a30 & b30;
assign g31 = a31 & b31;
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


assign cg16 = (p16 & g15) | g16;

assign cp16 = p16 & p15;


assign cg17 = (p17 & g16) | g17;

assign cp17 = p17 & p16;


assign cg18 = (p18 & g17) | g18;

assign cp18 = p18 & p17;


assign cg19 = (p19 & g18) | g19;

assign cp19 = p19 & p18;


assign cg20 = (p20 & g19) | g20;

assign cp20 = p20 & p19;


assign cg21 = (p21 & g20) | g21;

assign cp21 = p21 & p20;


assign cg22 = (p22 & g21) | g22;

assign cp22 = p22 & p21;


assign cg23 = (p23 & g22) | g23;

assign cp23 = p23 & p22;


assign cg24 = (p24 & g23) | g24;

assign cp24 = p24 & p23;


assign cg25 = (p25 & g24) | g25;

assign cp25 = p25 & p24;


assign cg26 = (p26 & g25) | g26;

assign cp26 = p26 & p25;


assign cg27 = (p27 & g26) | g27;

assign cp27 = p27 & p26;


assign cg28 = (p28 & g27) | g28;

assign cp28 = p28 & p27;


assign cg29 = (p29 & g28) | g29;

assign cp29 = p29 & p28;


assign cg30 = (p30 & g29) | g30;

assign cp30 = p30 & p29;


assign cg31 = (p31 & g30) | g31;

assign cp31 = p31 & p30;

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


assign ccg16 = (cp16 & cg14) | cg16;

assign ccp16 = cp16 & cp14;


assign ccg17 = (cp17 & cg15) | cg17;

assign ccp17 = cp17 & cp15;


assign ccg18 = (cp18 & cg16) | cg18;

assign ccp18 = cp18 & cp16;


assign ccg19 = (cp19 & cg17) | cg19;

assign ccp19 = cp19 & cp17;


assign ccg20 = (cp20 & cg18) | cg20;

assign ccp20 = cp20 & cp18;


assign ccg21 = (cp21 & cg19) | cg21;

assign ccp21 = cp21 & cp19;


assign ccg22 = (cp22 & cg20) | cg22;

assign ccp22 = cp22 & cp20;


assign ccg23 = (cp23 & cg21) | cg23;

assign ccp23 = cp23 & cp21;


assign ccg24 = (cp24 & cg22) | cg24;

assign ccp24 = cp24 & cp22;


assign ccg25 = (cp25 & cg23) | cg25;

assign ccp25 = cp25 & cp23;


assign ccg26 = (cp26 & cg24) | cg26;

assign ccp26 = cp26 & cp24;


assign ccg27 = (cp27 & cg25) | cg27;

assign ccp27 = cp27 & cp25;


assign ccg28 = (cp28 & cg26) | cg28;

assign ccp28 = cp28 & cp26;


assign ccg29 = (cp29 & cg27) | cg29;

assign ccp29 = cp29 & cp27;


assign ccg30 = (cp30 & cg28) | cg30;

assign ccp30 = cp30 & cp28;


assign ccg31 = (cp31 & cg29) | cg31;

assign ccp31 = cp31 & cp29;

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


assign cccg16 = (ccp16 & ccg12) | ccg16;

assign cccp16 = ccp16 & ccp12;


assign cccg17 = (ccp17 & ccg13) | ccg17;

assign cccp17 = ccp17 & ccp13;


assign cccg18 = (ccp18 & ccg14) | ccg18;

assign cccp18 = ccp18 & ccp14;


assign cccg19 = (ccp19 & ccg15) | ccg19;

assign cccp19 = ccp19 & ccp15;


assign cccg20 = (ccp20 & ccg16) | ccg20;

assign cccp20 = ccp20 & ccp16;


assign cccg21 = (ccp21 & ccg17) | ccg21;

assign cccp21 = ccp21 & ccp17;


assign cccg22 = (ccp22 & ccg18) | ccg22;

assign cccp22 = ccp22 & ccp18;


assign cccg23 = (ccp23 & ccg19) | ccg23;

assign cccp23 = ccp23 & ccp19;


assign cccg24 = (ccp24 & ccg20) | ccg24;

assign cccp24 = ccp24 & ccp20;


assign cccg25 = (ccp25 & ccg21) | ccg25;

assign cccp25 = ccp25 & ccp21;


assign cccg26 = (ccp26 & ccg22) | ccg26;

assign cccp26 = ccp26 & ccp22;


assign cccg27 = (ccp27 & ccg23) | ccg27;

assign cccp27 = ccp27 & ccp23;


assign cccg28 = (ccp28 & ccg24) | ccg28;

assign cccp28 = ccp28 & ccp24;


assign cccg29 = (ccp29 & ccg25) | ccg29;

assign cccp29 = ccp29 & ccp25;


assign cccg30 = (ccp30 & ccg26) | ccg30;

assign cccp30 = ccp30 & ccp26;


assign cccg31 = (ccp31 & ccg27) | ccg31;

assign cccp31 = ccp31 & ccp27;


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


assign ccccg16 = (cccp16 & cccg8) | cccg16;

assign ccccp16 = cccp16 & cccp8;


assign ccccg17 = (cccp17 & cccg9) | cccg17;

assign ccccp17 = cccp17 & cccp9;


assign ccccg18 = (cccp18 & cccg10) | cccg18;

assign ccccp18 = cccp18 & cccp10;


assign ccccg19 = (cccp19 & cccg11) | cccg19;

assign ccccp19 = cccp19 & cccp11;


assign ccccg20 = (cccp20 & cccg12) | cccg20;

assign ccccp20 = cccp20 & cccp12;


assign ccccg21 = (cccp21 & cccg13) | cccg21;

assign ccccp21 = cccp21 & cccp13;


assign ccccg22 = (cccp22 & cccg14) | cccg22;

assign ccccp22 = cccp22 & cccp14;


assign ccccg23 = (cccp23 & cccg15) | cccg23;

assign ccccp23 = cccp23 & cccp15;


assign ccccg24 = (cccp24 & cccg16) | cccg24;

assign ccccp24 = cccp24 & cccp16;


assign ccccg25 = (cccp25 & cccg17) | cccg25;

assign ccccp25 = cccp25 & cccp17;


assign ccccg26 = (cccp26 & cccg18) | cccg26;

assign ccccp26 = cccp26 & cccp18;


assign ccccg27 = (cccp27 & cccg19) | cccg27;

assign ccccp27 = cccp27 & cccp19;


assign ccccg28 = (cccp28 & cccg20) | cccg28;

assign ccccp28 = cccp28 & cccp20;


assign ccccg29 = (cccp29 & cccg21) | cccg29;

assign ccccp29 = cccp29 & cccp21;


assign ccccg30 = (cccp30 & cccg22) | cccg30;

assign ccccp30 = cccp30 & cccp22;


assign ccccg31 = (cccp31 & cccg23) | cccg31;

assign ccccp31 = cccp31 & cccp23;

//--------------------------------------------------//

assign cccccg0 = ccccg0;

assign cccccp0 = ccccp0;


assign cccccg1 = ccccg1;

assign cccccp1 = ccccp1;


assign cccccg2 = ccccg2;

assign cccccp2 = ccccp2;


assign cccccg3 = ccccg3;

assign cccccp3 = ccccp3;


assign cccccg4 = ccccg4;

assign cccccp4 = ccccp4;


assign cccccg5 = ccccg5;

assign cccccp5 = ccccp5;


assign cccccg6 = ccccg6;

assign cccccp6 = ccccp6;


assign cccccg7 = ccccg7;

assign cccccp7 = ccccp7;


assign cccccg8 = ccccg8;

assign cccccp8 = ccccp8;


assign cccccg9 = ccccg9;

assign cccccp9 = ccccp9;


assign cccccg10 = ccccg10;

assign cccccp10 = ccccp10;


assign cccccg11 = ccccg11;

assign cccccp11 = ccccp11;


assign cccccg12 = ccccg12;

assign cccccp12 = ccccp12;


assign cccccg13 = ccccg13;

assign cccccp13 = ccccp13;


assign cccccg14 = ccccg14;

assign cccccp14 = ccccp14;


assign cccccg15 = (ccccp15 & cin) | ccccg15;

assign cccccp15 = ccccp15;


assign cccccg16 = (ccccp16 & ccccg0) | ccccg16;

assign cccccp16 = ccccp16 & ccccp0;


assign cccccg17 = (ccccp17 & ccccg1) | ccccg17;

assign cccccp17 = ccccp17 & ccccp1;


assign cccccg18 = (ccccp18 & ccccg2) | ccccg18;

assign cccccp18 = ccccp18 & ccccp2;


assign cccccg19 = (ccccp19 & ccccg3) | ccccg19;

assign cccccp19 = ccccp19 & ccccp3;


assign cccccg20 = (ccccp20 & ccccg4) | ccccg20;

assign cccccp20 = ccccp20 & ccccp4;


assign cccccg21 = (ccccp21 & ccccg5) | ccccg21;

assign cccccp21 = ccccp21 & ccccp5;


assign cccccg22 = (ccccp22 & ccccg6) | ccccg22;

assign cccccp22 = ccccp22 & ccccp6;


assign cccccg23 = (ccccp23 & ccccg7) | ccccg23;

assign cccccp23 = ccccp23 & ccccp7;


assign cccccg24 = (ccccp24 & ccccg8) | ccccg24;

assign cccccp24 = ccccp24 & ccccp8;


assign cccccg25 = (ccccp25 & ccccg9) | ccccg25;

assign cccccp25 = ccccp25 & ccccp9;


assign cccccg26 = (ccccp26 & ccccg10) | ccccg26;

assign cccccp26 = ccccp26 & ccccp10;


assign cccccg27 = (ccccp27 & ccccg11) | ccccg27;

assign cccccp27 = ccccp27 & ccccp11;


assign cccccg28 = (ccccp28 & ccccg12) | ccccg28;

assign cccccp28 = ccccp28 & ccccp12;


assign cccccg29 = (ccccp29 & ccccg13) | ccccg29;

assign cccccp29 = ccccp29 & ccccp13;


assign cccccg30 = (ccccp30 & ccccg14) | ccccg30;

assign cccccp30 = ccccp30 & ccccp14;


assign cccccg31 = (ccccp31 & ccccg15) | ccccg31;

assign cccccp31 = ccccp31 & ccccp15;

//--------------------------------------------------//
assign c0 = cccccg0;
assign c1 = cccccg1;
assign c2 = cccccg2;
assign c3 = cccccg3;
assign c4 = cccccg4;
assign c5 = cccccg5;
assign c6 = cccccg6;
assign c7 = cccccg7;
assign c8 = cccccg8;
assign c9 = cccccg9;
assign c10 = cccccg10;
assign c11 = cccccg11;
assign c12 = cccccg12;
assign c13 = cccccg13;
assign c14 = cccccg14;
assign c15 = cccccg15;
assign c16 = cccccg16;
assign c17 = cccccg17;
assign c18 = cccccg18;
assign c19 = cccccg19;
assign c20 = cccccg20;
assign c21 = cccccg21;
assign c22 = cccccg22;
assign c23 = cccccg23;
assign c24 = cccccg24;
assign c25 = cccccg25;
assign c26 = cccccg26;
assign c27 = cccccg27;
assign c28 = cccccg28;
assign c29 = cccccg29;
assign c30 = cccccg30;
assign c31 = cccccg31;

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

assign sum16  = p16  ^ c15;

assign sum17  = p17  ^ c16;

assign sum18  = p18  ^ c17;

assign sum19  = p19  ^ c18;

assign sum20  = p20  ^ c19;

assign sum21  = p21  ^ c20;

assign sum22  = p22  ^ c21;

assign sum23  = p23  ^ c22;

assign sum24  = p24  ^ c23;

assign sum25  = p25  ^ c24;

assign sum26 = p26 ^ c25;

assign sum27 = p27 ^ c26;

assign sum28 = p28 ^ c27;

assign sum29 = p29 ^ c28;

assign sum30 = p30 ^ c29;

assign sum31 = p31 ^ c30;

assign cout = (cccccp31 & cin) | cccccg31;

//--------------------------------------------------//
endmodule
