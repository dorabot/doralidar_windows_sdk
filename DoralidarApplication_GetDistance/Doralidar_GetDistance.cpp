// RadarApplicationSample.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "DoralidarDriver.h"
#include <iostream>
using namespace std;
using namespace dl_ls;
int _tmain(int argc, _TCHAR* argv[])
{
	dl_ls_driver lsDriver;
	lsDriver.open("192.168.0.10",2112);						
	lsDriver.set_scanning_parameters(0,810);					
	lsDriver.start_measurement();
	int iLoopNumber =1000;
	vector<int> vcData;
	int iFirstFrame;
	int iLastFrame;
	for(int i=0;i<iLoopNumber;i++) {
		vcData.clear();
		if(lsDriver.get_distance(vcData)) {
			if(i==0) {
				iFirstFrame = lsDriver.iCurFrameNumber;
			}
			if(i==iLoopNumber-1) {
				iLastFrame = lsDriver.iCurFrameNumber;
			}
			cout<<"Current frame id："<<lsDriver.iCurFrameNumber<<"	";
			cout<<"data size："<<vcData.size()<<"	";
			cout<<"first distance data："<<vcData[0]<<"mm ---- last distance data:"<<vcData[vcData.size()-1]<<"mm"<<endl;
		}
	}
	cout<<"first frame："<<iFirstFrame<<" last frame："<<iLastFrame<<"total read frame "<<(iLastFrame-iFirstFrame+1);
	return 0;
}

