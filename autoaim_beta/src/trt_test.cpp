#include "trt_module.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

void test_yolov5()
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

    
}

void test_classifier()
{
    TRT_Engine engine("/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/net_config/classifier.trt",
                     "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/net_config/net_params.yaml");
    cv::Mat img = cv::imread("/home/rcclub/ggbond/autoaim_ws/src/res/roi_tmp.jpg");
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    cv::threshold(img, img, 127, 255, cv::THRESH_BINARY);
    cv::resize(img, img, cv::Size(32,32));
    std::vector<cv::Mat> input_imgs = {img};
    float* output_buffer = engine(input_imgs);

}
int main()
{
    test_classifier();
    return 0;
}   


