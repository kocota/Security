#include "m95.h"
#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include "gpio.h"

#include "fm25v02.h"
#include "stdio.h"




extern UART_HandleTypeDef huart3;
extern osThreadId M95TaskHandle;
extern osThreadId ModbusTaskHandle;
extern osTimerId AT_TimerHandle;
extern uint8_t modem_rx_data[100];
extern char modem_rx_buffer[100];
extern uint8_t modem_rx_number;
extern uint8_t read_rx_state;
extern osSemaphoreId TransmissionStateHandle;
extern osSemaphoreId ReceiveStateHandle;
extern osMessageQId ModbusQueueHandle;
extern osMutexId UartMutexHandle;
extern osThreadId CurrentID;

extern uint16_t ID_H; // номер устройства
extern uint16_t ID_L; // номер устройства

extern uint8_t Version_H;  // версия прошивки, старший байт
extern uint8_t Version_L; // версия прошивки, младший байт


uint8_t at[3] = "AT\n";
uint8_t at_csq[7] = "AT+CSQ\n";
uint8_t at_cops[9] = "AT+COPS?\n";
//uint8_t at_qiopen[38] = "AT+QIOPEN=\"TCP\",\"195.208.163.67\",35050";
uint8_t at_qiopen[50];

uint8_t state;
uint8_t level;

char str11[4]; // максимальное число 1460



extern uint8_t t;


uint8_t buf_send[9] = {0x01, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x33};

extern uint8_t modbus_buffer[256];

osEvent ModbusEvent;

uint32_t crc_temp;

uint8_t buf_out[20];
uint8_t buf_out1[20];
uint8_t buf_out2[20];
uint8_t b;

unsigned int CRC16( unsigned char * pucFrame, unsigned int usLen );


void m95_power_on(void) // функция включения питания
{
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_RESET);
}

void m95_power_off(void)
{
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(700);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_RESET);
}



void modem_rx_buffer_clear (void)
{
	for(uint16_t i=0; i<256; i++)
	{
		modem_rx_buffer[i] = 0;
	}
}

uint8_t AT (void)
{
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, at, 3);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}

	}
	return AT_ERROR;
}

uint8_t AT_CSQ (uint8_t* signal_level)
{
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, at_csq, 7);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "+CSQ:") != NULL )
		{
			if(modem_rx_buffer[15]==',')
			{
				*signal_level = modem_rx_buffer[14]-0x30;
			}
			else
			{
				*signal_level = (modem_rx_buffer[14]-0x30)*10 + (modem_rx_buffer[15]-0x30);
			}
		}
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
	}
	return AT_ERROR;
}

uint8_t AT_COPS (void)
{
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, at_cops, 9);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 5000);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "MTS") != NULL )
		{
			// Здесь должно быть то, что необходимо сделать, если пришло значение "МТС"
		}

		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}


	}
	return AT_ERROR;
}

uint8_t AT_QIOPEN (char* type , uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port)
{
	char str1[3];
	char str2[3];
	char str3[3];
	char str4[3];
	char str5[3];
	char str6[5];
	char str7[40];
	uint8_t n;

	sprintf(str1, "%s", type);
	sprintf(str2, "%u", ip1);
	sprintf(str3, "%u", ip2);
	sprintf(str4, "%u", ip3);
	sprintf(str5, "%u", ip4);
	sprintf(str6, "%u", port);

	n = sprintf(str7, "AT+QIOPEN=\"%s\",\"%s.%s.%s.%s\",%s\n", str1, str2, str3, str4, str5, str6);

 	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();

	HAL_UART_Transmit_DMA(&huart3, str7, n);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 3000);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if( (strstr(modem_rx_buffer, "CONNECT OK") != NULL) || (strstr(modem_rx_buffer, "ALREADY CONNECT") != NULL) )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		else if( (strstr(modem_rx_buffer, "CONNECT FAIL") != NULL) )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;

}

uint8_t AT_QISEND (uint8_t* buf, uint16_t length) // maximum length = 1460
{
	uint8_t n1;
	char str_out[15];

	n1 = sprintf(str_out, "AT+QISEND=%u\n", length);

 	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();

	HAL_UART_Transmit_DMA(&huart3, str_out, n1);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(osThreadGetId());
		//osSemaphoreWait(ReceiveStateHandle, osWaitForever);
		if( strstr(modem_rx_buffer, "> ") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			modem_rx_number = 0;
			modem_rx_buffer_clear();

			HAL_UART_Transmit_DMA(&huart3, buf, length);

			osSemaphoreWait(TransmissionStateHandle, osWaitForever);

			osTimerStart(AT_TimerHandle, 300); // маленькое время!!!!!
			while(read_rx_state == ACTIVE)
			{
				//osThreadSuspend(osThreadGetId());
				//osSemaphoreWait(ReceiveStateHandle, osWaitForever);
				if( strstr(modem_rx_buffer, "SEND OK") != NULL )
				{
					osTimerStop(AT_TimerHandle);
					read_rx_state = NOT_ACTIVE;
					return AT_OK;
				}
			}
			return AT_ERROR;
		}

	}
	return AT_ERROR;

}

uint8_t AT_QIFGCNT (uint8_t id)
{
	uint8_t str_out1[13];
	sprintf(str_out1, "AT+QIFGCNT=%u\n", id);
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out1, 13);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QIMUX (uint8_t mode) // Команда для включения или отключения возможности нескольких сессий TCP/IP, 1 - включено, 0 - выключено
{
	char str_out[11];
	sprintf(str_out, "AT+QIMUX=%u\n", mode);
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 11);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QIMODE (uint8_t mode) // Команда для переключения режима передачи для TCP/UDP соединения, 1 - прозрачный режим, 0 - нормальный режим
{
	char str_out[12];
	sprintf(str_out, "AT+QIMODE=%u\n", mode);
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 12);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QIREGAPP  (char* apn, char* user, char* password)
{
	uint8_t n;
	char str_out[100];
	sprintf(str_out, "AT+QIREGAPP=\"%s\",\"%s\",\"%s\"\n", apn, user, password);
	n = strlen(str_out);

	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, n);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;

}

uint8_t AT_QIACT (void)
{
	uint8_t str_out[9];
	sprintf(str_out, "AT+QIACT\n");
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 9);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 150000);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QIDEACT (void)
{
	uint8_t str_out[11];
	sprintf(str_out, "AT+QIDEACT\n");
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 11);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 40000);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QISTATE (void)
{
	uint8_t str_out[11];
	sprintf(str_out, "AT+QISTATE\n");
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 11);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 10000);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "IP INITIAL") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return IP_INITIAL;
		}
		if(strstr(modem_rx_buffer, "IP START") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return IP_START;
		}
		if(strstr(modem_rx_buffer, "IP CONFIG") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return IP_CONFIG;
		}
		if(strstr(modem_rx_buffer, "IP IND") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return IP_IND;
		}
		if(strstr(modem_rx_buffer, "IP GPRSACT") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return IP_GPRSACT;
		}
		if(strstr(modem_rx_buffer, "IP STATUS") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return IP_STATUS;
		}
		if(strstr(modem_rx_buffer, "TCP CONNECTING") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return TCP_CONNECTING;
		}
		if(strstr(modem_rx_buffer, "UDP CONNECTING") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return UDP_CONNECTING;
		}
		if(strstr(modem_rx_buffer, "IP CLOSE") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return IP_CLOSE;
		}
		if(strstr(modem_rx_buffer, "CONNECT OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return CONNECT_OK;
		}
		if(strstr(modem_rx_buffer, "PDP DEACT") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return PDP_DEACT;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QIHEAD (uint8_t mode) // функция для включения или отключения хидера "IPD(data lengh):" на приеме
{
	uint8_t str_out[12];
	sprintf(str_out, "AT+QIHEAD=%u\n", mode);
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 12);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QISHOWPT (uint8_t mode) // функция для включения или отключения типа протокола на приеме
{
	uint8_t str_out[14];
	sprintf(str_out, "AT+QISHOWPT=%u\n", mode);
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 14);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_OK;
		}
		if(strstr(modem_rx_buffer, "ERROR") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			read_rx_state = NOT_ACTIVE;
			return AT_ERROR;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QPOWD (uint8_t mode) // функция отключения питания. mode: 1 - сообщение NORMAL POWER DOWN включено, 0 - сообщение NORMAL POWER DOWN выключено
{
	uint8_t str_out[11];
	sprintf(str_out, "AT+QPOWD=%u\n", mode);
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Transmit_DMA(&huart3, str_out, 11);

	osSemaphoreWait(TransmissionStateHandle, osWaitForever);

	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		//osThreadSuspend(M95TaskHandle);
		if(mode==0)
		{
			if(strstr(modem_rx_buffer, "OK") != NULL )
			{
				osTimerStop(AT_TimerHandle);
				read_rx_state = NOT_ACTIVE;
				return AT_OK;
			}
		}
		else if(mode==1)
		{
			if(strstr(modem_rx_buffer, "NORMAL POWER DOWN") != NULL )
			{
				osTimerStop(AT_TimerHandle);
				read_rx_state = NOT_ACTIVE;
				return AT_OK;
			}
		}


	}
	return AT_ERROR;
}


void ThreadM95Task(void const * argument)
{
	osSemaphoreWait(TransmissionStateHandle, osWaitForever);
	osSemaphoreWait(ReceiveStateHandle, osWaitForever);
	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);

	HAL_Delay(1000);
	while(AT()==AT_ERROR)
	{

	}
	if(AT()==AT_ERROR)
	{
		m95_power_on();
		//HAL_Delay(10000);
	}




	//osSemaphoreWait(TransmissionStateHandle, osWaitForever);
	//osMutexRelease(UartMutexHandle);

	for(;;)
	{





/*
		if(AT()==AT_ERROR)
		{
			m95_power_on();
			while(AT()==AT_ERROR)
			{

			}
			read_rx_state = ACTIVE;
			modem_rx_number = 0;
			modem_rx_buffer_clear();
			HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);

			while(read_rx_state == ACTIVE)
			{
				osThreadSuspend(M95TaskHandle);
				if(strstr(modem_rx_buffer, "Call Ready") != NULL )
				{
					HAL_UART_DMAStop(&huart3);
					read_rx_state = NOT_ACTIVE;
				}


			}
		}

*/


		//osMutexRelease(UartMutexHandle);
		osMutexWait(UartMutexHandle, osWaitForever);
		//osMutexWait(UartMutexHandle, osWaitForever);
		//osMutexWait(UartMutexHandle, osWaitForever);
		//osMutexWait(UartMutexHandle, osWaitForever);


		switch(AT_QISTATE())
		{
			case IP_INITIAL:
				//osMutexWait(UartMutexHandle, 0);
				if(AT_QIREGAPP("mts.internet.ru", "mts", "mts") == AT_OK)
				{

				}
				//osMutexRelease(UartMutexHandle);
			break;
			case IP_START:
				//osMutexWait(UartMutexHandle, 0);
				if(AT_QIACT()==AT_OK)
				{

				}
				//osMutexRelease(UartMutexHandle);
			break;
			case IP_IND:
				//osMutexWait(UartMutexHandle, 0);
				if(AT_QIDEACT()==AT_OK)
				{

				}
				//osMutexRelease(UartMutexHandle);
			break;
			case IP_GPRSACT:
				//osMutexWait(UartMutexHandle, 0);
				if( AT_QIOPEN("TCP",195,208,163,67,35050) == AT_OK )
				{
					//if (AT_QISEND(&buf_send[0], 9) == AT_OK)
					//{

					//}
					//else
					//{
						//LED_VD4_ON();
					//}

				}
				//osMutexRelease(UartMutexHandle);
			break;
			case IP_CLOSE:
				//osMutexWait(UartMutexHandle, 0);
				if( AT_QIOPEN("TCP",195,208,163,67,35050) == AT_OK )
				{
					//if (AT_QISEND(&buf_send[0], 9) == AT_OK)
					//{

					//}
					//else
					//{
						//LED_VD4_ON();
					//}

				}
				//osMutexRelease(UartMutexHandle);
			break;

		}
		osMutexRelease(UartMutexHandle);










		//if (AT_QISEND(9) == AT_OK)
		//{
			//HAL_Delay(3000);
			//LED_VD4_TOGGLE();
		//}



		/*
		if(AT_CSQ(&level) == AT_OK)
		{

		}

		if(AT_COPS() == AT_OK)
		{

		}

		if(AT_QIFGCNT(0) == AT_OK)
		{
			//LED_VD4_TOGGLE();
		}
		if(AT_QIMUX(0) == AT_OK)
		{

		}
		if(AT_QIMODE(0) == AT_OK)
		{

		}
		if(AT_QISTATE() == IP_INITIAL)
		{
			LED_VD4_TOGGLE();
		}
		if(AT_QIHEAD(1) == AT_OK)
		{

		}
		if(AT_QISHOWPT(0) == AT_OK)
		{

		}
		*/


		//if(AT_QIREGAPP("mts.internet.ru", "mts", "mts") == AT_OK)
		//{

		//}
		//if(AT_QIACT() == AT_OK)
		//{

		//}

		osDelay(1000);
		//osThreadSuspend(M95TaskHandle);
	}
}


void ThreadModbusTask(void const * argument)
{
	uint8_t i=0;
	uint8_t i_max;
	uint16_t modbus_size;
	uint16_t modbus_address;
	uint16_t modbus_data;

	uint8_t data[10];
	data[0]=(uint8_t)((ID_H>>8)&0xFF); // стартовый номер устройства, старший байт 1
	data[1]=(uint8_t)(ID_H&0xFF); // стартовый номер устройства, старший байт 2
	data[2]=(uint8_t)((ID_L>>8)&0xFF); // стартовый номер устройства, младший байт 1
	data[3]=(uint8_t)(ID_L&0xFF); // стартовый номер устройства, младший байт 2
	data[4]=Version_H; // версия прошивки, старший байт
	data[5]=Version_L; // версия прошивки, младший байт

	fm25v02_fast_write(VERSION_REG, &data[0], 4);// записываем в память ID устройства
	fm25v02_fast_write(0x1000*2, &data[4], 2);// записываем в память номер версии прошивки


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
			if( (i >= i_max) && (i != 0) && (i_max != 0) )
			{
				crc_temp = CRC16(modbus_buffer, i_max-2); // считаем контрольную сумму принятого пакета
				if( ( ((crc_temp>>8)&0x00FF) == modbus_buffer[i_max-1] ) && ( (crc_temp&0x00FF) == modbus_buffer[i_max-2]) ) // проверяем контрольную сумму принятого пакета
				{
					switch(modbus_buffer[1]) // проверяем тип поступившей команды MODBUS
					{
						case(0x03): // чтение регистра

							modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для чтения
							modbus_size = (((((uint16_t)modbus_buffer[4])<<8)&0xFF00)|(((uint16_t)modbus_buffer[5])&0xFF)); //  считаем количество регистров для чтения
							if( modbus_address == 4158 ) //
							{
								osMutexWait(UartMutexHandle, osWaitForever);
								AT_CSQ(&level);
								osMutexRelease(UartMutexHandle);
								fm25v02_write(2*modbus_address, 0);
								fm25v02_write(2*modbus_address+1, level);
							}
							else
							{
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
							}

						break;

						case(0x06): // запись одного регистра

							if( (modbus_address>=0x1090) && (modbus_address<=0x10FF) )
							{
								modbus_address = (((((uint16_t)modbus_buffer[2])<<8)&0xFF00)|(((uint16_t)modbus_buffer[3])&0xFF)); // считаем адрес регистра для записи

								fm25v02_fast_write(2*modbus_address, &modbus_buffer[4], 2);

								osMutexWait(UartMutexHandle, osWaitForever);
								AT_QISEND(&modbus_buffer[0], 8);
								osMutexRelease(UartMutexHandle);
							}

						break;

						case(0x10): // запись нескольких регистров

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


// Блок расчета CRC16
const unsigned char MB_Slave_aucCRCHi[] =
{
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

const unsigned char MB_Slave_aucCRCLo[] =
{
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
  0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
  0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
  0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
  0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
  0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
  0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
  0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
  0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
  0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
  0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
  0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
  0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
  0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
  0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

//--------------------- Процедура расчета CRC16 -------------------------------------//
 unsigned int CRC16( unsigned char * pucFrame, unsigned int usLen ){
  unsigned char MB_Slave_ucCRCHi = 0xFF;
  unsigned char MB_Slave_ucCRCLo = 0xFF;
  int iIndex;
  //
  while( usLen-- ){
    iIndex = MB_Slave_ucCRCLo ^ *( pucFrame++ );
    MB_Slave_ucCRCLo = MB_Slave_ucCRCHi ^ MB_Slave_aucCRCHi[iIndex];
    MB_Slave_ucCRCHi = MB_Slave_aucCRCLo[iIndex];
  }
  return MB_Slave_ucCRCHi << 8 | MB_Slave_ucCRCLo;
}//------------------------------------------------------------------------------------//
