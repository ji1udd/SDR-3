//
//  Hermes Lite
// 
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

// (C) Steve Haynal KF7O 2014-2018

module ad9866 (
  clk,
  clkenb,
  rst,

  rffe_ad9866_sdio,
  rffe_ad9866_sclk,
  rffe_ad9866_sen_n,

  // Command slave interface
  i2c_tx_gain_reg,
  i2c_rx_gain_reg,
  i2c_tx_gain_update_req,
  i2c_rx_gain_update_req
);

input             clk;
input             clkenb;
input             rst;

output            rffe_ad9866_sdio;
output            rffe_ad9866_sclk;
output            rffe_ad9866_sen_n;

input  [3:0]      i2c_tx_gain_reg;
input  [5:0]      i2c_rx_gain_reg;
input             i2c_tx_gain_update_req;
input             i2c_rx_gain_update_req;


// SPI
logic   [15:0]    datain;
logic             start;
logic   [3:0]     dut2_bitcount;
logic             dut2_state;
logic   [15:0]    dut2_data;
logic   [5:0]     dut1_pc;
logic             sdo;
logic             istart;

logic   [8:0]     initarrayv;
reg     [8:0]     initarray [19:0];

localparam 
  CMD_IDLE    = 2'b00,
  CMD_TXGAIN  = 2'b01,
  CMD_RXGAIN  = 2'b11;

// Command slave
logic [1:0]       cmd_state = CMD_IDLE;
logic [12:0]      icmd_data = 12'h000;

initial begin
  // First bit is 1'b1 for write enable to that address
  initarray[0] = {1'b0,8'h80}; // Address 0x00, enable 4 wire SPI
  initarray[1] = {1'b0,8'h00}; // Address 0x01,
  initarray[2] = {1'b0,8'h00}; // Address 0x02,
  initarray[3] = {1'b0,8'h00}; // Address 0x03,
  initarray[4] = {1'b0,8'h00}; // Address 0x04,
  initarray[5] = {1'b0,8'h00}; // Address 0x05,
  initarray[6] = {1'b1,8'h54}; // Address 0x06, Disable clkout2
  initarray[7] = {1'b1,8'h30}; // Address 0x07, Initiate DC offset calibration and RX filter on, 21 to 20 to disable RX filter
  initarray[8] = {1'b0,8'h4b}; // Address 0x08, RX filter f-3db at ~34 MHz after scaling
  initarray[9] = {1'b0,8'h00}; // Address 0x09,
  initarray[10] = {1'b0,8'h00}; // Address 0x0a,
  initarray[11] = {1'b1,8'h00}; // Address 0x0b, No RX gain on PGA
  initarray[12] = {1'b1,8'h43}; // Address 0x0c, TX twos complement and interpolation factor
  initarray[13] = {1'b1,8'h03}; // Address 0x0d, RX twos complement
  initarray[14] = {1'b1,8'h81}; // Address 0x0e, Enable/Disable IAMP
  initarray[15] = {1'b0,8'h00}; // Address 0x0f,
  initarray[16] = {1'b1,8'h80}; // Address 0x10, Select TX gain
  initarray[17] = {1'b1,8'h00}; // Address 0x11, Select TX gain
  initarray[18] = {1'b1,8'h00}; // Address 0x12,
  initarray[19] = {1'b0,8'h00}; // Address 0x13,
end

reg tx_gain_update ;
reg rx_gain_update ;
reg i2c_tx_gain_update_ack;
reg i2c_rx_gain_update_ack;
always @(posedge clk) begin
  if (i2c_tx_gain_update_req == 1'b1)
    tx_gain_update <= 1'b1 ;
  else if (i2c_tx_gain_update_ack == 1'b1)
    tx_gain_update <= 1'b0 ;
end

always @(posedge clk) begin
  if (i2c_rx_gain_update_req == 1'b1)
    rx_gain_update <= 1'b1 ;
  else if (i2c_rx_gain_update_ack == 1'b1)
    rx_gain_update <= 1'b0 ;
end

always @(posedge clk) begin
if (clkenb) begin
  case(cmd_state)
    CMD_IDLE: begin
      istart <= 1'b0;
      if (tx_gain_update == 1'b1) begin                // TX Gain Setting
        if (rffe_ad9866_sen_n) begin
          i2c_tx_gain_update_ack <= 1'b1 ;
          cmd_state <= CMD_TXGAIN;
        end
      end else if (rx_gain_update == 1'b1) begin      // RX Gain Setting
        if (rffe_ad9866_sen_n) begin
          i2c_rx_gain_update_ack <= 1'b1 ;
          cmd_state <= CMD_RXGAIN;
        end
      end
    end
	

    CMD_TXGAIN: begin
      istart     = 1'b1;
      icmd_data  = {5'h0a, 4'b0100, i2c_tx_gain_reg}; // tx_gain 4bit
      i2c_tx_gain_update_ack <= 1'b0 ;
      cmd_state <= CMD_IDLE;
    end
    
    CMD_RXGAIN: begin
      istart     = 1'b1;
      icmd_data  = {5'h09, 2'b01, i2c_rx_gain_reg};   // rx_gain 6bit
      i2c_rx_gain_update_ack <= 1'b0 ;
      cmd_state  <= CMD_IDLE;
    end
    
    default:
      cmd_state  <= CMD_IDLE;

  endcase
end
end

// SPI interface
assign sdo       = 1'b0;

// Init program counter
always @(posedge clk) begin
if (clkenb) begin
  if (rst) begin
    dut1_pc <= 6'h00;
  end
  else begin
    if ((dut1_pc != 6'h3f) & rffe_ad9866_sen_n) begin
      dut1_pc <= (dut1_pc + 6'h01);
    end
    else if ((dut1_pc == 6'h3f) & rffe_ad9866_sen_n) begin  // Toggle LSB
      dut1_pc <= 6'h3e;
    end
  end
end
end

always @* begin
  initarrayv = initarray[dut1_pc[5:1]];
  datain = {3'b000,icmd_data};   
  start = 1'b0;
  if (rffe_ad9866_sen_n) begin
    if (dut1_pc[5:1] <= 6'h13) begin
      if (dut1_pc[0] == 1'b0) begin
        datain = {3'h0,dut1_pc[5:1],initarrayv[7:0]};
        start = initarrayv[8];
      end
    end else begin
      start = istart;
    end
  end
end


assign rffe_ad9866_sdio = dut2_data[15];

// SPI state machine

always @(posedge clk) begin
if (clkenb) begin
  if (rst) begin
    rffe_ad9866_sen_n <= 1'b1;
    rffe_ad9866_sclk  <= 1'b0;
    dut2_state        <= 1'b0;
    dut2_data         <= 16'b0;
    dut2_bitcount     <= 4'd0;
  end else begin
    case (dut2_state)
      1'b0: begin
              rffe_ad9866_sclk <= 1'b0;
              dut2_bitcount    <= 4'd15;
              if (start) begin               // start SPI with datain
                dut2_data <= datain;
                rffe_ad9866_sen_n <= 1'b0;
                dut2_state <= 1'b1;
              end else begin
                rffe_ad9866_sen_n <= 1'b1;
              end
            end

      1'b1: begin
              dut2_state <= 1'b1;
              if ((!rffe_ad9866_sclk)) begin
                rffe_ad9866_sclk <= 1'b1;
              end else begin
                dut2_data <= {dut2_data[15-1:0], sdo};
                dut2_bitcount <= (dut2_bitcount - 4'h1);
                rffe_ad9866_sclk <= 1'b0;
                if ((dut2_bitcount == 4'd0)) begin
                  dut2_state <= 1'b0;
                end
              end
            end
    endcase
  end
end
end

endmodule
