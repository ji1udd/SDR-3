/*
--------------------------------------------------------------------------------
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
Boston, MA  02110-1301, USA.
--------------------------------------------------------------------------------
*/


//------------------------------------------------------------------------------
//           Copyright (c) 2008 Alex Shovkoplyas, VE3NEA
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//           Copyright (c) 2013 Phil Harman, VK6APH 
//------------------------------------------------------------------------------

// 2013 Jan 26 - varcic now accepts 2...40 as decimation and CFIR
//               replaced with Polyphase FIR - VK6APH

// 2015 Jan 31 - updated for Hermes-Lite 12bit Steve Haynal KF7O

module receiver(
  input clock,                  // 73.728 MHz
  input [5:0] rate,             // RATE48 = 6'd24;
  input [31:0] frequency,
  output out_strobe,
  input signed [11:0] in_data,
  output [23:0] out_data_I,
  output [23:0] out_data_Q
  );

  parameter CICRATE;             // 6'd8

wire signed [17:0] cordic_outdata_I;
wire signed [17:0] cordic_outdata_Q;

// gain adjustment, Hermes reduced by 6dB to match previous receiver code.
// Hermes-Lite gain reduced to calibrate QtRadio
wire signed [23:0] out_data_I2;  //24 bit
wire signed [23:0] out_data_Q2;
//assign out_data_I = out_data_I2; //>>> 3);
//assign out_data_Q = out_data_Q2; //>>> 3);
assign out_data_I = (out_data_I2 >>> 1);
assign out_data_Q = (out_data_Q2 >>> 1);

//------------------------------------------------------------------------------
//                               cordic
//------------------------------------------------------------------------------

cordic cordic_inst(
  .clock(clock),
  .in_data(in_data),             //12 bit 
  .frequency(frequency),         //32 bit
  .out_data_I(cordic_outdata_I), //18 bit
  .out_data_Q(cordic_outdata_Q)
  );

  
// Receive CIC filters followed by FIR filter
wire decimA_avail, decimB_avail;
wire signed [13:0] decimA_real, decimA_imag;
wire signed [15:0] decimB_real, decimB_imag;

localparam VARCICWIDTH = (CICRATE == 10) ? 34 : (CICRATE == 13) ? 34 : (CICRATE == 5) ? 41 : 37; // Last is default rate of 8
localparam ACCWIDTH = (CICRATE == 10) ? 28 : (CICRATE == 13) ? 30 : (CICRATE == 5) ? 25 : 27; // Last is default rate of 8


// CIC filter  1/8
//I channel
cic #(.STAGES(3), .DECIMATION(CICRATE), .IN_WIDTH(18), .ACC_WIDTH(ACCWIDTH), .OUT_WIDTH(14))      
  cic_inst_I2(
    .clock(clock),
    .in_strobe(1'b1),
    .out_strobe(decimA_avail),
    .in_data(cordic_outdata_I), //18 bit
    .out_data(decimA_real)      //14 bit
    );

//Q channel
cic #(.STAGES(3), .DECIMATION(CICRATE), .IN_WIDTH(18), .ACC_WIDTH(ACCWIDTH), .OUT_WIDTH(14))  
  cic_inst_Q2(
    .clock(clock),
    .in_strobe(1'b1),
    .out_strobe(),
    .in_data(cordic_outdata_Q),
    .out_data(decimA_imag)
    );


//  Variable CIC filter 1/24
//I channel
varcic #(.STAGES(5), .IN_WIDTH(14), .ACC_WIDTH(VARCICWIDTH), .OUT_WIDTH(16), .CICRATE(CICRATE))
  varcic_inst_I1(
    .clock(clock),
    .in_strobe(decimA_avail),
    .decimation(rate),
    .out_strobe(decimB_avail),
    .in_data(decimA_real),     //14 bit
    .out_data(decimB_real)     //16 bit
    );

//Q channel
varcic #(.STAGES(5), .IN_WIDTH(14), .ACC_WIDTH(VARCICWIDTH), .OUT_WIDTH(16), .CICRATE(CICRATE))
  varcic_inst_Q1(
    .clock(clock),
    .in_strobe(decimA_avail),
    .decimation(rate),
    .out_strobe(),
    .in_data(decimA_imag),
    .out_data(decimB_imag)
);

// Fir filter  1/8
firX8R8 fir2 (
  clock,
  decimB_avail,
  {decimB_real, 2'b0},
  {decimB_imag, 2'b0},
  out_strobe,
  out_data_I2,
  out_data_Q2
);

endmodule
