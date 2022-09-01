#ifndef DS18B20
#define DS18B20

#include "main.h"

#define huartx    &huart1
#define OW_RESET  0xF0
#define OW_W1     0xFF
#define OW_W0     0x00
#define OW_R1     0xFF

typedef struct{
uint32_t IbuttonROM_High;
uint32_t IbuttonROM_Low;
} IbuttonROM_Struct;

uint8_t write_0 (void);
uint8_t write_1 (void);
HAL_StatusTypeDef ibutton_reset (void);
HAL_StatusTypeDef ibutton_read_rom (IbuttonROM_Struct* id);
uint8_t onewire_crc_update(uint8_t crc, uint8_t b);
unsigned char calc_crc( unsigned char *mas, unsigned char Len );
HAL_StatusTypeDef ibutton_search_rom(IbuttonROM_Struct* id);
HAL_StatusTypeDef ibutton_write_rom(IbuttonROM_Struct* id);
HAL_StatusTypeDef ibutton_delete_rom(IbuttonROM_Struct* id);






#endif
