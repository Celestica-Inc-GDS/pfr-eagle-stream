;#####################################################################################################################################
;#####  Remote FPGA Access Tool (R-FAT)
;#####  Project: Archer City board MainFpga
;#####  Developer: Alejandro Larios (a.larios@intel.com)
;#####  Version : 0.5
;#####  Updates: for updates, please check on ArcherCity wiki at goto/ac_wiki
;#####  Pluease report issues or suggestions to the developer
;#####################################################################################################################################

set pclean 200    ;#clear screen rows
set delay1 100     ;#delay for infinite loops
set delay2 1000   ;#delay for instructions message

set HwName [lindex [get_hardware_names] 0]                                        ;#get usb-blaster id and store in HwName

if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
	puts "PLATFORM OFF or not connected to USB-BLASTER device, please check and retry"
	exit
}
#set DevName [lindex [get_device_names -hardware_name $HwName] 0]                  ;#get device name from jtag chain 
start_insystem_source_probe -device_name $DevName -hardware_name $HwName          ;#launch in-system sources & probes tool

set Rev [read_probe_data -value_in_hex -instance_index 2]                         ;#get Version data (32b) and store in Rev
set MainRev1 [string range $Rev 0 0]                                              ;#get each version and store in proper var
set MainRev2 [string range $Rev 1 1]
set MainTestRev1 [string range $Rev 2 2]
set MainTestRev2 [string range $Rev 3 3]
set SecondaryRev1 [string range $Rev 4 4]
set SecondaryRev2 [string range $Rev 5 5]
set SecondaryTestRev1 [string range $Rev 6 6]
set SecondaryTestRev2 [string range $Rev 7 7]
for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
	puts ""
}
puts "*************************************************************"
puts "*************** Board FPGA Versions *************************"
puts "*************************************************************"
puts [concat {MainFpga Rev: } $MainRev1.$MainRev2]
puts [concat {MainFpga Test Rev: } $MainTestRev1.$MainTestRev2]
puts "----------------------------------"
puts [concat {SecondaryFpga Rev: } $SecondaryRev1.$SecondaryRev2]
puts [concat {SecondaryFpga Test Rev: } $SecondaryTestRev1.$SecondaryTestRev2]
puts "*************************************************************"

set read_val ""

puts "Hit <ENTER> to continue"
gets stdin

for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
	puts ""
}

while {$read_val != "x"} {                                                       ;#loop main menu 
	puts ""
	puts ""
	puts ""
	puts "*************************************************************"
	puts "******************* MAIN MENU *******************************"
	puts "*************************************************************"
	puts "1) Get FPGA & BIOS Post-Code once"
	puts "2) Get FPGA & BIOS Post-Codes continuosly"
	puts "3) Get FPGA Versions"
	puts "4) Enter PowerSeq sub-menu"
	puts "x) Exit"
	puts "Select your option and hit <ENTER>"
	set read_val [gets stdin]
	if {$read_val == "x"} {
		for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
			puts ""
		}
		puts "GOODBYE!"
		end_insystem_source_probe
		exit
	} elseif {$read_val == "1"} {
		for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
			puts ""
		}
		set Fpga [read_probe_data -value_in_hex -instance_index 0]                         ;#get fpga postcode and print
		puts "*************************************************************"
		puts "*************** FPGA & BIOS PostCodes ***********************"
		puts "*************************************************************"
		puts [concat {FPGA CODE: } $Fpga]
		puts "------------------------------"
		set Bios [read_probe_data -value_in_hex -instance_index 1]                         ;#get BIOS postcode and print
		puts [concat {BIOS CODE: } $Bios]
		puts "*************************************************************"
		puts "Hit <ENTER> to continue"
		gets stdin
	} elseif {$read_val == "2"} {
		fconfigure stdin -blocking 0
		for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
			puts ""
		}
		puts "Press x + <enter> to stop."
		after $delay2
		while {[gets stdin] != "x"} {
			while {[catch {set Fpga [read_probe_data -value_in_hex -instance_index 0]} errmsg]} {                     ;#get FPGA & BIOS postcodes and print (in a loop)
				if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
					puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
					after $delay2
				} else {
					end_insystem_source_probe
					start_insystem_source_probe -device_name $DevName -hardware_name $HwName
				}
			}
			while {[catch {set Bios [read_probe_data -value_in_hex -instance_index 1]} errmsg]} {
				if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
					puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
					after $delay2
				} else {
					end_insystem_source_probe
					start_insystem_source_probe -device_name $DevName -hardware_name $HwName
				}
			}
			puts "*************************************************************"
			puts "*************** FPGA & BIOS PostCodes ***********************"
			puts [concat {FPGA CODE: } $Fpga ]
			puts "------------------------------"
			puts [concat {BIOS CODE: } $Bios ]
			puts "*************************************************************"
			puts ""
			after $delay1;           # Slow the loop down!
		}
		# Set it back to normal
		fconfigure stdin -blocking 1
	} elseif {$read_val == "3"} {
		for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
			puts ""
		}
		puts "*************************************************************"
		puts "*************** Board FPGA Versions *************************"
		puts "*************************************************************"
		puts [concat {MainFpga Rev: } $MainRev1.$MainRev2]                                ;#print FPGAs versions (gotten at the beginning, no need for re-read)
		puts [concat {MainFpga Test Rev: } $MainTestRev1.$MainTestRev2]
		puts "----------------------------------"
		puts [concat {SecondaryFpga Rev: } $SecondaryRev1.$SecondaryRev2]
		puts [concat {SecondaryFpga Test Rev: } $SecondaryTestRev1.$SecondaryTestRev2]
		puts "*************************************************************"
		puts ""
		puts ""
		puts "Hit <ENTER> to continue"
		gets stdin
	} elseif {$read_val == "4"} {
		#Pwr-seq sub-menu
		set read_val2 "0"
		puts $read_val2
		while {$read_val2 != "x"} {
			for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
				puts ""
			}
			puts "*************************************************************"
			puts "************ Power Seq sub-menu *****************************"
			puts "*************************************************************"
			puts "1) BMC "
			puts "2) PCH "
			puts "3) PSU "
			puts "4) CPUs & MEM"
			puts "x) exit"
			puts ""
			puts "Select your option and hit <ENTER>"
			set read_val2 [gets stdin]
			if {$read_val2=="1"} {
                
                fconfigure stdin -blocking 0
				for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
					puts ""
				}
				puts "Press x + <enter> to stop."
				after $delay2
				while {[gets stdin] != "x"} {
				
					#for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
					#	puts ""
					#}
				
					while {[catch {set BMC_H [read_probe_data -value_in_hex -instance_index 3]} errmsg]} {                     ;#get BMC signals
						if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
							puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
							after $delay2
						} else {
							end_insystem_source_probe
							start_insystem_source_probe -device_name $DevName -hardware_name $HwName
						}
					}

                    binary scan [binary format H* $BMC_H] B* BMC_B                            ;#convert to BIN

                    puts "**************** BMC PwrSeq Signals *************************"
                    puts [concat {BMC_P2V5_ENABLE  = } [string range $BMC_B 4 4]]
                    puts [concat {BMC_P2V5_POWERGD = } [string range $BMC_B 3 3]]             ;#BMC has 9 bits, using 3 nibbles, so first 3 bits (0-2, MSb, are unused
                    puts "---------------------------------"
                    puts [concat {PCH_P1V8_ENABLE  = } [string range $BMC_B 6 6]]
                    puts [concat {PCH_P1V8_POWERGD = } [string range $BMC_B 5 5]]
                    puts "---------------------------------"
                    puts [concat {BMC_P1V2_ENABLE  = } [string range $BMC_B 8 8]]
                    puts [concat {BMC_P1V2_POWERGD = } [string range $BMC_B 7 7]]
                    puts "---------------------------------"
                    puts [concat {BMC_P1V0_ENABLE  = } [string range $BMC_B 10 10]]
                    puts [concat {BMC_P1V0_POWERGD = } [string range $BMC_B 9 9]]
                    puts "---------------------------------"
                    puts [concat {BMC_SRESET_N     = } [string range $BMC_B 11 11]]
                    puts "---------------------------------"
                    
                    after $delay1
					for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
						puts ""
					}
                }
				fconfigure stdin -blocking 1

			} elseif {$read_val2=="2"} {
				fconfigure stdin -blocking 0
				for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
					puts ""
				}
				puts "Press x + <enter> to stop."
				after $delay2
				while {[gets stdin] != "x"} {
					while {[catch {set BMC_H [read_probe_data -value_in_hex -instance_index 3]} errmsg]} {                     ;#get BMC signals
						if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
							puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
							after $delay2
						} else {
							end_insystem_source_probe
							start_insystem_source_probe -device_name $DevName -hardware_name $HwName
						}
					}
					while {[catch {set PCH_H [read_probe_data -value_in_hex -instance_index 4]} errmsg]} {                     ;#get PCH signals
						if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
						puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
						after $delay2
						} else {
							end_insystem_source_probe
							start_insystem_source_probe -device_name $DevName -hardware_name $HwName
						}
					}
				
				
					#set BMC_H [read_probe_data -value_in_hex -instance_index 3]               ;#get BMC signals
					binary scan [binary format H* $BMC_H] B* BMC_B                            ;#convert to BIN
					#set PCH_H [read_probe_data -value_in_hex -instance_index 4]               ;#get PCH signals
					binary scan [binary format H* $PCH_H] B* PCH_B                            ;#convert to BIN
					#puts [concat {PCH HEX: } $PCH_H]
					#puts [concat {PCH BIN: } $PCH_B]
					#for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
					#	puts ""
					#}
					puts "**************** PCH PwrSeq Signals *************************"
					puts [concat {PCH_P1V8_ENABLE   = } [string range $BMC_B 6 6]]           ;#PCH_P1V8 comes in the BMC data
					puts [concat {PCH_P1V8_POWERGD  = } [string range $BMC_B 5 5]]
					puts "---------------------------------"
					puts [concat {PCH_PVNN_ENABLE   = } [string range $PCH_B 1 1]]
					puts [concat {PCH_PVNN_POWERGD  = } [string range $PCH_B 0 0]]           ;#PCH uses 7 bits (2 nibbles), so first bit (bit 0, the MSb) is unused
					puts "---------------------------------"
					puts [concat {PCH_P1V05_ENABLE  = } [string range $PCH_B 3 3]]
					puts [concat {PCH_P1V05_POWERGD = } [string range $PCH_B 2 2]]
					puts "---------------------------------"
					puts [concat {PCH_SLEEP_S5      = } [string range $PCH_B 5 5]]
					puts [concat {PCH_SLEEP_S3      = } [string range $PCH_B 4 4]]
					puts "---------------------------------"
					puts [concat {PCH_RESUME_RESET_N = } [string range $PCH_B 6 6]]
					puts "---------------------------------"
					puts [concat {PCH_PLATFORM_RESET_N = } [string range $PCH_B 7 7]]
					puts "---------------------------------"
					#puts "Hit <ENTER> to continue"
					#gets stdin
					after $delay1
					for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
						puts ""
					}
				}
				fconfigure stdin -blocking 1
			} elseif {$read_val2=="3"} {			
					
				fconfigure stdin -blocking 0
				for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
					puts ""
				}
				puts "Press x + <enter> to stop."
				after $delay2
				while {[gets stdin] != "x"} {
					while {[catch {set PSU_H [read_probe_data -value_in_hex -instance_index 5]} errmsg]} {                     ;#get PSU signals
						if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
							puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
							after $delay2
						} else {
							end_insystem_source_probe
							start_insystem_source_probe -device_name $DevName -hardware_name $HwName
						}
					}
					binary scan [binary format H* $PSU_H] B* PSU_B                            ;#convert to BIN
					#puts [concat {PSU HEX: } $PSU_H]
					#puts [concat {PSU BIN: } $PSU_B]
					puts "**************** PCH PwrSeq Signals *************************"
					puts [concat {PSU_POWER_ENABLE  = } [string range $PSU_B 1 1]]
					puts [concat {PSU_POWER_OK      = } [string range $PSU_B 0 0]]
					puts "---------------------------------"
					puts [concat {PSU_P5V_ENABLE    = } [string range $PSU_B 3 3]]
					puts [concat {PSU_P3V3_POWERGD  = } [string range $PSU_B 2 2]]
					puts "---------------------------------"
					after $delay1
					for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
						puts ""
					}
				}
				fconfigure stdin -blocking 1

			} elseif {$read_val2=="4"} {

                for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
						puts ""
					}

                puts "0) check for CPU related signals"
                puts "1) check for MEM related signals"
                set read_val3 [gets stdin]

                if {$read_val3 == "0"} {
                    
                    fconfigure stdin -blocking 0
                    for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
                        puts ""
                    }
                    puts "Press x + <enter> to stop."
                    after $delay2

                    while {[gets stdin] != "x"} {
                        while {[catch {set CPU_H [read_probe_data -value_in_hex -instance_index 6]} errmsg]} {     ;#get CPUs & MEM signals
                            if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
                                puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
                                after $delay2
                            } else {
                                end_insystem_source_probe
                                start_insystem_source_probe -device_name $DevName -hardware_name $HwName
                            }
                        }
                        binary scan [binary format H* $CPU_H] B* CPU_B                            ;#convert to BIN

                        puts "**************** CPUs & MEM PwrSeq Signals *******************"
                        puts "************************** CPU 0 *****************************"
                        puts [concat {CPU0_PVCCD_HV_ENABLE          = } [string range $CPU_B 1 1]]
                        puts [concat {CPU0_PVCCD_HV_POWERGD         = } [string range $CPU_B 0 0]]
                        puts "---------------------------------"
                        puts [concat {CPU0_PVPP_ENABLE              = } [string range $CPU_B 3 3]]
                        puts [concat {CPU0_PVPP_POWERGD             = } [string range $CPU_B 2 2]]
                        puts "---------------------------------"
                        puts [concat {CPU0_PVCCFA_EHV_ENABLE        = } [string range $CPU_B 5 5]]
                        puts [concat {CPU0_PVCCFA_EHV_POWERGD       = } [string range $CPU_B 4 4]]
                        puts "---------------------------------"
                        puts [concat {CPU0_PVCCFA_EHV_FIVRA_ENABLE  = } [string range $CPU_B 7 7]]
                        puts [concat {CPU0_PVCCFA_EHV_FIVRA_POWERGD = } [string range $CPU_B 6 6]]
                        puts "---------------------------------"
                        puts [concat {CPU0_PVCCINFAON_ENABLE        = } [string range $CPU_B 9 9]]
                        puts [concat {CPU0_PVCCINFAON_POWERGD       = } [string range $CPU_B 8 8]]
                        puts "---------------------------------"
                        puts [concat {CPU0_PVNN_MAIN_ENABLE         = } [string range $CPU_B 11 11]]
                        puts [concat {CPU0_PVNN_MAIN_POWERGD        = } [string range $CPU_B 10 10]]
                        puts "---------------------------------"
                        puts [concat {CPU0_PVCCIN_ENABLE            = } [string range $CPU_B 13 13]]
                        puts [concat {CPU0_PVCCIN_POWERGD           = } [string range $CPU_B 12 12]]
                        puts "***************************************************************"
                        puts "************************** CPU 1 *****************************"
                        puts [concat {CPU1_PVCCD_HV_ENABLE          = } [string range $CPU_B 15 15]]
                        puts [concat {CPU1_PVCCD_HV_POWERGD         = } [string range $CPU_B 14 14]]
                        puts "---------------------------------"
                        puts [concat {CPU1_PVPP_ENABLE              = } [string range $CPU_B 17 17]]
                        puts [concat {CPU1_PVPP_POWERGD             = } [string range $CPU_B 16 16]]
                        puts "---------------------------------"
                        puts [concat {CPU1_PVCCFA_EHV_ENABLE        = } [string range $CPU_B 19 19]]
                        puts [concat {CPU1_PVCCFA_EHV_POWERGD       = } [string range $CPU_B 18 18]]
                        puts "---------------------------------"
                        puts [concat {CPU1_PVCCFA_EHV_FIVRA_ENABLE  = } [string range $CPU_B 21 21]]
                        puts [concat {CPU1_PVCCFA_EHV_FIVRA_POWERGD = } [string range $CPU_B 20 20]]
                        puts "---------------------------------"
                        puts [concat {CPU1_PVCCINFAON_ENABLE        = } [string range $CPU_B 23 23]]
                        puts [concat {CPU1_PVCCINFAON_POWERGD       = } [string range $CPU_B 22 22]]
                        puts "---------------------------------"
                        puts [concat {CPU1_PVNN_MAIN_ENABLE         = } [string range $CPU_B 25 25]]
                        puts [concat {CPU1_PVNN_MAIN_POWERGD        = } [string range $CPU_B 24 24]]
                        puts "---------------------------------"
                        puts [concat {CPU1_PVCCIN_ENABLE            = } [string range $CPU_B 27 27]]
                        puts [concat {CPU1_PVCCIN_POWERGD           = } [string range $CPU_B 26 26]]
                        puts "***************************************************************"
                        after $delay1
                        for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
                            puts ""
                        }
                    }
                    fconfigure stdin -blocking 1
                } elseif {$read_val3 == "1"} {
                    fconfigure stdin -blocking 0
                    for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
                        puts ""
                    }
                    puts "Press x + <enter> to stop."
                    after $delay2

                    while {[gets stdin] != "x"} {
                        while {[catch {set CPU_H [read_probe_data -value_in_hex -instance_index 6]} errmsg]} {     ;#get CPUs & MEM signals
                            if {[catch {set DevName [lindex [get_device_names -hardware_name $HwName] 0]} errmsg]} {
                                puts "Platform powered off or USB-BLASTER not connected to the platform, will retry"
                                after $delay2
                            } else {
                                end_insystem_source_probe
                                start_insystem_source_probe -device_name $DevName -hardware_name $HwName
                            }
                        }
                        binary scan [binary format H* $CPU_H] B* CPU_B                            ;#convert to BIN


                        puts "************************ MEM CPU 0 ****************************"
                        puts [concat {CPU0_POWERGD_FAIL_AB          = } [string range $CPU_B 28 28]]
                        puts [concat {CPU0_DRAM_PWROK_AB            = } [string range $CPU_B 32 32]]
                        puts "---------------------------------"
                        puts [concat {CPU0_POWERGD_FAIL_CD          = } [string range $CPU_B 29 29]]
                        puts [concat {CPU0_DRAM_PWROK_CD            = } [string range $CPU_B 33 33]]
                        puts "---------------------------------"
                        puts [concat {CPU0_POWERGD_FAIL_EF          = } [string range $CPU_B 30 30]]
                        puts [concat {CPU0_DRAM_PWROK_EF            = } [string range $CPU_B 34 34]]
                        puts "---------------------------------"
                        puts [concat {CPU0_POWERGD_FAIL_GH          = } [string range $CPU_B 31 31]]
                        puts [concat {CPU0_DRAM_PWROK_GH            = } [string range $CPU_B 35 35]]
                        puts "***************************************************************"
                        puts "************************ MEM CPU 1 ****************************"
                        puts [concat {CPU1_POWERGD_FAIL_AB          = } [string range $CPU_B 36 36]]
                        puts [concat {CPU1_DRAM_PWROK_AB            = } [string range $CPU_B 40 40]]
                        puts "---------------------------------"
                        puts [concat {CPU1_POWERGD_FAIL_CD          = } [string range $CPU_B 37 37]]
                        puts [concat {CPU1_DRAM_PWROK_CD            = } [string range $CPU_B 41 41]]
                        puts "---------------------------------"
                        puts [concat {CPU1_POWERGD_FAIL_EF          = } [string range $CPU_B 38 38]]
                        puts [concat {CPU1_DRAM_PWROK_EF            = } [string range $CPU_B 42 42]]
                        puts "---------------------------------"
                        puts [concat {CPU1_POWERGD_FAIL_GH          = } [string range $CPU_B 39 39]]
                        puts [concat {CPU1_DRAM_PWROK_GH            = } [string range $CPU_B 43 43]]
                        puts "---------------------------------"
                        after $delay1
                        for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
                            puts ""
                        }
                    }
                    fconfigure stdin -blocking 1
                }
			}
		}
		
	}
	for {set i 0} {$i < $pclean} {incr i} {      ;#doing some sort of clear-screen
		puts ""
	}
}

end_insystem_source_probe

