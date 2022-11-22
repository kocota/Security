#include "ReadRegistersTask.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"

extern ADC_HandleTypeDef hadc1;
extern osThreadId LedTaskHandle;
extern osMutexId Fm25v02MutexHandle;
extern status_register_struct status_registers;
extern control_register_struct control_registers;


void ThreadReadRegistersTask(void const * argument)
{
	read_status_registers();
	read_control_registers();
	osThreadResume(LedTaskHandle);
	osDelay(1000);


	for(;;)
	{

		read_status_registers();
		read_control_registers();

		osDelay(1000);
	}
}
