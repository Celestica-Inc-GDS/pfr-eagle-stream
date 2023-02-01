`timescale 1 ps / 1 ps
`default_nettype none

interface common_core_if(input wire nreset);
    logic RST_RSMRST_PLD_R_N;
    logic RST_SRST_BMC_PLD_R_N;
    logic RST_PLTRST_PLD_N;
    logic LED_CONTROL_0;
    logic LED_CONTROL_1;
    logic LED_CONTROL_2;
    logic LED_CONTROL_3;
    logic LED_CONTROL_4;
    logic LED_CONTROL_5;
    logic LED_CONTROL_6;
    logic LED_CONTROL_7;
    logic FM_POST_7SEG1_SEL_N;
    logic FM_POST_7SEG2_SEL_N;
    logic FM_POSTLED_SEL;

    wire FM_AUX_SW_EN;
    logic FM_BMC_NMI_PCH_EN;
    logic FM_BMC_ONCTL_N_PLD;
    wire FM_BMC_PFR_PWRBTN_OUT_R_N;
    logic FM_BMC_PWRBTN_OUT_N;
    logic FM_CPU0_INTR_PRSNT;
    logic FM_CPU0_PKGID0;
    logic FM_CPU0_PKGID1;
    logic FM_CPU0_PKGID2;
    logic FM_CPU0_PROC_ID0;
    logic FM_CPU0_PROC_ID1;
    logic FM_CPU0_SKTOCC_LVT3_PLD_N;
    logic FM_CPU1_PKGID0;
    logic FM_CPU1_PKGID1;
    logic FM_CPU1_PKGID2;
    logic FM_CPU1_PROC_ID0;
    logic FM_CPU1_PROC_ID1;
    logic FM_CPU1_SKTOCC_LVT3_PLD_N;
    logic FM_DIMM_12V_CPS_S5_N;
    logic FM_FORCE_PWRON_LVC3;
    wire FM_P1V0_BMC_AUX_EN;
    wire FM_P1V05_PCH_AUX_EN;
    wire FM_P1V2_BMC_AUX_EN;
    wire FM_P2V5_BMC_AUX_EN;
    wire FM_P5V_EN;
    wire FM_PCH_P1V8_AUX_EN;
    logic FM_PCH_PRSNT_N;
    wire FM_PFR_MUX_OE_CTL_PLD;
    wire FM_PLD_CLK_25M_R_EN;
    wire FM_PLD_CLKS_DEV_R_EN;
    wire FM_PLD_HEARTBEAT_LVC3;
    logic FM_PLD_REV_N;
    wire FM_PS_EN_PLD_R;
    wire FM_PVCCD_HV_CPU0_EN;
    wire FM_PVCCD_HV_CPU1_EN;
    wire FM_PVCCFA_EHV_CPU0_R_EN;
    wire FM_PVCCFA_EHV_CPU1_R_EN;
    wire FM_PVCCFA_EHV_FIVRA_CPU0_R_EN;
    wire FM_PVCCFA_EHV_FIVRA_CPU1_R_EN;
    wire FM_PVCCIN_CPU0_R_EN;
    wire FM_PVCCIN_CPU1_R_EN;
    wire FM_PVCCINFAON_CPU0_R_EN;
    wire FM_PVCCINFAON_CPU1_R_EN;
    wire FM_PVNN_MAIN_CPU0_EN;
    wire FM_PVNN_MAIN_CPU1_EN;
    wire FM_PVNN_PCH_AUX_EN;
    wire FM_PVPP_HBM_CPU0_EN;
    wire FM_PVPP_HBM_CPU1_EN;
    logic FM_RST_PERST_BIT0;
    logic FM_RST_PERST_BIT1;
    logic FM_RST_PERST_BIT2;
    logic FM_SLP_SUS_RSM_RST_N;
    logic FM_SLPS3_PLD_N;
    logic FM_SLPS4_PLD_N;
    wire FM_SX_SW_P12V_R_EN;
    wire FM_SX_SW_P12V_STBY_R_EN;
    wire FM_SYS_THROTTLE_R_N;
    wire FM_THERMTRIP_DLY_LVC1_R_N;
    wire H_CPU0_MEMHOT_IN_LVC1_R_N;
    logic H_CPU0_MEMHOT_OUT_LVC1_N;
    logic H_CPU0_MEMTRIP_LVC1_N;
    logic H_CPU0_MON_FAIL_PLD_LVC1_N;
    wire H_CPU0_PROCHOT_LVC1_R_N;
    logic H_CPU0_THERMTRIP_LVC1_N;
    wire H_CPU1_MEMHOT_IN_LVC1_R_N;
    logic H_CPU1_MEMHOT_OUT_LVC1_N;
    logic H_CPU1_MEMTRIP_LVC1_N;
    logic H_CPU1_MON_FAIL_PLD_LVC1_N;
    wire H_CPU1_PROCHOT_LVC1_R_N;
    logic H_CPU1_THERMTRIP_LVC1_N;
    logic IRQ_CPU0_MEM_VRHOT_N;
    logic IRQ_CPU0_VRHOT_N;
    logic IRQ_CPU1_MEM_VRHOT_N;
    logic IRQ_CPU1_VRHOT_N;
    wire M_AB_CPU0_FPGA_RESET_R_N;
    logic M_AB_CPU0_RESET_N;
    wire M_AB_CPU1_FPGA_RESET_R_N;
    logic M_AB_CPU1_RESET_N;
    wire M_CD_CPU0_FPGA_RESET_R_N;
    logic M_CD_CPU0_RESET_N;
    wire M_CD_CPU1_FPGA_RESET_R_N;
    logic M_CD_CPU1_RESET_N;
    wire M_EF_CPU0_FPGA_RESET_R_N;
    logic M_EF_CPU0_RESET_N;
    wire M_EF_CPU1_FPGA_RESET_R_N;
    logic M_EF_CPU1_RESET_N;
    wire M_GH_CPU0_FPGA_RESET_R_N;
    logic M_GH_CPU0_RESET_N;
    wire M_GH_CPU1_FPGA_RESET_R_N;
    logic M_GH_CPU1_RESET_N;
    wire PWRGD_CPU0_LVC1_R;
    wire PWRGD_CPU1_LVC1_R;
    logic PWRGD_CPUPWRGD_LVC1;
    wire PWRGD_DRAMPWRGD_CPU0_AB_R_LVC1;
    wire PWRGD_DRAMPWRGD_CPU0_CD_R_LVC1;
    wire PWRGD_DRAMPWRGD_CPU0_EF_R_LVC1;
    wire PWRGD_DRAMPWRGD_CPU0_GH_R_LVC1;
    wire PWRGD_DRAMPWRGD_CPU1_AB_R_LVC1;
    wire PWRGD_DRAMPWRGD_CPU1_CD_R_LVC1;
    wire PWRGD_DRAMPWRGD_CPU1_EF_R_LVC1;
    wire PWRGD_DRAMPWRGD_CPU1_GH_R_LVC1;
    logic PWRGD_FAIL_CPU0_AB_PLD_IN;
    logic PWRGD_FAIL_CPU0_AB_PLD_OE;
    wire PWRGD_FAIL_CPU0_AB_PLD;
    logic PWRGD_FAIL_CPU0_CD_PLD_IN;
    logic PWRGD_FAIL_CPU0_CD_PLD_OE;
    wire PWRGD_FAIL_CPU0_CD_PLD;
    logic PWRGD_FAIL_CPU0_EF_PLD_IN;
    logic PWRGD_FAIL_CPU0_EF_PLD_OE;
    wire PWRGD_FAIL_CPU0_EF_PLD;
    logic PWRGD_FAIL_CPU0_GH_PLD_IN;
    logic PWRGD_FAIL_CPU0_GH_PLD_OE;
    wire PWRGD_FAIL_CPU0_GH_PLD;
    logic PWRGD_FAIL_CPU1_AB_PLD_IN;
    logic PWRGD_FAIL_CPU1_AB_PLD_OE;
    wire PWRGD_FAIL_CPU1_AB_PLD;
    logic PWRGD_FAIL_CPU1_CD_PLD_IN;
    logic PWRGD_FAIL_CPU1_CD_PLD_OE;
    wire PWRGD_FAIL_CPU1_CD_PLD;
    logic PWRGD_FAIL_CPU1_EF_PLD_IN;
    logic PWRGD_FAIL_CPU1_EF_PLD_OE;
    wire PWRGD_FAIL_CPU1_EF_PLD;
    logic PWRGD_FAIL_CPU1_GH_PLD_IN;
    logic PWRGD_FAIL_CPU1_GH_PLD_OE;
    wire PWRGD_FAIL_CPU1_GH_PLD;
    logic PWRGD_P1V0_BMC_AUX;
    logic PWRGD_P1V05_PCH_AUX;
    logic PWRGD_P1V2_BMC_AUX;
    logic PWRGD_P1V2_MAX10_AUX_PLD_R;
    logic PWRGD_P1V8_PCH_AUX_PLD;
    logic PWRGD_P2V5_BMC_AUX;
    logic PWRGD_P3V3;
    wire PWRGD_PCH_PWROK_R;
    wire PWRGD_PLT_AUX_CPU0_LVT3_R;
    wire PWRGD_PLT_AUX_CPU1_LVT3_R;
    logic PWRGD_PS_PWROK_PLD_R;
    logic PWRGD_PVCCD_HV_CPU0;
    logic PWRGD_PVCCD_HV_CPU1;
    logic PWRGD_PVCCFA_EHV_CPU0;
    logic PWRGD_PVCCFA_EHV_CPU1;
    logic PWRGD_PVCCFA_EHV_FIVRA_CPU0;
    logic PWRGD_PVCCFA_EHV_FIVRA_CPU1;
    logic PWRGD_PVCCIN_CPU0;
    logic PWRGD_PVCCIN_CPU1;
    logic PWRGD_PVCCINFAON_CPU0;
    logic PWRGD_PVCCINFAON_CPU1;
    logic PWRGD_PVNN_MAIN_CPU0;
    logic PWRGD_PVNN_MAIN_CPU1;
    logic PWRGD_PVNN_PCH_AUX;
    logic PWRGD_PVPP_HBM_CPU0;
    logic PWRGD_PVPP_HBM_CPU1;
    wire PWRGD_SYS_PWROK_R;
    wire RST_CPU0_LVC1_R_N;
    wire RST_CPU1_LVC1_R_N;
    logic RST_DEDI_BUSY_PLD_N;
    wire RST_PLD_PCIE_CPU0_DEV_PERST_N;
    wire RST_PLD_PCIE_CPU1_DEV_PERST_N;
    wire RST_PLD_PCIE_PCH_DEV_PERST_N;
    wire RST_PLTRST_PLD_B_N;
    logic SGPIO_BMC_CLK;
    wire SGPIO_BMC_DIN_R;
    logic SGPIO_BMC_DOUT;
    logic SGPIO_BMC_LD_N;
    logic SMB_S3M_CPU0_SCL_LVC1;
    logic SMB_S3M_CPU0_SDA_LVC1_IN;
    logic SMB_S3M_CPU0_SDA_LVC1_OE;
    wire SMB_S3M_CPU0_SDA_LVC1;
    logic SMB_S3M_CPU1_SCL_LVC1;
    logic SMB_S3M_CPU1_SDA_LVC1_IN;
    logic SMB_S3M_CPU1_SDA_LVC1_OE;
    wire SMB_S3M_CPU1_SDA_LVC1;
    wire FM_CPU_CATERR_LVT3_R_N;
    wire FM_ADR_ACK_R;
    logic FM_ADR_COMPLETE;
    wire FM_ADR_TRIGGER_N;
    logic FM_BMC_CRASHLOG_TRIG_N;
    wire FM_PCH_CRASHLOG_TRIG_R_N;
    logic FM_PCH_PLD_GLB_RST_WARN_N;
    logic FM_PMBUS_ALERT_B_EN;
    logic FM_THROTTLE_R_N;
    logic H_CPU_CATERR_LVC1_R_N;
    wire H_CPU_NMI_LVC1_R;
    logic IRQ_BMC_CPU_NMI;
    logic IRQ_PCH_CPU_NMI_EVENT_N;
    logic IRQ_SML1_PMBUS_PLD_ALERT_N_IN;
    logic IRQ_SML1_PMBUS_PLD_ALERT_N_OE;
    wire IRQ_SML1_PMBUS_PLD_ALERT_N;
    wire FM_CPU1_DIMM_CH1_4_FAULT_LED_SEL;
    wire FM_CPU1_DIMM_CH5_8_FAULT_LED_SEL;
    wire FM_CPU0_DIMM_CH1_4_FAULT_LED_SEL;
    wire FM_CPU0_DIMM_CH5_8_FAULT_LED_SEL;
    wire FM_FAN_FAULT_LED_SEL_R_N;
    logic H_CPU_ERR0_LVC1_R_N;
    logic H_CPU_ERR1_LVC1_R_N;
    logic H_CPU_ERR2_LVC1_R_N;
    wire FM_CPU_ERR0_LVT3_N;
    wire FM_CPU_ERR1_LVT3_N;
    wire FM_CPU_ERR2_LVT3_N;
    logic FM_DIS_PS_PWROK_DLY;
    wire FM_HBM2_HBM3_VPP_SEL;
    wire PWRGD_S0_PWROK_CPU0_LVC1_R;
    wire PWRGD_S0_PWROK_CPU1_LVC1_R;
    logic SGPIO_DEBUG_PLD_CLK;
    logic SGPIO_DEBUG_PLD_DOUT;
    logic SGPIO_DEBUG_PLD_LD;
    wire SGPIO_DEBUG_PLD_DIN;
    logic FP_BMC_PWR_BTN_R2_N;
    logic FM_VAL_BOARD_PRSNT_N;
    logic FM_ADR_MODE0;
    logic FM_ADR_MODE1;
    wire IRQ_BMC_PCH_NMI;
    logic FM_S3M_CPU0_CPLD_CRC_ERROR;
    logic FM_S3M_CPU1_CPLD_CRC_ERROR;
    logic SGPIO_IDV_CLK_R;
    logic SGPIO_IDV_DOUT_R;
    wire SGPIO_IDV_DIN_R;
    logic SGPIO_IDV_LD_R_N;



assign PWRGD_FAIL_CPU0_AB_PLD = (PWRGD_FAIL_CPU0_AB_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU0_AB_PLD_IN : 1'bZ;
assign PWRGD_FAIL_CPU0_CD_PLD = (PWRGD_FAIL_CPU0_CD_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU0_CD_PLD_IN : 1'bZ;
assign PWRGD_FAIL_CPU0_EF_PLD = (PWRGD_FAIL_CPU0_EF_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU0_EF_PLD_IN : 1'bZ;
assign PWRGD_FAIL_CPU0_GH_PLD = (PWRGD_FAIL_CPU0_GH_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU0_GH_PLD_IN : 1'bZ;
assign PWRGD_FAIL_CPU1_AB_PLD = (PWRGD_FAIL_CPU1_AB_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU1_AB_PLD_IN : 1'bZ;
assign PWRGD_FAIL_CPU1_CD_PLD = (PWRGD_FAIL_CPU1_CD_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU1_CD_PLD_IN : 1'bZ;
assign PWRGD_FAIL_CPU1_EF_PLD = (PWRGD_FAIL_CPU1_EF_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU1_EF_PLD_IN : 1'bZ;
assign PWRGD_FAIL_CPU1_GH_PLD = (PWRGD_FAIL_CPU1_GH_PLD_OE == 1'b1) ? PWRGD_FAIL_CPU1_GH_PLD_IN : 1'bZ;
assign SMB_S3M_CPU0_SDA_LVC1 = (SMB_S3M_CPU0_SDA_LVC1_OE == 1'b1) ? SMB_S3M_CPU0_SDA_LVC1_IN : 1'bZ;
assign SMB_S3M_CPU1_SDA_LVC1 = (SMB_S3M_CPU1_SDA_LVC1_OE == 1'b1) ? SMB_S3M_CPU1_SDA_LVC1_IN : 1'bZ;
assign IRQ_SML1_PMBUS_PLD_ALERT_N = (IRQ_SML1_PMBUS_PLD_ALERT_N_OE == 1'b1) ? IRQ_SML1_PMBUS_PLD_ALERT_N_IN : 1'bZ;



task disable_all_bidir;
    PWRGD_FAIL_CPU0_AB_PLD_OE <= 1'b0;
    PWRGD_FAIL_CPU0_CD_PLD_OE <= 1'b0;
    PWRGD_FAIL_CPU0_EF_PLD_OE <= 1'b0;
    PWRGD_FAIL_CPU0_GH_PLD_OE <= 1'b0;
    PWRGD_FAIL_CPU1_AB_PLD_OE <= 1'b0;
    PWRGD_FAIL_CPU1_CD_PLD_OE <= 1'b0;
    PWRGD_FAIL_CPU1_EF_PLD_OE <= 1'b0;
    PWRGD_FAIL_CPU1_GH_PLD_OE <= 1'b0;
    SMB_S3M_CPU0_SDA_LVC1_OE <= 1'b0;
    SMB_S3M_CPU1_SDA_LVC1_OE <= 1'b0;
    IRQ_SML1_PMBUS_PLD_ALERT_N_OE <= 1'b0;
endtask



endinterface



