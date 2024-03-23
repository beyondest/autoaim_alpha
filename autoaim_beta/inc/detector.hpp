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
    std::vector<int> roi_shape = {32, 32};
    cv::Size gauss_ksize;
    cv::Size close_ksize;


public:
    Tradition_Detector(const Mode mode, 
                       const std::string& tradition_config_folder,
                       const std::string& armor_color_,
                       const std::vector<int>& roi_shape_);

    ~Tradition_Detector(){};
    IF_SUCCESS operator()(const cv::Mat& img,
                          std::vector<std::vector<cv::Point2f>>& big_rec_list,
                          std::vector<cv::Mat>& roi_list) const;

};





class Net_Detector_Params : public Params
{

public:
    std::vector<Enemy_Car_Info> enemy_car_list;
    float conf_thresh = 0.5;
    float ious_thresh = 0.5;
    int max_det = 20;
    bool agnostic = false;
    bool load_params_from_yaml(const std::string& file_path);
    void print_show_params();
    Net_Detector_Params(std::vector<Enemy_Car_Info> enemy_car_list_):enemy_car_list(enemy_car_list_){};
    ~Net_Detector_Params(){};

};

class Net_Detector
{
private:
    YAML::Node class_info;
    Mode mode;
    bool if_yolov5 = false; 
    Net_Detector_Params params;
    TRT_Engine engine;
    int class_num = 0;
public:
    Net_Detector(Mode mode,
                 const std::string& net_config_folder,
                 bool if_yolov5,
                 std::vector<Enemy_Car_Info> enemy_car_list_);
    ~Net_Detector(){};
    std::vector<detect_result_t> operator()(const std::vector<cv::Mat>& bin_rois,const std::vector<std::vector<cv::Point2f>>& big_recs) const;
};


std::vector<std::vector<cv::Point>> trans_float_contours_to_int(std::vector<std::vector<cv::Point2f>>& contours);
std::vector<cv::Point> trans_float_contour_to_int(std::vector<cv::Point2f>& contour);


#endif 