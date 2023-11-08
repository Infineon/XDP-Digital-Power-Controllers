`include "tap_top.v"
`include "trace32_transactor_jtag.v"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

module tap_basic(// JTAG pads
                tms_pad_i,  //TMS pad of TAP
                tck_pad_i,  //TCK pad of TAP
                ntrst_pad_i, //NTRST pad of TAP
                tdi_pad_i, //TDI pad of TAP
                tdo_pad_o //TDO pad of TAP
           );

input tms_pad_i, tck_pad_i, ntrst_pad_i, tdi_pad_i;
output tdo_pad_o;
wire ntrst_to_tap;
assign ntrst_to_tap = !ntrst_pad_i;

tap_top tapex(
                // JTAG pads
                tms_pad_i, 
                tck_pad_i, 
                ntrst_to_tap, 
                tdi_pad_i, 
                tdo_pad_o, 
                tdo_padoe_o,

                // TAP states
                shift_dr_o,
                pause_dr_o, 
                update_dr_o,
                capture_dr_o,
                
                // Select signals for boundary scan or mbist
                extest_select_o, 
                sample_preload_select_o,
                mbist_select_o,
                debug_select_o,
                
                // TDO signal that is connected to TDI of sub-modules.
                tdo_o, 
                
                // TDI signals from sub-modules
                debug_tdi_i,    // from debug module
                bs_chain_tdi_i, // from Boundary Scan Chain
                mbist_tdi_i     // from Mbist Chain
              ); 
endmodule

module tap_basic_rtck(
                clk_i, //CPU clock to sample TCK
                tms_i, //TMS pad of TAP
                tck_i, //TCK pad of TAP
                ntrst_i, //NTRST pad of TAP
                tdi_i, //TDI pad of TAP
                tdo_o, //TDO pad of TAP
                rtck_o //RTCK pad of TAP
                );

input clk_i, tms_i, tck_i, ntrst_i, tdi_i;
output tdo_o, rtck_o;

tap_basic sub_tap(tms_clk, tck_clk, ntrst_i, tdi_clk, tdo_o);

reg tck1, tck2, tck3, tck4, tms_clk, tdi_clk;
wire sample_tdi_tms, rtck_o, tck_clk;

assign tck_clk = !tck2 & tck3;        //rising
assign sample_tdi_tms = tck2 & !tck3; //falling
assign rtck_o = tck4 | tck3;

always @ (posedge clk_i)
begin
  tck1 <= tck_i;
  tck2 <= tck1;
  tck3 <= tck2;
  tck4 <= tck3;
  if (sample_tdi_tms == 1)
  begin
	tms_clk <= tms_i;
	tdi_clk <= tdi_i;
  end
end

endmodule

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

module trace32_tb;

wire tck, tdi, tdo, tms, ntrst, rtck_ta; //wires to connect modules
wire nreset_dp; //nreset from probe
wire trigger_dp; //trigger from probe
reg nreset_ta; //nreset from target
reg power; //power from target
reg trigger_ta; //trigger from target
reg runline_i; //runline from target
reg cpuclk; //cpuclk inside target to sample tck and generate rtck

initial begin
	power = 1;
	nreset_ta = 1;
	trigger_ta = 0;
	runline_i = 0;
	cpuclk = 0;
end

always
//	#500 cpuclk <= !cpuclk; //1000khz
	#5000 cpuclk <= !cpuclk; //100khz

always @ (nreset_dp)
	nreset_ta <= nreset_dp;

trace32_transactor_jtag_debugport_v1 t32jtag(tck, tms, tdi, tdo, ntrst, nreset_dp, nreset_ta, power, trigger_ta, trigger_dp, runline_i, rtck_ta);
defparam t32jtag.instance_index = 0;
defparam t32jtag.has_runline_i = 0;
defparam t32jtag.has_trigger_i = 0;
defparam t32jtag.has_rtck_i = 1;

tap_basic_rtck tap(cpuclk, tms, tck, ntrst, tdi, tdo, rtck_ta);

//always @ (cpuclk, tck, rtck_ta)
//$monitor ("%0t cpuclk %b, tck %b , rtck_ta %b", $time, cpuclk, tck, rtck_ta);

//always @ (tck, rtck_ta, tdi, tms, tdo, ntrst, nreset_dp)
//$monitor ("tck %b, rtck_ta %b, tdi %b, tms %b, tdo %b, ntrst %b, nreset %b", tck, rtck_ta, tdi, tms, tdo, ntrst, nreset_dp);

endmodule
