/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
#include "myalu.h"

#define MYALU_BASE_ADDR 0x43C00000

void myalu_test(void *not_used);

int main( void )
{
	BaseType_t res;
	res = xTaskCreate( 	myalu_test,
					( const char * ) "myalu",
					1024 * 1,
					NULL,
					tskIDLE_PRIORITY,
					NULL );

	configASSERT( pdPASS == res );
	vTaskStartScheduler();

	for( ;; );

	return 0;
}

void myalu_test(void *not_used)
{
	xil_printf("enable the core\n\r");
	MYALU_mWriteReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG0_OFFSET, 0x00000001);

	xil_printf("wait for the core to be ready for data\n\r");
	u32 r1 = 0x00000000;
	u32 ready = 0x00000000;
	do{
		r1 = MYALU_mReadReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG1_OFFSET);
		ready = (r1 >> 1);
	} while(0x00000000 == ready);
	xil_printf("core ready\n\r");


	xil_printf("set all inputs to zero\n\r");
	MYALU_mWriteReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG2_OFFSET, 0x00000000);
	MYALU_mWriteReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG3_OFFSET, 0x00000000);
	MYALU_mWriteReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG4_OFFSET, 0x00000000);
	vTaskDelay(1);



	xil_printf("overflow addition test\n\r");
	MYALU_mWriteReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG2_OFFSET, 0x00000005); // add
	MYALU_mWriteReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG3_OFFSET, 0xF000000F); // op1
	MYALU_mWriteReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG4_OFFSET, 0xFFFFFFFF); // op2
	vTaskDelay(1);

	u32 result = 0x00000000;
	u32 carry = 0x00000000;

	result = MYALU_mReadReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG5_OFFSET);
	carry = MYALU_mReadReg(MYALU_BASE_ADDR, MYALU_S00_AXI_SLV_REG1_OFFSET);
	carry = carry & (0x00000001);

	xil_printf("c: %d | o: %x\n\r", carry, result);

	while(1);
}
