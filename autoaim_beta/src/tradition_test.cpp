#include <iostream>
#include <opencv2/opencv.hpp>


#include "detector.hpp"
#include "mv_class.hpp"
#include "CameraDefine.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <csignal>
#include <vector>

std::string camera_config_folder = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_alpha/config/camera_config";
std::string armor_color = "blue";
Mode mode = Mode::Dbg;
bool if_yolov5 = false;
unsigned int camera_output_format = CAMERA_MEDIA_TYPE_BGR8;
int trigger_mode = 0;
unsigned int wTimes_ms = 1000;

int iHeight = 1024;
int iWidth = 1280;

extern volatile __sig_atomic_t g_sig_int_flag;


cv::Mat img(iHeight, iWidth, CV_8UC3);
cv::Mat img_show(640,640,CV_8UC3);
std::string log_save_folder = "/home/liyuxuan/.ros/log/custom_log";
std::string tradition_config_folder = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_alpha/config/tradition_config";
std::vector<int> roi_shape = {32,32};


std::chrono::high_resolution_clock::time_point t1, t2;
int fps = 0;

const cv::Scalar colors[3] = {{255, 0, 0}, {0, 0, 255}, {0, 255, 0}};


int main()
{
    signal(SIGINT, signal_handler);

    try
    {

        INIT_GLOBAL_LOGGER( log_save_folder,
                            spdlog::level::debug,
                            spdlog::level::debug,
                            true);
        Tradition_Detector detector(mode, tradition_config_folder, armor_color, roi_shape);
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

                cv::resize(img, img_show, cv::Size(640, 384));
                cv::flip(img_show,img_show, -1);
                std::vector<std::vector<cv::Point2f>> big_rec_list;
                std::vector<cv::Mat> roi_list;
                if (detector(img_show, big_rec_list, roi_list) != IF_SUCCESS::SUCCESS)
                {
                    std::cout << "no target" << std::endl;
                    continue;   
                }
                std::cout << "len" << big_rec_list.size() << std::endl;
                auto brl = trans_float_contours_to_int(big_rec_list);
                cv::drawContours(img_show,brl,-1,colors[0],2);
                cv::Mat roi_all ; 
                cv::vconcat(roi_list,roi_all);
                cv::imshow("camera", img_show);
                cv::imshow("roi", roi_all);
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
   
