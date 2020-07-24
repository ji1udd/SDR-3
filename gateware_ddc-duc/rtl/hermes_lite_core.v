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

// (C) Phil Harman VK6APH, Kirk Weedman KD7IRS  2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 
// (C) Steve Haynal KF7O 2014, 2015


// This is a port of the Hermes project from www.openhpsdr.org to work with
// the Hermes-Lite hardware described at http://github.com/softerhardware/Hermes-Lite.
// It was forked from Hermes V2.5 but kept up to date to v3.1

module hermes_lite_core(
  input AD9866clkX1,
  input extreset,            // initialize AD9866 reset generator

  output [3:0] leds, 
  output exp_ptt_n,
  output [5:0] userout,

  // AD9866 Half Duplex mode
  inout [11:0] ad9866_adio,
  output ad9866_rxen, 
  output ad9866_rxclk,
  output ad9866_txen,
  output ad9866_txclk,

  output ad9866_sclk,
  output ad9866_sdio,
  output ad9866_sen_n,

  output ad9866_rst_n,

  // ----------------------------------------
  //  SDR-3 I2S I/F, Tx/Rx IQ data
  // ----------------------------------------
  output	CBCLK,	// I2S BCK ; 3072kHz
  output	CLRCIO,	// I2S LRCK ; 48kHz
  output	CDIN,		// I2S Data Out
  input	CDOUT,	// I2S Data In

  // -------------------------------------------
  //  SDR-3 I2C I/F, NCO setting. other control
  // -------------------------------------------
  inout nco_sda,
  input nco_scl
);

parameter CLK_FREQ = 73728000;
localparam RATE48  = 6'd24;
localparam CICRATE = 6'd8;
localparam GBITS   = 31 ;
localparam RRRR    = 192 ;

assign exp_ptt_n   = FPGA_PTT;


//----------------------------
//  Control data
//-----------------------------
wire [7:0] i2cReg4, i2cReg5, i2cReg6, i2cReg7 ;
wire       ic2Reg6req, ic2Reg5req ;

wire       FPGA_PTT  = i2cReg4[0] ;   // 1: Tx , 0: Rx
wire [5:0] RxGain    = i2cReg5[5:0] ; // RxGain 0dB=6'd12, +6dB=6'd18
wire [3:0] TxGain    = i2cReg6[3:0] ; // 0: -7.5dB(Min) , 15: 0dB(Max)
assign     userout   = i2cReg7[5:0] ; // Select LPF

wire       RxGainReq = ic2Reg5req ;
wire       TxGainReq = ic2Reg6req ;


//--------------------------------------------------------------
// Reset/Clock
//--------------------------------------------------------------
reg[2:0] clkdiv8 ;
always @(posedge AD9866clkX1)
  clkdiv8 <= clkdiv8 + 1'b1 ;

//wire clk_div2_enb = clkdiv8[0] ;
wire clk_div8_enb = ( clkdiv8 == 3'b111 ) ;

wire C122_rst;
wire IF_rst = !extreset ;          // hold code in reset until PLLs are locked

cdc_sync #(1) reset_C122 (
  .siga(IF_rst),
  .rstb(IF_rst),
  .clkb(AD9866clkX1),
  .sigb(C122_rst)                   // 122.88MHz clock domain reset
);

wire CLRCLK;
wire C122_cbclk, C122_cbrise, C122_cbfall;
Hermes_clk_lrclk_gen #(.CLK_FREQ(CLK_FREQ)) clrgen (
  .reset(C122_rst),
  .CLK_IN(AD9866clkX1),
  .BCLK(C122_cbclk),
  .Brise(C122_cbrise),
  .Bfall(C122_cbfall),
  .LRCLK(CLRCLK),
  .LRrise(),
  .LRfall(),
  .MCLK(),
  .MCLKrise()
);

assign CBCLK   = C122_cbclk ;	// BCLK 73728000/48000/64 = 3072000 Hz
assign CLRCIO  = CLRCLK;		// LRCK 48 kHz


// -----------
// AD9866 Code
// -----------
assign ad9866_txen  = FPGA_PTT;
assign ad9866_rxen  =~FPGA_PTT;
assign ad9866_rxclk = AD9866clkX1;
assign ad9866_txclk = AD9866clkX1;
assign ad9866_adio  = FPGA_PTT ? DACDp : 12'bZ;

// ----
//  Rx
// ----
reg [11:0]temp_ADC;
always @ (posedge AD9866clkX1)
//  temp_ADC <= FPGA_PTT ? DACD_tx_mon : ad9866_adio;
  temp_ADC <= ad9866_adio;

// for LEDs and AGC
wire rxclipp    = (temp_ADC == 12'b011111111111);
wire rxclipn    = (temp_ADC == 12'b100000000000);
//wire rxnearclip = (temp_ADC[11:8] == 4'b0111) | (temp_ADC[11:8] == 4'b1000);
wire rxgoodlvlp = (temp_ADC[11:9] == 3'b011);
wire rxgoodlvln = (temp_ADC[11:9] == 3'b100);

//------------------------------------------------------------------------------
//                 Receiver code
//------------------------------------------------------------------------------
wire      [31:0] nco_phase_word;
wire      [23:0] rx_I ;
wire      [23:0] rx_Q ;
wire             strobe ;

reg [11:0] adcpipe ;
always @ (posedge AD9866clkX1) begin
  adcpipe <= temp_ADC;
end

receiver #(.CICRATE(CICRATE)) receiver_inst (
  .clock(AD9866clkX1),
  .rate(RATE48),              //  48ksps
  .frequency(nco_phase_word),
  .out_strobe(strobe),
  .in_data(adcpipe),
  .out_data_I(rx_I),
  .out_data_Q(rx_Q)
);

reg [23:0] filter_out_I ;
reg [23:0] filter_out_Q ;
always @(posedge AD9866clkX1)
  if (strobe)
    begin
      filter_out_I <= rx_I ;
      filter_out_Q <= rx_Q ;
    end
	
wire [23:0] i2s_tx_I = FPGA_PTT ? { {8{i2s_rx_I[15]}},i2s_rx_I} : filter_out_I ; 
wire [23:0] i2s_tx_Q = FPGA_PTT ? { {8{i2s_rx_Q[15]}},i2s_rx_Q} : filter_out_Q ; 
//wire [23:0] i2s_tx_I = FPGA_PTT ? { i2s_rx_I, 8'b0} : filter_out_I ; 
//wire [23:0] i2s_tx_Q = FPGA_PTT ? { i2s_rx_Q, 8'b0} : filter_out_Q ; 

I2S_xmit_24b2 LR (
  .rst(C122_rst),
  .lrclk(CLRCLK),
  .clk(AD9866clkX1),
  .CBrise(C122_cbrise),
  .CBfall(C122_cbfall),
  .sample({i2s_tx_Q, i2s_tx_I}),
  .outbit(CDIN)
);

//---------------------------------------------------------
//                 Transmitter code 
//--------------------------------------------------------- 
wire [31:0] C122_mic_LR;
wire        C122_mic_rdy;

I2S_rcv #(32,2,1) MIC (
  .xrst(C122_rst),
  .xclk(AD9866clkX1),
  .BCLK(CBCLK),
  .LRCLK(CLRCLK),
  .din(CDOUT),
  .xData(C122_mic_LR),        // 16bit * 2ch
  .xData_rdy(C122_mic_rdy),
  .xlrclk(),
  .xBrise(),
  .xBfall(),
  .xLRrise(),
  .xLRfall()
);

reg [15:0] i2s_rx_I ;
reg [15:0] i2s_rx_Q ;
always @(posedge AD9866clkX1)
  if (C122_mic_rdy)
    begin
      i2s_rx_I <= C122_mic_LR[15:0] ;
      i2s_rx_Q <= C122_mic_LR[31:16] ;
    end

wire signed [15:0] C122_I_PWM = i2s_rx_I ; // Tx I data
wire signed [15:0] C122_Q_PWM = i2s_rx_Q ; // Tx Q data

reg  signed [15:0] C122_fir_i;
reg  signed [15:0] C122_fir_q;
always @ (posedge AD9866clkX1) begin
  if (req1) begin 
    C122_fir_i = C122_I_PWM;
    C122_fir_q = C122_Q_PWM;
  end 
end

wire req1, req2;
wire [19:0] y1_r, y1_i;
wire [15:0] y2_r, y2_i;

FirInterp8_1024 fi (
  AD9866clkX1,
  req2,                   // req2 enables an output sample.
  req1,                   // req1 requests next input sample.
  C122_fir_i, C122_fir_q,
  y1_r, y1_i
);

CicInterpM5 #(.RRRR(RRRR), .IBITS(20), .OBITS(16), .GBITS(GBITS)) in2 (
  AD9866clkX1,
  1'd1,
  req2,
  y1_r, y1_i,
  y2_r, y2_i
);

//--------------
//  CORDIC NCO 
//--------------
wire signed [14:0] C122_cordic_i_out;
wire signed [14:0] C122_cordic_q_out;

cpl_cordic #(.OUT_WIDTH(16)) cordic_inst (
  .clock(AD9866clkX1),
  .frequency(nco_phase_word),
  .in_data_I(y2_i),
  .in_data_Q(y2_r),
  .out_data_I(C122_cordic_i_out),
  .out_data_Q(C122_cordic_q_out)
);

wire signed [15:0] txsum  = (C122_cordic_i_out >>> 2);

reg [11:0] DACD;
always @ (posedge AD9866clkX1)
  DACD <= txsum[11:0];

reg [11:0] DACDp;
always @ (negedge AD9866clkX1)
  DACDp <= DACD;

// for LEDs
wire txclipp    = (C122_cordic_i_out[13:2] == 12'b011111111111);
wire txclipn    = (C122_cordic_i_out[13:2] == 12'b100000000000);
wire txgoodlvlp = (C122_cordic_i_out[13:11] == 3'b011);
wire txgoodlvln = (C122_cordic_i_out[13:11] == 3'b100);


//---------------------------------------------------------
// AD9866 Instance
//---------------------------------------------------------
ad9866 ad9866_inst(
  .clk(AD9866clkX1),
  .clkenb(clk_div8_enb),
  .rst(~ad9866_rst_n),
  .rffe_ad9866_sdio(ad9866_sdio),
  .rffe_ad9866_sclk(ad9866_sclk),
  .rffe_ad9866_sen_n(ad9866_sen_n),
  .i2c_tx_gain_reg(TxGain),           // TxGain[3:0]
  .i2c_rx_gain_reg(RxGain),           // RxGain[5:0]
  .i2c_tx_gain_update_req(TxGainReq), // TxGain update Req
  .i2c_rx_gain_update_req(RxGainReq)  // RxGain update Req
);


reg [15:0] resetcounter;
always @ (posedge AD9866clkX1 or negedge extreset)
  if (~extreset)
    resetcounter <= 16'h00;
  else if (~resetcounter[15] & clk_div8_enb)
    resetcounter <= resetcounter + 16'h01;

assign ad9866_rst_n = resetcounter[15];

// ============================================================================== //
//     LEDs
// ============================================================================== //
// Heartbeat period is 1.1Hz
reg [21:0] heartbeat_cnt;
always @(posedge AD9866clkX1)
  if (clk_div8_enb)                    // 1.1 = 73728000/2^26 
    heartbeat_cnt <= heartbeat_cnt + 1'b1;

assign leds[3] = heartbeat_cnt[21];    // Heartbeat
assign leds[2] = ~FPGA_PTT ;           // Tx

// LED ON period is 0.2 seconds at 73.728 MHz clock
localparam half_second = 24'd14745600; // 0.2 = 14745600/73728000
//Led_flash Flash_LED1(.clock(AD9866clkX1), .signal(rxclipp | rxclipn | txclipp | txclipn), .LED(leds[1]), .period(half_second));
//Led_flash Flash_LED0(.clock(AD9866clkX1), .signal(rxgoodlvlp | rxgoodlvln | txgoodlvlp | txgoodlvln), .LED(leds[0]), .period(half_second));
Led_flash Flash_LED1(.clock(AD9866clkX1), .signal(rxclipp | rxclipn), .LED(leds[1]), .period(half_second));
Led_flash Flash_LED0(.clock(AD9866clkX1), .signal(rxgoodlvlp | rxgoodlvln), .LED(leds[0]), .period(half_second));

// ============================================================================== //
//     NCO setting I2C
// ============================================================================== //
i2cSlave i2cSlave(
  .clk(AD9866clkX1),
  .rst(C122_rst),
  .sda(nco_sda),
  .scl(nco_scl),
  .nco_phase_word(nco_phase_word),
  .myReg4(i2cReg4),
  .myReg5(i2cReg5),
  .myReg6(i2cReg6),
  .myReg7(i2cReg7),

  .myReg5update(ic2Reg5req),  // RxGainReq
  .myReg6update(ic2Reg6req)   // TxGainReq
);


// ============================================================================== //

function integer clogb2;
input [31:0] depth;
begin
  for(clogb2=0; depth>0; clogb2=clogb2+1)
  depth = depth >> 1;
end
endfunction

endmodule 
