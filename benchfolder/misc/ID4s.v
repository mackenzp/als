// Benchmark "integer_divider" written by ABC on Mon May 28 12:05:19 2018

module integer_divider ( 
    X3, X2, X1, X0, D3, D2, D1, D0,
    Q3, Q2, Q1, Q0, R3, R2, R1, R0  );
  input  X3, X2, X1, X0, D3, D2, D1, D0;
  output Q3, Q2, Q1, Q0, R3, R2, R1, R0;
  wire n17, n18, n19, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31,
    n32, n33, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46,
    n47, n48, n49, n50, n51, n52, n53, n54, n55, n56, n57, n58, n59, n61,
    n62, n63, n64, n65, n66, n67, n68, n69, n70, n71, n72, n73, n74, n75,
    n76, n77, n78, n79, n80, n81, n82, n83, n84, n85, n86, n87, n88, n89,
    n90, n91, n92, n93, n94, n95, n96, n98, n99, n100, n101, n103, n104,
    n105, n106, n108, n109, n110, n111, n113, n114, n115, n116;
  assign n17 = ~D3 & ~D2;
  assign n18 = ~D1 & n17;
  assign n19 = ~X3 & D0;
  assign Q3 = n18 & ~n19;
  assign n21 = ~X3 & D1;
  assign n22 = n17 & ~n21;
  assign n23 = ~X2 & D0;
  assign n24 = ~X3 & ~Q3;
  assign n25 = ~X3 & ~D0;
  assign n26 = X3 & D0;
  assign n27 = ~n25 & ~n26;
  assign n28 = Q3 & ~n27;
  assign n29 = ~n24 & ~n28;
  assign n30 = D1 & n29;
  assign n31 = ~D1 & ~n29;
  assign n32 = ~n30 & ~n31;
  assign n33 = n23 & ~n32;
  assign Q2 = n22 & ~n33;
  assign n35 = ~n23 & ~n32;
  assign n36 = n23 & n32;
  assign n37 = ~n35 & ~n36;
  assign n38 = Q2 & ~n37;
  assign n39 = ~n29 & ~Q2;
  assign n40 = ~n38 & ~n39;
  assign n41 = D2 & ~n40;
  assign n42 = ~D3 & ~n41;
  assign n43 = ~X1 & D0;
  assign n44 = ~X2 & ~D0;
  assign n45 = X2 & D0;
  assign n46 = ~n44 & ~n45;
  assign n47 = Q2 & ~n46;
  assign n48 = ~X2 & ~Q2;
  assign n49 = ~n47 & ~n48;
  assign n50 = D1 & n49;
  assign n51 = ~D1 & ~n49;
  assign n52 = ~n50 & ~n51;
  assign n53 = n43 & ~n52;
  assign n54 = D1 & ~n49;
  assign n55 = ~n53 & ~n54;
  assign n56 = D2 & n40;
  assign n57 = ~D2 & ~n40;
  assign n58 = ~n56 & ~n57;
  assign n59 = ~n55 & ~n58;
  assign Q1 = n42 & ~n59;
  assign n61 = ~X0 & D0;
  assign n62 = ~X1 & ~D0;
  assign n63 = X1 & D0;
  assign n64 = ~n62 & ~n63;
  assign n65 = Q1 & ~n64;
  assign n66 = ~X1 & ~Q1;
  assign n67 = ~n65 & ~n66;
  assign n68 = D1 & n67;
  assign n69 = ~D1 & ~n67;
  assign n70 = ~n68 & ~n69;
  assign n71 = n61 & ~n70;
  assign n72 = D1 & ~n67;
  assign n73 = ~n71 & ~n72;
  assign n74 = ~n43 & ~n52;
  assign n75 = n43 & n52;
  assign n76 = ~n74 & ~n75;
  assign n77 = Q1 & ~n76;
  assign n78 = ~n49 & ~Q1;
  assign n79 = ~n77 & ~n78;
  assign n80 = D2 & n79;
  assign n81 = ~D2 & ~n79;
  assign n82 = ~n80 & ~n81;
  assign n83 = ~n73 & ~n82;
  assign n84 = D2 & ~n79;
  assign n85 = ~n83 & ~n84;
  assign n86 = n55 & ~n58;
  assign n87 = ~n55 & n58;
  assign n88 = ~n86 & ~n87;
  assign n89 = Q1 & ~n88;
  assign n90 = ~n40 & ~Q1;
  assign n91 = ~n89 & ~n90;
  assign n92 = D3 & n91;
  assign n93 = ~D3 & ~n91;
  assign n94 = ~n92 & ~n93;
  assign n95 = ~n85 & ~n94;
  assign n96 = D3 & ~n40;
  assign Q0 = ~n95 & ~n96;
  assign n98 = n85 & n94;
  assign n99 = ~n95 & ~n98;
  assign n100 = Q0 & ~n99;
  assign n101 = n91 & ~Q0;
  assign R3 = n100 | n101;
  assign n103 = n73 & n82;
  assign n104 = ~n83 & ~n103;
  assign n105 = Q0 & ~n104;
  assign n106 = n79 & ~Q0;
  assign R2 = n105 | n106;
  assign n108 = ~n61 & n70;
  assign n109 = ~n71 & ~n108;
  assign n110 = Q0 & ~n109;
  assign n111 = n67 & ~Q0;
  assign R1 = n110 | n111;
  assign n113 = X0 & ~D0;
  assign n114 = ~n61 & ~n113;
  assign n115 = Q0 & ~n114;
  assign n116 = X0 & ~Q0;
  assign R0 = n115 | n116;
endmodule


