#include "cmsis_os.h"
#include "ibutton.h"
#include "stdio.h"
#include "fm25v02.h"


extern UART_HandleTypeDef huart1;
extern osMutexId Fm25v02MutexHandle;

//uint32_t ibutton_base[100] = {
//0x1d000001, 0x86075f01,
//0x92000a04, 0x5ad95901
//};

unsigned char calc_crc( unsigned char *mas, unsigned char Len )
{
  unsigned char i,dat,crc,fb,st_byt;
  st_byt=0; crc=0;
  do{
    dat=mas[st_byt];
    for( i=0; i<8; i++) {  // счетчик битов в байте
      fb = crc ^ dat;
      fb &= 1;
      crc >>= 1;
      dat >>= 1;
      if( fb == 1 ) crc ^= 0x8c; // полином
    }
    st_byt++;
  } while( st_byt < Len ); // счетчик байтов в массиве
  return crc;
}

uint8_t write_0 (void)
{
	uint8_t buft = 0x00;
	uint8_t bufr;
	HAL_UART_Transmit(huartx, &buft, 1, 1);
	HAL_UART_Receive(huartx, &bufr, 1, 1);
	if(bufr == 0x00)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t write_1 (void)
{
	uint8_t buft = 0xFF;
	uint8_t bufr;
	HAL_UART_Transmit(huartx, &buft, 1, 1);
	HAL_UART_Receive(huartx, &bufr, 1, 1);
	if(bufr == 0xFF)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//Функция инициализации ibutton таблетки на шине 1-wire
HAL_StatusTypeDef ibutton_reset (void)
{
	uint8_t bufT[1];
	uint8_t bufR[1];
	bufT[0]=OW_RESET;
	huart1.Init.BaudRate = 9600;
	HAL_UART_Init(huartx);
	if( HAL_UART_Transmit(huartx, bufT, 1, 1) != HAL_OK ) {}
	if( HAL_UART_Receive(huartx, bufR, 1, 1) != HAL_OK ) {}
	huart1.Init.BaudRate = 115200;
	if ( HAL_UART_Init(huartx) != HAL_OK ) {}
	//HAL_Delay(1);// Добавил задержку для нормальной инициализации скорости UART
	if(bufR[0]==OW_RESET)
	{
		return HAL_ERROR;
	}
	else
	{
		return HAL_OK;
	}
}
// Функция чтения ID ROM устройства ibutton таблетки
// Возвращает значение типа IbuttonROM_Struct структуры
HAL_StatusTypeDef ibutton_read_rom (IbuttonROM_Struct* id)
{
	uint8_t family= 0x00;// Семейство микроконтроллера
	uint8_t id1= 0x00;//1-й байт данных
	uint8_t id2= 0x00;//2-й байт данных
	uint8_t id3= 0x00;//3-й байт данных
	uint8_t id4= 0x00;//4-й байт данных
	uint8_t id5= 0x00;//5-й байт данных
	uint8_t id6= 0x00;//6-й байт данных
	uint8_t crc_ow= 0x00; // Принятая контрольная сумма
	uint8_t bufft = 0xFF;// байт для отправки
	uint8_t buffr=0x00;// байт для приема
	uint8_t i;
	unsigned char buffer_crc[7];// буффер для рассчета контрольной суммы
	uint8_t buf[8]={0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};// буффер со значением 0х33
	if( HAL_UART_Transmit(huartx, buf, 8, 1) != HAL_OK ) {printf("error reset\n");}// отправляем команду чтения "0х33"
	//if( HAL_UART_Receive(huartx, buf, 8, 1) != HAL_OK ) {printf("error reset\n");}
	for(i=0; i<8; i++) //Принимаем данные устройства по 1-wire
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) != HAL_OK ) {}
		if(buffr == 0xFF)
		{
			family = family|(0x01<<i);
		}
	}
	for(i=0; i<8; i++)
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) != HAL_OK ) {}
		if(buffr == 0xFF)
		{
			id1 = id1|(0x01<<i);
		}
	}
	for(i=0; i<8; i++)
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) != HAL_OK ) {}
		if(buffr == 0xFF)
		{
			id2 = id2|(0x01<<i);
		}
	}
	for(i=0; i<8; i++)
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) != HAL_OK ) {}
		if(buffr == 0xFF)
		{
			id3 = id3|(0x01<<i);
		}
	}
	for(i=0; i<8; i++)
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) !=HAL_OK ) {}
		if(buffr == 0xFF)
		{
			id4 = id4|(0x01<<i);
		}
	}
	for(i=0; i<8; i++)
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) != HAL_OK ) {}
		if(buffr == 0xFF)
		{
			id5 = id5|(0x01<<i);
		}
	}
	for(i=0; i<8; i++)
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) != HAL_OK ) {}
		if(buffr == 0xFF)
		{
			id6 = id6|(0x01<<i);
		}
	}
	for(i=0; i<8; i++)
	{
		if( HAL_UART_Transmit(huartx, &bufft, 1, 1) != HAL_OK ) {}
		if( HAL_UART_Receive(huartx, &buffr, 1, 1) != HAL_OK ) {}
		if(buffr == 0xFF)
		{
			crc_ow = crc_ow|(0x01<<i);
		}
	}

	buffer_crc[0] = family; // заполняем буффер принятыми данными
	buffer_crc[1] = id1;
	buffer_crc[2] = id2;
	buffer_crc[3] = id3;
	buffer_crc[4] = id4;
	buffer_crc[5] = id5;
	buffer_crc[6] = id6;

	if( calc_crc(&buffer_crc[0], 7) == crc_ow)// Рассчитывавем контрольную сумму принятых данных и проверяем ее на соответствие с принятой "crc_ow"
		{
			id->IbuttonROM_High= ((uint32_t)id4)|(((uint32_t)id5)<<8)|(((uint32_t)id6)<<16)|(((uint32_t)crc_ow)<<24);// Заполняем указанную в параметре структуру
			id->IbuttonROM_Low= ((uint32_t)family)|(((uint32_t)id1)<<8)|(((uint32_t)id2)<<16)|(((uint32_t)id3)<<24);
			return HAL_OK;
		}
	else
		{
			return HAL_ERROR;
		}
}

// Функция поиска ibutton в FRAM памяти
// параметр id это указатель к IbuttonROM_Struct структуре, которая содержит прочитанный ROM таблетки
// возращает значение типа HAL_StatusTypeDef. Может принимать значения: HAL_OK; HAL_ERROR
// HAL_OK - ROM найден в памяти FRAM
// HAL_ERROR - ROM не найден в памяти FRAM
HAL_StatusTypeDef ibutton_search_rom(IbuttonROM_Struct* id)
{
	//uint8_t temp_ibutton_state;
	uint8_t id_fram[16];
	uint32_t id_temp_rom_high=0;
	uint32_t id_temp_rom_low=0;

	for(uint16_t add=0x1310; add<=0x1F80; add=add+16)
	{
		//fm25v02_read(add, &temp_ibutton_state);
		//if(temp_ibutton_state == 0x01) // Проверяем есть ли запись в памяти
		//{
			osMutexWait(Fm25v02MutexHandle, osWaitForever); // берем мьютекс для работы с чтением/записью FRAM памяти.
			fm25v02_fast_read(2*add, &id_fram[0], 16);
			osMutexRelease(Fm25v02MutexHandle); // отдаем мьютекс для работы с чтением/записью FRAM памяти.
			id_temp_rom_high = (((uint32_t)id_fram[15])<<24)|(((uint32_t)id_fram[13])<<16)|(((uint32_t)id_fram[11])<<8)|((uint32_t)id_fram[9]);
			id_temp_rom_low = (((uint32_t)id_fram[7])<<24)|(((uint32_t)id_fram[5])<<16)|(((uint32_t)id_fram[3])<<8)|((uint32_t)id_fram[1]);
			//id_temp_rom_high = (((uint32_t)id_fram[0])<<24)|(((uint32_t)id_fram[1])<<16)|(((uint32_t)id_fram[2])<<8)|((uint32_t)id_fram[3]);
			//id_temp_rom_low = (((uint32_t)id_fram[4])<<24)|(((uint32_t)id_fram[5])<<16)|(((uint32_t)id_fram[6])<<8)|((uint32_t)id_fram[7]);
			if( (id_temp_rom_high == id->IbuttonROM_High) && (id_temp_rom_low == id->IbuttonROM_Low) )
			{
				return HAL_OK;
			}
			//else
			//{
				//return HAL_ERROR;
			//}
		//}
	}
	return HAL_ERROR;
	/*
	uint8_t temp_ibutton_state;
	uint8_t id_fram[8];
	uint32_t id_temp_rom_high=0;
	uint32_t id_temp_rom_low=0;

	for(uint16_t add=0x0001; add<0x37C; add=add+9)
	{
		fm25v02_read(add, &temp_ibutton_state);
		if(temp_ibutton_state == 0x01) // Проверяем есть ли запись в памяти
		{
			fm25v02_fast_read(add+1, &id_fram[0], 8);
			id_temp_rom_high = (((uint32_t)id_fram[0])<<24)|(((uint32_t)id_fram[1])<<16)|(((uint32_t)id_fram[2])<<8)|((uint32_t)id_fram[3]);
			id_temp_rom_low = (((uint32_t)id_fram[4])<<24)|(((uint32_t)id_fram[5])<<16)|(((uint32_t)id_fram[6])<<8)|((uint32_t)id_fram[7]);
			if(id_temp_rom_high == id->IbuttonROM_High)
			{
				if(id_temp_rom_low == id->IbuttonROM_Low)
				{
					return HAL_OK;
				}
			}
		}
	}
	return HAL_ERROR;
	*/
}

HAL_StatusTypeDef ibutton_write_rom(IbuttonROM_Struct* id)
{
	uint8_t temp_data;
	uint8_t temp_buf[8];

	temp_buf[0] = (uint8_t)(id->IbuttonROM_High>>24);
	temp_buf[1] = (uint8_t)(id->IbuttonROM_High>>16);
	temp_buf[2] = (uint8_t)(id->IbuttonROM_High>>8);
	temp_buf[3] = (uint8_t)(id->IbuttonROM_High);

	temp_buf[4] = (uint8_t)(id->IbuttonROM_Low>>24);
	temp_buf[5] = (uint8_t)(id->IbuttonROM_Low>>16);
	temp_buf[6] = (uint8_t)(id->IbuttonROM_Low>>8);
	temp_buf[7] = (uint8_t)(id->IbuttonROM_Low);

	if( ibutton_search_rom(id) == HAL_OK )
	{
		return HAL_BUSY; // ID уже есть в памяти
	}
	else
	{
		for(uint16_t address=0x0001; address<0x37C; address=address+9)
		{
			fm25v02_read(address, &temp_data);
			if(temp_data==0x00)
			{
				fm25v02_write(address, 0x01);
				for(uint8_t i=0; i<8; i++)
				{
					fm25v02_write(address+1+i, temp_buf[i]);
				}

				return HAL_OK; // ID записан в память
			}
		}
	}
	return HAL_ERROR; // Нет места для записи ID
}

HAL_StatusTypeDef ibutton_delete_rom(IbuttonROM_Struct* id)
{
	uint8_t temp_ibutton_state;
	uint8_t id_fram[8];
	uint32_t id_temp_rom_high=0;
	uint32_t id_temp_rom_low=0;

	for(uint16_t add=0x0001; add<0x37C; add=add+9)
	{
		fm25v02_read(add, &temp_ibutton_state);
		if(temp_ibutton_state == 0x01) // Проверяем есть ли запись в памяти
		{
			fm25v02_fast_read(add+1, &id_fram[0], 8);
			id_temp_rom_high = (((uint32_t)id_fram[0])<<24)|(((uint32_t)id_fram[1])<<16)|(((uint32_t)id_fram[2])<<8)|((uint32_t)id_fram[3]);
			id_temp_rom_low = (((uint32_t)id_fram[4])<<24)|(((uint32_t)id_fram[5])<<16)|(((uint32_t)id_fram[6])<<8)|((uint32_t)id_fram[7]);
			if(id_temp_rom_high == id->IbuttonROM_High)
			{
				if(id_temp_rom_low == id->IbuttonROM_Low)
				{
					fm25v02_write(add, 0x00);
					for(uint8_t i=1; i<9; i++)
					{
						fm25v02_write(add+i, 0x00);
					}
					return HAL_OK; // ID удален из памяти
				}
			}
		}
	}
	return HAL_ERROR; // Нет ID в памяти

}
