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

//#define DEBUG_ROI

std::string armor_color = "blue";
Mode mode = Mode::Dbg;
bool if_yolov5 = false;

int iHeight = 1024;
int iWidth = 1280;
int img_show_wid = 640;
int img_show_hei = 384;

extern volatile __sig_atomic_t g_sig_int_flag;


cv::Mat img(iHeight, iWidth, CV_8UC3);
cv::Mat img_show(img_show_wid,img_show_hei,CV_8UC3);
std::string camera_config_folder = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/camera_config";
std::string log_save_folder = "/home/rcclub/.ros/log/custom_log";
std::string tradition_config_folder = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/tradition_config";
std::string net_config_folder = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/net_config";
std::string pnp_config_params_path = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/other_config/pnp_params.yaml";
std::vector<int> roi_shape = {32,32};


std::chrono::high_resolution_clock::time_point t1, t2;
int fps = 0;


std::vector<Enemy_Car_Info> enemy_car_info_list;

int main()
{
    signal(SIGINT, signal_handler);
    enemy_car_info_list.push_back(Enemy_Car_Info{"3x",1});
    try
    {

        INIT_GLOBAL_LOGGER( log_save_folder,
                            spdlog::level::debug,
                            spdlog::level::debug,
                            true);

        Tradition_Detector detector(mode, tradition_config_folder, armor_color);
        Net_Detector net_detector(mode,net_config_folder,false,enemy_car_info_list);
        PNP_Solver pnp_solver(mode,pnp_config_params_path,img_show_wid,img_show_hei);
        Mindvision_Camera mv(mode,
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
                cv::resize(img, img_show, cv::Size(img_show_wid,img_show_hei));
                cv::flip(img_show,img_show, -1);

                if (!if_yolov5)
                {
                    std::vector<std::vector<cv::Point2f>> big_rec_list;
                    std::vector<cv::Mat> roi_list;
                    if (detector(img_show, big_rec_list, roi_list) != IF_SUCCESS::SUCCESS)
                    {
                        std::cout << "no target" << std::endl;
                    }
                    if(big_rec_list.size() > 0)
                    {
                        #ifdef DEBUG_ROI
                        cv::Mat all_roi;
                        cv::vconcat(roi_list,all_roi);
                        cv::imshow("roi_all",all_roi);
                        cv::waitKey(1);
                        #endif  
                        auto results = net_detector(roi_list,big_rec_list);
                        pnp_solver(results);
                        visualize_results(img_show,results);
                    }
                    else
                    {
                        std::cout << "no target" << std::endl;
                    }
                }
                else
                {
                    auto results = net_detector(img_show);
                    pnp_solver(results);
                    visualize_results(img_show,results);
                    if (results.size() == 0) std::cout << "no target" << std::endl;
                }
            }
            cv::imshow("camera", img_show);
            cv::waitKey(1);
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
   
