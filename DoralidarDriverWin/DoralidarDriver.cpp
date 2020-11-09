// WRRadarDriverW.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DoralidarDriver.h"
#include <iostream>
#include <vector>
#include "dl_ls_constant.h"
using namespace std;
using namespace dl_ls;
dl_ls_driver::dl_ls_driver(void)
{
	first_step_index=0;
	last_step_index=2160;
}


dl_ls_driver::~dl_ls_driver(void)
{
}
bool dl_ls_driver::start_measurement()
{
	int iSendNumber;
	iSendNumber = sendto(sockClient,CMD_START_SCAN,SIZE_OF_CMD,0,(sockaddr *)&addrLsService,sock_addr_size);
	if(iSendNumber != SIZE_OF_CMD)
	{
		is_actived =false;
		return false;
	}
	is_actived = true;
	return true;
}

bool dl_ls_driver::get_distance(vector<int>& iDistance)
{
	if(!is_actived)
		return false;
	unsigned int iFrameTotalIndex=0;
	unsigned int rawDataLen=0;			//      帧中边界或者回波强度数据长度
	unsigned int totalDataLen =0;		//		帧中存储的数据长度
	unsigned int subPkgNum=0;
	unsigned int subPkgIndex=0;
	unsigned char checkSum = 0; 
	bool bBreak=false;
	while(!bBreak)
	{
		if(recvfrom(sockClient,(char *)mRecvBuf,RECV_BUFFER_SIZE,0,(sockaddr *)&addrLsService,&sock_addr_size)<=0)
		{
			return false;
		}
		iFrameTotalIndex = *(mRecvBuf + CMD_FRAME_HEADER_TOTAL_INDEX_H)<<8|(*(mRecvBuf + CMD_FRAME_HEADER_TOTAL_INDEX_L)); 
		subPkgNum = *(mRecvBuf + CMD_FRAME_HEADER_SUB_PKG_NUM);
		subPkgIndex = *(mRecvBuf + CMD_FRAME_HEADER_SUB_INDEX);
		totalDataLen =*(mRecvBuf + CMD_FRAME_HEADER_LENGTH_H) << 8 | *(mRecvBuf + CMD_FRAME_HEADER_LENGTH_L);
		rawDataLen=totalDataLen-(CMD_FRAME_DATA_START-CMD_FRAME_HEADER_CHECK_SUM);
		checkSum=0;
		for(int i =0;i<totalDataLen-1;i++)
		{
		    checkSum += mRecvBuf[CMD_FRAME_HEADER_TYPE+i];
		}
		if(checkSum !=mRecvBuf[CMD_FRAME_HEADER_CHECK_SUM])
		{
			continue;
		}
		mDataSaveSt[subPkgIndex].iTotalFrameIndex = iFrameTotalIndex;
		mDataSaveSt[subPkgIndex].iRawDataLens=rawDataLen ;
		mDataSaveSt[subPkgIndex].subPkgNum = subPkgNum;
		mDataSaveSt[subPkgIndex].subPkgIndex = subPkgIndex;
		memcpy(mDataSaveSt[subPkgIndex].sens_data,mRecvBuf+CMD_FRAME_DATA_START,rawDataLen);
		bool checkResult = true;
		for(int i = 0; i < subPkgNum - 1; i++)
        {
			if(mDataSaveSt[i].iTotalFrameIndex != mDataSaveSt[i + 1].iTotalFrameIndex || mDataSaveSt[i].subPkgIndex != mDataSaveSt[i + 1].subPkgIndex - 1)
			{
				checkResult = false;
				break;
			}
        }
		if(!checkResult)
		{
			continue;
		}
		int iDataPointCount=0;
		vector<int> vcData;
		int iCurDataNumber;
		int iDistFramCount;
		for(int i=0;i<subPkgNum;i++)					//数据点总个数求解
		{
			iDataPointCount+=mDataSaveSt[i].iRawDataLens/2;
		}
		if(iDataPointCount==272+64||iDataPointCount==542+64||iDataPointCount==1082+64||iDataPointCount==1622+64||iDataPointCount==2162+64||iDataPointCount==4322+64)		//带有回波强度、附加信息
		{
			int iAng = (mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION])<<24|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+1]<<16)|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+2])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+3]);
			iPointsNumber=(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS+1]);
			iRunStatus=mDataSaveSt[0].sens_data[EXTRA_INFO_RUNSTATUS];
			bCarryIntensity=mDataSaveSt[0].sens_data[EXTRA_INFO_INTENSITY]==0?false:true;
			bHighPrecision=mDataSaveSt[0].sens_data[EXTRA_INFO_PRECISION]==0?false:true;
			memcpy(mDataSaveSt[0].sens_data,mDataSaveSt[0].sens_data+EXTRA_INFO_DATA_START,mDataSaveSt[0].iRawDataLens-128);
			mDataSaveSt[0].iRawDataLens-=128;
			switch(iAng)
			{
				case 1250:
					dAngResolution = 0.125;
					break;
				case 2500:
					dAngResolution = 0.25;
					break;
				case 3300:
					dAngResolution = 1.0/3.0;
					break;
				case 5000:
					dAngResolution = 0.5;
					break;
				case 10000:
					dAngResolution = 1;
					break;
				case 20000:
					dAngResolution = 2;
					break;
				default :
					for(int i =0;i<iPointsNumber;i++)
					{
						iDistance.push_back(1);
					}
					return false;
			}
			if(iRunStatus==1)										//设备故障
			{
				for(int i =0;i<iPointsNumber;i++)
				{
					iDistance.push_back(1);
				}
				return false;
			}
		}
		else if(iDataPointCount==272||iDataPointCount==542||iDataPointCount==1082||iDataPointCount==1622||iDataPointCount==2162||iDataPointCount==4322)	//带有回波强度、不带附加信息
		{
			switch(iDataPointCount)
			{
				case 272:
					dAngResolution = 2;
					iPointsNumber=136;
					break;
				case 542:
					dAngResolution = 1;
					iPointsNumber=271;
					break;
				case 1082:
					dAngResolution = 0.5;
					iPointsNumber=541;
					break;
				case 1622:
					dAngResolution = 1.0/3.0;
					iPointsNumber=811;
					break;
				case 2162:
					dAngResolution = 0.25;
					iPointsNumber=1081;
					break;
				case 4322:
					dAngResolution = 0.125;
					iPointsNumber=2161;
					break;
				default :
					break;
			}
			iRunStatus=0;
			bCarryIntensity=true;
			bHighPrecision=false;
		}
		else if(iDataPointCount==136+64||iDataPointCount==271+64||iDataPointCount==541+64||iDataPointCount==811+64||iDataPointCount==1081+64||iDataPointCount==2161+64)	//不带回波强度、带有附加信息
		{
			int iAng = (mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION])<<24|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+1]<<16)|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+2])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+3]);
			iPointsNumber=(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS+1]);
			iRunStatus=mDataSaveSt[0].sens_data[EXTRA_INFO_RUNSTATUS];
			bCarryIntensity=mDataSaveSt[0].sens_data[EXTRA_INFO_INTENSITY]==0?false:true;
			bHighPrecision=mDataSaveSt[0].sens_data[EXTRA_INFO_PRECISION]==0?false:true;
			memcpy(mDataSaveSt[0].sens_data,mDataSaveSt[0].sens_data+EXTRA_INFO_DATA_START,mDataSaveSt[0].iRawDataLens-128);
			mDataSaveSt[0].iRawDataLens-=128;
			switch(iAng)
			{
				case 1250:
					dAngResolution = 0.125;
					break;
				case 2500:
					dAngResolution = 0.25;
					break;
				case 3300:
					dAngResolution = 1.0/3.0;
					break;
				case 5000:
					dAngResolution = 0.5;
					break;
				case 10000:
					dAngResolution = 1;
					break;
				case 20000:
					dAngResolution = 2;
					break;
				default :
					for(int i =0;i<iPointsNumber;i++)
					{
						iDistance.push_back(1);
					}
					return false;
			}
			if(iRunStatus==1)										//设备故障
			{
				for(int i =0;i<iPointsNumber;i++)
				{
					iDistance.push_back(1);
				}
				return false;
			}
		}
		else if(iDataPointCount==136||iDataPointCount==271||iDataPointCount==541||iDataPointCount==811||iDataPointCount==1081||iDataPointCount==2161)	//不带回波强度、不带附加信息
		{
			switch(iDataPointCount)
			{
				case 136:
					dAngResolution = 2;
					iPointsNumber=136;
					break;
				case 271:
					dAngResolution = 1;
					iPointsNumber=271;
					break;
				case 541:
					dAngResolution = 0.5;
					iPointsNumber=541;
					break;
				case 811:
					dAngResolution = 1.0/3.0;
					iPointsNumber=811;
					break;
				case 1081:
					dAngResolution = 0.25;
					iPointsNumber=1081;
					break;
				case 2161:
					dAngResolution = 0.125;
					iPointsNumber=2161;
					break;
				default :
					break;
			}
			iRunStatus=0;
			bCarryIntensity=false;
			bHighPrecision=false;
		}
		else		//其它情况
		{
			for(int i =0;i<iPointsNumber;i++)
			{
				iDistance.push_back(1);
			}
			return false;
		}
		if(bCarryIntensity)
		{
			iDistFramCount = subPkgNum/2;
		}
		else
		{
			iDistFramCount= subPkgNum;
		}
		for(int i =0;i<iDistFramCount;i++)
		{
			iCurDataNumber = mDataSaveSt[i].iRawDataLens/2;
			for(int j =0;j<iCurDataNumber;j++)
			{
				vcData.push_back(mDataSaveSt[i].sens_data[2*j]*256+mDataSaveSt[i].sens_data[2*j+1]);
			}
		}
		iCurFrameNumber = iFrameTotalIndex;
		for(int i= 0;i<iPointsNumber;i++)
		{	
			if(i>=first_step_index&&i<=last_step_index)
			{
				iDistance.push_back(vcData[i]);
			}
			else if(i>last_step_index)
			{
				break;
			}
		}
		bBreak = true;
	}
	return true;
}
bool dl_ls_driver::get_distance_intensity(vector<int>& iDistance,vector<int>& iIntensity)
{
	if(!is_actived)
		return false;
	unsigned int iFrameTotalIndex=0;
	unsigned int rawDataLen=0;			//      帧中边界或者回波强度数据长度
	unsigned int totalDataLen =0;		//		帧中存储的数据长度
	unsigned int subPkgNum;
	unsigned int subPkgIndex;
	unsigned char checkSum = 0; 
	bool bBreak=false;
	while(!bBreak)
	{
		if(recvfrom(sockClient,(char *)mRecvBuf,RECV_BUFFER_SIZE,0,(sockaddr *)&addrLsService,&sock_addr_size)<=0)
		{
			return false;
		}
		iFrameTotalIndex = *(mRecvBuf + CMD_FRAME_HEADER_TOTAL_INDEX_H)<<8|(*(mRecvBuf + CMD_FRAME_HEADER_TOTAL_INDEX_L)); 
		subPkgNum = *(mRecvBuf + CMD_FRAME_HEADER_SUB_PKG_NUM);
		subPkgIndex = *(mRecvBuf + CMD_FRAME_HEADER_SUB_INDEX);
		totalDataLen =*(mRecvBuf + CMD_FRAME_HEADER_LENGTH_H) << 8 | *(mRecvBuf + CMD_FRAME_HEADER_LENGTH_L);
		rawDataLen=totalDataLen-(CMD_FRAME_DATA_START-CMD_FRAME_HEADER_CHECK_SUM);
		checkSum=0;
		for(int i =0;i<totalDataLen-1;i++)
		{
		    checkSum += mRecvBuf[CMD_FRAME_HEADER_TYPE+i];
		}
		if(checkSum !=mRecvBuf[CMD_FRAME_HEADER_CHECK_SUM])
		{
			continue;
		}
		mDataSaveSt[subPkgIndex].iTotalFrameIndex = iFrameTotalIndex;
		mDataSaveSt[subPkgIndex].iRawDataLens=rawDataLen ;
		mDataSaveSt[subPkgIndex].subPkgNum = subPkgNum;
		mDataSaveSt[subPkgIndex].subPkgIndex = subPkgIndex;
		memcpy(mDataSaveSt[subPkgIndex].sens_data,mRecvBuf+CMD_FRAME_DATA_START,rawDataLen);
		bool checkResult = true;
		for(int i = 0; i < subPkgNum - 1; i++)
        {
			if(mDataSaveSt[i].iTotalFrameIndex != mDataSaveSt[i + 1].iTotalFrameIndex || mDataSaveSt[i].subPkgIndex != mDataSaveSt[i + 1].subPkgIndex - 1)
			{
				checkResult = false;
				break;
			}
        }
		if(!checkResult)
		{
			continue;
		}
		vector<int> vcDistance;
		vector<int> vcIntensity;
		int iCurDataNumber ;
		int iDistFramCount;
		int iDataPointCount=0;
		for(int i=0;i<subPkgNum;i++)					//数据点总个数求解
		{
			iDataPointCount+=mDataSaveSt[i].iRawDataLens/2;
		}
		if(iDataPointCount==272+64||iDataPointCount==542+64||iDataPointCount==1082+64||iDataPointCount==1622+64||iDataPointCount==2162+64||iDataPointCount==4322+64)		//带有回波强度、附加信息
		{
			int iAng = (mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION])<<24|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+1]<<16)|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+2])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+3]);
			iPointsNumber=(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS+1]);
			iRunStatus=mDataSaveSt[0].sens_data[EXTRA_INFO_RUNSTATUS];
			bCarryIntensity=mDataSaveSt[0].sens_data[EXTRA_INFO_INTENSITY]==0?false:true;
			bHighPrecision=mDataSaveSt[0].sens_data[EXTRA_INFO_PRECISION]==0?false:true;
			memcpy(mDataSaveSt[0].sens_data,mDataSaveSt[0].sens_data+EXTRA_INFO_DATA_START,mDataSaveSt[0].iRawDataLens-128);
			mDataSaveSt[0].iRawDataLens-=128;
			switch(iAng)
			{
				case 1250:
					dAngResolution = 0.125;
					break;
				case 2500:
					dAngResolution = 0.25;
					break;
				case 3300:
					dAngResolution = 1.0/3.0;
					break;
				case 5000:
					dAngResolution = 0.5;
					break;
				case 10000:
					dAngResolution = 1;
					break;
				case 20000:
					dAngResolution = 2;
					break;
				default :
					for(int i =0;i<iPointsNumber;i++)
					{
						iDistance.push_back(1);
						iIntensity.push_back(1);
					}
					return false;
			}
			if(iRunStatus==1)										//设备故障
			{
				for(int i =0;i<iPointsNumber;i++)
				{
					iDistance.push_back(1);
					iIntensity.push_back(1);
				}
				return false;
			}
		}
		else if(iDataPointCount==272||iDataPointCount==542||iDataPointCount==1082||iDataPointCount==1622||iDataPointCount==2162||iDataPointCount==4322)	//带有回波强度、不带附加信息
		{
			switch(iDataPointCount)
			{
				case 272:
					dAngResolution = 2;
					iPointsNumber=136;
					break;
				case 542:
					dAngResolution = 1;
					iPointsNumber=271;
					break;
				case 1082:
					dAngResolution = 0.5;
					iPointsNumber=541;
					break;
				case 1622:
					dAngResolution = 1.0/3.0;
					iPointsNumber=811;
					break;
				case 2162:
					dAngResolution = 0.25;
					iPointsNumber=1081;
					break;
				case 4322:
					dAngResolution = 0.125;
					iPointsNumber=2161;
					break;
				default :
					break;
			}
			iRunStatus=0;
			bCarryIntensity=true;
			bHighPrecision=false;
		}
		else if(iDataPointCount==136+64||iDataPointCount==271+64||iDataPointCount==541+64||iDataPointCount==811+64||iDataPointCount==1081+64||iDataPointCount==2161+64)	//不带回波强度、带有附加信息
		{
			int iAng = (mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION])<<24|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+1]<<16)|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+2])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_RESOLUTION+3]);
			iPointsNumber=(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS])<<8|(mDataSaveSt[0].sens_data[EXTRA_INFO_SAMPLE_COUNTS+1]);
			iRunStatus=mDataSaveSt[0].sens_data[EXTRA_INFO_RUNSTATUS];
			bCarryIntensity=mDataSaveSt[0].sens_data[EXTRA_INFO_INTENSITY]==0?false:true;
			bHighPrecision=mDataSaveSt[0].sens_data[EXTRA_INFO_PRECISION]==0?false:true;
			memcpy(mDataSaveSt[0].sens_data,mDataSaveSt[0].sens_data+EXTRA_INFO_DATA_START,mDataSaveSt[0].iRawDataLens-128);
			mDataSaveSt[0].iRawDataLens-=128;
			switch(iAng)
			{
				case 1250:
					dAngResolution = 0.125;
					break;
				case 2500:
					dAngResolution = 0.25;
					break;
				case 3300:
					dAngResolution = 1.0/3.0;
					break;
				case 5000:
					dAngResolution = 0.5;
					break;
				case 10000:
					dAngResolution = 1;
					break;
				case 20000:
					dAngResolution = 2;
					break;
				default :
					for(int i =0;i<iPointsNumber;i++)
					{
						iDistance.push_back(1);
						iIntensity.push_back(1);
					}
					return false;
			}
			if(iRunStatus==1)										//设备故障
			{
				for(int i =0;i<iPointsNumber;i++)
				{
					iDistance.push_back(1);
					iIntensity.push_back(1);
				}
				return false;
			}
		}
		else if(iDataPointCount==136||iDataPointCount==271||iDataPointCount==541||iDataPointCount==811||iDataPointCount==1081||iDataPointCount==2161)	//不带回波强度、不带附加信息
		{
			switch(iDataPointCount)
			{
				case 136:
					dAngResolution = 2;
					iPointsNumber=136;
					break;
				case 271:
					dAngResolution = 1;
					iPointsNumber=271;
					break;
				case 541:
					dAngResolution = 0.5;
					iPointsNumber=541;
					break;
				case 811:
					dAngResolution = 1.0/3.0;
					iPointsNumber=811;
					break;
				case 1081:
					dAngResolution = 0.25;
					iPointsNumber=1081;
					break;
				case 2161:
					dAngResolution = 0.125;
					iPointsNumber=2161;
					break;
				default :
					break;
			}
			iRunStatus=0;
			bCarryIntensity=false;
			bHighPrecision=false;
		}
		else		//其它情况
		{
			for(int i =0;i<iPointsNumber;i++)
			{
				iDistance.push_back(1);
				iIntensity.push_back(1);
			}
			return false;
		}
		if(bCarryIntensity)
		{
			iDistFramCount = subPkgNum/2;
		}
		else
		{
			iDistFramCount= subPkgNum;
		}
		for(int i =0;i<iDistFramCount;i++)
		{
			iCurDataNumber = mDataSaveSt[i].iRawDataLens/2;
			for(int j =0;j<iCurDataNumber;j++)
			{
				vcDistance.push_back(mDataSaveSt[i].sens_data[2*j]*256+mDataSaveSt[i].sens_data[2*j+1]);
			}
		}
		for(int i=iDistFramCount;i<subPkgNum;i++)
		{
			iCurDataNumber = mDataSaveSt[i].iRawDataLens/2;
			for(int j =0;j<iCurDataNumber;j++)
			{
				vcIntensity.push_back(mDataSaveSt[i].sens_data[2*j]*256+mDataSaveSt[i].sens_data[2*j+1]);
			}
		}
		for(int i= 0;i<iPointsNumber;i++)
		{
			if(i>=first_step_index&&i<=last_step_index)
			{
				iDistance.push_back(vcDistance[i]);
				iIntensity.push_back(vcIntensity[i]>5000?(vcIntensity[i]>55000?600:200+(vcIntensity[i]-5000)/1200):vcIntensity[i]/25);
			}
			else if(i>last_step_index)
			{
				break;
			}
		}
		iCurFrameNumber = mDataSaveSt[0].iTotalFrameIndex;
		bBreak = true;
	}
	return true;
}
bool dl_ls_driver::open()
{
	static int is_initialized = 0;
    WORD wVersionRequested = 0x0202;
    WSADATA WSAData;
    int err;
	u_long flag;
    if (!is_initialized) {
        err = WSAStartup(wVersionRequested, &WSAData);
        if (err != 0) {
            return false;
        }
        is_initialized = 1;
    }
	if(LOBYTE(wVersionRequested)!=2||LOBYTE(wVersionRequested)!=2)			//检查Socket库版本是否为2.2
	{
		WSACleanup();
		return false;
	}
	//创建客户端套接字
	sockClient = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(INVALID_SOCKET==sockClient)
		return false;
	sock_addr_size = sizeof (struct sockaddr_in);
	memset(&addrLsService,0,sock_addr_size);
	addrLsService.sin_family = AF_INET;
	addrLsService.sin_port = htons(2112);
	unsigned long ip = inet_addr("192.168.0.10");
	if(ip == INADDR_NONE|| ip ==INADDR_ANY)
		return false;
	addrLsService.sin_addr.S_un.S_addr = ip;
	int nNetTimeout=5000;
	setsockopt(sockClient,SOL_SOCKET,SO_RCVTIMEO,(const char*)&nNetTimeout,sizeof(int));
	return true;
}
bool dl_ls_driver::open(const char* device_ip, long portNum)
{
	static int is_initialized = 0;
    WORD wVersionRequested = 0x0202;
    WSADATA WSAData;
    int err;
	//u_long flag;
    if (!is_initialized) {
        err = WSAStartup(wVersionRequested, &WSAData);
        if (err != 0) {
            return false;
        }
        is_initialized = 1;
    }
	if(LOBYTE(wVersionRequested)!=2||LOBYTE(wVersionRequested)!=2)			//检查Socket库版本是否为2.2
	{
		WSACleanup();
		return false;
	}
	//创建客户端套接字
	sockClient = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(INVALID_SOCKET==sockClient)
		return false;
	sock_addr_size = sizeof (struct sockaddr_in);
	memset(&addrLsService,0,sock_addr_size);
	addrLsService.sin_family = AF_INET;
	addrLsService.sin_port = htons(portNum);
	addrLsService.sin_addr.S_un.S_addr = inet_addr(device_ip);
	int nNetTimeout=5000;
	setsockopt(sockClient,SOL_SOCKET,SO_RCVTIMEO,(const char*)&nNetTimeout,sizeof(int));
	return true;
}
bool dl_ls_driver::set_scanning_parameters(int iFirstStepIndex,int iLastStepIndex)
{
	if(iFirstStepIndex<0||iLastStepIndex>2160 ||iFirstStepIndex>iLastStepIndex)
		return false;
	else
	{
		first_step_index= iFirstStepIndex;
		last_step_index = iLastStepIndex;
		return true;
	}
}
double dl_ls_driver::get_angular_resolution()
{
	return dAngResolution ;
}
int dl_ls_driver::get_run_status()
{
	return iRunStatus;
}
bool dl_ls_driver::is_high_precision()
{
	return bHighPrecision;
}
int dl_ls_driver::get_point_count()
{
	return iPointsNumber;
}

