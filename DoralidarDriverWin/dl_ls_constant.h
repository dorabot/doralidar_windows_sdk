#ifndef SDKELI_LS_CONSTANT
#define SDKELI_LS_CONSTANT

#define RECV_BUFFER_SIZE						65536		
#define CMD_FRAME_HEADER_START					0
#define CMD_FRAME_HEADER_LENGTH_H				4
#define CMD_FRAME_HEADER_LENGTH_L				5
#define CMD_FRAME_HEADER_CHECK_SUM				6
#define CMD_FRAME_HEADER_TYPE					7
#define CMD_FRAME_HEADER_TOTAL_INDEX_H			8
#define CMD_FRAME_HEADER_TOTAL_INDEX_L			9
#define CMD_FRAME_HEADER_SUB_PKG_NUM			10  /* 包含的子包数目 */
#define CMD_FRAME_HEADER_SUB_INDEX				11
#define CMD_FRAME_DATA_START					12
#define EXTRA_INFO_DIRTY						0
#define EXTRA_INFO_TYPE							20
#define EXTRA_INFO_RESOLUTION					52
#define EXTRA_INFO_SAMPLE_COUNTS				56
#define EXTRA_INFO_SOLFT_VERSION				58
#define EXTRA_INFO_HARD_VERSION					62
#define EXTRA_INFO_PRECISION					66
#define EXTRA_INFO_INTENSITY					67
#define EXTRA_INFO_RUNSTATUS					68
#define EXTRA_INFO_DATA_START					128
#define INDEX_RANGE_MAX							65415       //65535-(20*60)
#define INDEX_RANGE_MIN							(20*60)     //60 sencod
#define CMD_FRAME_MAX_LEN						1500
#define CMD_FRAME_MAX_SUB_PKG_NUM				8
#define CMD_FRAME_MIN_SUB_PKG_NUM				2

const char CMD_START_SCAN[]					 = {0xFA, 0x5A, 0xA5, 0xAA, 0x00, 0x02, 0x01, 0x01};
const char CMD_STOP_SCAN[]					 = {0xFA, 0x5A, 0xA5, 0xAA, 0x00, 0x02, 0x02, 0x02};
const char CMD_REBOOT[]                      = {0xFA, 0x5A, 0xA5, 0xAA, 0x00, 0x02, 0x03, 0x03};
const char CMD_GET_SCAN_STATE[]              = {0xFA, 0x5A, 0xA5, 0xAA, 0x00, 0x02, 0x04, 0x04};
const char CMD_GET_SERIAL_NUMBER[]			 = {0xFA, 0x5A, 0xA5, 0xAA, 0x00, 0x02, 0x05, 0x05};
const char CMD_GET_SCAN_HEART[]				 = {0xFA, 0x5A, 0xA5, 0xAA, 0x00, 0x02, 0x06, 0x06};
#define SIZE_OF_CMD sizeof(CMD_START_SCAN)


const char STX = 0xAA;
const char ETX = 0x66;

const unsigned int  FRAME_LENGTH = 1622; //1642;    //FRAME_LENGTH = 1631;

#define SWITCH_UINT16( _Value )   ((uint16_t) (((uint16_t)( _Value ) << 8) | \
                                   ((uint16_t)( _Value ) >> 8)) )

#endif //SDKELI_LS_COMMANDS_