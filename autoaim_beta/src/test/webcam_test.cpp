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

int main()
{
    signal(SIGINT, signal_handler);

    try
    {

        INIT_GLOBAL_LOGGER( log_save_folder,
                            spdlog::level::debug,
                            spdlog::level::debug,
                            true);
                            
        Mindvision_Camera mv(mode,
                            camera_config_folder,
                            camera_output_format,
                            trigger_mode,
                            wTimes_ms,
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