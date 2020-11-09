#pragma once
#include <WinSock2.h>
#include<vector>
#include "dl_ls_constant.h"
using namespace std;
namespace dl_ls
{
	class dl_ls_driver
	{
		struct DataSaveSt
		{
			unsigned int   iTotalFrameIndex;
			unsigned char  subPkgNum;
			unsigned char  subPkgIndex;
			unsigned int   iRawDataLens;
			unsigned char  sens_data[CMD_FRAME_MAX_LEN];
		};
	public:
		dl_ls_driver(void);
		~dl_ls_driver(void);
		bool open();
		bool open(const char* device_name, long baudrate);
		bool set_scanning_parameters(int iFirstStepIndex,int iLastStepIndex);
		bool start_measurement();
		bool get_distance(vector<int>& iDistance);
		bool get_distance_intensity(vector<int>& iDistance,vector<int>& iIntensity);
		int iCurFrameNumber;
		int get_point_count();
		double get_angular_resolution();
		int get_run_status();
		bool is_high_precision();
	private:
		SOCKET sockClient;
		sockaddr_in addrLsService;
		WSADATA WSAData;
		int sock_addr_size;
		DataSaveSt mDataSaveSt[CMD_FRAME_MAX_SUB_PKG_NUM];
		int iLastFrameNumber;
		bool is_actived;
		unsigned char mRecvBuf[RECV_BUFFER_SIZE];
		unsigned char   mStoreBuffer[RECV_BUFFER_SIZE]; 
		int first_step_index;
		int last_step_index;
		double dAngResolution;
		int iPointsNumber;
		int iRunStatus;
		bool bCarryIntensity;
		bool bHighPrecision;
	};
}


