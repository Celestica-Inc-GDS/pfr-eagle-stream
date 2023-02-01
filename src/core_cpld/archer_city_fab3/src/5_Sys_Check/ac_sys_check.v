/* ==========================================================================================================
   = Project     :  Archer City Power Sequencing
   = Title       :  SYSTEM CHECK
   = File        :  ac_sys_check.v
   ==========================================================================================================
   = Author      :  Pedro Saldana
   = Division    :  BD GDC
   = E-mail      :  pedro.a.saldana.zepeda@intel.com
   ==========================================================================================================
   = Updated by  :  
   = Division    :  
   = E-mail      :  
   ==========================================================================================================
   = Description :  Checks if the CPU IDs and Package IDs match the correct way in the platform.

   = Constraints :  
   
   = Notes       :  
   
   ==========================================================================================================
   = Revisions   :  
     July 24, 2019;    0.1; Initial release
   ========================================================================================================== */

module ac_sys_check
(
// ------------------------
// Clock and Reset signals
// ------------------------
    input       iClk, //clock for sequential logic 
    input       iRst_n, //reset signal from PLL Lock, resets state machine to initial state
// ----------------------------
// inputs and outputs
// ---------------------------- 
    input [1:0] ivCPU_SKT_OCC, //Socket occupied (input vector for CPU0 and CPU1 SKT_OCC signal)
    input [1:0] ivPROC_ID_CPU0, //CPU0 Processor ID
    input [1:0] ivPROC_ID_CPU1, //CPU1 Processor ID
    input [2:0] ivPKG_ID_CPU0, //CPU0 Package ID
    input [2:0] ivPKG_ID_CPU1, //CPU1 Package ID
    input       iCPU_INTR, //Interposer present, CPU0/1 interposer
     //input          iAUX_PWR_DONE,              //This signal is an enabler for this module, this signal migth be redundant as 3.3V_AUX power source FPGA
                                                  //CPU straps are refernced to 3.3V_AUX, so it migth not be needed

    input       iFmPchPrsnt_n,            //These 2 inputs configure if we have EBG, LBG or the ValBrd (EVB) connected      
    input       iFmEvbPrsnt_n,         // 2'b00-->EBG ; 2'b01-->LBG (N-1) ; (2'b10-->EVB) ; (2'b11-->no PCH)
 
    output reg  oSYS_OK, //System validation Ok, once this module has detected a valid CPU configurqation this signal will be set 
    output reg  oCPU_MISMATCH, //CPU Mismatch, if not CPU ID or PKG ID were identified then this signal will remain low, this signal is used in BMC SGPIOs module
    output reg  oHBM, //Output enabler for HBM VR
    output reg  oSOCKET_REMOVED, //Socket Removed
    output reg  oHbm2Hbm3VppSel,  //

    output reg  oPchPrsnt_n,   //active-low, enables PCH sequencer if a device is detected
    output reg  oEvbPrsnt_n   //active-low, will indicate MstrSeq to override BMC ONCTL signal if EVB is prsnt
);

//////////////////////////////////////////////////////////////////////////////////
// States for root state machine
//////////////////////////////////////////////////////////////////////////////////
localparam INIT                   = 4'd0;
localparam VALID_CPU0             = 4'd1;
localparam VALID_CPU1             = 4'd2;
localparam SYS_OK_HBM             = 4'd3;
localparam SYS_OK                 = 4'd4;
localparam CPU_MISMATCH           = 4'd5;
localparam SKT_REMOVED            = 4'd6;

//////////////////////////////////////////////////////////////////////////////////
// Parameters
//////////////////////////////////////////////////////////////////////////////////
localparam  LOW =1'b0;
localparam  HIGH=1'b1;

localparam  SPR  = 2'b00;  //SPR
localparam  RFU1 = 2'b01;  //Reserved
localparam  GNR  = 2'b10;  //GNR
localparam  RFU2 = 2'b11;  //Reserved

localparam  NON_MCP = 3'b000;  //NON_MCP
localparam  RFU3    = 3'b001;  //Reserved
localparam  HBM     = 3'b010;  //HBM
localparam  BOARD   = 3'b111;  //alarios: Board default (used for debug)
   

reg [3:0] root_state;     //main sequencing state machine
reg ffCPU1_PRESENT;    

    //state machine for system CPU configuration check.
     always @ (posedge iClk, negedge iRst_n)
     begin
        if(!iRst_n)                         //asynchronous, negative-edge reset
        begin
           oSYS_OK         <= LOW;        //Initail state for system ok!
           oCPU_MISMATCH   <= LOW;        //Initail state for CPU mismatch         
           oHBM            <= LOW;        //Output enabler for HBM VR
           oSOCKET_REMOVED <= LOW;
           ffCPU1_PRESENT  <= LOW;
           root_state  <= INIT;           //reset state machines to initial states
           
           oHbm2Hbm3VppSel <= 1'b1;       //alarios: reset value to select SPR config
           
        end
        else
        begin
          if(!iCPU_INTR)                    //No interposer
          begin        
          case(root_state)
            INIT: 
              begin                         
               if(ivCPU_SKT_OCC[0] == LOW)     //Verify CPU0 is present
               begin
                  root_state <= VALID_CPU0;    //Go to check if CPU0 is valid      
               end
            end
                    
             VALID_CPU0:
             begin
                if ((ivPROC_ID_CPU0 == SPR || ivPROC_ID_CPU0 == GNR) && ivCPU_SKT_OCC[1] == HIGH)    //If CPU0 is a SPR or GNR and CPU1 not present then check if CPU0 SKU is valid 
                  begin
                     if (ivPROC_ID_CPU0 == SPR)              //alarios:if we have SPR, we need oHbm2Hbm3VppSel to be HIGH to select 2.5V on PVPP VR to feed HBM2, else 1.8V for HBM3 (GNR)
                       begin
                          oHbm2Hbm3VppSel <= 1'b1;           //alarios: there is only 1 signal for both CPU VRs
                       end
                     else
                       begin
                          oHbm2Hbm3VppSel <= 1'b0;
                       end
                     case (ivPKG_ID_CPU0)                        
                       NON_MCP:   root_state <= SYS_OK;       //CPU0 is supported 
                       HBM:       root_state <= SYS_OK_HBM;   //CPU0 is supported
                       BOARD:     root_state <= SYS_OK_HBM;   //alarios: Board case, no CPUs, used for debug
                       default:   root_state <= CPU_MISMATCH; //CPU0 is not supported
                     endcase
                  end
                else if ((ivPROC_ID_CPU0 == SPR || ivPROC_ID_CPU0 == GNR) && ivCPU_SKT_OCC[1] == LOW) begin      //Check if CPU1 is present
                   if (ivPROC_ID_CPU0 == SPR)              //alarios:if we have SPR, we need oHbm2Hbm3VppSel to be HIGH to select 2.5V on PVPP VR to feed HBM2, else 1.8V for HBM3 (GNR)
                       begin
                          oHbm2Hbm3VppSel <= 1'b1;           //alarios: there is only 1 signal for both CPU VRs
                       end
                     else
                       begin
                          oHbm2Hbm3VppSel <= 1'b0;
                       end
                   ffCPU1_PRESENT  <= HIGH;
                   root_state      <= VALID_CPU1;                //next state -> check if CPU1 is valid
                end
                else begin
                   root_state <= CPU_MISMATCH;
                end                
             end // case: VALID_CPU0            

            VALID_CPU1:
              begin
                if ((ivPROC_ID_CPU1 == SPR || ivPROC_ID_CPU1 == GNR) && ivCPU_SKT_OCC[1] == LOW)       //If CPU1 is a SPR or GNR then check both CPUs are compatible
                  begin                     
                     case ({ivPKG_ID_CPU0, ivPKG_ID_CPU1})           //CPU1 is present then check PKG IDs
                       {NON_MCP, NON_MCP}:  root_state <= SYS_OK;        //Check if CPUs are compatible
                       {HBM, HBM}:          root_state <= SYS_OK_HBM;    //Check if CPUs are compatible
                       {BOARD, BOARD} :     root_state <= SYS_OK_HBM;   //alarios: Board case, no CPUs, used for debug
                       default:             root_state <= CPU_MISMATCH;  //CPUs no compatible
                     endcase
                  end
                else begin
                   root_state <= CPU_MISMATCH;
                end  
              end

            SYS_OK:                                                        // system ok!, the only way to get out of this state is AC cycle or Deep Sleep or detecting CPU removed
             begin
                oSYS_OK <= HIGH;
                                                                           //Set to one to indicate system configuration is valid
                if ((ivCPU_SKT_OCC[0] == HIGH) || (ivCPU_SKT_OCC[1] == HIGH && ffCPU1_PRESENT == HIGH)) //If CPU0 or CPU1 is removed 
                begin
                    root_state <= SKT_REMOVED;                             //Fault state, CPU removed while system was on 
                end
             end

             SYS_OK_HBM:                                                   // system ok!, the only way to get out of this state is AC cycle or Deep Sleep or detecting CPU removed
             begin
                oSYS_OK <= HIGH;                                           //Set to one to indicate system configuration is valid
                oHBM    <= HIGH;                                           // HBM CPU SKU present        

                if ((ivCPU_SKT_OCC[0] == HIGH) || (ivCPU_SKT_OCC[1] == HIGH && ffCPU1_PRESENT == HIGH)) //If CPU0 or CPU1 is removed 
                begin
                    root_state <= SKT_REMOVED;                             //Fault state, CPU removed while system was on 
                end  
             end

             CPU_MISMATCH:                    //The only way to get out of this state is AC cycle or a Deep Sleep 
             begin
               oSYS_OK         <= LOW;        //System should not boot
               oCPU_MISMATCH   <= HIGH;        //CPU mismatch
               root_state      <= CPU_MISMATCH;
             end

             SKT_REMOVED:                     //The only way to get out of this state is AC cycle or a Deep Sleep 
             begin
              oSYS_OK         <= LOW;        //System should not boot
              oSOCKET_REMOVED <= HIGH;        //Fault state, CPU removed while system was on
              root_state      <= SKT_REMOVED; 
             end
            
            default: root_state <= INIT;
            endcase
            end    
          //End sys_check state Machine
          else if(iCPU_INTR)               //Remove this code it is only for interposer support
          begin
            oSYS_OK         <= HIGH;        //Initail state for system ok!
          end
        end
     end // always @ (posedge iClk, negedge iRst_n)


   //This logic will take Pch & ValBrd present signals to determine what device is configured
   //to know what to do
   always @(*)
     begin
        case ({iFmEvbPrsnt_n,iFmPchPrsnt_n})
          default : begin   //2'b11 No device present as PCH 
             oPchPrsnt_n <= 1'b1;
             oEvbPrsnt_n <= 1'b1;
          end
          2'b00 : begin
             oPchPrsnt_n <= 1'b0;    //EBG present
             oEvbPrsnt_n <= 1'b1;
          end

          2'b01 : begin
             oPchPrsnt_n <= 1'b0;    //EVB present (both signals need to be asserted)
             oEvbPrsnt_n <= 1'b0;    //to enable pch seq and override BMC onctl on MstrSeq
          end
          
          2'b10 : begin
             oPchPrsnt_n <= 1'b0;    //Interposer present (pch n-1 )
             oEvbPrsnt_n <= 1'b1;    
          end
        endcase // case ({iFmEvbPrsnt_n,iFmPchPrsnt_n})
     end // always @ (*)
   
   
endmodule