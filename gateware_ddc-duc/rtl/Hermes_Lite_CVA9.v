//
//  Hermes Lite Core Wrapper for BeMicro CV
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
//
// (C) Steve Haynal KF7O 2014
//

module Hermes_Lite(
	input AD9866clk,
	input extreset,        // Pin H18, Push SW S1 Active"L"

	output [3:0] leds,
	output exp_ptt_n,      // Pin U16
	output txenb,          // PA Bias and OPA2677 power
	output [5:0] userout,

	inout [11:0] ad9866_adio,
	output ad9866_rxen,
	output ad9866_rxclk,
	output ad9866_txen,
	output ad9866_txclk,

	output ad9866_sclk,
   output ad9866_sdio,
   output ad9866_sen_n,
   output ad9866_rst_n,  // U13
	output ad9866_mode,
	output ad9866_pga5,

	// ----------------------------------------
	//  SDR-3 I2S I/F, Tx/Rx IQ data
	// ----------------------------------------
	output	CBCLK,	// I2S BCK ; 3072kHz
	output	CLRCIO,	// I2S LRCK ; 48kHz
	output	CDIN,		// I2S Data Out
	input		CDOUT,	// I2S Data In

   // -------------------------------------------
   //  SDR-3 I2C I/F, NCO setting. other control
   // -------------------------------------------
	inout nco_sda,
   input nco_scl
);

assign txenb = exp_ptt_n ;
assign ad9866_pga5 = 1'b0 ;
assign ad9866_mode = 1'b0 ; // Half-duplex

hermes_lite_core hermes_lite_core_inst(
	.AD9866clkX1(AD9866clk),
 	.extreset(extreset),

	.leds(leds),
	.exp_ptt_n(exp_ptt_n),
	.userout(userout),

	.ad9866_adio(ad9866_adio),
	.ad9866_rxen(ad9866_rxen),
	.ad9866_rxclk(ad9866_rxclk),
	.ad9866_txen(ad9866_txen),
	.ad9866_txclk(ad9866_txclk),

	.ad9866_sclk(ad9866_sclk),
	.ad9866_sdio(ad9866_sdio),
	.ad9866_sen_n(ad9866_sen_n),
	.ad9866_rst_n(ad9866_rst_n),
 
	.CBCLK(CBCLK),		// I2S BCK ; 3072kHz
	.CLRCIO(CLRCIO),	// I2S LRCK ; 48kHz
	.CDIN(CDIN),		// I2S Data Out
	.CDOUT(CDOUT),		// I2S Data In

   .nco_sda(nco_sda),
   .nco_scl(nco_scl)
); 
endmodule 
