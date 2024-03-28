#ifndef MV_CLASS_H
#define MV_CLASS_H

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "CameraApi.h"
#include "CameraDefine.h"
#include "basic.hpp"


void CHECK_MV_STATUS(CameraSdkStatus status);


class Mindvision_Camera_Params : public Params
{
public:

    int camera_roi_resolution_wid = 1280;
    int camera_roi_resolution_hei = 1024;
    int camera_fps_rank = 2;
    std::string yolov5_isp_params_name = "yolov5_isp_params.yaml";
    std::string blue_isp_params_name = "blue_isp_params.yaml";
    std::string red_isp_params_name = "red_isp_params.yaml";

#ifdef _WIN32
    std::string platform = "windows";
#elif __linux__
    std::string platform = "linux";
#endif

    double exposure_time_us = 30000;
    int gamma = 100;
    int r_gain = 100;
    int g_gain = 100;
    int b_gain = 100;
    int analog_gain = 100;
    float analog_gain_x = 0;
    int sharpness = 100;
    int saturation = 100;
    int contrast = 100;
    int grab_resolution_wid = 640;
    int grab_resolution_hei = 640;

    
    Mindvision_Camera_Params(){}; 
    ~Mindvision_Camera_Params(){};
    bool load_params_from_yaml(const std::string& file_path);
    void print_show_params();
};



class Mindvision_Camera
{
private:
    Mindvision_Camera_Params params;
    IF_SUCCESS load_params_from_folder(const std::string& , const std::string& armor_color);
    IF_SUCCESS init(bool if_use_last_params);   

public:
    Mode camera_mode;
    bool if_yolov5;
    const int trigger_mode = 0; // 0: continuous, 1: software trigger, 2: hardware trigger
    const unsigned int output_format = CAMERA_MEDIA_TYPE_BGR8;
    const unsigned int wTimes_ms = 1000;
    const int nWidth = 1280;
    const int nHeight = 1024;
    CameraHandle hcamera = -1;
    Mindvision_Camera(const Mode camera_mode,
                       const std::string& camera_config_folder,
                       const bool if_use_last_params,
                       const std::string& armor_color,
                       const bool if_yolov5);

    ~Mindvision_Camera();
    IF_SUCCESS open();
    IF_SUCCESS close();
    IF_SUCCESS get_img(cv::Mat& img);


};















#endif  