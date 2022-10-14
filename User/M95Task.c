#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"
#include "m95.h"

extern UART_HandleTypeDef huart3;
extern osSemaphoreId TransmissionStateHandle;
extern osSemaphoreId ReceiveStateHandle;
extern osMutexId UartMutexHandle;

extern uint8_t modem_rx_data[100];

volatile uint8_t ip1 = 0; // стартовое значение ip адреса сервера
volatile uint8_t ip2 = 0; // стартовое значение ip адреса сервера
volatile uint8_t ip3 = 0; // стартовое значение ip адреса сервера
volatile uint8_t ip4 = 0;  // стартовое значение ip адреса сервера
volatile uint8_t port_high_reg = 0; // старший байт номера порта сервера
volatile uint8_t port_low_reg = 0;  // младший байт номера порта сервера
volatile uint16_t port = 0;   // номер порта сервера

uint8_t state;
uint8_t Version_H = 1;  // версия прошивки, старший байт
uint8_t Version_L = 13; // версия прошивки, младший байт
uint8_t id2[10]; // номер CCID симкарты
uint64_t id1[20];



void ThreadM95Task(void const * argument)
{

	osSemaphoreWait(TransmissionStateHandle, osWaitForever); // обнуляем семафор, при создании семафора его значение равно 1
	osSemaphoreWait(ReceiveStateHandle, osWaitForever); // обнуляем семафор, при создании семафора его значение равно 1
	HAL_UART_Receive_DMA(&huart3, &modem_rx_data[0], 1); // включаем прием от модема

	HAL_Delay(2000); // ждем
	state = AT(); // проверяем связь с модемом

	if(AT()==AT_ERROR)
	{
		m95_power_on();
		HAL_Delay(7000);
	}

	state = AT(); // проверяем связь с модемом

	//----Обнуление регистров IP адреса и порта сервера, обнуление ID устройства------
	// Для записи регистров раскоментировать строки и прошить контроллер
	/*
	uint8_t data0 = 0;
	fm25v02_fast_write(ID_HIGH_REG, &data0, 1);
	fm25v02_fast_write(ID_LOW_REG, &data0, 1);

	fm25v02_fast_write(IP_1_REG, &data0, 1);
	fm25v02_fast_write(IP_2_REG, &data0, 1);
	fm25v02_fast_write(IP_3_REG, &data0, 1);
	fm25v02_fast_write(IP_4_REG, &data0, 1);
	fm25v02_fast_write(PORT_HIGH_REG, &data0, 1);
	fm25v02_fast_write(PORT_LOW_REG, &data0, 1);
	*/
	//---------------------------------------------------------------------------------------------------------



	fm25v02_fast_write(VERSION_REG, &Version_L, 1); // записываем в память версию прошивки

	fm25v02_fast_read(IP_1_REG, &ip1, 1); // читаем значение IP адреса сервера из памяти
	fm25v02_fast_read(IP_2_REG, &ip2, 1);
	fm25v02_fast_read(IP_3_REG, &ip3, 1);
	fm25v02_fast_read(IP_4_REG, &ip4, 1);
	fm25v02_fast_read(PORT_HIGH_REG, &port_high_reg, 1); // читаем значение старшего байта порта сервера
	fm25v02_fast_read(PORT_LOW_REG, &port_low_reg, 1); // читаем занчение младшего байта порта сервера
	port = (((uint16_t)port_high_reg)<<8)|((uint16_t)port_low_reg); // вычисляем общее значение регистра порта

	if ( (ip1==0)&&(ip2==0)&&(ip3==0)&&(ip4==0)&&(port==0) ) // Если значения ip адреса сервера и его номера порта при инициализации нулевые, то выставляем их значения по умолчанию
	{
		ip1 = 195;    // значение по умолчанию
		ip2 = 208;    // значение по умолчанию
		ip3 = 163;    // значение по умолчанию
		ip4 = 67;     // значение по умолчанию
		port = 35050; // значение по умолчанию
	}

	for(;;)
	{
		osMutexWait(UartMutexHandle, osWaitForever);

		if(AT()==AT_ERROR) // два раза проверяем, есть ли ответ на команду АТ, если нет, включаем питание
		{
			if(AT()==AT_ERROR)
			{
				m95_power_on();
			}
			//HAL_Delay(10000);
		}

		osMutexRelease(UartMutexHandle);

		osMutexWait(UartMutexHandle, osWaitForever);

		switch(AT_QISTATE())
		{
			case IP_INITIAL:
				if( AT_COPS() == AT_OK )
				{

				}
				if(	AT_QCCID(&id2[0], &id1[0]) == AT_OK ) // читаем CCID сим-карты
				{
					fm25v02_fast_write(ICCID_NUMBER_REG1, &id2[0], 8); // записываем в регистры CCID сим-карты
					//fm25v02_fast_write(ICCID_NUMBER_REG1*2, &id2[0], 8); // записываем в регистры CCID сим-карты
				}

				if(AT_QIREGAPP("mts.internet.ru", "mts", "mts") == AT_OK)
				{

				}

			break;
			case IP_START:

				if(AT_QIACT()!=AT_OK)
				{

				}

			break;
			case IP_IND:

				if(AT_QIDEACT()!=AT_OK)
				{

				}

			break;
			case IP_GPRSACT:

				if( AT_QIOPEN("TCP", ip1, ip2, ip3, ip4, port) != AT_OK )
				{

				}

			break;
			case IP_CLOSE:

				if( AT_QIOPEN("TCP", ip1 , ip2, ip3, ip4, port) != AT_OK )
				{

				}

			break;
			case PDP_DEACT:

				if(AT_QIACT()!=AT_OK)
				{

				}
			break;
			case CONNECT_OK:
				// Если соединение установлено
				if( AT_COPS() == AT_OK )
				{

				}


			break;
			case AT_ERROR:
				//LED_VD4_TOGGLE();
			break;
			default:
				//LED_VD4_TOGGLE();
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

	}
}
