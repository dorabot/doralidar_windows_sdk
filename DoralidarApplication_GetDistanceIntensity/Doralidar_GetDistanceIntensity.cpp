#include "stdafx.h"
#include "DoralidarDriver.h"
#include <iostream>
using namespace std;
using namespace dl_ls;

int _tmain(int argc, _TCHAR* argv[])
{
	dl_ls_driver lsDriver;
	if(!lsDriver.open("192.168.0.10",2112)) {				//Start connection
		cout<<"fail to open lidar£¡"<<endl;					//Error open port
		return 0;
	} else {
		cout << "open lidar success!" << endl;
	}
	if(lsDriver.set_scanning_parameters(0,810)) {				//set ouput sample number range
		cout<<"set scan sample range£º0-810"<<endl;
	} else {
		cout<<"current sample range£º0-810"<<endl;
	}
	if(!lsDriver.start_measurement()) {						//start receiving laser measurement
		cout<<"Error start laser measurement£¡"<<endl;
		return 0;
	} else {
		cout<<"Laser measurement started£¡"<<endl;
	}
	int iLoopNumber =2000;											//loop number
	vector<int> vcDistance;
	vector<int> vcIntensity;
	int iFirstFrame;
	int iLastFrame;
	for(int i=0;i<iLoopNumber;i++) {
		vcDistance.clear();
		vcIntensity.clear();
		if(lsDriver.get_distance_intensity(vcDistance,vcIntensity)) {					//Read Distance and intensity
			if(i==0) {
				iFirstFrame = lsDriver.iCurFrameNumber;
			}
			if(i==iLoopNumber-1) {
				iLastFrame = lsDriver.iCurFrameNumber;
			}
			cout<<"Current Frame ID£º"<<lsDriver.iCurFrameNumber<<"	";
			cout<<"Sample number£º"<<vcDistance.size()<<"	";
			cout<<"First and last sample distance£º"<<vcDistance[0]<<"mm --"<<vcDistance[vcDistance.size()-1]
				<<"mm  First and last sample intensity£º"<<vcIntensity[0]<<"--"<<vcIntensity[vcIntensity.size()-1]<<endl;
		}
	}
	int iTotalNumber = iLastFrame>iFirstFrame?iLastFrame-iFirstFrame+1:65536+iLastFrame-iFirstFrame+1;
	cout<<"First frame£º"<<iFirstFrame<<"Last frame£º"<<iLastFrame<<"Total read frame number£º"<<iTotalNumber;
	return 0;
}

