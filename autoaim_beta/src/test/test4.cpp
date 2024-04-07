#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <mv/CameraApi.h>
#include <mv_class.hpp>
std::string window_name = "config";
void visualize_isp_config_by_isp_params()
{
    
    cv::namedWindow(window_name);
    cv::createTrackbar("exp",window_name,0,10000,NULL);
    cv::createTrackbar("r_gain",window_name,0,300,NULL);
    cv::createTrackbar("g_gain",window_name,0,300,NULL);
    cv::createTrackbar("b_gain",window_name,0,300,NULL);
    cv::createTrackbar("gain",window_name,0,192,NULL);

}

Mindvision_Camera_Params update_camera_isp_params()
{
    Mindvision_Camera_Params prs;
    prs.exposure_time_us = cv::getTrackbarPos("exp",window_name);
    prs.r_gain = cv::getTrackbarPos("r_gain",window_name);
    prs.g_gain = cv::getTrackbarPos("g_gain",window_name);
    prs.b_gain = cv::getTrackbarPos("b_gain",window_name);
    prs.analog_gain = cv::getTrackbarPos("gain",window_name);
    return prs;
}


void set_isp_params(Mindvision_Camera_Params prs, CameraHandle hcamera)
{
    CameraSetExposureTime(hcamera, (double)prs.exposure_time_us);
    CameraSetGain(hcamera,(int)prs.r_gain,(int)prs.g_gain,(int)prs.b_gain);
    CameraSetAnalogGain(hcamera, (int)prs.analog_gain);

}