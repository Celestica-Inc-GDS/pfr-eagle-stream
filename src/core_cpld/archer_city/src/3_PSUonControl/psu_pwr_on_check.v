
//////////////////////////////////////////////////////////////////////////////////
/* Description: The following finite state machine describes the sequential and combinational behavior 
across PCH and CPU states to check for valid conditions and power on the Power Supply Unit (PSU). This 
FSM is Moore style.
*/
//////////////////////////////////////////////////////////////////////////////////

module psu_pwr_on_check
(
    input            iClk,   					 //System clock
    input            iRst_n, 					 //synchronous Reset 
	 input            ienable,					 //internal input signal coming from master to start the FSM
	 input 				iPWRGD_PS_PWROK, 	 //Signal generated by the PSU, indicates wihen 12V Main is stablished
	 input 				iPWRGD_P3V3, 			//power good coming from 3.3V voltage regulator (powered by 12V main)  ##NEW

    output reg       oFM_PS_EN,             //Only output signal, this enables PSU
	 output reg       ofault,                 //Failure output, to indicate to master sequencer that something failed on this block
	 output reg[2:0] oFaultCode,           //This output indicates to Master which is the code of the failure
	 output reg       odone,                 //Indicates if the function is done
	 output reg       oFM_P5V_EN,  			 // Enable signal for P5V VR on System VRs	   ##NEW
	 output reg 		oFM_PLD_CLK_DEV_EN    //Output to enable 100MHz external generator from SI5332 or CK440
);

//////////////////////////////////////////////////////////////////////////////////
// wires
//////////////////////////////////////////////////////////////////////////////////

wire sel;
wire doneCnt;



//////////////////////////////////////////////////////////////////////////////////
// Parameters
//////////////////////////////////////////////////////////////////////////////////

parameter  LOW     = 1'b0;
parameter  HIGH    = 1'b1;
parameter  init    = 4'b0000;   //State of FSM
parameter  st_On   = 4'b0001;   //State of FSM
parameter  st_3v3OK =4'b0010;  //State of FSM
parameter  st_done = 4'b0011;   //State of FSM
parameter  st_wait = 4'b0100;   //State of FSM
parameter  st_fault_A_PSU= 4'b0101;   //State of FSM
parameter  st_fault_B_PSU= 4'b0110;   //State of FSM
parameter  st_fault_A_3v3= 4'b0111;   //State of FSM
parameter  st_fault_B_3v3= 4'b1000;   //State of FSM


//////////////////////////////////////////////////////////////////////////////////
// Internal Signals
//////////////////////////////////////////////////////////////////////////////////
reg [3:0] state, // Current state
 nxtState; // Next state
reg trigger;
 
 //Instance of counter to generate a delay of 50ms or 1sec
counterPSU u1 (.iClk(iClk),.iRst_n(iRst_n),.enable(trigger),.isel(sel),.done(doneCnt));

//Combinational select delay for counter
assign sel     = (state == st_On) ? 1'b0 : (state == st_3v3OK) ? 1'b1:1'b0;
 
//FSM starts here

//////////////////////////////////////////////////////////////////////////////////
// Combinational Logic
//////////////////////////////////////////////////////////////////////////////////
always @(*) begin
	nxtState = state;
	oFM_PS_EN = LOW;
	ofault    = LOW;
	odone    = LOW;
	oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input	
	oFM_PLD_CLK_DEV_EN = LOW;
	trigger = LOW;
	case (state)
		init : 	begin           //init state... after powering up FGPA, the FSM should be in this state. 
					//#############################################################################################
					//OUTPUTS of INIT STATE//
					oFM_PS_EN = LOW;	            // Output: PSU off
					ofault    = LOW;               // Output: No failure
					odone     = LOW;               // Output: DONE
					oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input
					oFaultCode= 3'b000;			//This ouput provides a code to recognize PSU PWRGD failure	
					oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440
					//#############################################################################################
					
					//#############################################################################################
					//Conditions to move to next state
					if (!ienable) begin                //if enable == 0, coming from master seq, be in init
						nxtState = init;   
						end
					else if(ienable) begin            //if enable == 1, coming from master seq, move st_On
						nxtState = st_On;  				 
						trigger = LOW;						 //Reset timer
						end
					end
					//#############################################################################################
					
		st_On : 	begin
					//#############################################################################################
					//OUTPUTS of st_On STATE//
					oFM_PS_EN = HIGH;					// Output: PSU on
					ofault    = LOW;               // Output: No failure
					odone     = LOW;               // Output: DONE		
					oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input
					oFaultCode= 3'b000;			//This ouput provides a code to recognize PSU PWRGD failure	
					oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440
					trigger = HIGH;					//Starts timer for 1sec, after 1sec doneCnt is asserted
					//#############################################################################################
					
					//#############################################################################################
					//Conditions to move to next state
					if (!ienable) begin   					//if enable == 0, coming from master seq, be in init
						nxtState = init; 		
						trigger = LOW;							//reset timer
						end
					else if (ienable && iPWRGD_PS_PWROK) begin  //if enable and PSU pwr ok, move to 3v3OK state
						nxtState = st_3v3OK;
						trigger = LOW;									//reset timer
						end
//					else if (ienable && !iPWRGD_PS_PWROK && 1'b0/*doneCnt*/) begin  //Fault condition, after timer expires and PSU pwr ok is not done, //alarios: making this condition invalid ALWAYS (no timer checK)
//						nxtState = st_fault_B_PSU;  									//it goes to failure state
//						trigger = LOW;									//reset timer
//						end					
					end
					//#############################################################################################
					
					
		st_3v3OK:begin
					//#############################################################################################
					//OUTPUTS of st_3v3OK STATE//
					oFM_PS_EN = HIGH;					// Output: PSU on
					ofault    = LOW;               // Output: No failure
					odone     = LOW;               // Output: ~DONE		
					oFM_P5V_EN= HIGH;					//Output to enable VR 5V and 3.3V with 12V Main as input
					oFaultCode= 3'b000;			//This ouput provides a code to recognize PSU PWRGD failure	
					oFM_PLD_CLK_DEV_EN = HIGH;		//Output to enable 100MHz external generator from SI5332 or CK440
					trigger = HIGH;					//Starts timer for 50ms, after 50ms doneCnt is asserted
					//#############################################################################################
					
					//#############################################################################################
					//Conditions to move to next state
					if (ienable && iPWRGD_P3V3) begin   			//if enable and 3.3Vr pwr good move to st_done
						nxtState = st_done;   
						trigger = LOW;									//reset timer
						end
//					else if (ienable && !iPWRGD_P3V3 && doneCnt) begin  //if timer expires  and 3.3 pwrgood is not, move to fault     
//						nxtState = st_fault_B_3v3; 
//						trigger = LOW;									//reset timer
//						end					
					end	
					//#############################################################################################	
		
		
		st_done :begin
					//#############################################################################################
					//OUTPUTS of st_done STATE//
					oFM_PS_EN = HIGH;					// Output: PSU on
					ofault    = LOW;               // Output: No failure
					odone     = HIGH;               // Output: ~DONE
					oFM_P5V_EN= HIGH;					//Output to enable VR 5V and 3.3V with 12V Main as input
					oFaultCode= 3'b000;			//This ouput provides a code to recognize PSU PWRGD failure	
					oFM_PLD_CLK_DEV_EN = HIGH;		//Output to enable 100MHz external generator from SI5332 or CK440					
					//#############################################################################################
					
					//#############################################################################################
					//Conditions to move to next state
					if (ienable && !iPWRGD_PS_PWROK) begin			//if PSU pwr_Ok goes down, move to faultState
						nxtState = st_fault_A_PSU;        
						end
					else if (ienable && !iPWRGD_P3V3) begin			//if 3.3 V pwrgd goes down, move to faultState
						nxtState = st_fault_A_3v3;        
						end						
					else if (ienable && iPWRGD_PS_PWROK) begin    //if PSU pwrOK is keep st_done
						nxtState = st_done;   
						end
					else if (!ienable && iPWRGD_P3V3 && iPWRGD_PS_PWROK) begin	//if enable is 0, move to wait. (power done seq)
						nxtState = st_wait;   
						end
//					else begin							 //Any othe com	
//						nxtState = st_fault_A_PSU;		 
//						end
					end
					//#############################################################################################
					
		st_wait:	begin
					//#############################################################################################
					//OUTPUTS of st_wait STATE//
					oFM_PS_EN = LOW;					// Output: PSU off
					ofault    = LOW;               // Output: No failure
					odone     = HIGH;               // Output: DONE
					oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input
					oFaultCode= 3'b000;			//This ouput provides a code to recognize PSU PWRGD failure		
					oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440				
					//#############################################################################################
					
					//#############################################################################################
					//Conditions to move to next state
					if (!ienable && iPWRGD_P3V3) begin						 //if power down seq, but 3.3pwrok wait
						nxtState = st_wait;        
						end
					else if (!ienable && !iPWRGD_P3V3) begin			    //if power down seq, but 3.3pwrok "0" move to init
						nxtState = init;   		
						end
					end
					//#############################################################################################
		
		st_fault_A_PSU:	begin	
					//#############################################################################################
					//OUTPUTS of st_fault_A_PSU STATE//
					oFM_PS_EN = LOW;					 // Output: PSU off
					ofault    = HIGH;               // Output: failure Present
					odone     = LOW;               // Output: DONE	
					oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input					
					oFaultCode= 3'b001;				//fault code for  			
					oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440	
					nxtState = st_fault_A_PSU;            //Infinite loop, keeping in faultSt
					end	
					//#############################################################################################
					
		st_fault_B_PSU:	begin
					//#############################################################################################
					//OUTPUTS of st_fault_B_PSU STATE//		
					oFM_PS_EN = LOW;					 // Output: PSU off
					ofault    = HIGH;               // Output: failure Present
					odone     = LOW;               // Output: DONE	
					oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input					
					oFaultCode= 3'b101;				//This ouput provides a code to recognize PSU PWRGD failure	
					oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440				
					nxtState = st_fault_B_PSU;            //Infinite loop, keeping in faultSt
					end
					//#############################################################################################
					
		st_fault_A_3v3:	begin
					//#############################################################################################
					//OUTPUTS of st_fault_A_3v3 STATE//			
					oFM_PS_EN = LOW;					 // Output: PSU off
					ofault    = HIGH;               // Output: failure Present
					odone     = LOW;               // Output: DONE	
					oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input
					oFaultCode= 3'b010;				//This ouput provides a code to recognize PSU PWRGD failure
					oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440				
					nxtState = st_fault_A_3v3;            //Infinite loop, keeping in faultSt
					end	
					//#############################################################################################		
					
		st_fault_B_3v3:	begin
					//#############################################################################################
					//OUTPUTS of st_fault_B_3v3 STATE//			
					oFM_PS_EN = LOW;					 // Output: PSU off
					ofault    = HIGH;               // Output: failure Present
					odone     = LOW;               // Output: DONE	
					oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input
					oFaultCode= 3'b110;				//This ouput provides a code to recognize PSU PWRGD failure		
					oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440				
					nxtState = st_fault_B_3v3;            //Infinite loop, keeping in faultSt
					end	
					//#############################################################################################							
					
		default : begin
			oFM_PS_EN = LOW;
			nxtState = st_fault_A_PSU;
			odone     = LOW;               // Output: DONE				
			ofault    = HIGH;
			oFaultCode= 3'b000;			//This ouput provides a code to recognize PSU PWRGD failure		
			oFM_P5V_EN= LOW;					//Output to enable VR 5V and 3.3V with 12V Main as input		
			oFM_PLD_CLK_DEV_EN = LOW;		//Output to enable 100MHz external generator from SI5332 or CK440				
			end
	endcase
end



//////////////////////////////////////////////////////////////////////////////////
// Secuencial Logic
//////////////////////////////////////////////////////////////////////////////////



always @(posedge iClk) begin
	if (!iRst_n) begin
		state <= init; // Initial state
	end 
	else begin
		state <= nxtState; //Assigning next state
	end
end 







endmodule 
