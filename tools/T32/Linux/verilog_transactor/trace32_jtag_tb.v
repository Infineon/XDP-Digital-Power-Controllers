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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

module trace32_tb;

wire tck, tdi, tdo, tms, ntrst; //wires to connect modules
wire nreset_dp; //nreset from probe
wire trigger_dp; //trigger from probe
reg nreset_ta; //nreset from target
reg power; //power from target
reg trigger_ta; //trigger from target
reg runline_i; //runline from target
reg rtck_i;

initial begin
	power = 1;
	nreset_ta = 1;
	trigger_ta = 0;
	runline_i = 0;
	rtck_i = 0;
end

always @ (nreset_dp)
	nreset_ta <= nreset_dp;

trace32_transactor_jtag_debugport_v1 t32jtag(tck, tms, tdi, tdo, ntrst, nreset_dp, nreset_ta, power, trigger_ta, trigger_dp, runline_i, rtck_i);
defparam t32jtag.instance_index = 0;
defparam t32jtag.has_runline_i = 0;
defparam t32jtag.has_trigger_i = 0;
defparam t32jtag.has_rtck_i = 0;

tap_basic tap(tms, tck, ntrst, tdi, tdo);

//always @ (cpuclk, tck)
//$monitor ("%0t\tcpuclk %b\ttck %b", $time, cpuclk, tck);

//always @ (tck, tdi, tms, tdo, ntrst, nreset_dp)
//$monitor ("%0t\ttck %b\ttdi %b\ttms %b\ttdo %b\tntrst %b\tnreset %b", $time, tck, tdi, tms, tdo, ntrst, nreset_dp);

endmodule
