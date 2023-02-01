//////////////////////////////////////////////////////////////////////////////////
/*!
    \brief      <b>Parameterizable Glitch Filter</b>\n
    \details    Delivers a glitchless signal if the sampled data in all stages\n
                is consistent between each other.\n
    \file       GlitchFilter.v
    \author     amr/carlos.l.bernal@intel.com
    \date       Oct 2, 2011
    \brief      $RCSfile: GlitchFilter.v.rca $
                $Date: Fri Feb 15 17:52:11 2013 $
                $Author: edgarara $
                $Revision: 1.3 $
                $Aliases: Arandas_20130312_1720_006C,Arandas_20130314_1025_006C,Arandas_20130321_1930_0100,Arandas_20130623_1550_0102,BellavistaVRTBCodeBase_20130705_0950,Arandas_20130802_2240_0103,BellavistaVRTB_20130912_1700,PowerupSequenceWorkingMissigPowerDown,Durango-FabA-0x0021,Durango-FabA-0x0022,Durango-FabA-0x0023,Durango-FabA-0x0024,Durango-FabA-0x0025,Durango-FabA-0x0028,Durango-FabA-0x0029,Durango-FabA-0x002A,Durango-FabA-0x002B,Durango-FabA-0x0101,Rev_0x0D,ColimaFabA0x0001,ColimaFabA0x0002,ColimaFabA0x0003,DurangoFabA0x0104,DurangoFabA0x0105,ColimaFabA0x0004,DurangoFabA0x0106,ColimaFabA0x0006,DurangoFabA0x0110,ColimaFabA0x000C,ColimaFabA0x000D,DurangoFabA0x0116,DurangoFabA0x0117,DurangoFabA0x011A,DurangoFabA0x011B,Arandas_20131223_1100_0104,Durango0x0120,Durango0x0123,Durango0x0124,Durango0x0125,Colima0x0011,Durango0x0127,Colima0x0013,Durango0x0128,Durango0x0129,Colima0x0017,Colima0x001A,Colima0x001B,Colima0x002D,Durango0x012D,Durango0x012E,Colima0x001E,Durango0x0133,Durango0x0134,Durango0x0136,Durango0x0137,Durango0x0138,Durango0x013A $
                <b>Project:</b> FPGA coding\n
                <b>Group:</b> PVE-VHG\n
                <b>Testbench:</b> GlitchFilterTB.v\n
                <b>Resources:</b>   <ol>
                                        <li>Spartan3AN
                                            - 4 Slices (Depends on parameters)
                                    </ol>
                <b>Instances:</b>   <ol>

                                    </ol>
                <b>References:</b>  <ol>
                                        <li>Arandas
                                        <li>Summer Valley
                                    </ol>
                <b>Block Diagram:</b>
    \verbatim
        +---------------------------------+
 -----> |> iClk     .   oFilteredSignals  |----->
 -----> |  iRst_n   .       .       .     |
 -----> |  iSignal  .       .       .     |
        +---------------------------------+
                   GlitchFilter
    \endverbatim

    \version
                20120916 \b clbernal - File creation\n
                20130214 \b edgarara - Added Comments and Documentation Format\n
                20160325 \b mdeckar1 - Changed rvSampledData_q reset value to static '0'\n
    \copyright Intel Proprietary -- Copyright 2011 Intel -- All rights reserved
*/

module GlitchFilter #(
	parameter	NUMBER_OF_SIGNALS	= 1,
    parameter   RST_VALUE = {NUMBER_OF_SIGNALS{1'b0}})       //reset value for all filter stages
  (
    input                            iClk, //% Clock Input<br>
    input                            iARst_n,//% Asynchronous Reset Input<br>
    input                            iSRst_n, //Synchronous Reset Input
    input                            iEna, //%enable signal (only executes when this is HIGH)
    input [NUMBER_OF_SIGNALS-1 : 0]  iSignal,//% Input Signals<br>
    output [NUMBER_OF_SIGNALS-1 : 0] oFilteredSignals//%Glitchless Signal<br>
   );
   
   //Internal signals
   reg [NUMBER_OF_SIGNALS-1 : 0]     rFilter;
   reg [NUMBER_OF_SIGNALS-1 : 0]     rFilteredSignals;

   integer                           i;
   
   
   always @(posedge iClk, negedge iARst_n) begin
      if (!iARst_n) begin //asynch active-low Reset condition
         rFilter			<= RST_VALUE;     //{NUMBER_OF_SIGNALS{1'b0}};
         rFilteredSignals	<= RST_VALUE;     //{NUMBER_OF_SIGNALS{1'b0}};
      end
      else begin
         if (!iSRst_n)
           begin
              rFilteredSignals <= RST_VALUE;
              rFilter          <= RST_VALUE;
           end
         else if (iEna)          //if this module requires a slower than core clock, we generate a pulse with proper 
           begin            //frequency and feed to iEna input signal, otherwise it can be HIGH all the time
	          rFilter <= iSignal; //Input signal flip flop

              for (i=0; i<=NUMBER_OF_SIGNALS-1; i=i+1)
                begin
	               if (iSignal[i] == rFilter[i]) //if previous and current signal are the same output is enabled
                     begin 
		                rFilteredSignals[i] <= rFilter[i];
                     end
	            end
           end // if (iEna)
         else
           begin
              rFilteredSignals <= rFilteredSignals;
           end
         
      end // else: !if(!iARst_n)
   end // always @ (posedge iClk, negedge iARst_n)
   
   //Output assignment
   assign oFilteredSignals = rFilteredSignals;
   
endmodule
