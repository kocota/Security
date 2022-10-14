#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"
#include "m95.h"

extern osMessageQId ModbusQueueHandle;
extern osMutexId UartMutexHandle;

extern uint8_t modbus_buffer[256];

osEvent ModbusEvent;

uint32_t crc_temp;
uint8_t level;
uint8_t buf_out[256];
uint8_t buf_out1[256];


void ThreadModbusTask(void const * argument)
{
	//uint8_t id1[20]; // номер CCID симкарты

	uint8_t i=0;
	uint8_t i_max;
	uint16_t modbus_size;
	uint16_t modbus_address;
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
		ModbusEvent = osMessageGet(ModbusQueueHandle, 0); // ожидаем сообщение
		if(ModbusEvent.status == osEventMessage) // если сообщение пришло
		{

			switch(ModbusEvent.value.v) // проверяем полученное значение из очереди
			{
				case(0x01):
					if(i==0)
					{
						modbus_buffer[i] = ModbusEvent.value.v;
						i++;
					}
					else if(i==1)
					{
						i = 0;
						i_max = 0;
					}
					else if(i>1)
					{
						if( (i==6) && (modbus_buffer[1] == 0x10) )
						{
							modbus_buffer[i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[i] = ModbusEvent.value.v;
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
						modbus_buffer[i] = ModbusEvent.value.v;
						i++;
						i_max = 8;
					}
					else if(i>1)
					{
						if( (i==6) && (modbus_buffer[1] == 0x10) )
						{
							modbus_buffer[i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[i] = ModbusEvent.value.v;
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
						modbus_buffer[i] = ModbusEvent.value.v;
						i++;
						i_max = 8;
					}
					if(i>1)
					{
						if( (i==6) && (modbus_buffer[1] == 0x10) )
						{
							modbus_buffer[i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[i] = ModbusEvent.value.v;
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
						modbus_buffer[i] = ModbusEvent.value.v;
						i++;
					}
					else if(i>1)
					{
						if( (i==6) && (modbus_buffer[1] == 0x10) )
						{
							modbus_buffer[i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[i] = ModbusEvent.value.v;
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
						if( (i==6) && (modbus_buffer[1] == 0x10) )
						{
							modbus_buffer[i] = ModbusEvent.value.v;
							i_max = 9 + (uint8_t)ModbusEvent.value.v;
							i++;
						}
						else
						{
							modbus_buffer[i] = ModbusEvent.value.v;
							i++;
						}
					}

				break;
			}
			if( (i >= i_max) && (i != 0) && (i_max != 0) ) // если число принятых байт соответствует длине соответствующей команды
			{
				crc_temp = CRC16(modbus_buffer, i_max-2); // считаем контрольную сумму принятого пакета
				if( ( ((crc_temp>>8)&0x00FF) == modbus_buffer[i_max-1] ) && ( (crc_temp&0x00FF) == modbus_buffer[i_max-2]) ) // проверяем контрольную сумму принятого пакета
				{
					switch(modbus_buffer[1]) // проверяем тип поступившей команды MODBUS и формируем соответствующий ответ
					{
						case(0x03): // чтение регистра

							modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для чтения
							modbus_size = (((((uint16_t)modbus_buffer[4])<<8)&0xFF00)|(((uint16_t)modbus_buffer[5])&0xFF)); //  считаем количество регистров для чтения
							if( modbus_address == SIGNAL_LEVEL ) // Если запрашивается уровень сигнала
							{

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_CSQ(&level);
								osMutexRelease(UartMutexHandle);
								fm25v02_write(modbus_address, level);

								fm25v02_fast_read( modbus_address , &buf_out[0] , modbus_size); // читаем из памяти необходимое количество регистров

								buf_out1[0] = 0x01;
								buf_out1[1] = 0x03;
								buf_out1[2] = 2*modbus_size;
								for(uint8_t i=0; i<2*modbus_size; i++)
								{
									buf_out1[2*i+3] = 0;
									buf_out1[2*i+4] = buf_out[i];
								}
								crc_temp = CRC16(&buf_out1[0], 3+2*modbus_size);
								buf_out1[2*modbus_size+3] = (uint8_t)(crc_temp&0x00FF);
								buf_out1[2*modbus_size+4] = (uint8_t)((crc_temp>>8)&0x00FF);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&buf_out1[0], 2*modbus_size+5);
								osMutexRelease(UartMutexHandle);

								/*
								osMutexWait(UartMutexHandle, osWaitForever);
								AT_CSQ(&level);
								osMutexRelease(UartMutexHandle);
								fm25v02_write(2*modbus_address, 0);
								fm25v02_write(2*modbus_address+1, level);

								fm25v02_fast_read( 2*modbus_address , &buf_out[0] , 2*modbus_size); // читаем из памяти необходимое количество регистров

								buf_out1[0] = 0x01;
								buf_out1[1] = 0x03;
								buf_out1[2] = 2*modbus_size;
								for(uint8_t i=0; i<2*modbus_size; i++)
								{
									buf_out1[i+3] = buf_out[i];
								}
								crc_temp = CRC16(&buf_out1[0], 3+2*modbus_size);
								buf_out1[2*modbus_size+3] = (uint8_t)(crc_temp&0x00FF);
								buf_out1[2*modbus_size+4] = (uint8_t)((crc_temp>>8)&0x00FF);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&buf_out1[0], 2*modbus_size+5);
								osMutexRelease(UartMutexHandle);
								*/

							}

							else
							{
								fm25v02_fast_read( modbus_address , &buf_out[0] , modbus_size); // читаем из памяти необходимое количество регистров

								buf_out1[0] = 0x01;
								buf_out1[1] = 0x03;
								buf_out1[2] = 2*modbus_size;
								for(uint8_t i=0; i<2*modbus_size; i++)
								{
									buf_out1[2*i+3] = 0;
									buf_out1[2*i+4] = buf_out[i];
								}
								crc_temp = CRC16(&buf_out1[0], 3+2*modbus_size);
								buf_out1[2*modbus_size+3] = (uint8_t)(crc_temp&0x00FF);
								buf_out1[2*modbus_size+4] = (uint8_t)((crc_temp>>8)&0x00FF);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&buf_out1[0], 2*modbus_size+5);
								osMutexRelease(UartMutexHandle);

								/*
								fm25v02_fast_read( 2*modbus_address , &buf_out[0] , 2*modbus_size); // если запрашивается чтение регистров, читаем из памяти необходимое количество регистров

								buf_out1[0] = 0x01;
								buf_out1[1] = 0x03;
								buf_out1[2] = 2*modbus_size;
								for(uint8_t i=0; i<2*modbus_size; i++)
								{
									buf_out1[i+3] = buf_out[i];
								}
								crc_temp = CRC16(&buf_out1[0], 3+2*modbus_size);
								buf_out1[2*modbus_size+3] = (uint8_t)(crc_temp&0x00FF);
								buf_out1[2*modbus_size+4] = (uint8_t)((crc_temp>>8)&0x00FF);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&buf_out1[0], 2*modbus_size+5);
								osMutexRelease(UartMutexHandle);
								*/
							}

						break;

						case(0x06): // запись одного регистра

							modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для записи

							if( (modbus_address>=0x1090) && (modbus_address<=0x10FF) )
							{
								//modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для записи

								fm25v02_fast_write(modbus_address, &modbus_buffer[4], 2);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&modbus_buffer[0], 8);
								osMutexRelease(UartMutexHandle);
							}
							/*
							if( (modbus_address>=0x1090) && (modbus_address<=0x10FF) )
							{
								modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для записи

								fm25v02_fast_write(2*modbus_address, &modbus_buffer[4], 2);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&modbus_buffer[0], 8);
								osMutexRelease(UartMutexHandle);
							}
							*/

						break;

						case(0x10): // запись нескольких регистров

							modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для записи

							if( !( (modbus_address>=0x1000) && (modbus_address<=0x108F) ) )
							{

								for(uint8_t a=0; a<(modbus_buffer[6])/2; a++) // исправил 'i' на 'a', так как в функции fm25v02_fast_write() внутри уже есть 'i'
								{
									fm25v02_fast_write(modbus_address+a, &modbus_buffer[8+a*2], 1);
								}


								buf_out1[0] = 0x01;
								buf_out1[1] = 0x10;
								buf_out1[2] = modbus_buffer[2];
								buf_out1[3] = modbus_buffer[3];
								buf_out1[4] = modbus_buffer[4];
								buf_out1[5] = modbus_buffer[5];

								crc_temp = CRC16(&buf_out1[0], 6);

								buf_out1[6] = (uint8_t)(crc_temp&0x00FF);
								buf_out1[7] = (uint8_t)((crc_temp>>8)&0x00FF);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&buf_out1[0], 8);
								osMutexRelease(UartMutexHandle);
							}

							/*
							if( (modbus_address>=0x1090) && (modbus_address<=0x10FF) )
							{
								modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для записи
								fm25v02_fast_write(2*modbus_address, &modbus_buffer[7], modbus_buffer[6]);

								buf_out1[0] = 0x01;
								buf_out1[1] = 0x10;
								buf_out1[2] = modbus_buffer[2];
								buf_out1[3] = modbus_buffer[3];
								buf_out1[4] = modbus_buffer[4];
								buf_out1[5] = modbus_buffer[5];

								crc_temp = CRC16(&buf_out1[0], 6);

								buf_out1[6] = (uint8_t)(crc_temp&0x00FF);
								buf_out1[7] = (uint8_t)((crc_temp>>8)&0x00FF);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&buf_out1[0], 8);
								osMutexRelease(UartMutexHandle);
							}
							*/

						break;
					}
				}

				i = 0;     // обнуляем значение текущего принятого байта
				i_max = 0; // обнуляем максимальное значение принятого байта

			}
		}




		osDelay(1);
	}
}
