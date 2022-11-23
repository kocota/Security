#include "LedTask.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "gpio.h"
#include "fm25v02.h"

extern osThreadId LedTaskHandle;
extern osMutexId Fm25v02MutexHandle;
extern osThreadId IbuttonTaskHandle;
extern status_register_struct status_registers;
extern uint8_t security_state;

void ThreadLedTask(void const * argument)
{

	uint8_t door_open_alarm_counter = 0; // счетчик времени для процесса тревоги открытия двери
	uint8_t arming_process_counter = 0; // счетчик времени для процесса постановки на охрану
	uint8_t arming_error_counter = 0; // счетчик времени для процесса ошибки постановки на охрану

	osThreadSuspend(LedTaskHandle); // переводим поток ThreadLedTask в спящий режим
	osThreadResume(IbuttonTaskHandle); // пробуждаем поток ThreadIbuttonTask из спящего режима



	for(;;)
	{
		switch(status_registers.security_status_reg)
		{
			case(ENABLED_BY_SERVER): // Если включена из центра

				LED_OUT_ON();
				LED2_ON();

			break;

			case(DISABLED_BY_SERVER): // Если отключена из центра

				LED_OUT_OFF();
				LED2_OFF();

			break;

			case(ENABLED_BY_IBUTTON): // Если включена с таблетки

				LED_OUT_ON();
				LED2_ON();

			break;

			case(DISABLED_BY_IBUTTON): // Если отключена с таблетки

				LED_OUT_OFF();
				LED2_OFF();

			break;

			case(DOOR_OPEN_ALARM): // Если тревога открытия двери

				door_open_alarm_counter++; // увеличиваем значение счетчика времени для процесса тревоги открытия двери

				if(door_open_alarm_counter>=20)
				{
					door_open_alarm_counter = 0;
					LED_OUT_TOGGLE();
					LED2_TOGGLE();
				}

			break;

			case(ARMING_PROCESS): // Если в процессе постановки на охрану

				arming_process_counter++; // увеличиваем значение счетчика времени для процесса постановки на охрану

				if(arming_process_counter>=100)
				{
					arming_process_counter = 0;
					LED_OUT_TOGGLE();
					LED2_TOGGLE();
				}

			break;

			case(ARMING_ERROR): // Если ошибка постановки на охрану

				arming_error_counter++; // увеличиваем значение счетчика времени для процесса ошибки постановки на охрану

				if(arming_error_counter>=10)
				{
					arming_error_counter = 0;
					LED_OUT_TOGGLE();
					LED2_TOGGLE();
				}

			break;
		}


		osDelay(10); // вызываем процесс один раз в 10 мс
	}
}
