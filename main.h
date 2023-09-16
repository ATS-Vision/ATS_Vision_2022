//
// Created by ymj on 2022/4/25.
//

#ifndef CIDP_RM_CV_MAIN_H
#define CIDP_RM_CV_MAIN_H

#endif //CIDP_RM_CV_MAIN_H
#include<opencv2/opencv.hpp>
#include<iostream>
#include<math.h>
pthread_mutex_t Globalmutex;
pthread_cond_t GlobalCondCV;
bool imageReadable = false;
GxCamera camera;
cv::Mat src = cv::Mat::zeros(640,480,CV_8UC3);
//cv::Mat src = cv::Mat::zeros(1280,1024,CV_8UC3);
void* imageUpdatingThread(void* PARAM);
void* imageUpdatingThread(void* PARAM){
    camera.initLib();

    //   open device      SN号
    //camera.openDevice("KE0200020184");
    //camera.openDevice("JK0210040012");
    //camera.openDevice("KE0200110077");
    camera.openDevice("KE0200020184");
    //Attention:   (Width-64)%2=0; (Height-64)%2=0; X%16=0; Y%2=0;
    //   ROI             Width           Height       X       Y

    camera.setRoiParam(   640,            480,        320,     272);
//    camera.setRoiParam(   1280,            1024,        0,     0);
    //   ExposureGain          autoExposure  autoGain  ExposureTime  AutoExposureMin  AutoExposureMax  Gain(<=16)  AutoGainMin  AutoGainMax  GrayValue
    camera.setExposureGainParam(false,     false,      3000,          1000,              3000,         12,         5,            16,        127);

    //   WhiteBalance             Applied?       light source type
    camera.setWhiteBalanceParam(true,    GX_AWB_LAMP_HOUSE_FLUORESCENCE );

    //   Acquisition Start!
    camera.acquisitionStart(&src);
}
