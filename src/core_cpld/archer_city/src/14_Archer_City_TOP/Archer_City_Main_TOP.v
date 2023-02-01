/* ==========================================================================================================
   = Project     :  Archer City
   = Title       :  Archer City TOP Module
   = File        :  Archer_City_Main_TOP.v
   ==========================================================================================================
   = Author      :  Alejandro Larios
   					Pedro Saldana
   = Division    :  BD GDC
   = E-mail      :  <a.larios@intel.com>
   					<pedro.a.saldana.zepeda@intel.com>
                    
   ==========================================================================================================
   = Updated by  :  
   = Division    :  
   = E-mail      :  
   ==========================================================================================================
   = Description :  

   = Constraints :  
   
   = Notes       :  
   
   ==========================================================================================================
   = Revisions   :  
     July 15, 2019;    0.1; Initial release
   ========================================================================================================== */
`timescale 1ns/1ns   
//`undef simul

module Archer_City_Main_TOP (

	// Input CLK
	input	CLK_25M_CKMNG_MAIN_PLD, 

	//BMC SGPIO
	input	SGPIO_BMC_CLK,
	input	SGPIO_BMC_DOUT,
	output	SGPIO_BMC_DIN_R,
	input	SGPIO_BMC_LD_N,

	//LED and 7-Seg Control Logic
	output [7:0] LED_CONTROL,
	output	FM_CPU0_DIMM_CH1_4_FAULT_LED_SEL,
	output	FM_CPU0_DIMM_CH5_8_FAULT_LED_SEL,
	output	FM_CPU1_DIMM_CH1_4_FAULT_LED_SEL,
	output	FM_CPU1_DIMM_CH5_8_FAULT_LED_SEL,
	output	FM_FAN_FAULT_LED_SEL_R_N,
	output	FM_POST_7SEG1_SEL_N,
	output	FM_POST_7SEG2_SEL_N,
	output	FM_POSTLED_SEL,

	//CATERR / RMCA / NMI Logic
	output	FM_CPU_RMCA_CATERR_DLY_LVT3_R_N,
	input	H_CPU_CATERR_LVC1_R_N,
	input	H_CPU_RMCA_LVC1_R_N,
	output	FM_CPU_RMCA_CATERR_LVT3_N,
	output	H_CPU_NMI_LVC1_R_N,
	input	IRQ_BMC_CPU_NMI,
	input	IRQ_PCH_CPU_NMI_EVENT_N,

	//ADR
	input	FM_ADR_COMPLETE,
	output	FM_ADR_ACK_R,
	// output	FM_ADR_SMI_GPIO_N,
	output	FM_ADR_TRIGGER_N,
	// input	FM_PLD_PCH_DATA_R,
	// input	FM_PS_PWROK_DLY_SEL,
	// input	FM_DIS_PS_PWROK_DLY,
	
	//SmaRT Logic
	input	FM_PMBUS_ALERT_B_EN,
	input	FM_THROTTLE_R_N,
	input	IRQ_SML1_PMBUS_PLD_ALERT_N,
	output	FM_SYS_THROTTLE_R_N,
	
	//Termtrip dly
	input	H_CPU0_THERMTRIP_LVC1_N,
	input	H_CPU1_THERMTRIP_LVC1_N,
	output	FM_THERMTRIP_DLY_LVC1_R_N,
	
	//MEMTRIP
	input	H_CPU0_MEMTRIP_LVC1_N,
	input	H_CPU1_MEMTRIP_LVC1_N,
	
	// PROCHOT
	input	IRQ_CPU0_VRHOT_N,
	input	IRQ_CPU1_VRHOT_N,
	output	H_CPU0_PROCHOT_LVC1_R_N,
	output	H_CPU1_PROCHOT_LVC1_R_N,
	
	//PERST & RST
	input	FM_RST_PERST_BIT0,
	input	FM_RST_PERST_BIT1,
	input	FM_RST_PERST_BIT2,
	output	RST_PCIE_CPU0_DEV_PERST_N,
	output	RST_PCIE_CPU1_DEV_PERST_N,
	output	RST_PCIE_PCH_DEV_PERST_N,
	output	RST_CPU0_LVC1_R_N,
	output	RST_CPU1_LVC1_R_N,
	output	RST_PLTRST_PLD_B_N,
	input	RST_PLTRST_PLD_N,
	
	//SysChk CPU
	input	FM_CPU0_PKGID0,
	input	FM_CPU0_PKGID1,
	input	FM_CPU0_PKGID2,
	input	FM_CPU0_PROC_ID0,
	input	FM_CPU0_PROC_ID1,
	input	FM_CPU0_INTR_PRSNT,
	input	FM_CPU0_SKTOCC_LVT3_PLD_N,
	input	FM_CPU1_PKGID0,
	input	FM_CPU1_PKGID1,
	input	FM_CPU1_PKGID2,
	input	FM_CPU1_PROC_ID0,
	input	FM_CPU1_PROC_ID1,
	input	FM_CPU1_SKTOCC_LVT3_PLD_N,
	
	
	//BMC
	input	FM_BMC_INTR_PRSNT,
	input	FM_BMC_PWRBTN_OUT_N,
	output	FM_BMC_PFR_PWRBTN_OUT_R_N,
	input	FM_BMC_ONCTL_N_PLD,
	output	RST_SRST_BMC_PLD_R_N,
	output	FM_P2V5_BMC_AUX_EN,
	input	PWRGD_P2V5_BMC_AUX,
	output	FM_P1V2_BMC_AUX_EN,
	input	PWRGD_P1V2_BMC_AUX,
	output	FM_P1V0_BMC_AUX_EN,
	input	PWRGD_P1V0_BMC_AUX,
	//input	FP_BMC_PWR_BTN_R2_N,

	//PCH
	output	RST_RSMRST_PLD_R_N,
	output	PWRGD_PCH_PWROK_R,
	output	PWRGD_SYS_PWROK_R,
	input	FM_SLP_SUS_RSM_RST_N,
	input	FM_SLPS3_PLD_N,
	input	FM_SLPS4_PLD_N,
	input	FM_PCH_PRSNT_N,
	output	FM_PCH_P1V8_AUX_EN,
	input	PWRGD_P1V05_PCH_AUX,
	input	PWRGD_P1V8_PCH_AUX_PLD,
		
	output	FM_P1V05_PCH_AUX_EN,
	output	FM_PVNN_PCH_AUX_EN,
	input	PWRGD_PVNN_PCH_AUX,
	
	//PSU Ctl
	output	FM_PS_EN_PLD_R,
	input	PWRGD_PS_PWROK_PLD_R,
	
	//Clock Logic
	output	FM_PLD_CLKS_DEV_R_EN,
	output	FM_PLD_CLK_25M_EN,
	
	//PLL reset Main FPGA
	input	PWRGD_P1V2_MAX10_AUX_R,  //PWRGD_P3V3_AUX in VRTB board
	
	//Main VR & Logic
	input	PWRGD_P3V3,
	output	FM_P5V_EN,
	output	FM_AUX_SW_EN,
	
	//CPU VRs
	output	PWRGD_CPU0_LVC1_R2,
	output	PWRGD_CPU1_LVC1_R,
	input	PWRGD_CPUPWRGD_LVC1,
	output	PWRGD_PLT_AUX_CPU0_LVT3_R,
	output	PWRGD_PLT_AUX_CPU1_LVT3_R,
		
	output	FM_PVCCIN_CPU0_R_EN,
	input	PWRGD_PVCCIN_CPU0,
	output	FM_PVCCFA_EHV_FIVRA_CPU0_R_EN,
	input	PWRGD_PVCCFA_EHV_FIVRA_CPU0,
	output	FM_PVCCINFAON_CPU0_R_EN,
	input	PWRGD_PVCCINFAON_CPU0,
	output	FM_PVCCFA_EHV_CPU0_R_EN,
	input	PWRGD_PVCCFA_EHV_CPU0,
	output	FM_PVCCD_HV_CPU0_EN,
	input	PWRGD_PVCCD_HV_CPU0,
	output	FM_PVNN_MAIN_CPU0_EN,
	input	PWRGD_PVNN_MAIN_CPU0,
	output	FM_PVPP_HBM_CPU0_EN,
	input	PWRGD_PVPP_HBM_CPU0,
	output	FM_PVCCIN_CPU1_R_EN,
	input	PWRGD_PVCCIN_CPU1,
	output	FM_PVCCFA_EHV_FIVRA_CPU1_R_EN,
	input	PWRGD_PVCCFA_EHV_FIVRA_CPU1,
	output	FM_PVCCINFAON_CPU1_R_EN,
	input	PWRGD_PVCCINFAON_CPU1,
	output	FM_PVCCFA_EHV_CPU1_R_EN,
	input	PWRGD_PVCCFA_EHV_CPU1,
	output	FM_PVCCD_HV_CPU1_EN,
	input	PWRGD_PVCCD_HV_CPU1,
	output	FM_PVNN_MAIN_CPU1_EN,
	input	PWRGD_PVNN_MAIN_CPU1,
	output	FM_PVPP_HBM_CPU1_EN,
	input	PWRGD_PVPP_HBM_CPU1,
	input	IRQ_PSYS_CRIT_N,
	
	//Dediprog Detection Support
	input	RST_DEDI_BUSY_PLD_N,

	//Debug Signals
	input	FM_FORCE_PWRON_LVC3,
	output	FM_PLD_HEARTBEAT_LVC3,
	// output	SGPIO_DEBUG_PLD_CLK,
	// input	SGPIO_DEBUG_PLD_DIN,
	// output	SGPIO_DEBUG_PLD_DOUT,
	// output	SGPIO_DEBUG_PLD_LD_N,
	//input	SMB_DEBUG_PLD_SCL,
	//inout	SMB_DEBUG_PLD_SDA,
	input	FM_PLD_REV_N,

	//PFR Dedicated Signals
	output	FAN_BMC_PWM_R,
	output	FM_PFR_DSW_PWROK_N,
	input	FM_ME_AUTHN_FAIL,
	input	FM_ME_BT_DONE,
	input	FM_PFR_TM1_HOLD_N,
	input	FM_PFR_POSTCODE_SEL_N,
	input	FM_PFR_FORCE_RECOVERY_N,
	output	FM_PFR_MUX_OE_CTL_PLD,
	output	RST_PFR_EXTRST_R_N,
	output	RST_PFR_OVR_RTC_R,
	output	RST_PFR_OVR_SRTC_R,
	output	FM_PFR_ON_R,
	output	FM_PFR_SLP_SUS_EN,
	output	FP_ID_LED_PFR_N,
	input	FP_ID_LED_N,
	output	FP_LED_STATUS_AMBER_PFR_N,
	input	FP_LED_STATUS_AMBER_N,
	output	FP_LED_STATUS_GREEN_PFR_N,
	input	FP_LED_STATUS_GREEN_N,

	//SPI BYPASS TOP!!!
	output	FM_SPI_PFR_PCH_MASTER_SEL_R,
	output	FM_SPI_PFR_BMC_BT_MASTER_SEL_R,
	output	RST_SPI_PFR_BMC_BOOT_N,
	output	RST_SPI_PFR_PCH_N,
	input	SPI_BMC_BOOT_CS_N,
	output	SPI_PFR_BMC_BT_SECURE_CS_R_N,
	input	SPI_PCH_PFR_CS0_N,
	output	SPI_PFR_PCH_SECURE_CS0_R_N,
	input	SPI_PCH_CS1_N,
	output	SPI_PFR_PCH_SECURE_CS1_N,
	inout	SPI_BMC_BT_MUXED_MON_CLK,
	inout	SPI_BMC_BT_MUXED_MON_MISO,
	inout	SPI_BMC_BT_MUXED_MON_MOSI,
	inout	SPI_BMC_BT_MUXED_MON_IO2,
	inout	SPI_BMC_BT_MUXED_MON_IO3,
	inout	SPI_PCH_MUXED_MON_CLK,
	inout	SPI_PCH_MUXED_MON_IO0,
	inout	SPI_PCH_MUXED_MON_IO1,
	inout	SPI_PCH_MUXED_MON_IO2,
	inout	SPI_PCH_MUXED_MON_IO3,
	inout	SPI_PFR_BMC_FLASH1_BT_CLK,
	inout	SPI_PFR_BMC_FLASH1_BT_MOSI,
	inout	SPI_PFR_BMC_FLASH1_BT_MISO,
	inout	SPI_PFR_BMC_BOOT_R_IO2,
	inout	SPI_PFR_BMC_BOOT_R_IO3,
	inout	SPI_PFR_PCH_R_CLK,
	inout	SPI_PFR_PCH_R_IO0,
	inout	SPI_PFR_PCH_R_IO1,
	inout	SPI_PFR_PCH_R_IO2,
	inout	SPI_PFR_PCH_R_IO3,

	//SMBus
	inout	SMB_BMC_HSBP_STBY_LVC3_SCL,            //changed to inout due to the SMBus bypass uses CLK-Stretching feature
	inout	SMB_BMC_HSBP_STBY_LVC3_SDA,
	inout	SMB_PFR_HSBP_STBY_LVC3_SCL,            //changed to inout due to the SMBus BYPASS uses CLK-Stretching feature
	inout	SMB_PFR_HSBP_STBY_LVC3_SDA,
	inout	SMB_PFR_PMB1_STBY_LVC3_SCL,            //changed to inout due to the SMBus BYPASS uses CLK-Stretching feature
	inout	SMB_PFR_PMB1_STBY_LVC3_SDA,
	inout	SMB_PMBUS_SML1_STBY_LVC3_SDA,
	inout	SMB_PMBUS_SML1_STBY_LVC3_SCL,          //changed to inout due to the SMBus BYPASS uses CLK-Stretching feature
	inout	SMB_PCH_PMBUS2_STBY_LVC3_SCL,
	inout	SMB_PCH_PMBUS2_STBY_LVC3_SDA,
	inout	SMB_PFR_PMBUS2_STBY_LVC3_SCL,          //changed to inout due to the SMBus BYPASS uses CLK-Stretching feature
	inout	SMB_PFR_PMBUS2_STBY_LVC3_SDA,
	inout	SMB_PFR_RFID_STBY_LVC3_SCL,            //changed to inout due to the SMBus BYPASS uses CLK-Stretching feature
	inout	SMB_PFR_RFID_STBY_LVC3_SDA,

	//Deltas for PFR
	//inout	SMB_S3M_CPU0_SDA_LVC1,
	//input	SMB_S3M_CPU0_SCL_LVC1,
	output	H_S3M_CPU0_SMBUS_ALERT_LVC1_R_N,
	//inout	SMB_S3M_CPU1_SDA_LVC1,
	//input	SMB_S3M_CPU1_SCL_LVC1,
	output	H_S3M_CPU1_SMBUS_ALERT_LVC1_R_N,
	inout	SMB_PCIE_STBY_LVC3_B_SDA,
	input	SMB_PCIE_STBY_LVC3_B_SCL,
	input	SPI_PCH_TPM_CS_N,
	output	SPI_PFR_TPM_CS_R2_N,
	output	FM_PFR_RNDGEN_AUX,

	//Crashlog & Crashdump
	input	FM_BMC_CRASHLOG_TRIG_N,
	output	FM_PCH_CRASHLOG_TRIG_R_N,
	input	FM_PCH_PLD_GLB_RST_WARN_N,

	//Reset from CPU to VT to CPLD
	input	M_AB_CPU0_RESET_N,
	input	M_CD_CPU0_RESET_N,
	input	M_EF_CPU0_RESET_N,
	input	M_GH_CPU0_RESET_N,
	input	M_AB_CPU1_RESET_N,
	input	M_CD_CPU1_RESET_N,
	input	M_EF_CPU1_RESET_N,
	input	M_GH_CPU1_RESET_N,

	//Reset from CPLD to VT to DIMM
	output	M_AB_CPU0_FPGA_RESET_R_N,
	output	M_CD_CPU0_FPGA_RESET_R_N,
	output	M_EF_CPU0_FPGA_RESET_R_N,
	output	M_GH_CPU0_FPGA_RESET_R_N,
	output	M_AB_CPU1_FPGA_RESET_R_N,
	output	M_CD_CPU1_FPGA_RESET_R_N,
	output	M_EF_CPU1_FPGA_RESET_R_N,
	output	M_GH_CPU1_FPGA_RESET_R_N,
	//Prochot & Memhot
	input	IRQ_CPU0_MEM_VRHOT_N,
	input	IRQ_CPU1_MEM_VRHOT_N,

	output	H_CPU0_MEMHOT_IN_LVC1_R_N,
	output	H_CPU1_MEMHOT_IN_LVC1_R_N,

	input	H_CPU0_MEMHOT_OUT_LVC1_N,
	input	H_CPU1_MEMHOT_OUT_LVC1_N,

	inout	PWRGD_FAIL_CPU0_AB,
	inout	PWRGD_FAIL_CPU0_CD,
	inout	PWRGD_FAIL_CPU0_EF,
	inout	PWRGD_FAIL_CPU0_GH,
	inout	PWRGD_FAIL_CPU1_AB,
	inout	PWRGD_FAIL_CPU1_CD,
	inout	PWRGD_FAIL_CPU1_EF,
	inout	PWRGD_FAIL_CPU1_GH,

	output	PWRGD_DRAMPWRGD_CPU0_AB_R_LVC1,
	output	PWRGD_DRAMPWRGD_CPU0_CD_R_LVC1,
	output	PWRGD_DRAMPWRGD_CPU0_EF_R_LVC1,
	output	PWRGD_DRAMPWRGD_CPU0_GH_R_LVC1,
	output	PWRGD_DRAMPWRGD_CPU1_AB_R_LVC1,
	output	PWRGD_DRAMPWRGD_CPU1_CD_R_LVC1,
	output	PWRGD_DRAMPWRGD_CPU1_EF_R_LVC1,
	output	PWRGD_DRAMPWRGD_CPU1_GH_R_LVC1,

	//IBL
//	output	FM_IBL_ADR_TRIGGER_PLD_R,
//	input	FM_IBL_ADR_COMPLETE_CPU0,
//	input	FM_IBL_ADR_ACK_CPU0_R,
//	output	FM_IBL_WAKE_CPU0_R_N,
//	output	FM_IBL_SYS_RST_CPU0_R_N,
//	output	FM_IBL_PWRBTN_CPU0_R_N,
//	output	FM_IBL_GRST_WARN_PLD_R_N,
//	input	FM_IBL_PLT_RST_SYNC_N,
//	input	FM_IBL_SLPS3_CPU0_N,
//	input	FM_IBL_SLPS4_CPU0_N,
//	input	FM_IBL_SLPS5_CPU0_N,
	input	FM_IBL_SLPS0_CPU0_N,

	//Errors
	input	H_CPU_ERR0_LVC1_N,
	input	H_CPU_ERR1_LVC1_N,
	input	H_CPU_ERR2_LVC1_N,
	output	FM_CPU_ERR0_LVT3_N,
	output	FM_CPU_ERR1_LVT3_N,
	output	FM_CPU_ERR2_LVT3_N,
	input	H_CPU0_MON_FAIL_PLD_LVC1_N,
	input	H_CPU1_MON_FAIL_PLD_LVC1_N,

	//eMMC
//	output	FM_EMMC_PFR_R_EN,
//
//	inout	EMMC_MUX_BMC_PFR_OUT_CMD,
//	inout	EMMC_MUX_BMC_PFR_LVT3_R_CMD,
//	inout	RST_EMMC_MUX_BMC_PFR_LVT3_R_N,
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA0_R,
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA1_R,
//
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA2_R,
//
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA3_R,
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA4_R,
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA5_R,
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA6_R,
//	inout	EMMC_MUX_BMC_PFR_LVT3_DATA7_R,
//
//	inout	EMMC_MUX_BMC_PFR_LVT3_R_CLK,

	//EMG Val board
	// input	FM_VAL_BOARD_PRSNT_N,

	//clock gen

	//In-Field System Test 
//	input	H_CPU0_IFST_DONE_PLD_LVC1,
//	input	H_CPU0_IFST_TRIG_PLD_LVC1,
//	input	H_CPU1_IFST_DONE_PLD_LVC1,
//	input	H_CPU1_IFST_TRIG_PLD_LVC1,
//	output	H_CPU0_IFST_KOT_PLD_LVC1,
//	output	H_CPU1_IFST_KOT_PLD_LVC1,

	//S3 Pwr-seq control
	output	FM_SX_SW_P12V_STBY_R_EN,
	output	FM_SX_SW_P12V_R_EN,
	input	FM_DIMM_12V_CPS_S5_N,

	// IDV Tool SGPIO
	// input	SGPIO_IDV_CLK_R,
	// input	SGPIO_IDV_DOUT_R,
	// output	SGPIO_IDV_DIN_R,
	// input	SGPIO_IDV_LD_R_N

	// Interposer PFR reuse ports
	input	SMB_S3M_CPU0_SDA_LVC1,
	input	SMB_S3M_CPU0_SCL_LVC1,
	input	SMB_S3M_CPU1_SDA_LVC1,
	input	SMB_S3M_CPU1_SCL_LVC1
);

//////////////////////////////////////////////////////////////////////////////////
// Parameters
//////////////////////////////////////////////////////////////////////////////////
parameter  LOW =1'b0;
parameter  HIGH=1'b1;  
parameter  Z=1'bz;

//////////////////////////////////////////////////////////////////////////////////
// Internal Signals
//////////////////////////////////////////////////////////////////////////////////
wire wClk_2M, wClk_50M, wRST_N;

////////////////////////////////////////////////////////////////////////////////// 
// Continuous assignments                                                          
////////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////////
// Instances HW Dedicated 
//////////////////////////////////////////////////////////////////////////////////

   
//alarios: This is PLL instance, but if we are in simulation, 
   // then we use inputs directly connected to internal signals
   
`ifdef simul
   assign wClk_2M =  CLK_25M_CKMNG_MAIN_PLD;
   assign wClk_50M = CLK_25M_CKMNG_MAIN_PLD;
   assign wRST_N = PWRGD_P1V2_MAX10_AUX_R;
`else

AC_PLL_CLK2	AC_PLL_CLK2_inst (
	.areset ( !PWRGD_P1V2_MAX10_AUX_R ),
	.inclk0 ( CLK_25M_CKMNG_MAIN_PLD ),
	.c0 ( wClk_2M ),
	.c1 ( wClk_50M ),
	.locked ( wRST_N )
	);
   
`endif

////////////////////////////////////////////////////////////////////////////////// 
// PFR By pass logic                                                              
////////////////////////////////////////////////////////////////////////////////// 
PFRBypass mPFR_ByPass
(
	.iClk(wClk_2M),
	.iclk_Bypass(CLK_25M_CKMNG_MAIN_PLD/*wClk_50M*/),
	.iRst_n(wRST_N),
	//PFR Dedicated Signals
	.FAN_BMC_PWM_R(FAN_BMC_PWM_R),
	.FM_PFR_DSW_PWROK_N(FM_PFR_DSW_PWROK_N),
	.FM_ME_AUTHN_FAIL(FM_ME_AUTHN_FAIL),
	.FM_ME_BT_DONE(FM_ME_BT_DONE),
	.FM_PFR_TM1_HOLD_N(FM_PFR_TM1_HOLD_N),
	.FM_PFR_POSTCODE_SEL_N(FM_PFR_POSTCODE_SEL_N),
	.FM_PFR_FORCE_RECOVERY_N(FM_PFR_FORCE_RECOVERY_N),
	//.FM_PFR_MUX_OE_CTL_PLD(FM_PFR_MUX_OE_CTL_PLD),
	.RST_PFR_EXTRST_R_N(RST_PFR_EXTRST_R_N),
	.RST_PFR_OVR_RTC_R(RST_PFR_OVR_RTC_R),
	.RST_PFR_OVR_SRTC_R(RST_PFR_OVR_SRTC_R),
	.FM_PFR_ON_R(FM_PFR_ON_R),
	.FM_PFR_SLP_SUS_EN(/*FM_PFR_SLP_SUS_EN*/),
	.FP_ID_LED_PFR_N(FP_ID_LED_PFR_N),
	.FP_ID_LED_N(FP_ID_LED_N),
	.FP_LED_STATUS_AMBER_PFR_N(FP_LED_STATUS_AMBER_PFR_N),
	.FP_LED_STATUS_AMBER_N(FP_LED_STATUS_AMBER_N),
	.FP_LED_STATUS_GREEN_PFR_N(FP_LED_STATUS_GREEN_PFR_N),
	.FP_LED_STATUS_GREEN_N(FP_LED_STATUS_GREEN_N),
	//SPI BYPASS
	.FM_SPI_PFR_PCH_MASTER_SEL_R(FM_SPI_PFR_PCH_MASTER_SEL_R),
	.FM_SPI_PFR_BMC_BT_MASTER_SEL_R(FM_SPI_PFR_BMC_BT_MASTER_SEL_R),
	.RST_SPI_PFR_BMC_BOOT_N(RST_SPI_PFR_BMC_BOOT_N),
	.RST_SPI_PFR_PCH_N(RST_SPI_PFR_PCH_N),
	.SPI_BMC_BOOT_CS_N(SPI_BMC_BOOT_CS_N),
	.SPI_PFR_BMC_BT_SECURE_CS_R_N(SPI_PFR_BMC_BT_SECURE_CS_R_N),
	.SPI_PCH_PFR_CS0_N(SPI_PCH_PFR_CS0_N),
	.SPI_PFR_PCH_SECURE_CS0_R_N(SPI_PFR_PCH_SECURE_CS0_R_N),
	.SPI_PCH_CS1_N(SPI_PCH_CS1_N),
	.SPI_PFR_PCH_SECURE_CS1_N(SPI_PFR_PCH_SECURE_CS1_N),
	.SPI_BMC_BT_MUXED_MON_CLK(SPI_BMC_BT_MUXED_MON_CLK),
	.SPI_BMC_BT_MUXED_MON_MISO(SPI_BMC_BT_MUXED_MON_MISO),
	.SPI_BMC_BT_MUXED_MON_MOSI(SPI_BMC_BT_MUXED_MON_MOSI),
	.SPI_BMC_BT_MUXED_MON_IO2(SPI_BMC_BT_MUXED_MON_IO2),
	.SPI_BMC_BT_MUXED_MON_IO3(SPI_BMC_BT_MUXED_MON_IO3),
	.SPI_PCH_MUXED_MON_CLK(SPI_PCH_MUXED_MON_CLK),
	.SPI_PCH_MUXED_MON_IO0(SPI_PCH_MUXED_MON_IO0),
	.SPI_PCH_MUXED_MON_IO1(SPI_PCH_MUXED_MON_IO1),
	.SPI_PCH_MUXED_MON_IO2(SPI_PCH_MUXED_MON_IO2),
	.SPI_PCH_MUXED_MON_IO3(SPI_PCH_MUXED_MON_IO3),
	.SPI_PFR_BMC_FLASH1_BT_CLK(SPI_PFR_BMC_FLASH1_BT_CLK),
	.SPI_PFR_BMC_FLASH1_BT_MOSI(SPI_PFR_BMC_FLASH1_BT_MOSI),
	.SPI_PFR_BMC_FLASH1_BT_MISO(SPI_PFR_BMC_FLASH1_BT_MISO),
	.SPI_PFR_BMC_BOOT_R_IO2(SPI_PFR_BMC_BOOT_R_IO2),
	.SPI_PFR_BMC_BOOT_R_IO3(SPI_PFR_BMC_BOOT_R_IO3),
	.SPI_PFR_PCH_R_CLK(SPI_PFR_PCH_R_CLK),
	.SPI_PFR_PCH_R_IO0(SPI_PFR_PCH_R_IO0),
	.SPI_PFR_PCH_R_IO1(SPI_PFR_PCH_R_IO1),
	.SPI_PFR_PCH_R_IO2(SPI_PFR_PCH_R_IO2),
	.SPI_PFR_PCH_R_IO3(SPI_PFR_PCH_R_IO3),
	//SMBus
	.SMB_BMC_HSBP_STBY_LVC3_SCL(SMB_BMC_HSBP_STBY_LVC3_SCL),
	.SMB_BMC_HSBP_STBY_LVC3_SDA(SMB_BMC_HSBP_STBY_LVC3_SDA),
	.SMB_PFR_HSBP_STBY_LVC3_SCL(SMB_PFR_HSBP_STBY_LVC3_SCL),
	.SMB_PFR_HSBP_STBY_LVC3_SDA(SMB_PFR_HSBP_STBY_LVC3_SDA),
	.SMB_PFR_PMB1_STBY_LVC3_SCL(SMB_PFR_PMB1_STBY_LVC3_SCL),
	.SMB_PFR_PMB1_STBY_LVC3_SDA(SMB_PFR_PMB1_STBY_LVC3_SDA),
	.SMB_PMBUS_SML1_STBY_LVC3_SDA(SMB_PMBUS_SML1_STBY_LVC3_SDA),
	.SMB_PMBUS_SML1_STBY_LVC3_SCL(SMB_PMBUS_SML1_STBY_LVC3_SCL),
	.SMB_PCH_PMBUS2_STBY_LVC3_SCL(SMB_PCH_PMBUS2_STBY_LVC3_SCL),
	.SMB_PCH_PMBUS2_STBY_LVC3_SDA(SMB_PCH_PMBUS2_STBY_LVC3_SDA),
	.SMB_PFR_PMBUS2_STBY_LVC3_SCL(SMB_PFR_PMBUS2_STBY_LVC3_SCL),
	.SMB_PFR_PMBUS2_STBY_LVC3_SDA(SMB_PFR_PMBUS2_STBY_LVC3_SDA),
	.SMB_PFR_RFID_STBY_LVC3_SCL(SMB_PFR_RFID_STBY_LVC3_SCL),
	.SMB_PFR_RFID_STBY_LVC3_SDA(SMB_PFR_RFID_STBY_LVC3_SDA),
	//Deltas for PFR
	//.SMB_S3M_CPU0_SDA_LVC1(SMB_S3M_CPU0_SDA_LVC1),
	//.SMB_S3M_CPU0_SCL_LVC1(SMB_S3M_CPU0_SCL_LVC1),
	.H_S3M_CPU0_SMBUS_ALERT_LVC1_R_N(H_S3M_CPU0_SMBUS_ALERT_LVC1_R_N),
	//.SMB_S3M_CPU1_SDA_LVC1(SMB_S3M_CPU1_SDA_LVC1),
	//.SMB_S3M_CPU1_SCL_LVC1(SMB_S3M_CPU1_SCL_LVC1),
	.H_S3M_CPU1_SMBUS_ALERT_LVC1_R_N(H_S3M_CPU1_SMBUS_ALERT_LVC1_R_N),
	.SMB_PCIE_STBY_LVC3_B_SDA(SMB_PCIE_STBY_LVC3_B_SDA),
	.SMB_PCIE_STBY_LVC3_B_SCL(SMB_PCIE_STBY_LVC3_B_SCL),
	.SPI_PCH_TPM_CS_N(SPI_PCH_TPM_CS_N),
	.SPI_PFR_TPM_CS_R2_N(SPI_PFR_TPM_CS_R2_N),
	.FM_PFR_RNDGEN_AUX(FM_PFR_RNDGEN_AUX)
);


Archer_City_Main mArcher_City_Main
(
	.iClk_2M                            (wClk_2M),
	.iClk_50M                           (wClk_50M),
	.iRST_N                             (wRST_N),

    .iFM_FORCE_PWRON                    (FM_FORCE_PWRON_LVC3),

    //LED CONTROL signals
    .oLED_CONTROL                       (LED_CONTROL),
    .oFM_POSTLED_SEL                    (FM_POSTLED_SEL),
    .oPostCodeSel1_n                    (FM_POST_7SEG1_SEL_N),               //to select Display1 (MSNibble)
    .oPostCodeSel2_n                    (FM_POST_7SEG2_SEL_N),               //to select Display2 (LSNibble)
    .oLedFanFaultSel_n                  (FM_FAN_FAULT_LED_SEL_R_N),          //to select Fan Fault LEDs
    .oFmCpu0DimmCh14FaultLedSel         (FM_CPU0_DIMM_CH1_4_FAULT_LED_SEL),  //to select CPU0 DIMMs 1&2 CH 1-4 fault LEDs
    .oFmCpu0DimmCh58FaultLedSel         (FM_CPU0_DIMM_CH5_8_FAULT_LED_SEL),  //to select CPU0 DIMMs 1&2 CH 5-8 fault LEDs
    .oFmCpu1DimmCh14FaultLedSel         (FM_CPU1_DIMM_CH1_4_FAULT_LED_SEL),  //to select CPU1 DIMMs 1&2 CH 1-4 fault LEDs
    .oFmCpu1DimmCh58FaultLedSel         (FM_CPU1_DIMM_CH5_8_FAULT_LED_SEL),  //to select CPU1 DIMMs 1&2 CH 5-8 fault LEDs

	//GSX Interface with BMC
	.SGPIO_BMC_CLK                      (SGPIO_BMC_CLK), 
	.SGPIO_BMC_DOUT                     (SGPIO_BMC_DOUT),
	.SGPIO_BMC_DIN                      (SGPIO_BMC_DIN_R),
	.SGPIO_BMC_LD_N                     (SGPIO_BMC_LD_N),

	//Termtrip dly
	.H_CPU0_THERMTRIP_LVC1_N			(H_CPU0_THERMTRIP_LVC1_N),
	.H_CPU1_THERMTRIP_LVC1_N			(H_CPU1_THERMTRIP_LVC1_N),
	.FM_THERMTRIP_DLY_LVC1_N			(FM_THERMTRIP_DLY_LVC1_R_N),
	
	//MEMTRIP
	.H_CPU0_MEMTRIP_LVC1_N				(H_CPU0_MEMTRIP_LVC1_N),
	.H_CPU1_MEMTRIP_LVC1_N				(H_CPU1_MEMTRIP_LVC1_N),
	
	// PROCHOT
	.IRQ_CPU0_VRHOT_N 					(IRQ_CPU0_VRHOT_N),
	.IRQ_CPU1_VRHOT_N 					(IRQ_CPU1_VRHOT_N),
	.H_CPU0_PROCHOT_LVC1_N 			    (H_CPU0_PROCHOT_LVC1_R_N),
	.H_CPU1_PROCHOT_LVC1_N 			    (H_CPU1_PROCHOT_LVC1_R_N),

	// PERST & RST
	.FM_RST_PERST_BIT0                  (FM_RST_PERST_BIT0), 
	.FM_RST_PERST_BIT1                  (FM_RST_PERST_BIT1), 
	.FM_RST_PERST_BIT2                  (FM_RST_PERST_BIT2), 

	.RST_PCIE_PERST0_N                  (RST_PCIE_CPU0_DEV_PERST_N),
	.RST_PCIE_PERST1_N                  (RST_PCIE_CPU1_DEV_PERST_N),
	.RST_PCIE_PERST2_N                  (RST_PCIE_PCH_DEV_PERST_N),

	.RST_CPU0_LVC1_N                    (RST_CPU0_LVC1_R_N),
	.RST_CPU1_LVC1_N                    (RST_CPU1_LVC1_R_N),

	.RST_PLTRST_B_N                     (RST_PLTRST_PLD_B_N),
	.RST_PLTRST_N                       (RST_PLTRST_PLD_N),
	
//CPU Misc
	.FM_CPU0_PKGID0                     (FM_CPU0_PKGID0), 
	.FM_CPU0_PKGID1                     (FM_CPU0_PKGID1), 
	.FM_CPU0_PKGID2                     (FM_CPU0_PKGID2), 
	.FM_CPU0_PROC_ID0                   (FM_CPU0_PROC_ID0), 
	.FM_CPU0_PROC_ID1                   (FM_CPU0_PROC_ID1), 
	.FM_CPU0_SKTOCC_LVT3_N              (FM_CPU0_SKTOCC_LVT3_PLD_N), 
	.FM_CPU1_PKGID0                     (FM_CPU1_PKGID0), 
	.FM_CPU1_PKGID1                     (FM_CPU1_PKGID1), 
	.FM_CPU1_PKGID2                     (FM_CPU1_PKGID2), 
	.FM_CPU1_PROC_ID0                   (FM_CPU1_PROC_ID0), 
	.FM_CPU1_PROC_ID1                   (FM_CPU1_PROC_ID1), 
	.FM_CPU1_SKTOCC_LVT3_N              (FM_CPU1_SKTOCC_LVT3_PLD_N),

	.FM_CPU0_INTR_PRSNT					(FM_CPU0_INTR_PRSNT),

//BMC
   .FM_BMC_INTR_PRSNT	                (FM_BMC_INTR_PRSNT),
	.FM_BMC_PWRBTN_OUT_N                (FM_BMC_PWRBTN_OUT_N),
//	.FP_BMC_PWR_BTN_N                   (FP_BMC_PWR_BTN_R2_N),                 //From FP-> LOT 6 MUX
	.FM_BMC_PFR_PWRBTN_OUT_N            (FM_BMC_PFR_PWRBTN_OUT_R_N),           //To LOT6 MUX -> PCH
	.FM_BMC_ONCTL_N                     (FM_BMC_ONCTL_N_PLD),
	.RST_SRST_BMC_PLD_N                 (RST_SRST_BMC_PLD_R_N),


	.FM_P2V5_BMC_AUX_EN					(FM_P2V5_BMC_AUX_EN),
	.PWRGD_P2V5_BMC_AUX					(PWRGD_P2V5_BMC_AUX),
	.FM_P1V2_BMC_AUX_EN					(FM_P1V2_BMC_AUX_EN),
	.PWRGD_P1V2_BMC_AUX					(PWRGD_P1V2_BMC_AUX),
	.FM_P1V0_BMC_AUX_EN					(FM_P1V0_BMC_AUX_EN),
	.PWRGD_P1V0_BMC_AUX					(PWRGD_P1V0_BMC_AUX), 

//PCH
	.RST_RSMRST_N                       (RST_RSMRST_PLD_R_N),

	.PWRGD_PCH_PWROK                    (PWRGD_PCH_PWROK_R), 
	.PWRGD_SYS_PWROK                    (PWRGD_SYS_PWROK_R),

	.FM_SLP_SUS_RSM_RST_N               (FM_SLP_SUS_RSM_RST_N), 
	.FM_SLPS3_N                         (FM_SLPS3_PLD_N), 
	.FM_SLPS4_N                         (FM_SLPS4_PLD_N), 
	.FM_PCH_PRSNT_N                     (FM_PCH_PRSNT_N), 

	.FM_PCH_P1V8_AUX_EN                 (FM_PCH_P1V8_AUX_EN),
	.FM_P1V05_PCH_AUX_EN				(FM_P1V05_PCH_AUX_EN),
	.FM_PVNN_PCH_AUX_EN					(FM_PVNN_PCH_AUX_EN),
	.PWRGD_P1V8_PCH_AUX                 (PWRGD_P1V8_PCH_AUX_PLD),
	.PWRGD_P1V05_PCH_AUX                (PWRGD_P1V05_PCH_AUX),
	.PWRGD_PVNN_PCH_AUX					(PWRGD_PVNN_PCH_AUX), 
	
//Dediprog Detection Support
	.RST_DEDI_BUSY_PLD_N                (RST_DEDI_BUSY_PLD_N),

//PSU Ctl
	.FM_PS_EN                           (FM_PS_EN_PLD_R), 
	.PWRGD_PS_PWROK                     (PWRGD_PS_PWROK_PLD_R), 

//Clock Logic    
	.FM_PLD_CLKS_DEV_EN                 (FM_PLD_CLKS_DEV_R_EN),
	.FM_PLD_CLK_25M_EN                  (FM_PLD_CLK_25M_EN),

//Base Logic
	.PWRGD_P3V3_AUX                     (PWRGD_P1V2_MAX10_AUX_R),

//Main VR & Logic
	.PWRGD_P3V3                         (PWRGD_P3V3), 
	.FM_P5V_EN                          (FM_P5V_EN),
	.FM_AUX_SW_EN                       (FM_AUX_SW_EN), 

//Mem



//CPU VRs
	.PWRGD_CPU0_LVC1 					(PWRGD_CPU0_LVC1_R2),
	.PWRGD_CPU1_LVC1 					(PWRGD_CPU1_LVC1_R),

	.PWRGD_CPUPWRGD_LVC1 				(PWRGD_CPUPWRGD_LVC1),
	.PWRGD_PLT_AUX_CPU0_LVT3 			(PWRGD_PLT_AUX_CPU0_LVT3_R),
	.PWRGD_PLT_AUX_CPU1_LVT3  			(PWRGD_PLT_AUX_CPU1_LVT3_R),
		
	.FM_PVCCIN_CPU0_EN 					(FM_PVCCIN_CPU0_R_EN),
	.PWRGD_PVCCIN_CPU0 					(PWRGD_PVCCIN_CPU0),

	.FM_PVCCFA_EHV_FIVRA_CPU0_EN 		(FM_PVCCFA_EHV_FIVRA_CPU0_R_EN),
	.PWRGD_PVCCFA_EHV_FIVRA_CPU0 		(PWRGD_PVCCFA_EHV_FIVRA_CPU0),

	.FM_PVCCINFAON_CPU0_EN 				(FM_PVCCINFAON_CPU0_R_EN),
	.PWRGD_PVCCINFAON_CPU0 				(PWRGD_PVCCINFAON_CPU0),

	.FM_PVCCFA_EHV_CPU0_EN 				(FM_PVCCFA_EHV_CPU0_R_EN),
	.PWRGD_PVCCFA_EHV_CPU0 				(PWRGD_PVCCFA_EHV_CPU0),

	.FM_PVCCD_HV_CPU0_EN 				(FM_PVCCD_HV_CPU0_EN),
	.PWRGD_PVCCD_HV_CPU0 				(PWRGD_PVCCD_HV_CPU0),

	.FM_PVNN_MAIN_CPU0_EN 				(FM_PVNN_MAIN_CPU0_EN),
	.PWRGD_PVNN_MAIN_CPU0 				(PWRGD_PVNN_MAIN_CPU0),

	.FM_PVPP_HBM_CPU0_EN 				(FM_PVPP_HBM_CPU0_EN),
	.PWRGD_PVPP_HBM_CPU0 				(PWRGD_PVPP_HBM_CPU0),

	.FM_PVCCIN_CPU1_EN 					(FM_PVCCIN_CPU1_R_EN),
	.PWRGD_PVCCIN_CPU1 					(PWRGD_PVCCIN_CPU1),

	.FM_PVCCFA_EHV_FIVRA_CPU1_EN 		(FM_PVCCFA_EHV_FIVRA_CPU1_R_EN),
	.PWRGD_PVCCFA_EHV_FIVRA_CPU1 		(PWRGD_PVCCFA_EHV_FIVRA_CPU1),

	.FM_PVCCINFAON_CPU1_EN 				(FM_PVCCINFAON_CPU1_R_EN),
	.PWRGD_PVCCINFAON_CPU1 				(PWRGD_PVCCINFAON_CPU1),

	.FM_PVCCFA_EHV_CPU1_EN 				(FM_PVCCFA_EHV_CPU1_R_EN),
	.PWRGD_PVCCFA_EHV_CPU1 				(PWRGD_PVCCFA_EHV_CPU1),

	.FM_PVCCD_HV_CPU1_EN     			(FM_PVCCD_HV_CPU1_EN),
	.PWRGD_PVCCD_HV_CPU1 				(PWRGD_PVCCD_HV_CPU1),

	.FM_PVNN_MAIN_CPU1_EN 				(FM_PVNN_MAIN_CPU1_EN),
	.PWRGD_PVNN_MAIN_CPU1 				(PWRGD_PVNN_MAIN_CPU1),

	.FM_PVPP_HBM_CPU1_EN 				(FM_PVPP_HBM_CPU1_EN),
	.PWRGD_PVPP_HBM_CPU1 				(PWRGD_PVPP_HBM_CPU1),
	.IRQ_PSYS_CRIT_N 					(IRQ_PSYS_CRIT_N),

	//Debug Signals
	.FM_PLD_HEARTBEAT_LVC3 				(FM_PLD_HEARTBEAT_LVC3), //Heart beat led
	.FM_PLD_REV_N                		(FM_PLD_REV_N),

	//Reset from CPU to VT to CPLD
	.M_AB_CPU0_RESET_N(M_AB_CPU0_RESET_N),
	.M_CD_CPU0_RESET_N(M_CD_CPU0_RESET_N),
	.M_EF_CPU0_RESET_N(M_EF_CPU0_RESET_N),
	.M_GH_CPU0_RESET_N(M_GH_CPU0_RESET_N),
	.M_AB_CPU1_RESET_N(M_AB_CPU1_RESET_N),
	.M_CD_CPU1_RESET_N(M_CD_CPU1_RESET_N),
	.M_EF_CPU1_RESET_N(M_EF_CPU1_RESET_N),
	.M_GH_CPU1_RESET_N(M_GH_CPU1_RESET_N),

	//Reset from CPLD to VT to DIMM
	.M_AB_CPU0_FPGA_RESET_N(M_AB_CPU0_FPGA_RESET_R_N),
	.M_CD_CPU0_FPGA_RESET_N(M_CD_CPU0_FPGA_RESET_R_N),
	.M_EF_CPU0_FPGA_RESET_N(M_EF_CPU0_FPGA_RESET_R_N),
	.M_GH_CPU0_FPGA_RESET_N(M_GH_CPU0_FPGA_RESET_R_N),
	.M_AB_CPU1_FPGA_RESET_N(M_AB_CPU1_FPGA_RESET_R_N),
	.M_CD_CPU1_FPGA_RESET_N(M_CD_CPU1_FPGA_RESET_R_N),
	.M_EF_CPU1_FPGA_RESET_N(M_EF_CPU1_FPGA_RESET_R_N),
	.M_GH_CPU1_FPGA_RESET_N(M_GH_CPU1_FPGA_RESET_R_N),

	.IRQ_CPU0_MEM_VRHOT_N(IRQ_CPU0_MEM_VRHOT_N),
	.IRQ_CPU1_MEM_VRHOT_N(IRQ_CPU1_MEM_VRHOT_N),

	.H_CPU0_MEMHOT_IN_LVC1_N(H_CPU0_MEMHOT_IN_LVC1_R_N),
	.H_CPU1_MEMHOT_IN_LVC1_N(H_CPU1_MEMHOT_IN_LVC1_R_N),

	.H_CPU0_MEMHOT_OUT_LVC1_N(H_CPU0_MEMHOT_OUT_LVC1_N),
	.H_CPU1_MEMHOT_OUT_LVC1_N(H_CPU1_MEMHOT_OUT_LVC1_N),

	.PWRGD_FAIL_CPU0_AB(PWRGD_FAIL_CPU0_AB),
	.PWRGD_FAIL_CPU0_CD(PWRGD_FAIL_CPU0_CD),
	.PWRGD_FAIL_CPU0_EF(PWRGD_FAIL_CPU0_EF),
	.PWRGD_FAIL_CPU0_GH(PWRGD_FAIL_CPU0_GH),
	.PWRGD_FAIL_CPU1_AB(PWRGD_FAIL_CPU1_AB),
	.PWRGD_FAIL_CPU1_CD(PWRGD_FAIL_CPU1_CD),
	.PWRGD_FAIL_CPU1_EF(PWRGD_FAIL_CPU1_EF),
	.PWRGD_FAIL_CPU1_GH(PWRGD_FAIL_CPU1_GH),

	.PWRGD_DRAMPWRGD_CPU0_AB_LVC1(PWRGD_DRAMPWRGD_CPU0_AB_R_LVC1),
	.PWRGD_DRAMPWRGD_CPU0_CD_LVC1(PWRGD_DRAMPWRGD_CPU0_CD_R_LVC1),
	.PWRGD_DRAMPWRGD_CPU0_EF_LVC1(PWRGD_DRAMPWRGD_CPU0_EF_R_LVC1),
	.PWRGD_DRAMPWRGD_CPU0_GH_LVC1(PWRGD_DRAMPWRGD_CPU0_GH_R_LVC1),
	.PWRGD_DRAMPWRGD_CPU1_AB_LVC1(PWRGD_DRAMPWRGD_CPU1_AB_R_LVC1),
	.PWRGD_DRAMPWRGD_CPU1_CD_LVC1(PWRGD_DRAMPWRGD_CPU1_CD_R_LVC1),
	.PWRGD_DRAMPWRGD_CPU1_EF_LVC1(PWRGD_DRAMPWRGD_CPU1_EF_R_LVC1),
	.PWRGD_DRAMPWRGD_CPU1_GH_LVC1(PWRGD_DRAMPWRGD_CPU1_GH_R_LVC1),


    //Misc Logic for CPU errors, DMI & PCH Crash-log 
    .oFM_CPU_RMCA_CATERR_DLY_N(FM_CPU_RMCA_CATERR_DLY_LVT3_R_N),
    .iCPU_CATERR_N(H_CPU_CATERR_LVC1_R_N),
    .iCPU_RMCA_N(H_CPU_RMCA_LVC1_R_N),
    .oFM_CPU_RMCA_CATERR_N(FM_CPU_RMCA_CATERR_LVT3_N),
    .oCPU_NMI_N(H_CPU_NMI_LVC1_R_N),
    .iIRQ_BMC_CPU_NMI(IRQ_BMC_CPU_NMI),
    .iIRQ_PCH_CPU_NMI_EVENT_N(IRQ_PCH_CPU_NMI_EVENT_N),
    .iFM_BMC_CRASHLOG_TRIG_N(FM_BMC_CRASHLOG_TRIG_N),
    .oFM_PCH_CRASHLOG_TRIG_N(FM_PCH_CRASHLOG_TRIG_R_N),
    .iFM_PCH_PLD_GLB_RST_WARN_N(FM_PCH_PLD_GLB_RST_WARN_N),

    //Smart Logic
    .iFM_PMBUS_ALERT_B_EN(FM_PMBUS_ALERT_B_EN),
	.iFM_THROTTLE_R_N(FM_THROTTLE_R_N),
	.iIRQ_SML1_PMBUS_PLD_ALERT_N(IRQ_SML1_PMBUS_PLD_ALERT_N),
	.oFM_SYS_THROTTLE_N(FM_SYS_THROTTLE_R_N),

	//Errors
	.H_CPU_ERR0_LVC1_N(H_CPU_ERR0_LVC1_N),
	.H_CPU_ERR1_LVC1_N(H_CPU_ERR1_LVC1_N),
	.H_CPU_ERR2_LVC1_N(H_CPU_ERR2_LVC1_N),
	.FM_CPU_ERR0_LVT3_N(FM_CPU_ERR0_LVT3_N),
	.FM_CPU_ERR1_LVT3_N(FM_CPU_ERR1_LVT3_N),
	.FM_CPU_ERR2_LVT3_N(FM_CPU_ERR2_LVT3_N),
	.H_CPU0_MON_FAIL_PLD_LVC1_N(H_CPU0_MON_FAIL_PLD_LVC1_N),
	.H_CPU1_MON_FAIL_PLD_LVC1_N(H_CPU1_MON_FAIL_PLD_LVC1_N),

 //Interposer VR powerGd signal (VTT) This only for CLX & needs to be modified when not in CLX
    .iPWRGD_VTT_PWRGD(FM_IBL_SLPS0_CPU0_N),
 
	//S3 Pwr-seq control
	.FM_SX_SW_P12V_STBY_EN(FM_SX_SW_P12V_STBY_R_EN),
	.FM_SX_SW_P12V_EN(FM_SX_SW_P12V_R_EN),
	.FM_DIMM_12V_CPS_S5_N(FM_DIMM_12V_CPS_S5_N),


    //ADR signaling
    .iFM_ADR_COMPLETE(FM_ADR_COMPLETE),          //from PCH, to indicate ADR process is completed
    .oFM_ADR_TRIGGER_N(FM_ADR_TRIGGER_N),        //To PCH, to indicate a Surprise Pwr-Dwn is detected and ADR needs to start
    .oFM_ADR_ACK(FM_ADR_ACK_R),                    //To PCH, once all is done in FPGA so PCH can start regular pwr-down.iFM_ADR_COMPLETE

    //PFR signals
    .FM_PFR_MUX_OE_CTL_PLD(FM_PFR_MUX_OE_CTL_PLD),
    .iPFROverride(1'b0),                //when asserted, PFR Post-Codes take precedence over BIOS or FPGA PostCodes
    .iPFRPostCode(8'hFF),               //PFR PostCodes (Hex format [7:0], MSNible for Display1 and LSNible for Display2)
                                        //will be encoded inside and added Display2 dot for identification
	.FM_PFR_SLP_SUS_EN(FM_PFR_SLP_SUS_EN),

    // Interposer PFR reuse ports
	.SMB_S3M_CPU0_SDA(SMB_S3M_CPU0_SDA_LVC1),
    .SMB_S3M_CPU0_SCL(SMB_S3M_CPU0_SCL_LVC1),
    .SMB_S3M_CPU1_SDA(SMB_S3M_CPU1_SDA_LVC1),
    .SMB_S3M_CPU1_SCL(SMB_S3M_CPU1_SCL_LVC1)
);

endmodule // Archer_City_Main_TOP