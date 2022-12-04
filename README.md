# MSE-IMD
Trabajo final de la asignatura "Implementación de manejadores de dispositivos" de la Maestría en Sistemas Embebidos de la Universidad de Buenos Aires

## IPCORE

### Lista de puertos

* clock_i: reloj del core.
* enable_i: habilita el funcionamiento del core.
* reset_n_i: reinicia el core cuando se aplica un nivel alto.
* carry_o: indica que la operación tuvo carry.
* ready_for_data_o: indica que el core se encuentra listo para operar.
* operation_i: selector de operación.
* operand1_i: primer operador.
* operand2_i: segundo operador.
* result_o: resultado de la operación.

![señales](img/signals.png)

### Datos del esclavo AXI

Dirección base: 0x43C00000

|Puerto           |Tipo|Registro|Bit |Offset|
|-----------------|----|--------|----|------|
|enable_i         |in  |R0      |0   |0     |
|reset_n_i        |in  |R0      |1   |0     |
|carry_o          |out |R1      |0   |4     |
|ready_for_data_o |out |R1      |1   |4     |
|operation_i      |in  |R2      |31:0|8     |
|operand1_i       |in  |R3      |31:0|12    |
|operand2_i       |in  |R4      |31:0|16    |
|result_o         |out |R5      |31:0|20    |

### Prueba del diseño de placa

Se realizó un programa en Vitis para verificar el funcionamiento del ipcore y su conexión con la unidad de procesamiento de aplicaciones.

![vitis](img/vitis.png)

![cutecom](img/cutecom.png)

### Prueba en Petalinux

Se realizó una prueba simple con `devmem` y se verificó que **5+5=10**.

![devmem](img/devmem.png)

## Petalinux

### Empaquetar imagen

`petalinux-package --boot --force --fsbl ./images/linux/zynq_fsbl.elf --fpga ./images/linux/system.bit --u-boot`

### Creación de loadable kernel module (LKM)

`petalinux-create -t modules --name mymodule --enable`