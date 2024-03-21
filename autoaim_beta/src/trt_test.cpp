#include "trt_module.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    TrtModule trt_module("/home/rcclub/ggbond/autoaim_ws/src/autoaim_beta/weights/opt4.onnx");
    cv::Mat img = cv::imread("/home/rcclub/ggbond/autoaim_ws/src/res/armorred.png");
    auto results = trt_module(img);
    std::cout << "results: " << results << std::endl;
    std::cout << "results size: " << results.size() << std::endl;   
    std::cout << "type of results: " << typeid(results).name() << std::endl;    

    
    return 0;
}   