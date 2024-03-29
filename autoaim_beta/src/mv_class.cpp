#include "mv_class.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <stdexcept>


#include <iostream>










void CHECK_MV_STATUS(CameraSdkStatus status)
{
    if (status!= CAMERA_STATUS_SUCCESS)
    {
        std::string error_msg =  CameraGetErrorString(status);
        spdlog::critical("CAMERA : {}", error_msg);
        throw std::runtime_error("CAMERA : Camera error! ");
    }
}







/**
 * @param camera_config_folder: the folder path of camera config files, including isp params, roi params, etc.
 * @param output_format: the output format of grabbed image, such as YUYV, RGB, BGR, etc.
 * @param trigger_mode: the trigger mode of camera, such as software trigger, hardware trigger, etc.
 * @param wTimes_ms: the waiting time between two grabbed images, in milliseconds.
 * @param if_use_last_params: if use the last params when open the camera.
 * @param armor_color: the color of the armor, such as blue, red, etc.
 * @param if_yolov5: if use yolov5 model to detect the armor.
 * 
*/
Mindvision_Camera::Mindvision_Camera(  const Mode camera_mode,
                                        const std::string& camera_config_folder,
                                        const bool if_use_last_params,
                                        const std::string& armor_color,
                                        const bool if_yolov5)
{
    this->camera_mode = camera_mode;
    this->if_yolov5 = if_yolov5;
    this->load_params_from_folder(camera_config_folder, armor_color);
    if(this->camera_mode == Mode::Dbg)
    {
        spdlog::debug("CAMERA : use params from folder : {}", camera_config_folder);
        this->params.print_show_params();
    }

    CameraSdkInit(0);
    this->init(if_use_last_params);


}
Mindvision_Camera::~Mindvision_Camera()
{
    this->close();
}

IF_SUCCESS Mindvision_Camera::init(bool if_use_last_params)
{
    int iCameraNums = 1;
    tSdkCameraDevInfo sCameraList[1];
    CHECK_MV_STATUS(CameraEnumerateDevice(sCameraList , &iCameraNums));
    if (iCameraNums == 0)
    {
        spdlog::critical("CAMERA : No MV Camera found!");
        throw std::runtime_error("CAMERA : No MV Camera found!");
    }
    else if (iCameraNums > 1)
    {
        SPDLOG_WARN("CAMERA : More than one camera found, only using the first one.");
    }
    
    
    CHECK_MV_STATUS(CameraInit(&sCameraList[0], -1, -1, &this->hcamera));

    
    if (!if_use_last_params)
    {   
        CHECK_MV_STATUS(CameraSetIspOutFormat(this->hcamera, this->output_format));
        CHECK_MV_STATUS(CameraSetAeState(this->hcamera , false));
        CHECK_MV_STATUS(CameraSetTriggerMode(this->hcamera, this->trigger_mode)); 
        CHECK_MV_STATUS(CameraSetFrameSpeed(this->hcamera, this->params.camera_fps_rank));

        CHECK_MV_STATUS(CameraSetExposureTime(this->hcamera, this->params.exposure_time_us));
        CHECK_MV_STATUS(CameraSetGamma(this->hcamera, this->params.gamma));
        CHECK_MV_STATUS(CameraSetGain(this->hcamera, this->params.r_gain, this->params.g_gain,this->params.b_gain));
        CHECK_MV_STATUS(CameraSetAnalogGain(this->hcamera, this->params.analog_gain));
        CHECK_MV_STATUS(CameraSetAnalogGainX(this->hcamera, this->params.analog_gain_x));
        CHECK_MV_STATUS(CameraSetSharpness(this->hcamera, this->params.sharpness));
        CHECK_MV_STATUS(CameraSetSaturation(this->hcamera, this->params.saturation));
        CHECK_MV_STATUS(CameraSetContrast(this->hcamera, this->params.contrast));

        //CHECK_MV_STATUS(CameraSetImageResolutionEx(this->hcamera,0xff,0,0,0,0,this->params.camera_roi_resolution_wid,this->params.camera_roi_resolution_hei,this->params.grab_resolution_wid,this->params.grab_resolution_hei));

    }

    spdlog::info("CAMERA : init camera id 0 success!");
    return IF_SUCCESS::SUCCESS;
}

IF_SUCCESS Mindvision_Camera::load_params_from_folder(const std::string& folder_path,const std::string& armor_color)
{   
    if (!CHECK_IF_FILE_EXIST(folder_path)) 
    {
        spdlog::critical("CAMERA : " + folder_path + " not exists");
        throw std::runtime_error("CAMERA : " + folder_path + " not exists");
    }

    std::string custom_isp_yaml_path;
    if (if_yolov5) 
    {
        custom_isp_yaml_path = folder_path +'/'+ this->params.yolov5_isp_params_name;
    } 
    else 
    {
        if (armor_color == "blue") 
        {
            custom_isp_yaml_path = folder_path + '/'+this->params.blue_isp_params_name;
        } 
        else 
        {
            custom_isp_yaml_path = folder_path +'/'+ this->params.red_isp_params_name;
        }
    }

    if (!CHECK_IF_FILE_EXIST(custom_isp_yaml_path)) 
    {
        spdlog::critical("CAMERA : " + custom_isp_yaml_path + " not exists");
        throw std::runtime_error("CAMERA : " + custom_isp_yaml_path + " not exists");
    }
    
    this->params.load_params_from_yaml(custom_isp_yaml_path);
    return IF_SUCCESS::SUCCESS;
}

IF_SUCCESS Mindvision_Camera::open()
{
    CHECK_MV_STATUS(CameraClearBuffer(this->hcamera));
    CHECK_MV_STATUS(CameraPlay(this->hcamera));
    if (this->trigger_mode == 1)
    {
        CameraSoftTrigger(this->hcamera);
    }
    if (this->camera_mode == Mode::Dbg)
    {
        spdlog::debug("CAMERA : camera start to play!");
    }
    return IF_SUCCESS::SUCCESS;
}

IF_SUCCESS Mindvision_Camera::close()
{
    CameraUnInit(this->hcamera);
    if (this->camera_mode == Mode::Dbg)
    {
        spdlog::info("CAMERA : camera uninit!");
    }
    return IF_SUCCESS::SUCCESS;
}


/**
 *@brief get image from camera
 *@param img : cv::Mat to store image, should be initialized before calling this function, and its size should be equal to the resolution of the camera
*/
IF_SUCCESS Mindvision_Camera::get_img(cv::Mat& img)
{
    /*
    if (this->hcamera == -1)
    {
        if (this->camera_mode == Mode::Dbg)
        {
            spdlog::debug("CAMERA : camera not init!");
        }
        return IF_SUCCESS::FAIL;
    }*/
    
    tSdkFrameHead head;
    BYTE *buffer;
    CameraSdkStatus status;

    
    if ((status = CameraGetImageBuffer(this->hcamera, &head, &buffer, this->wTimes_ms)) == CAMERA_STATUS_SUCCESS)
    {
        if (this->trigger_mode == 1)
        {
            CHECK_MV_STATUS(CameraSoftTrigger(this->hcamera));
        }
        //img = cv::Mat(head.iHeight, head.iWidth, CV_8UC3);

        CHECK_MV_STATUS(CameraImageProcess(this->hcamera, buffer, img.data, &head));
        CHECK_MV_STATUS(CameraReleaseImageBuffer(this->hcamera, buffer));
    }
    else
    {
        if (this->trigger_mode == 1)
        {
            CHECK_MV_STATUS(CameraSoftTrigger(this->hcamera));
        }
        std::string error_msg =  CameraGetErrorString(status);
        SPDLOG_ERROR("CAMERA : get image buffer fail! {}", error_msg);
        CHECK_MV_STATUS(CameraReleaseImageBuffer(this->hcamera, buffer));
    }
    return IF_SUCCESS::SUCCESS;
}







bool Mindvision_Camera_Params::load_params_from_yaml(const std::string& file_path)
{
    // 读取 YAML 文件
    YAML::Node config = YAML::LoadFile(file_path);
    this->safe_load_key(config, "exposure_time_us", this->exposure_time_us);
    this->safe_load_key(config, "gamma", this->gamma);
    this->safe_load_key(config, "fps", this->camera_fps_rank);
    this->safe_load_key(config, "r_gain", this->r_gain);
    this->safe_load_key(config, "g_gain", this->g_gain);
    this->safe_load_key(config, "b_gain", this->b_gain);
    this->safe_load_key(config, "analog_gain", this->analog_gain);
    this->safe_load_key(config, "analog_gain_x", this->analog_gain_x);
    this->safe_load_key(config, "sharpness", this->sharpness);
    this->safe_load_key(config, "saturation", this->saturation);
    this->safe_load_key(config, "contrast", this->contrast);
    this->safe_load_key(config, "grab_resolution_wid", this->grab_resolution_wid);
    this->safe_load_key(config, "grab_resolution_hei", this->grab_resolution_hei);
    return true;

}

void Mindvision_Camera_Params::print_show_params()
{
    spdlog::info("CAMERA : exposure_time_us : {}", this->exposure_time_us);
    spdlog::info("CAMERA : gamma : {}", this->gamma);
    spdlog::info("CAMERA : camera_fps_rank : {}", this->camera_fps_rank);
    spdlog::info("CAMERA : r_gain : {}", this->r_gain);
    spdlog::info("CAMERA : g_gain : {}", this->g_gain);
    spdlog::info("CAMERA : b_gain : {}", this->b_gain);
    spdlog::info("CAMERA : analog_gain : {}", this->analog_gain);
    spdlog::info("CAMERA : analog_gain_x : {}", this->analog_gain_x);
    spdlog::info("CAMERA : sharpness : {}", this->sharpness);
    spdlog::info("CAMERA : saturation : {}", this->saturation);
    spdlog::info("CAMERA : contrast : {}", this->contrast);
    spdlog::info("CAMERA : grab_resolution_wid : {}", this->grab_resolution_wid);
    spdlog::info("CAMERA : grab_resolution_hei : {}", this->grab_resolution_hei);
}
