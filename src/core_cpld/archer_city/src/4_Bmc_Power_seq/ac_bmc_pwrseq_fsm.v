/* ==========================================================================================================
   = Project     :  Archer City Power Sequencing
   = Title       :  BMC Power Sequence State Machine
   = File        :  ac_bmc_pwrseq_fsm.v
   ==========================================================================================================
   = Author      :  Pedro Saldana
   = Division    :  BD GDC
   = E-mail      :  pedro.a.saldana.zepeda@intel.com
   ==========================================================================================================
   = Updated by  :  
   = Division    :  
   = E-mail      :  
   ==========================================================================================================
   = Description :  This state machine runs through the BMC AST2600 power-up and power-down sequence. 
                    It also support Archer City AST2500 interposer.
                    This module does not depend on Master Sequence to enable the VR's.
                    The BMC VR's are enabled on S5 state.


   = Constraints :  
   
   = Notes       :  
   
   ==========================================================================================================
   = Revisions   :  
     July 15, 2019;    0.1; Initial release
   ========================================================================================================== */

module ac_bmc_pwrseq_fsm
(
//------------------------
//Clock and Reset signals
//------------------------

    input           iClk,               //clock for sequential logic 
    input           iRst_n,             //reset signal, resets state machine to initial state

// ----------------------------
// Power-up sequence inputs and outputs
// ---------------------------- 
     input          iPWRGD_P2V5_BMC_AUX,          //Pwrgd from P2V5 Aux VR
     input          iPWRGD_P1V8_BMC_AUX,          //Pwrgd from P1V8 Aux VR
     input          iPWRGD_P1V2_BMC_AUX,          //Pwrgd from P1V2 Aux VR
     input          iPWRGD_P1V0_BMC_AUX,          //Pwrgd from P1V0 Aux VR

     input          iFM_BMC_INTR_PRSNT,           //Hardware strap for BMC interposer support ("0" == no interposer), it will change the power sequence between AST2500 and AST2600
     input          iBMC_PWR_EN,                  //If set FSM will start power-down sequencing   
     input          iRST_DEDI_BUSY_PLD_N,         //Dediprog Detection Support 
     
     output reg     oBMC_PWR_FAULT,                //Fault BMC VR's 
     output reg     oRST_SRST_BMC_N ,              //SRST# 
     output reg     oFM_P2V5_BMC_AUX_EN,           //Output for BMC P2V5 AUX VR enable
     output reg     oFM_PCH_P1V8_AUX_EN,           //Output PCH P1V8 VR enable, for module ac_pch_pwrseq_fsm and LPC interface
     output reg     oFM_P1V2_BMC_AUX_EN,           //Output BMC P1V2 VR enable
     output reg     oFM_P1V0_BMC_AUX_EN,           //Output for BMC P1V0 AUX VR enable
     output reg     oBMC_PWR_P2V5_FAULT,           //BMC VR Faults
     output reg     oBMC_PWR_P1V8_FAULT, 
     output reg     oBMC_PWR_P1V2_FAULT, 
     output reg     oBMC_PWR_P1V0_FAULT,
     output reg     oBMC_PWR_OK                    //If set it indicates all VRs are up 
);

//////////////////////////////////////////////////////////////////////////////////
// States for root state machine
//////////////////////////////////////////////////////////////////////////////////
     localparam INIT                    = 4'd0;
     localparam P2V5_ON                 = 4'd1;
     localparam P1V0_ON                 = 4'd2;
     localparam P1V2_ON                 = 4'd3;
     localparam P1V8_ON                 = 4'd4;
     localparam BMC_PWR_OK              = 4'd5;
     localparam PWR_DOWN                = 4'd6;
     localparam P2V5_OFF                = 4'd7;
     localparam P1V0_OFF                = 4'd8;
     localparam P1V2_OFF                = 4'd9;
     localparam P1V8_OFF                = 4'd10;
//////////////////////////////////////////////////////////////////////////////////
// Parameters
//////////////////////////////////////////////////////////////////////////////////
     localparam  LOW =1'b0;
     localparam  HIGH=1'b1;
     localparam  T_1mS_2M  =  11'd2000;
     localparam  T_2mS_2M  =  12'd4000;
     
     reg [3:0] root_state;                        //main sequencing state machine
     reg [3:0]  rPREVIOUS_STATE = INIT;
     reg       rPWR_FAULT;
     reg       rPWRGD_P2V5_BMC_FF1;
     reg       rPWRGD_P1V8_BMC_FF1;
     reg       rPWRGD_P1V2_BMC_FF1;
     reg       rPWRGD_P1V0_BMC_FF1;
     reg       rPWRGD_P2V5_BMC_FAIL;
     reg       rPWRGD_P1V8_BMC_FAIL;
     reg       rPWRGD_P1V2_BMC_FAIL;
     reg       rPWRGD_P1V0_BMC_FAIL;
     wire      wPWRGD_All_VRs_DLY_2mS;
     wire      wDLY_1mS;
     wire      wTIME_OUT_1mS;
     wire      wCNT_RST_N;

//////////////////////////////////////////////////////////////////////////////////
// Continuous assignments    /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
assign  wCNT_RST_N       = (root_state == rPREVIOUS_STATE);
assign  wTIME_OUT_1mS    = wDLY_1mS && wCNT_RST_N;

     //state machine for AST2600 Power-up sequence and AST2500 interposer
     always @ (posedge iClk, negedge iRst_n)
     begin
          if(!iRst_n)                             //asynchronous, negative-edge reset
          begin
               oFM_P2V5_BMC_AUX_EN <= LOW;        //disable all VR's, initial state
               oFM_P1V0_BMC_AUX_EN <= LOW;                 
               oFM_P1V2_BMC_AUX_EN <= LOW;
               oFM_PCH_P1V8_AUX_EN <= LOW;
               root_state  <= INIT;                 //reset state machines to initial states
               oBMC_PWR_OK <= LOW;                  //reset BMC PWR OK signal
          end
          else
          begin
          if(!iFM_BMC_INTR_PRSNT)                                     //verify if AST2500 interposer is not present
               begin
                    case(root_state)
                    INIT: 
                    begin
                    oBMC_PWR_OK <= LOW;                                //BMC PWR OK signal low at initial state     
                         if(iRst_n)                                    //wait for reset to go high
                         begin
                              if(iBMC_PWR_EN && !rPWR_FAULT)  //if P3V3_AUX rail is up and PWR DOWN and PWR FAULT is not SET
                              begin
                                   root_state <= P2V5_ON;              //then go to state of waiting for power-up to be initiated
                              end
                              
                         end       
                    end
                    
                    P2V5_ON:
                    begin
                         oFM_P2V5_BMC_AUX_EN <= HIGH;                   //power up the VR 
                         if(!iBMC_PWR_EN || rPWR_FAULT)                    //if power down flag set, skip to power down sequence
                              root_state <= P1V8_OFF;
                         else if(iPWRGD_P2V5_BMC_AUX && wTIME_OUT_1mS)  //if the power good signal for P2V5 is high, implement next state now   
                         begin
                              root_state <= P1V0_ON;                    //go to next state
                         end
                    end
                    
                    P1V0_ON: 
                    begin
                         oFM_P1V0_BMC_AUX_EN <= HIGH;                    //power up the VR  
                         if(!iBMC_PWR_EN || rPWR_FAULT)                     //if power down flag set, skip to power down sequence
                              root_state <= P1V8_OFF;
                         else if(iPWRGD_P1V0_BMC_AUX && wTIME_OUT_1mS)   //if the power good signal for P1V0 is high, implement next state now
                         begin
                              root_state <= P1V2_ON;                     //go to next state
                         end
                    end

                    P1V2_ON: 
                    begin
                         oFM_P1V2_BMC_AUX_EN <= HIGH;                    //assert next enable
                         if(!iBMC_PWR_EN || rPWR_FAULT)                     //if power down flag set, skip to power down sequence
                              root_state <= P1V8_OFF;
                         else if(iPWRGD_P1V2_BMC_AUX && wTIME_OUT_1mS)   //if power good P1V2 is high
                         begin
                              root_state <= P1V8_ON;                     //go to next state
                         end
                    end
          
                    P1V8_ON: 
                    begin
                         oFM_PCH_P1V8_AUX_EN <= HIGH;                     //assert next enable
                         if(!iBMC_PWR_EN || rPWR_FAULT)                      //if power down flag set, skip to power down sequence
                              root_state <= P1V8_OFF;
                         else if(iPWRGD_P1V8_BMC_AUX)                     //if power good P1V8 is high
                         begin
                              root_state <= BMC_PWR_OK;                   //go to next root state                      
                         end 
                    end 

                    BMC_PWR_OK: 
                    begin
                         oBMC_PWR_OK <= HIGH;                             //All BMC VR's Up and Running 
                         if(!iBMC_PWR_EN || rPWR_FAULT) 
                              root_state <= P1V8_OFF; 
                    end
               
                    P1V8_OFF: 
                    begin
                         oFM_PCH_P1V8_AUX_EN <= LOW;                       //deassert enable to power down 
                         if(!iPWRGD_P1V8_BMC_AUX && wTIME_OUT_1mS)         //if current stage is off
                         begin
                              root_state <= P1V2_OFF;                      //go to next state
                         end
                    end
                    
                    P1V2_OFF: 
                    begin
                         oFM_P1V2_BMC_AUX_EN <= LOW;                       //deassert enable to power down 
                         if(!iPWRGD_P1V2_BMC_AUX && wTIME_OUT_1mS)         //if current stage is off
                         begin
                              root_state <= P1V0_OFF;                      //go to next state
                         end
                    end
                                        
                    P1V0_OFF: 
                    begin
                         oFM_P1V0_BMC_AUX_EN <= LOW;                        //deassert enable to power down 
                         if(!iPWRGD_P1V0_BMC_AUX && wTIME_OUT_1mS)          //if current stage is off
                         begin
                              root_state <= P2V5_OFF;                       //go to next stage of power-down sequence
                         end
                    end
                    
                    P2V5_OFF: 
                    begin
                         oFM_P2V5_BMC_AUX_EN <= LOW;                        //deassert enable to power down 
                         if(!iPWRGD_P2V5_BMC_AUX && wTIME_OUT_1mS)          //if current stage is off
                         begin
                              root_state <= INIT;                           //now go to state of waiting for power-up
                         end
                    end
                    
                    default: 
                    begin
                         root_state <= INIT;
                    end
                    endcase
                    rPREVIOUS_STATE <= root_state;     
               end
          //End of AST2600 state Machine

          //State machine for AST2500 interposer, delete after PPO testing
          else if(iFM_BMC_INTR_PRSNT)                                  //verify if AST2500 interposer is present
               begin
                    case(root_state)
                    INIT: 
                    begin
                    oBMC_PWR_OK <= LOW;                                //BMC PWR OK signal low at initial state 
                         if(iRst_n)                                    //wait for reset to go high
                         begin
                              if(iBMC_PWR_EN && !rPWR_FAULT)  //if P3V3_AUX rail is up and PWR DOWN and PWR FAULT is not SET
                              begin
                                   root_state <= P1V8_ON;              //then go to state of waiting for power-up to be initiated
                              end
                              
                         end       
                    end
                    
                    P1V8_ON: 
                    begin
                         oFM_PCH_P1V8_AUX_EN <= HIGH;                     //assert next enable
                         if(!iBMC_PWR_EN || rPWR_FAULT)                      //if power down flag set, skip to power down sequence
                              root_state <= P1V0_OFF;
                         else if(iPWRGD_P1V8_BMC_AUX)                     //if power good P1V8 is high
                         begin
                              root_state <= P2V5_ON;                     //go to next root state                      
                         end 
                    end 

                    P2V5_ON:
                    begin
                         oFM_P2V5_BMC_AUX_EN <= HIGH;                   //power up the VR 
                         if(!iBMC_PWR_EN || rPWR_FAULT)                    //if power down flag set, skip to power down sequence
                              root_state <= P1V0_OFF;
                         else if(iPWRGD_P2V5_BMC_AUX)                   //if the power good signal for P2V5 is high, implement next state now   
                         begin
                              root_state <= P1V2_ON;                    //go to next state
                         end
                    end

                    P1V2_ON: 
                    begin
                         oFM_P1V2_BMC_AUX_EN <= HIGH;                    //assert next enable
                         if(!iBMC_PWR_EN || rPWR_FAULT)                     //if power down flag set, skip to power down sequence
                              root_state <= P1V0_OFF;
                         else if(iPWRGD_P1V2_BMC_AUX)                    //if power good P1V2 is high
                         begin
                              root_state <= P1V0_ON;                     //go to next state
                         end
                    end
                    
                    P1V0_ON: 
                    begin
                         oFM_P1V0_BMC_AUX_EN <= HIGH;                    //power up the VR  
                         if(!iBMC_PWR_EN || rPWR_FAULT)                     //if power down flag set, skip to power down sequence
                              root_state <= P1V0_OFF;
                         else if(iPWRGD_P1V0_BMC_AUX)                    //if the power good signal for P1V0 is high, implement next state now
                         begin
                              root_state <= BMC_PWR_OK;                  //go to next state
                         end
                    end

                    BMC_PWR_OK: 
                    begin
                         oBMC_PWR_OK <= HIGH;                             //All BMC VR's Up and Running 
                         if(!iBMC_PWR_EN || rPWR_FAULT) 
                              root_state <= P1V0_OFF; 
                    end
                    
                    P1V0_OFF: //Thi rail is P1V1 for Archer city, P1V0 VR rework is needed to configure output to 1.1V (required for AST2500 BMC).
                    begin
                         oFM_P1V0_BMC_AUX_EN <= LOW;                        //deassert enable to power down 
                         if(!iPWRGD_P1V0_BMC_AUX)                           //if current stage is off
                         begin
                              root_state <= P1V2_OFF;                       //go to next stage of power-down sequence
                         end
                    end

                    P1V2_OFF: 
                    begin
                         oFM_P1V2_BMC_AUX_EN <= LOW;                       //deassert enable to power down 
                         if(!iPWRGD_P1V2_BMC_AUX)                          //if current stage is off
                         begin
                              root_state <= P2V5_OFF;                      //go to next state
                         end
                    end
                    
                    P2V5_OFF: 
                    begin
                         oFM_P2V5_BMC_AUX_EN <= LOW;                        //deassert enable to power down 
                         if(!iPWRGD_P2V5_BMC_AUX)                           //if current stage is off
                         begin
                              root_state <= P1V8_OFF;                           //now go to state of waiting for power-up
                         end
                    end

                    P1V8_OFF: 
                    begin
                         oFM_PCH_P1V8_AUX_EN <= LOW;                       //deassert enable to power down 
                         if(!iPWRGD_P1V8_BMC_AUX)                          //if current stage is off
                         begin
                              root_state <= INIT;                      //go to next state
                         end
                    end
                    
                    default: 
                    begin
                         root_state <= INIT;
                    end
                    endcase
                    rPREVIOUS_STATE <= root_state;    
               end
               //End of AST2500 interposer state Machine
          end
     end

//////////////////////////////////////////////////////////////////////////////////
// Secuencial Logic
//////////////////////////////////////////////////////////////////////////////////

always @ ( posedge iClk) 
     begin 
          if (  !iRst_n  )   
               begin
                    oBMC_PWR_FAULT                  <= LOW;

                    rPWRGD_P2V5_BMC_FF1             <= LOW;
                    rPWRGD_P1V8_BMC_FF1             <= LOW;
                    rPWRGD_P1V2_BMC_FF1             <= LOW;
                    rPWRGD_P1V0_BMC_FF1             <= LOW;

                    rPWRGD_P2V5_BMC_FAIL            <= LOW;
                    rPWRGD_P1V8_BMC_FAIL            <= LOW;
                    rPWRGD_P1V2_BMC_FAIL            <= LOW;
                    rPWRGD_P1V0_BMC_FAIL            <= LOW;

                    oBMC_PWR_P2V5_FAULT             <= LOW;
                    oBMC_PWR_P1V8_FAULT             <= LOW; 
                    oBMC_PWR_P1V2_FAULT             <= LOW; 
                    oBMC_PWR_P1V0_FAULT             <= LOW;

                    rPWR_FAULT                      <= LOW;
                    oRST_SRST_BMC_N                 <= LOW;
               end 
          else 
               begin
                    rPWRGD_P2V5_BMC_FF1             <= iPWRGD_P2V5_BMC_AUX;
                    rPWRGD_P1V8_BMC_FF1             <= iPWRGD_P1V8_BMC_AUX;
                    rPWRGD_P1V2_BMC_FF1             <= iPWRGD_P1V2_BMC_AUX;
                    rPWRGD_P1V0_BMC_FF1             <= iPWRGD_P1V0_BMC_AUX; 

                    rPWRGD_P2V5_BMC_FAIL            <= (oFM_P2V5_BMC_AUX_EN && rPWRGD_P2V5_BMC_FF1 && !iPWRGD_P2V5_BMC_AUX)  ?  HIGH: rPWRGD_P2V5_BMC_FAIL;
                    rPWRGD_P1V8_BMC_FAIL            <= (oFM_PCH_P1V8_AUX_EN && rPWRGD_P1V8_BMC_FF1 && !iPWRGD_P1V8_BMC_AUX)  ?  HIGH: rPWRGD_P1V8_BMC_FAIL;
                    rPWRGD_P1V2_BMC_FAIL            <= (oFM_P1V2_BMC_AUX_EN && rPWRGD_P1V2_BMC_FF1 && !iPWRGD_P1V2_BMC_AUX)  ?  HIGH: rPWRGD_P1V2_BMC_FAIL;
                    rPWRGD_P1V0_BMC_FAIL            <= (oFM_P1V0_BMC_AUX_EN && rPWRGD_P1V0_BMC_FF1 && !iPWRGD_P1V0_BMC_AUX)  ?  HIGH: rPWRGD_P1V0_BMC_FAIL;
                    oBMC_PWR_P2V5_FAULT             <= rPWRGD_P2V5_BMC_FAIL;
                    oBMC_PWR_P1V8_FAULT             <= rPWRGD_P1V8_BMC_FAIL; 
                    oBMC_PWR_P1V2_FAULT             <= rPWRGD_P1V2_BMC_FAIL; 
                    oBMC_PWR_P1V0_FAULT             <= rPWRGD_P1V0_BMC_FAIL;
                    rPWR_FAULT                      <= (rPWRGD_P2V5_BMC_FAIL || rPWRGD_P1V8_BMC_FAIL || rPWRGD_P1V2_BMC_FAIL || rPWRGD_P1V0_BMC_FAIL) ? HIGH: rPWR_FAULT;
                    oBMC_PWR_FAULT                  <= (rPWRGD_P2V5_BMC_FAIL || rPWRGD_P1V8_BMC_FAIL || rPWRGD_P1V2_BMC_FAIL || rPWRGD_P1V0_BMC_FAIL) ? HIGH: oBMC_PWR_FAULT;
                    oRST_SRST_BMC_N                 <= (wPWRGD_All_VRs_DLY_2mS && oBMC_PWR_OK /*&& iRST_DEDI_BUSY_PLD_N*/);                                     
               end
     end  

//////////////////////////////////////////////////////////////////////
// Instances 
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// 2ms Delay for power UP  RST_SRST_BMC_N
//
counter2 #( .MAX_COUNT(T_2mS_2M) ) PWR_OK_2mS_DLY
( .iClk        (iClk), 
.iRst_n        (iRst_n),

.iCntRst_n     (oBMC_PWR_OK),
.iCntEn        (oBMC_PWR_OK),
.iLoad         (!(oBMC_PWR_OK)),
.iSetCnt       (T_2mS_2M[11:0]),  // set the time out time as 2ms
.oDone         (wPWRGD_All_VRs_DLY_2mS),
.oCntr         ( ) 
);

counter2 #( .MAX_COUNT(T_1mS_2M) ) PWR_UP_DOWN_1mS_DLY
( .iClk         (iClk), 
.iRst_n        (iRst_n),

.iCntRst_n     (wCNT_RST_N),
.iCntEn        (wCNT_RST_N),
.iLoad         (!(wCNT_RST_N)),
.iSetCnt       (T_1mS_2M[10:0]),  // set the time out time as 1ms
.oDone         (wDLY_1mS),
.oCntr         ( ) 
);

endmodule