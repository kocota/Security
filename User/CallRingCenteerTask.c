#include "CallRingCenterTask.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "m95.h"

extern osThreadId CallRingCenterTaskHandle;
extern osMutexId UartMutexHandle;
extern control_register_struct control_registers;


void ThreadCallRingCenterTask(void const * argument)
{



	for(;;)
	{
		osThreadSuspend(CallRingCenterTaskHandle); // переходим в спящий режим и ждем

		while(control_registers.gprs_call_reg != CALL_ON) // пока регистр флага дозвона в центр не включен, ничего не делаем
		{

		}

			osMutexWait(UartMutexHandle, osWaitForever);
			while(request_to_server() != AT_OK){}; // делаем запрос на сервер
			osMutexRelease(UartMutexHandle);

		osDelay(5000); // ждем, если флаг не сброшен запрос повторяется
	}
}
