#include "gg_class.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
int main()
{
    GG_Camera gg(Mode::Dbg);
    cv::Mat img(gg.nHeight, gg.nWidth, CV_8UC3);
    cv::Mat img_show(384,680,CV_8UC3);
    gg.open();
    while (1)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        gg.get_img(img);
        cv::resize(img,img_show,cv::Size(680,384));
        cv::imshow("img",img_show);
        cv::waitKey(1);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        auto fps = 1000000.0 / duration.count();
        std::cout << "fps: " << fps << std::endl;
    }
    
}

