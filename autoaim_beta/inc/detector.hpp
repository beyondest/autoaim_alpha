#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include "basic.hpp"

typedef struct detect_result {
    std::vector<cv::Point> big_rec;
    std::vector<cv::Point> rvec;
    std::vector<cv::Point> tvec;
    float prob;
    std::string result;

} detect_result_t;


class Tradition_Detector_Params : public Params
{

public:
    std::pair<int,int> gauss_ksize;
    

};


class Detector {
private:

    std::vector<detect_result_t> final_results;



public:
    Detector();
    ~Detector();

    bool detect(cv::Mat& img, cv::Rect& rect);

};


#endif // DETECTOR_HPP