#include "mv_class.hpp"
#include "CameraDefine.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <csignal>

std::string camera_config_folder = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_alpha/config/camera_config";
std::string armor_color = "red";
Mode mode = Mode::Dbg;
bool if_yolov5 = true;
unsigned int camera_output_format = CAMERA_MEDIA_TYPE_BGR8;
int trigger_mode = 0;
unsigned int wTimes_ms = 1000;

int iHeight = 1024;
int iWidth = 1280;

extern volatile __sig_atomic_t g_sig_int_flag;


cv::Mat img(iHeight, iWidth, CV_8UC3);
cv::Mat img_show(640,640,CV_8UC3);
std::string log_save_folder = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_beta/log";

std::chrono::high_resolution_clock::time_point t1, t2;
int fps = 0;


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
qq
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


int main()
{
    signal(SIGINT, signal_handler);
    visualize_isp_config_by_isp_params();
    try
    {

        INIT_GLOBAL_LOGGER( log_save_folder,
                            spdlog::level::debug,
                            spdlog::level::debug,
                            true);
                            
        Mindvision_Camera mv(Mode::Dbg,
                            camera_config_folder,
                            false,
                            armor_color,
                            if_yolov5
                            );

        mv.open();
        while (!g_sig_int_flag)
        {
            t1 = std::chrono::high_resolution_clock::now();
            if (mv.get_img(img) == IF_SUCCESS::SUCCESS)
            {
                cv::resize(img, img_show, cv::Size(640, 640));
                cv::flip(img_show,img_show, -1);
                auto prs = update_camera_isp_params();
                set_isp_params(prs,mv.hcamera);
                cv::imshow("camera", img_show);
                cv::waitKey(1);
            }
            t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> time_used = t2 - t1;
            fps = 1000 / time_used.count();
            std::cout << "fps: " << fps << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr<<"Exception: " << e.what() << std::endl;
    }

    cv::destroyAllWindows();
    return 0;

}