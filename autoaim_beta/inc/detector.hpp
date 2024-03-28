#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include "basic.hpp"
#include "yaml-cpp/yaml.h"


#include "trt_module.hpp"


struct alignas(4) detect_result_t {
    std::vector<cv::Point2f> big_rec;
    float conf;
    std::string result;
    std::vector<float> tvec;
    std::vector<float> rvec;

};



class Tradition_Detector_Params : public Params
{

public:
    std::vector<int> gauss_ksize = {3,3};
    int gauss_x = 1;
    std::vector<int> close_ksize = {2,2};
    int strech_max = 0;
    std::vector<int> yuv_range = {152,255};
    int bin_thresh = 5;
    std::string armor_color ;

    Tradition_Detector_Params(std::string armor_color_):armor_color(armor_color_){};
    ~Tradition_Detector_Params(){};
    bool load_params_from_yaml(const std::string& file_path);
    void print_show_params();

};




class Tradition_Detector {
private:

    Tradition_Detector_Params params;
    Mode mode;
    std::string armor_color;
    std::vector<float> accept_big_rec_ratio_range = {0.01, 10.0};
    std::vector<float> accept_lightbar_ratio_range = {0.5, 2.0};
    std::vector<int> accept_big_rec_angle_range = {0, 90};
    cv::Size gauss_ksize;
    cv::Size close_ksize;
    const std::vector<int> roi_shape = {32, 32};



public:
    Tradition_Detector(const Mode mode, 
                       const std::string& tradition_config_folder,
                       const std::string& armor_color_);

    ~Tradition_Detector(){};
    IF_SUCCESS operator()(const cv::Mat& img,
                          std::vector<std::vector<cv::Point2f>>& big_rec_list,
                          std::vector<cv::Mat>& roi_list) const;

};



class Net_Detector_Params : public Params
{

public:
    std::vector<Enemy_Car_Info> enemy_car_list;
    float conf_thres = 0.5;
    float iou_thres = 0.5;
    int max_det = 20;
    bool agnostic = false;
    int white_num_thresh = 5;
    std::vector<int> yuv_range = {152,255};
    std::string armor_color = "blue";
    bool load_params_from_yaml(const std::string& file_path);
    void print_show_params();
    Net_Detector_Params(std::vector<Enemy_Car_Info> enemy_car_list_, std::string armor_color_):enemy_car_list(enemy_car_list_),armor_color(armor_color_){};
    ~Net_Detector_Params(){};

};

class Net_Detector
{
private:
    YAML::Node class_info;
    Mode mode;
    bool if_yolov5 = false; 
    TRT_Engine* engine = nullptr;
    TRTModule* yolo_engine = nullptr;
    int class_num = 0;
    std::string armor_color;
    Net_Detector_Params params;

public:
    Net_Detector(Mode mode,
                 const std::string& net_config_folder,
                 bool if_yolov5,
                 std::vector<Enemy_Car_Info> enemy_car_list_,
                 const std::string& armor_color_);
    ~Net_Detector(){
        if (if_yolov5) delete yolo_engine;
        else delete engine;
    };
    std::vector<detect_result_t> operator()(const std::vector<cv::Mat>& bin_rois,const std::vector<std::vector<cv::Point2f>>& big_recs) const;
    std::vector<detect_result_t> operator()(const cv::Mat& img_bgr) const;
    bool if_is_gray(const cv::Mat& img_bgr, const std::vector<cv::Point2f>& big_rec) ;

};


class PNP_Params : public Params
{

public:
    PNP_Params(int& img_show_wid,int& img_show_hei):img_show_wid(img_show_wid),img_show_hei(img_show_hei){
        camera_center = cv::Point2f(img_show_wid/2,img_show_hei/2);
    };
    ~PNP_Params(){};
    bool load_params_from_yaml(const std::string& file_path);
    void print_show_params();

    int img_show_wid;
    int img_show_hei;
    cv::Point2f camera_center;

    std::vector<cv::Point3f> small_obj_points;
    std::vector<cv::Point3f> big_obj_points;
    std::vector<std::string> small_armor_name_list;
    cv::Mat mtx = cv::Mat::zeros(3, 3, CV_32F);
    cv::Mat dist = cv::Mat::zeros(5, 1, CV_32F);


};

class PNP_Solver
{
private:
    PNP_Params params;
    Mode mode;
    float shrink_scale_x = 0.5;
    float shrink_scale_y = 0.5;

public:
    PNP_Solver(Mode mode_,
               const std::string& pnp_config_params_file,
               int& img_show_wid,
               int& img_show_hei):
               params(img_show_wid,img_show_hei)
               {    
                    mode = mode_;
                    params.load_params_from_yaml(pnp_config_params_file);
                    shrink_scale_x = (float)params.img_show_wid/1280.f;
                    shrink_scale_y = (float)params.img_show_hei/1024.f;
                    if (mode == Mode::Dbg) params.print_show_params();
                    
               };

    ~PNP_Solver(){};
    void operator()(std::vector<detect_result_t>& detect_results) const;
};

std::vector<std::vector<cv::Point>> trans_float_contours_to_int(const std::vector<std::vector<cv::Point2f>>& contours,const int& img_wid=640,const int& img_hei=384);
std::vector<cv::Point> trans_float_contour_to_int(const std::vector<cv::Point2f>& contour, const int& img_wid=640,const int& img_hei=384);
void visualize_results(cv::Mat& img_show, const std::vector<detect_result_t> results);







#endif 