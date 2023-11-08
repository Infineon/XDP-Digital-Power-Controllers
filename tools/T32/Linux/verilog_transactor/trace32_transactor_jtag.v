//
//
//
//	TRACE32 PowerView - Verilog VPI JTAG Transactor
//
//  $Date: $
//  $Revision: $
//
//	initial release
//	2011/09/19
//		- support VPI Verilog Interface on Linux machines 
//		- tested with open source "GPL Cver v2.12a : http://sourceforge.net/projects/gplcver/files/gplcver/
//		- support JTAG with ARM RTCK
//		- reset line input/output
//		- exact virtual runtime counter by target trigger or run line
//		- trigger line from/to target
//		- power line signal
//      2011/10/25
//              - introduce version parameter to have warning when versions are not compatible
//              - model maximum jtag frequency
//      2011/11/02
//              - fix jtag frequency change feedback
//              - fix NRESET, NTRST level
//              - set output signals initial to 'z'
//      2012/10/09
//              - rename files from *actuator* to *transactor*
//      2013/04/08
//              - rename "instance" to "instance_index" to avoid keyword conflict
//
//	usage:
//		- the verilog simulation must use an instance of an trace32_transactor_jtag_debugport_v1 or an trace32_transactor_jtag_jtag module
//		  that can be fould in trace32_transactor.v . The modules will enable Trace32 PowerView to control the JTAG port by the Verilog VPI interface.
//		- a sample is done trace32_tb.v
//
//	
//	execution:
//		- there is no specific start order of the simulator or Trace32 PowerView
//		- Verilog Simulator:
//			- the shared library needs to be loaded and linked with simulator
//			- the approach may depend to the simulation environment some sample can be found here:
//			  http://www.asic-world.com/verilog/pli6.html#Linking_with_Simulator
//			- a sample script can be found in ./run.sh
//		- Trace32 PowerView:
//			- config.t32 needs to be modified in order to use HostMCI debug interface, therefore the connection to host must be "PBI=MCILIB"
//			- start Trace32 PowerView by t32m????? -c config.t32
//			- use debug port VerilogTransactor0 by the command: "SYS.CONFIG.DEBUGPORT VA0"
//			- use system.jtagclock to setup the virtual JTAG Frequency or RTCK mode
//
//  (c) Lauterbach GmbH
//  http://www.lauterbach.com/
//



module trace32_transactor_jtag_debugport_v1(
	//JTAG Header
	tck_o, //to TCK pad of CPU
	tms_o, //to TMS pad of CPU
	tdi_o, //to TDI pad of CPU
	tdo_i, //from TDO pad of CPU
	ntrst_o, // TRST (when "0" TAP reset from debugger is asserted, may be not connected)
	//Additional Target Signals
	nreset_o, // hardreset out (when "0" system reset from debugger is asserted)
	nreset_i, // reset detection (when "0" reset is asserted)
	power_i,  //power (when "1" JTAG connector is powered)
	trigger_i, //trigger line from target (triggers at 0->1 transitions)
	trigger_o, //trigger line to target (when "1" trigger is asserted)
	runline_i, //run line from target, used to have an accurate runtime counter (when "1" cpu is running)
	rtck_i //from RTCK pad of target
);

parameter instance_index = -1; //must be overwritten by value 0 to activate this instance
parameter has_reset_i = 1; //set to 0 if reset_i is not available
parameter has_power_i = 0; //set to 1 if power_i is available
parameter has_trigger_i = 0; //set to 1 if trigger_i is available
parameter has_runline_i = 0; //set to 1 if runline_i is available
parameter has_rtck_i = 0; //set to 1 if rtck_i not available
parameter poll_frequency_hz = 1000; //The connection to Trace32 PowerView is polled periodically in case a new command is expected. An higher poll_frequency decrease the communication latency after longer pauses, but will consume more simulation time.
parameter version = 4; //don't touch, it's used to make sure that this file matches to the VPI code

output tck_o, tms_o, tdi_o, ntrst_o, nreset_o, trigger_o;
input  tdo_i, nreset_i, power_i, trigger_i, runline_i,rtck_i;
reg tck_o, tms_o, tdi_o, ntrst_o,trigger_o, nreset_o;
wire tdo_i, power_i, trigger_i, nreset_i, runline_i, rtck_i;

reg resetdetect; //FF to detect reset edges
reg powerlossdetect; //FF to detect power loss
reg triggerdetect; //FF to detect trigger events
reg waittimesignal; //internal signal to get called after a certain has elapsed
reg shifttimeoutsignal; //internal signal to get called after a certain has elapsed

initial begin
	tck_o = 1'bz;
	tms_o = 1'bz;
	tdi_o = 1'bz;
	ntrst_o = 1'bz;
	nreset_o = 1'bz;
	trigger_o = 1'bz;
	waittimesignal = 0;
	shifttimeoutsignal = 0;
	powerlossdetect = 0;
	resetdetect = 0;
	triggerdetect = 0;
	if (instance_index == 0)
		$trace32_transactor_jtag_init(version, tck_o, tms_o, tdi_o, tdo_i, ntrst_o, nreset_o, 
	trigger_o, nreset_i, resetdetect, power_i, powerlossdetect, trigger_i, triggerdetect, runline_i, rtck_i,
	has_reset_i, has_power_i, has_trigger_i, has_runline_i, has_rtck_i, waittimesignal, shifttimeoutsignal, poll_frequency_hz
	);
	if (instance_index == 0)
		#100 waittimesignal <= 1;
end


always @ (runline_i)
	$trace32_transactor_jtag_on_runtime_counter_check_runsignal;

always @ (negedge power_i && power_i == 0)
	powerlossdetect <= 1;

always @ (posedge trigger_i && trigger_i == 1)
begin
	$trace32_transactor_jtag_on_runtime_counter_trigger_off;
	triggerdetect <= 1;
end

always @ (negedge nreset_i && nreset_i == 0)
	resetdetect <= 1;

always @ (waittimesignal)
begin
	//$monitor("waittimesignal %b", waittimesignal);
	$trace32_transactor_jtag_on_waittimesignal;
end

always @ (shifttimeoutsignal)
	$trace32_transactor_jtag_on_shifttimeoutsignal;

always @ (rtck_i)
	$trace32_transactor_jtag_on_rtck;

endmodule

module trace32_transactor_jtag(
	//JTAG Header
	tck_o, //to TCK pad of CPU
	tms_o, //to TMS pad of CPU
	tdi_o, //to TDI pad of CPU
	tdo_i, //from TDO pad of CPU
	ntrst_o // TRST (when "0" TAP reset from debugger is asserted, may be not connected)
);

parameter instance_index = -1; //must be overwritten by value 0 to activate this instance
parameter poll_frequency_hz = 1000; //The connection to Trace32 PowerView is polled periodically in case a new command is expected. An higher poll_frequency decrease the communication latency after longer pauses, but will consume more simulation time.

input tdo_i;
output tck_o, tms_o, tdi_o, ntrst_o;

wire nreset_o, trigger_o;
reg nreset_i, power_i, trigger_i, runline_i, rtck_i;

trace32_transactor_jtag_debugport_v1 port(tck_o, tms_o, tdi_o, tdo_i, ntrst_o, nreset_o, nreset_i, power_i, trigger_i, trigger_o, runline_i, rtck_i);

defparam port.has_reset_i = 0;
defparam port.has_power_i = 0;
defparam port.has_trigger_i = 0;
defparam port.has_runline_i = 0;
defparam port.has_rtck_i = 0;
defparam port.instance_index = instance_index;
defparam port.poll_frequency_hz = poll_frequency_hz;

initial begin
	nreset_i =  1'bx;
	power_i = 1'bx;
	trigger_i = 1'bx;
	runline_i = 1'bx;
	rtck_i = 1'bx;
end

endmodule
