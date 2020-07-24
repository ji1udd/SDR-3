#**************************************************************
# Time Information
#**************************************************************
set_time_format -unit ns -decimal_places 3


#**************************************************************************************
# Create Clock
#**************************************************************************************
# externally generated clocks (with respect to the FPGA)
#
create_clock -name AD9866clk -period 73.728MHz [get_ports {AD9866clk}]


#*************************************************************************************
# Create Generated Clock
#*************************************************************************************
# internally generated clocks
#
#derive_pll_clocks -use_net_name
#derive_clock_uncertainty


#**************************************************************
# Create Generated Clock (internal to the FPGA)
#**************************************************************
# NOTE: Whilst derive_pll_clocks constrains PLL clocks if these are connected to an FPGA output pin then a generated
# clock needs to be attached to the pin and a false path set to it

create_generated_clock -divide_by 24 -source AD9866clk -name BCLK {hermes_lite_core:hermes_lite_core_inst|Hermes_clk_lrclk_gen:clrgen|BCLK}
#create_generated_clock -divide_by 6 -source AD9866clk -name MCLK {hermes_lite_core:hermes_lite_core_inst|Hermes_clk_lrclk_gen:clrgen|MCLK}

derive_clock_uncertainty

#************************************************************** 
# Set Input Delay
#**************************************************************
# If setup and hold delays are equal then only need to specify once without max or min 


#**************************************************************
# Set Output Delay
#**************************************************************
# If setup and hold delays are equal then only need to specify once without max or min 


#*************************************************************************************
# Set Clock Groups
#*************************************************************************************

set_clock_groups -asynchronous \
					-group { AD9866clk BCLK }


#**************************************************************
# Set Maximum Delay
#************************************************************** 


#**************************************************************
# Set Minimum Delay
#**************************************************************


#**************************************************************
# Set False Paths
#**************************************************************
#set_false_path -from * -to {leds[*] userout[*] exp_ptt_n}
#set_false_path -from {extreset dipsw[*]} -to *
#set_false_path -from * -to [get_ports {txenb}]
set_output_delay -clock AD9866clk 4 [get_ports {leds[*]}]
set_output_delay -clock AD9866clk 4 [get_ports {userout[*]}]
set_output_delay -clock AD9866clk 4 [get_ports {exp_ptt_n}]
set_output_delay -clock AD9866clk 4 [get_ports {txenb}]

set_false_path -from * -to [get_ports {ad9866_mode}]
set_false_path -from * -to [get_ports {ad9866_pga5}]

set_input_delay  -clock AD9866clk 4 [get_ports {extreset}]
set_output_delay -clock AD9866clk 4 [get_ports {ad9866_sclk}]
set_output_delay -clock AD9866clk 4 [get_ports {ad9866_sdio}]
set_output_delay -clock AD9866clk 4 [get_ports {ad9866_sen_n}]
set_output_delay -clock AD9866clk 4 [get_ports {ad9866_rst_n}]
set_output_delay -clock AD9866clk 4 [get_ports {CBCLK}]
set_output_delay -clock AD9866clk 4 [get_ports {CLRCIO}]
set_output_delay -clock AD9866clk 4 [get_ports {CDIN}]
set_input_delay  -clock AD9866clk 4 [get_ports {CDOUT}]
set_input_delay  -clock AD9866clk 4 [get_ports {nco_scl}]
set_input_delay  -clock AD9866clk 4 [get_ports {nco_sda}]
set_output_delay -clock AD9866clk 4 [get_ports {nco_sda}]

## AD9866 RX Path
## rxen is not time critical and is on 48 MHz clock
set_output_delay -add_delay -max -clock AD9866clk -reference_pin [get_ports ad9866_rxclk] 1 [get_ports {ad9866_rxen}]
#set_output_delay -add_delay -min -clock AD9866clk -reference_pin [get_ports ad9866_rxclk] -2.5 [get_ports {ad9866_rxen}]
set_output_delay -add_delay -min -clock AD9866clk -reference_pin [get_ports ad9866_rxclk] -2 [get_ports {ad9866_rxen}]

#set_input_delay -add_delay -max -clock AD9866clk -reference_pin [get_ports ad9866_rxclk] 1.9 [get_ports {ad9866_adio[*]}]
#set_input_delay -add_delay -min -clock AD9866clk -reference_pin [get_ports ad9866_rxclk] -0.6 [get_ports {ad9866_adio[*]}]
set_input_delay -add_delay -max -clock AD9866clk -reference_pin [get_ports ad9866_rxclk] 1 [get_ports {ad9866_adio[*]}]
set_input_delay -add_delay -min -clock AD9866clk -reference_pin [get_ports ad9866_rxclk] -2.5 [get_ports {ad9866_adio[*]}]


## AD9866 TX Path
## txen is not time critical and is on 48 MHz clock
set_output_delay -add_delay -max -clock AD9866clk -reference_pin [get_ports ad9866_txclk] 1 [get_ports {ad9866_txen}]
set_output_delay -add_delay -min -clock AD9866clk -reference_pin [get_ports ad9866_txclk] -2.5 [get_ports {ad9866_txen}]

set_output_delay -add_delay -max -clock AD9866clk -reference_pin [get_ports ad9866_txclk] 1 [get_ports {ad9866_adio[*]}]
set_output_delay -add_delay -min -clock AD9866clk -reference_pin [get_ports ad9866_txclk] -2.5 [get_ports {ad9866_adio[*]}]

set_false_path -from [get_pins {ad9866_adio[*]~output|o}] -to [get_pins {ad9866_adio[*]~input|i}]
