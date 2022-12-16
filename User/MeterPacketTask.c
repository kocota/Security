#include "ModbusPacketTask.h"
#include "cmsis_os.h"

osEvent MeterEvent;
extern osMessageQId MeterQueueHandle;

void ThreadMeterPacketTask(void const * argument)
{

	//uint8_t i=0;
	//uint8_t i_max;


	for(;;)
	{
		/*
		MeterEvent = osMessageGet(MeterQueueHandle, osWaitForever); // ожидаем сообщение
		if(MeterEvent.status == osEventMessage) // если сообщение пришло
		{
			switch(MeterEvent.value.v)
			{
				case(0x01):

				break;
			}
		}
		*/

		osDelay(1);
	}
}
