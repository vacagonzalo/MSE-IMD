# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_test_system/_ide/scripts/debugger_myalu_test-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_test_system/_ide/scripts/debugger_myalu_test-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Arty Z7 003017A5C7CDA" && level==0 && jtag_device_ctx=="jsn-Arty Z7-003017A5C7CDA-23727093-0"}
fpga -file /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_test/_ide/bitstream/my_artyz7_20.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_artyz7_20/export/myalu_artyz7_20/hw/my_artyz7_20.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_test/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_test/Debug/myalu_test.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
