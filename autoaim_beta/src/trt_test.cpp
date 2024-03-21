#include "trt_module.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    TRTModule trt_module("/home/rcclub/ggbond/autoaim_ws/src/autoaim_beta/weights/opt4.onnx");
    cv::Mat img = cv::imread("/home/rcclub/ggbond/autoaim_ws/src/res/armorred.png");
    auto results = trt_module(img);
    std::cout << "type of results: " << typeid(results).name() << std::endl;   
    const cv::Scalar colors[3] = {{255, 0, 0}, {0, 0, 255}, {0, 255, 0}};
 
    for (auto &b : results)
    {
        cv::line(img, b.pts[0], b.pts[1], colors[2], 2);
        cv::line(img, b.pts[1], b.pts[2], colors[2], 2);
        cv::line(img, b.pts[2], b.pts[3], colors[2], 2);
        cv::line(img, b.pts[3], b.pts[0], colors[2], 2);
        cv::putText(img, std::to_string(b.tag_id), b.pts[0], cv::FONT_HERSHEY_SIMPLEX, 1, colors[b.color_id]);
    } 
    cv::imshow("result", img);
    cv::waitKey(0);

    cv::destroyAllWindows();

    
    return 0;
}   


