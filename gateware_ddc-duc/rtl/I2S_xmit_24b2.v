//
//  HPSDR - High Performance Software Defined Radio
//
//  Hermes code. 
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// (C) Kirk Weedman KD7IRS  2006, 2007, 2008, 2009, 2010, 2011, 2012 




// By Kirk Weedman KD7IRS - Feb 27 2009
// 
// Implements I2S transmit, DATA_BITS of data per channel, two channels 
// 
// 
// In I2S mode the MSB of the left sample is available on the second rising edge of BCLK
// following a falling  edge on lrclk.  Simiarily the MSB of the right sample will be available on 
// the second rising edge of BCLK following the rising edge of lrclk.

// NOTE: the data to send is latched on the positive edge of lrclk
`timescale 1ns/100ps

module I2S_xmit_24b2 (rst, lrclk, clk, CBrise, CBfall, sample, outbit);

input  wire          rst;
input  wire          lrclk;        // left/right clock 
input  wire          clk;          // system clock that generates BCLK
input  wire          CBrise;       // CBCLK rising edge trigger 
input  wire          CBfall;       // CBCLK rising edge trigger - when to change data 
input  wire [47:0]   sample;       // left & right data to be clocked out 
output reg           outbit;       // output bit 

reg [23:0] data;                   // data to be clocked out 
reg [47:0] last_data; 
reg  [4:0] bit_count;              // how many bits clocked, must prst to MSB 
reg  [2:0] TLV_state;
reg  [2:0] TLV_state_next;
reg        obit;

localparam TLV_IDLE   = 0,
           TLV_WH     = 1,
           TLV_LR_LO  = 2,
           TLV_WL     = 3,
           TLV_LR_HI  = 4;

always @(posedge clk)
begin
  if (rst)
    TLV_state <= TLV_IDLE;
  else
    TLV_state <= TLV_state_next;

  if (rst)
    last_data <= 48'b0;
  else if (TLV_state == TLV_IDLE)  // save samples in this state
    last_data <= sample;

  if (TLV_state == TLV_WH)         // load
    data <= last_data[47:24];      // Left Data
  else if (TLV_state == TLV_WL)    // load
    data <= last_data[23:0];       // Right Data
  else if (CBrise)
    data <= {data[22:0],1'b0} ;    // shift out MSB first

  if (CBrise)
    obit <= data[23];

  if (CBfall)
    outbit <= obit;

  if ((TLV_state == TLV_WH) || (TLV_state == TLV_WL))
    bit_count <= 5'd23;
  else if ((bit_count != 5'd0) && CBrise)
    bit_count <= bit_count - 1'b1;
end

always @*
begin
  case(TLV_state)
    TLV_IDLE:
    begin
      if (!lrclk)
        TLV_state_next = TLV_IDLE;       // loop until lrclk is high
      else
        TLV_state_next = TLV_WH;
    end

    TLV_WH:
    begin
      if (lrclk)
        TLV_state_next = TLV_WH;         // loop until lrclk is low
      else      
        TLV_state_next = TLV_LR_LO;
    end

    TLV_LR_LO:
    begin
      if ((bit_count == 5'd0) & CBrise)  // wait till full length of last bit has finished
        TLV_state_next = TLV_WL; 
      else
        TLV_state_next = TLV_LR_LO;  
    end

    TLV_WL:
    begin
      if (!lrclk)
        TLV_state_next = TLV_WL;         // loop until lrclk is high
      else
        TLV_state_next = TLV_LR_HI;
    end

    TLV_LR_HI:
    begin
      if ((bit_count == 5'd0) & CBrise)  // wait till full length of last bit has finished
        TLV_state_next = TLV_IDLE;       // done so start again
      else
        TLV_state_next = TLV_LR_HI;
    end 
  
    default:
      TLV_state_next = TLV_IDLE;
  endcase
end

endmodule
