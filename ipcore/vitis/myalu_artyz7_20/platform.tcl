# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_artyz7_20/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/gonzalo/workspace/MSE-IMD/ipcore/vitis/myalu_artyz7_20/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {myalu_artyz7_20}\
-hw {/home/gonzalo/workspace/MSE-IMD/hardware/my_artyz7_20.xsa}\
-proc {ps7_cortexa9_0} -os {freertos10_xilinx} -out {/home/gonzalo/workspace/MSE-IMD/ipcore/vitis}

platform write
platform generate -domains 
platform active {myalu_artyz7_20}
bsp reload
bsp write
platform generate
