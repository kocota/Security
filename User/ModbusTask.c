#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"
#include "m95.h"

extern osTimerId Ring_Center_TimerHandle;
extern osMessageQId ModbusQueueHandle;
extern osMutexId UartMutexHandle;
extern osMutexId Fm25v02MutexHandle;
extern osSemaphoreId ModbusPacketReceiveHandle;

extern uint8_t modbus_buffer[20][256];

osEvent ModbusEvent;

uint32_t crc_temp;
uint8_t level;
uint8_t buf_out[256];
uint8_t buf_out1[256];

uint8_t modbus_packet_number = 0;
uint8_t modbus_packet_number1 = 0;


void ThreadModbusTask(void const * argument)
{
	//uint8_t id1[20]; // номер CCID симкарты

	uint8_t i=0;
	uint8_t i_max;
	//uint16_t modbus_size;
	//uint16_t modbus_address;
	//uint8_t modbus_packet_number;
	//uint16_t modbus_data;

	//uint8_t data[10];
	//data[0]=(uint8_t)((ID_H>>8)&0xFF); // стартовый номер устройства, старший байт 1
	//data[1]=(uint8_t)(ID_H&0xFF); // стартовый номер устройства, старший байт 2
	//data[2]=(uint8_t)((ID_L>>8)&0xFF); // стартовый номер устройства, младший байт 1
	//data[3]=(uint8_t)(ID_L&0xFF); // стартовый номер устройства, младший байт 2
	//data[4]=Version_H; // версия прошивки, старший байт
	//data[5]=Version_L; // версия прошивки, младший байт

	//fm25v02_fast_write(VERSION_REG, &data[0], 4);// записываем в память ID устройства
	//fm25v02_fast_write(0x1000*2, &data[4], 2);// записываем в память номер версии прошивки


	for(;;)
	{
		ModbusEvent = osMessageGet(ModbusQueueHandle, osWaitForever); // ожидаем сообщение
		if(ModbusEvent.status == osEventMessage) // если сообщение пришло
		{

			switch(ModbusEvent.value.v) // проверяем полученное значение из очереди
			{
				case(0x01):
					if(i==0)
					{
						modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
						i++;
					}
					else if(i==1)
					{
						i = 0;
						i_max = 0;
					}
					else if(i>1)
					{
						if( (i==6) && (modbus_buffer[modbus_packet_number][1] == 0x10) )
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i++;
						}
					}

				break;

				case(0x03):
					if(i==0)
					{
						i = 0;
						i_max = 0;
					}
					else if(i==1)
					{
						modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
						i++;
						i_max = 8;
					}
					else if(i>1)
					{
						if( (i==6) && (modbus_buffer[modbus_packet_number][1] == 0x10) )
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i++;
						}
					}

				break;

				case(0x06):
					if(i==0)
					{
						i = 0;
						i_max = 0;
					}
					else if(i==1)
					{
						modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
						i++;
						i_max = 8;
					}
					if(i>1)
					{
						if( (i==6) && (modbus_buffer[modbus_packet_number][1] == 0x10) )
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i++;
						}
					}

				break;

				case(0x10):
					if(i==0)
					{
						i = 0;
						i_max = 0;
					}
					else if(i==1)
					{
						modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
						i++;
					}
					else if(i>1)
					{
						if( (i==6) && (modbus_buffer[modbus_packet_number][1] == 0x10) )
						{
							//if( modbus_buffer[modbus_packet_number][i-1] == (ModbusEvent.value.v)/2 )
							//{
								modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
								i_max = 9 + (uint8_t)ModbusEvent.value.v;
								i++;
							//}
							//else
							//{
								//i = 0;
								//i_max = 0;
							//}
						}
						else
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i++;
						}
					}

				break;

				default:
					if(i==0)
					{
						i = 0;
						i_max = 0;
					}
					else if(i==1)
					{
						i = 0;
						i_max = 0;
					}
					else if(i>1)
					{
						if( (i==6) && (modbus_buffer[modbus_packet_number][1] == 0x10) )
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[modbus_packet_number][i] = ModbusEvent.value.v;
							i++;
						}
					}

				break;
			}
			if( (i >= i_max) && (i != 0) && (i_max != 0) ) // если число принятых байт соответствует длине соответствующей команды
			{
				crc_temp = CRC16(&modbus_buffer[modbus_packet_number][0], i_max-2); // считаем контрольную сумму принятого пакета
				if( ( ((crc_temp>>8)&0x00FF) == modbus_buffer[modbus_packet_number][i_max-1] ) && ( (crc_temp&0x00FF) == modbus_buffer[modbus_packet_number][i_max-2]) ) // проверяем контрольную сумму принятого пакета
				{
					modbus_packet_number1 = modbus_packet_number;
					modbus_packet_number++;
					if( modbus_packet_number >= 20)
					{
						modbus_packet_number = 0;
					}
					//osThreadSuspend(thread_id)
					osSemaphoreRelease(ModbusPacketReceiveHandle);
				}

				i = 0;     // обнуляем значение текущего принятого байта
				i_max = 0; // обнуляем максимальное значение принятого байта

			}
		}




		osDelay(1);
	}
}
