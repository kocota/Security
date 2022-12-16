#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "MeterTask.h"
#include "ce303.h"

extern osSemaphoreId RS485TransmissionStateHandle;
extern UART_HandleTypeDef huart6;
extern uint8_t meter_rx_number;
extern uint8_t meter_data[256];




void ThreadMeterTask(void const * argument)
{

	osSemaphoreWait(RS485TransmissionStateHandle, osWaitForever); // обнуляем семафор, при создании семафора его значение равно 1



	for(;;)
	{

		ce303_voltage();
		ce303_current();
		ce303_power();
		ce303_tarif();


		osDelay(2000);
	}
}
