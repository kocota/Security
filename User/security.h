#ifndef SECURITY
#define SECURITY


//----биты состояния байта SECURITY_STATUS_REG статус режима охраны------

#define RESERVED_0           0x00 // зарезервированно
#define ENABLED_BY_SERVER   0x01 // включена из центра
#define DISABLED_BY_SERVER  0x02 // отключена из центра
#define ENABLED_BY_IBUTTON  0x03 // включена таблеткой
#define DISABLED_BY_IBUTTON 0x04 // отключена таблеткой
#define DOOR_OPEN_ALARM     0x05 // тревога открытия двери
#define ARMING_PROCESS      0x06 // процесс постановки на охрану
#define ARMING_ERROR        0x07 // не удается поставить на охрану

//-----------------------------------------------------------------------


void ThreadSecurityTask(void const * argument);



#endif
