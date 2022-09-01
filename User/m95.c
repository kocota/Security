#include "m95.h"
#include "main.h"
#include "cmsis_os.h"
#include "string.h"
#include "gpio.h"

#include "fm25v02.h"


extern UART_HandleTypeDef huart3;
extern osThreadId M95TaskHandle;
extern osTimerId AT_TimerHandle;
extern uint8_t modem_rx_data[100];
extern uint8_t modem_rx_buffer[100];
extern uint8_t modem_rx_number;
extern uint8_t read_rx_state;

uint8_t at[3] = "AT\n";
uint8_t at_csq[7] = "AT+CSQ\n";
uint8_t at_cops[9] = "AT+COPS?\n";
//uint8_t at_qiopen[38] = "AT+QIOPEN=\"TCP\",\"195.208.163.67\",35050";
uint8_t at_qiopen[50];

uint8_t state;
uint8_t level;

char str11[4]; // максимальное число 1460
char str22[15];
char str23[15];

uint8_t n1;
uint8_t n2;



void m95_power_on(void) // функция включения питания
{
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_6, GPIO_PIN_RESET);
}

void m95_power_off(void)
{

}

void modem_rx_buffer_clear (void)
{
	for(uint8_t i=0; i<100; i++)
	{
		modem_rx_buffer[i] = 0;
	}
}

uint8_t AT (void)
{
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);
	HAL_UART_Transmit_DMA(&huart3, at, 3);
	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "OK") != NULL )
		{
			osTimerStop(AT_TimerHandle);
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
	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);
	HAL_UART_Transmit_DMA(&huart3, at_csq, 7);
	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		osThreadSuspend(M95TaskHandle);
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
			osTimerStop(AT_TimerHandle);
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
	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);
	HAL_UART_Transmit_DMA(&huart3, at_cops, 9);
	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "MTS") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			return AT_OK;
		}

	}
	return AT_ERROR;
}

uint8_t AT_QIOPEN (char* s , uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port)
{
	char str1[3];
	char str2[3];
	char str3[3];
	char str4[3];
	char str5[3];
	char str6[5];
	char str7[40];
	uint8_t n;

	sprintf(str1, "%s", s);
	sprintf(str2, "%u", ip1);
	sprintf(str3, "%u", ip2);
	sprintf(str4, "%u", ip3);
	sprintf(str5, "%u", ip4);
	sprintf(str6, "%u", port);

	n = sprintf(str7, "AT+QIOPEN=\"%s\",\"%s.%s.%s.%s\",%s\n", str1, str2, str3, str4, str5, str6);

 	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();

	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);
	HAL_UART_Transmit_DMA(&huart3, str7, n);
	osTimerStart(AT_TimerHandle, 3000);
	while(read_rx_state == ACTIVE)
	{
		osThreadSuspend(M95TaskHandle);
		if( (strstr(modem_rx_buffer, "CONNECT OK") != NULL) || (strstr(modem_rx_buffer, "ALREADY CONNECT") != NULL) )
		{
			osTimerStop(AT_TimerHandle);
			return AT_OK;
		}

	}
	return AT_ERROR;

}

uint8_t AT_QISEND (uint16_t length) // maximum length = 1460
{


	sprintf(str11, "%u", length);

	n1 = sprintf(str22, "AT+QISEND=%s\n", str11);
	n2 = sprintf(str23, "%c%c%c%c%c%c%c%c%c", (char)0x01,(char)0x03,(char)0x04,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0xFA,(char)0x33);

 	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();

	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);
	HAL_UART_Transmit_DMA(&huart3, str22, n1);
	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		osThreadSuspend(M95TaskHandle);
		if( strstr(modem_rx_buffer, ">") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			modem_rx_number = 0;
			modem_rx_buffer_clear();
			HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);
			HAL_UART_Transmit_DMA(&huart3, str23, n2);
			osTimerStart(AT_TimerHandle, 300); // маленькое время!!!!!
			while(read_rx_state == ACTIVE)
			{
				osThreadSuspend(M95TaskHandle);
				if( strstr(modem_rx_buffer, "SEND OK") != NULL )
				{
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
	char str_out[12];
	sprintf(str_out, "AT+QIFGCNT=%u", id);
	read_rx_state = ACTIVE;
	modem_rx_number = 0;
	modem_rx_buffer_clear();
	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1);
	HAL_UART_Transmit_DMA(&huart3, str_out, 12);
	osTimerStart(AT_TimerHandle, 300);
	while(read_rx_state == ACTIVE)
	{
		osThreadSuspend(M95TaskHandle);
		if(strstr(modem_rx_buffer, "MTS") != NULL )
		{
			osTimerStop(AT_TimerHandle);
			return AT_OK;
		}

	}
	return AT_ERROR;
}


void ThreadM95Task(void const * argument)
{

	if(AT() == AT_ERROR)
	{
		m95_power_on();
	}
	while(AT()==AT_ERROR){}

	for(;;)
	{

		if(AT_CSQ(&level) == AT_OK)
		{

			if(level<4 && level>=0)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
			}
			if(level<8 && level>=4)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
				LED2_ON();
			}
			if(level<12 && level>=8)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
				LED2_ON();
				LED3_ON();
			}
			if(level<16 && level>=12)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
			}
			if(level<20 && level>=16)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
				LED5_ON();
			}
			if(level<24 && level>=20)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
				LED5_ON();
				LED6_ON();
			}
			if(level<28 && level>=24)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
				LED5_ON();
				LED6_ON();
				LED7_ON();
			}
			if(level<32 && level>=28)
			{
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LED5_OFF();
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();

				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
				LED5_ON();
				LED6_ON();
				LED7_ON();
				LED8_ON();
			}
		}

		if(AT_COPS() == AT_OK)
		{

		}
		if( AT_QIOPEN("TCP",195,208,163,67,35050) == AT_OK )
		{
			LED_VD4_TOGGLE();
		}
		if (AT_QISEND(9) == AT_OK)
		{
			//HAL_Delay(3000);
			LED_VD4_TOGGLE();
		}

		HAL_Delay(50);
		//osThreadSuspend(M95TaskHandle);
	}
}
