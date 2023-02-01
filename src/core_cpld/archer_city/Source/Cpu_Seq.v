//////////////////////////////////////////////////////////////////////////////////
/*!

	\brief    <b>%Cpu Sequence Control </b> 
	\file     Cpu_Seq.v
	\details    <b>Image of the Block:</b>
				\image html  Cpu_Seq.png

				 <b>Description:</b> \n

				

                The Cpu_Seq control the PVCCIO, P1V8_PCIE, PVCCANA, PVCCIN and PVCCSA VR's; 
                The iCpuPwrEn is drive by Mstr_Seq.v  this net allow to turn ON/OFF the CPU VR's, 
                the fault conditions are generated by oCpuPwrFltVCCIO, oCpuPwrFltP1V8_PCIE, oCpuPwrFltVCCANA, oCpuPwrFltVCCIN,
                oCpuPwrFltVCCSA and also a general fault oCpuPwrFlt . The internal state of FSM can be monitored by oDbgCpuSt.\n 

                Table of the FSM:
                State Hex  | State Name | Next State | Condition 
                -----------|------------|------------|-----------
                1 | ST_CPUFAULT | ST_CPUOFF | iGoOutFltSt=1
                2 | ST_CPUOFF | ST_PVCCIO | iCpuPwrEn=1
                3 | ST_PVCCIO | ST_P1V8_PCIE_P1V1 | PWRGD_PVCCIO=1
                4 | ST_P1V8_PCIE_P1V1 | ST_PVCCANA | PWRGD_P1V8_PCIE or FM_INTR_PRSNT = 1
                5 | ST_PVCCANA | ST_PVCCIN | PWRGD_PVCCANA or FM_INTR_PRSNT =1
                6 | ST_PVCCIN  | ST_PVCCSA | PWRGD_PVCCIN =1
                7 | ST_PVCCSA  | ST_CPUDONE | PWRGD_PVCCSA =1
                8 | ST_CPUDONE | ST_PVCCSA_OFF | iCpuPwrEn=0
                9 | ST_PVCCSA_OFF | ST_PVCCIN_OFF | PWRGD_PVCCSA=0
                10 | ST_PVCCIN_OFF | ST_PVCCANA_OFF | PWRGD_PVCCIN=0
                11 | ST_PVCCANA_OFF | ST_P1V8_PCIE_P1V1_OFF | PWRGD_PVCCANA=0
                12 | ST_P1V8_PCIE_P1V1_OFF | ST_PVCCIO_OFF | PWRGD_P1V8_PCIE=0
                13 | ST_PVCCIO_OFF | ST_CPUOFF | PWRGD_PVCCIO=0


				
	\brief  <b>Last modified</b> 
			$Date:   Sept 7, 2018 $
			$Author:  David.bolanos@intel.com $			
			Project			: Wilson City RP  
			Group			: BD
	\version    
			 20160709 \b  David.bolanos@intel.com - File creation\n
			 20181901 \b  David.bolanos@intel.com - Modify to adapt to Wilson City, leverage from Mehlow\n
			   
	\copyright Intel Proprietary -- Copyright \htmlonly <script>document.write(new Date().getFullYear())</script> \endhtmlonly Intel -- All rights reserved
*/
//////////////////////////////////////////////////////////////////////////////////


module Cpu_Seq
(
	input			iClk,//%Clock input
	input			iRst_n,//%Reset enable on high
	input 		    i1mSCE, //% 1 mS Clock Enable

	input           iCpuPwrEn,//% Cpu power sequence enable
	input           iGoOutFltSt,//% Go out fault state.
	input           iEnableTimeOut,//% Enable the Timeout
	input           FM_INTR_PRSNT, //% Detect Interposer 

	input    		PWRGD_PVCCIO,//% CPU VR PWRGD VCCIO
	input           PWRGD_P1V8_PCIE,//% CPU VR PWRGD P1V8_PCIE
	input           PWRGD_PVCCANA,//% CPU VR PWRGD VCCANA
	input    		PWRGD_PVCCIN,//% CPU VR PWRGD VCCIN	
	input           PWRGD_PVCCSA,//% CPU VR PWRGD VCCSA 

	output  reg     FM_PVCCIO_EN,//% CPU VR EN PVCCIO
	output  reg     FM_P1V8_PCIE_EN,//% CPU VR EN P1V8_PCIE
	output  reg     FM_PVCCANA_EN,//% CPU VR EN PVCCANA
	output  reg     FM_PVCCIN_EN,//% CPU VR EN PVCCIN 
	output  reg     FM_PVCCSA_EN,//% CPU VR EN  PVCCSA

	output reg      oCpuPwrgd,//% PWRGD of all CPU VR's     
	output reg      oCpuPwrFlt,//% Fault CPU VR's

	output reg      oCpuPwrFltVCCIO,//% Fault  VCCIO
	output reg      oCpuPwrFltP1V8_PCIE,//% Fault  P1V8_PCIE
	output reg      oCpuPwrFltVCCANA,//% Fault VCCANA
	output reg      oCpuPwrFltVCCIN,//% Fault  VCCIN
	output reg      oCpuPwrFltVCCSA,//% Fault  VCCSA

	output [3:0]    oDbgCpuSt//% State of CPU Secuence.

);

//////////////////////////////////////////////////////////////////////////////////
// Parameters
//////////////////////////////////////////////////////////////////////////////////
parameter  ST_CPUFAULT            = 4'd1;
parameter  ST_CPUOFF              = 4'd2;
parameter  ST_PVCCIO              = 4'd3;
parameter  ST_P1V8_PCIE_P1V1      = 4'd4;
parameter  ST_PVCCANA             = 4'd5;
parameter  ST_PVCCIN              = 4'd6; 
parameter  ST_PVCCSA              = 4'd7;
parameter  ST_CPUDONE             = 4'd8;
parameter  ST_PVCCSA_OFF          = 4'd9;
parameter  ST_PVCCIN_OFF          = 4'd10; 
parameter  ST_PVCCANA_OFF         = 4'd11;
parameter  ST_P1V8_PCIE_P1V1_OFF  = 4'd12;
parameter  ST_PVCCIO_OFF          = 4'd13;

parameter  LOW =1'b0;
parameter  HIGH=1'b1;
parameter  T_500US_2M  =  10'd1000;

//////////////////////////////////////////////////////////////////////////////////
// Internal Signals
//////////////////////////////////////////////////////////////////////////////////
reg [3:0]   rCurrSt;
reg [3:0]   rCurrSt_ff1;
reg [3:0]   rNextSt;
reg         rCpuFlt;
reg         rPwrgd_VCCIO_Ff1;
reg         rPWRGD_P1V8_PCIE_Ff1;
reg         rPwrgd_VCCANA_Ff1;
reg         rPwrgd_VCCIN_Ff1;
reg         rPwrgd_VCCSA_Ff1;

wire        wDly_500us,wCntRst_n,wTimeOut_500us;
wire        wCpuPwrEn;

//////////////////////////////////////////////////////////////////////////////////
// Continuous assignments
//////////////////////////////////////////////////////////////////////////////////
assign  oDbgCpuSt      = rCurrSt;
assign  wCntRst_n      = (rCurrSt == rCurrSt_ff1);
assign  wTimeOut_500us = wDly_500us && wCntRst_n && iEnableTimeOut;
assign  wCpuPwrEn      = iCpuPwrEn;


//////////////////////////////////////////////////////////////////////////////////
// FSM CurrentState generation
//////////////////////////////////////////////////////////////////////////////////
always @ ( posedge iClk) 
begin 
	if (  !iRst_n  )   
		begin
			rCurrSt      <= ST_CPUOFF;
			rCurrSt_ff1  <= ST_CPUOFF;			
		end 
	else 
		begin								
			rCurrSt      <= rNextSt;
			rCurrSt_ff1  <= rCurrSt; 								
		end
end  


//////////////////////////////////////////////////////////////////////////////////
// State Machine logic
//////////////////////////////////////////////////////////////////////////////////
always @( * )
begin
	case( rCurrSt )			
		ST_CPUFAULT: //1 
			if( iGoOutFltSt )                  	         	rNextSt = ST_CPUOFF;
			else                                         	rNextSt = ST_CPUFAULT;          

		ST_CPUOFF:  //2
			if     ( rCpuFlt )                          	rNextSt = ST_CPUFAULT;
			else if(  wCpuPwrEn )                       	rNextSt = ST_PVCCIO;   
			else                                        	rNextSt = ST_CPUOFF; 

		ST_PVCCIO: //3
			if     ( rCpuFlt )                          	rNextSt = ST_PVCCIO_OFF;
			else if( !wCpuPwrEn )                       	rNextSt = ST_PVCCIO_OFF;      
			else if(  PWRGD_PVCCIO )                    	rNextSt = ST_P1V8_PCIE_P1V1;                            
			else                                        	rNextSt = ST_PVCCIO;

		ST_P1V8_PCIE_P1V1: //4
			if     ( rCpuFlt )                          	rNextSt = ST_P1V8_PCIE_P1V1_OFF;
			else if( !wCpuPwrEn )                       	rNextSt = ST_P1V8_PCIE_P1V1_OFF;      
			else if(  PWRGD_P1V8_PCIE || FM_INTR_PRSNT )	rNextSt = ST_PVCCANA;                            
			else                                        	rNextSt = ST_P1V8_PCIE_P1V1;

		ST_PVCCANA://5
			if     ( rCpuFlt )                           	rNextSt = ST_PVCCANA_OFF;
			else if( !wCpuPwrEn )                        	rNextSt = ST_PVCCANA_OFF;      
			else if( PWRGD_PVCCANA    || FM_INTR_PRSNT ) 	rNextSt = ST_PVCCIN;                            
			else  											rNextSt = ST_PVCCANA;				

		ST_PVCCIN://6
			if     ( rCpuFlt )                          	rNextSt = ST_PVCCIN_OFF;
			else if( !wCpuPwrEn )                       	rNextSt = ST_PVCCIN_OFF;      
			else if( PWRGD_PVCCIN)                      	rNextSt = ST_PVCCSA;                            
			else  											rNextSt = ST_PVCCIN;

		ST_PVCCSA://7
			if     ( rCpuFlt )                        		rNextSt = ST_PVCCSA_OFF;
			else if( !wCpuPwrEn )                     		rNextSt = ST_PVCCSA_OFF;      
			else if( PWRGD_PVCCSA)                    		rNextSt = ST_CPUDONE;                            
			else  											rNextSt = ST_PVCCSA;
			
		ST_CPUDONE: //8
			if( !wCpuPwrEn || rCpuFlt)						rNextSt = ST_PVCCSA_OFF;
			else                                         	rNextSt = ST_CPUDONE;	

		ST_PVCCSA_OFF: //9
			if( ~(PWRGD_PVCCSA)  || 
					wTimeOut_500us)                      	rNextSt = ST_PVCCIN_OFF;
			else											rNextSt = ST_PVCCSA_OFF;				

		ST_PVCCIN_OFF: //10
			if( ~(PWRGD_PVCCIN)  || 
					wTimeOut_500us)                     	rNextSt = ST_PVCCANA_OFF;
			else											rNextSt = ST_PVCCIN_OFF;	
		
		ST_PVCCANA_OFF: //11
			if(~PWRGD_PVCCANA  ||
					 wTimeOut_500us)                     	rNextSt = ST_P1V8_PCIE_P1V1_OFF;					
			else											rNextSt = ST_PVCCANA_OFF;											 

		ST_P1V8_PCIE_P1V1_OFF: //12
			if(~PWRGD_P1V8_PCIE  ||
					 wTimeOut_500us)                    	rNextSt = ST_PVCCIO_OFF;					
			else											rNextSt = ST_P1V8_PCIE_P1V1_OFF;	

		ST_PVCCIO_OFF: //13
			if(~PWRGD_PVCCIO  || wTimeOut_500us)         	rNextSt = ST_CPUOFF;					
			else                                         	rNextSt = ST_PVCCIO_OFF;
							
		default:											rNextSt = ST_CPUOFF;  			
	endcase  
end


//////////////////////////////////////////////////////////////////////////////////
// Output State Machine 
//////////////////////////////////////////////////////////////////////////////////


always @( posedge iClk) 
begin 
	if (  !iRst_n ) 
			begin					
				FM_PVCCIO_EN			  <= LOW;
				FM_P1V8_PCIE_EN           <= LOW;
				FM_PVCCANA_EN             <= LOW;
				FM_PVCCIN_EN	          <= LOW;
				FM_PVCCSA_EN              <= LOW;		
				
				oCpuPwrgd                 <= LOW;	
				oCpuPwrFlt	    	      <= LOW;
				rCpuFlt		    	      <= LOW;

				rPwrgd_VCCIO_Ff1          <= LOW;
				rPWRGD_P1V8_PCIE_Ff1      <= LOW;
				rPwrgd_VCCANA_Ff1         <= LOW;
				rPwrgd_VCCIN_Ff1          <= LOW;
				rPwrgd_VCCSA_Ff1          <= LOW;		

				oCpuPwrFltVCCIO           <= LOW;
				oCpuPwrFltP1V8_PCIE       <= LOW;	
				oCpuPwrFltVCCANA          <= LOW;	
				oCpuPwrFltVCCIN           <= LOW;	
				oCpuPwrFltVCCSA           <= LOW;
				
			end
	else 
			begin	

				FM_PVCCIO_EN              <= ((rCurrSt >= ST_PVCCIO    )      && (rCurrSt < ST_PVCCIO_OFF))                           ? HIGH : LOW;
				FM_P1V8_PCIE_EN           <= ((rCurrSt >= ST_P1V8_PCIE_P1V1 ) && (rCurrSt < ST_P1V8_PCIE_P1V1_OFF) && !FM_INTR_PRSNT) ? HIGH : LOW; // No turn on with interposer
				FM_PVCCANA_EN             <= ((rCurrSt >= ST_PVCCANA   )      && (rCurrSt < ST_PVCCANA_OFF)  && !FM_INTR_PRSNT )      ? HIGH : LOW; // No turn on with interposer
				FM_PVCCIN_EN	          <= ((rCurrSt >= ST_PVCCIN    )      && (rCurrSt < ST_PVCCIN_OFF   ))                        ? HIGH : LOW;	
				FM_PVCCSA_EN              <= ((rCurrSt >= ST_PVCCSA    )      && (rCurrSt < ST_PVCCSA_OFF   ))                        ? HIGH : LOW;	

				oCpuPwrgd                 <= ((rCurrSt >= ST_CPUDONE) && (rCurrSt < ST_PVCCIO_OFF)) ? HIGH : LOW;
				oCpuPwrFlt                <= (rCurrSt == ST_CPUFAULT);

				rPwrgd_VCCIO_Ff1          <= PWRGD_PVCCIO;
				rPWRGD_P1V8_PCIE_Ff1      <= PWRGD_P1V8_PCIE;
				rPwrgd_VCCANA_Ff1         <= PWRGD_PVCCANA;
				rPwrgd_VCCIN_Ff1          <= PWRGD_PVCCIN;
				rPwrgd_VCCSA_Ff1          <= PWRGD_PVCCSA;

				oCpuPwrFltVCCIO           <= (FM_PVCCIO_EN    && wCpuPwrEn && rPwrgd_VCCIO_Ff1  	&& !PWRGD_PVCCIO   )  ?  HIGH: oCpuPwrFltVCCIO;
				oCpuPwrFltP1V8_PCIE       <= (FM_P1V8_PCIE_EN && wCpuPwrEn && rPWRGD_P1V8_PCIE_Ff1 	&& !PWRGD_P1V8_PCIE)  ?  HIGH: oCpuPwrFltP1V8_PCIE;
				oCpuPwrFltVCCANA          <= (FM_PVCCANA_EN   && wCpuPwrEn && rPwrgd_VCCANA_Ff1    	&& !PWRGD_PVCCANA  )  ?  HIGH: oCpuPwrFltVCCANA;
				oCpuPwrFltVCCIN           <= (FM_PVCCIN_EN    && wCpuPwrEn && rPwrgd_VCCIN_Ff1     	&& !PWRGD_PVCCIN   )  ?  HIGH: oCpuPwrFltVCCIN;
				oCpuPwrFltVCCSA           <= (FM_PVCCSA_EN    && wCpuPwrEn && rPwrgd_VCCSA_Ff1     	&& !PWRGD_PVCCSA   )  ?  HIGH: oCpuPwrFltVCCSA;

				rCpuFlt                   <= (wCpuPwrEn && (oCpuPwrFltVCCIO || oCpuPwrFltP1V8_PCIE || oCpuPwrFltVCCANA || oCpuPwrFltVCCIN || oCpuPwrFltVCCSA)) ? HIGH: rCpuFlt;
			end
end 
		



//////////////////////////////////////////////////////////////////////
// Instances 
//////////////////////////////////////////////////////////////////////
counter2 #( .MAX_COUNT(T_500US_2M) ) ForceOff_500usDly
( .iClk         	( iClk           ), 
.iRst_n       	( iRst_n         ),

.iCntRst_n  	(  wCntRst_n  ),
.iCntEn      	(  wCntRst_n  ),
.iLoad        	(!(wCntRst_n) ),
.iSetCnt    	( T_500US_2M  [9: 0]   ),  // set the time out time as 500us

.oDone        	( wDly_500us   ),
.oCntr         	(             ) 
);


endmodule