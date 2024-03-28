#ifndef GG_CLASS_H
#define GG_CLASS_H
#include "GxIAPI.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "DxImageProc.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include "basic.hpp"

void CHECK_GG_STATUS(GX_STATUS status);

class GG_Camera
{

public:
    Mode camera_mode;
    const unsigned int output_format = DX_ORDER_BGR;
    const uint32_t wTimes_ms = 1000;
    GX_DEV_HANDLE hDevice = NULL;
    PGX_FRAME_BUFFER pFrameBuffer = NULL;
    const int nWidth = 1920;
    const int nHeight = 1200;
    GG_Camera(const Mode camera_mode, double exposure_time);

    ~GG_Camera();
    IF_SUCCESS open();
    IF_SUCCESS close();
    IF_SUCCESS get_img(cv::Mat &img);
};



#endif