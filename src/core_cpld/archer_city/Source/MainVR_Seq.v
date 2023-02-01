
//////////////////////////////////////////////////////////////////////////////////
/*!
 
	\brief    <b>%PSU Sequence Control </b> 
	\file     MainVR_Seq.v 
	\details    <b>Image of the Block:</b>
				\image html  MainVR_Seq.png

				 <b>Description:</b> \n
				This module controls the Main Vr's and AUX SW ON/OFF using the PWRGD_PS_PWROK and iMainVRPwrEn from Mstr_Seq.v; 
                moreover, the turning ON/OFF is generated by oMainVRPwrgd; the fault condition is produced by oMainPwrFlt. 

                Vr's 
                P3V3 Main
                P5V0 Main

                SW
                AUX_SW_EN

                PLD FM_P5V_EN - >EN P5V VR PWRGD -> EN P3V3 PWRGD -> PLD

                \image html  MainVR_Seq_di.png
               
				
	\brief  <b>Last modified</b> 
			$Date:   Jan 18, 2018 $
			$Author:  David.bolanos@intel.com $			
			Project			: Wilson City RP  
			Group			: BD
	\version    
			 20181901 \b  David.bolanos@intel.com - File creation\n
			   
	\copyright Intel Proprietary -- Copyright \htmlonly <script>document.write(new Date().getFullYear())</script> \endhtmlonly Intel -- All rights reserved
*/
//////////////////////////////////////////////////////////////////////////////////

module MainVR_Seq
(
	input			 iClk,//%Clock input.
	input			 iRst_n,//%Reset enable on high.

	input            iGoOutFltSt,//% Go out fault state.
	input            iMainVRPwrEn,//% Turn on Main VR's.
	input            PWRGD_PS_PWROK,//% PWRGD PS PWROK .

    input            PWRGD_P3V3,//% Pwrgd from P3V3 VR 

    output  reg      oMainPwrFlt,//% Fault condition.
    output  reg      oP3v3PwrFlt, //% Fault P3V3 VR
    output  reg      oMainVRPwrgd,//% Main Vr's Enable
   
	output reg       FM_AUX_SW_EN,//% 12V Only PSU Control  AUX to Main SW
	output reg       FM_P5V_EN//% P5V and P3V3 main Voltage enable

);


//////////////////////////////////////////////////////////////////////////////////
// Parameters
//////////////////////////////////////////////////////////////////////////////////

	parameter  LOW =1'b0;
	parameter  HIGH=1'b1;


//////////////////////////////////////////////////////////////////////////////////
// Internal Signals
//////////////////////////////////////////////////////////////////////////////////

reg  rPwrgd_P5V_P3V3_ff1;



//////////////////////////////////////////////////////////////////////////////////
// Secuencial Logic
//////////////////////////////////////////////////////////////////////////////////

always @ ( posedge iClk) 
	begin 
		if (  !iRst_n  )   
			begin

				oMainVRPwrgd        	<= LOW;
				oP3v3PwrFlt  			<= LOW;  					
				rPwrgd_P5V_P3V3_ff1 	<= LOW;
				FM_AUX_SW_EN        	<= LOW;
				FM_P5V_EN           	<= LOW;
				oMainPwrFlt         	<= LOW;
									
			end 
		else if ( iGoOutFltSt )
			begin				
				oMainPwrFlt 	   		<= LOW;
				oP3v3PwrFlt        		<= LOW;
			end
		else 
			begin	
								
				FM_AUX_SW_EN         	<= PWRGD_PS_PWROK;	 					
				FM_P5V_EN 	         	<= iMainVRPwrEn;    // P5V power good enables P3V3	
		
				oMainVRPwrgd         	<= PWRGD_P3V3;

				rPwrgd_P5V_P3V3_ff1  	<= PWRGD_P3V3;

				oP3v3PwrFlt       		<= (FM_P5V_EN      && (!PWRGD_P3V3)      && rPwrgd_P5V_P3V3_ff1 ) ? HIGH: oP3v3PwrFlt; 
				oMainPwrFlt       		<= (PWRGD_PS_PWROK && (oP3v3PwrFlt ))                             ? HIGH: oMainPwrFlt; 	


			end
	end  



//////////////////////////////////////////////////////////////////////
// Instances 
///////////////////////////////////////////////////////////////////////


endmodule 