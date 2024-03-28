#include "gg_class.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    GG_Camera gg(Mode::Dbg);
    cv::Mat img(gg.nHeight, gg.nWidth, CV_8UC3);
    cv::Mat img_show(384,680,CV_8UC3);
    gg.open();
    while (1)
    {
        gg.get_img(img);
        cv::resize(img,img_show,cv::Size(680,384));
        cv::imshow("img",img_show);
        cv::waitKey(1);
    }
    
}

