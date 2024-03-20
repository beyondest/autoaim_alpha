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

    int camera_roi_resolution_wid;
    int camera_roi_resolution_hei;
    int camera_fps_rank;
    std::string yolov5_isp_params_name;
    std::string blue_isp_params_name;
    std::string red_isp_params_name;

#ifdef _WIN32
    std::string platform = "windows";
#elif __linux__
    std::string platform = "linux";
#endif

    double exposure_time_us;
    int gamma;
    int r_gain;
    int g_gain;
    int b_gain;
    int analog_gain;
    float analog_gain_x;
    int sharpness;
    int saturation;
    int contrast;
    int grab_resolution_wid;
    int grab_resolution_hei;

    
    Mindvision_Camera_Params(): 
                                camera_roi_resolution_wid(1280),
                                camera_roi_resolution_hei(1024),
                                camera_fps_rank(2),
                                yolov5_isp_params_name("yolov5_isp_params.yaml"),
                                blue_isp_params_name("blue_isp_params.yaml"),
                                red_isp_params_name("red_isp_params.yaml"),
                                exposure_time_us(30000),
                                gamma(100),
                                r_gain(100),
                                g_gain(100),
                                b_gain(100),
                                analog_gain(100),
                                analog_gain_x(100),
                                sharpness(100),
                                saturation(100),
                                contrast(100),
                                grab_resolution_wid(640),
                                grab_resolution_hei(640)
                                
    {};
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
    int trigger_mode;
    unsigned int output_format;
    unsigned int wTimes_ms;
    CameraHandle hcamera = -1;
    Mindvision_Camera(const Mode camera_mode,
                       const std::string& camera_config_folder,
                       const unsigned int& output_format,
                       const int& trigger_mode,
                       unsigned int wTimes_ms,
                       const bool if_use_last_params,
                       const std::string& armor_color,
                       const bool if_yolov5);

    ~Mindvision_Camera();
    IF_SUCCESS open();
    IF_SUCCESS close();
    IF_SUCCESS get_img(cv::Mat& img);


};















#endif  