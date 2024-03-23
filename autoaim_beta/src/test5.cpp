#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "detector.hpp"
using namespace cv;
using namespace std;

Mode mode = Mode::Dbg;
std::string tradition_config_folder = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_alpha/config/tradition_config/";
std::string armor_color = "red";
std::vector<int> roi_shape = {32,32};
int main()
{
    std::vector<std::vector<cv::Point2f>> big_rec_list;
    std::vector<Mat> roi_list;
    IF_SUCCESS flag = IF_SUCCESS::FAIL;
    Mat img = imread("/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/res/armorred.png");
    if(img.empty())
    {
        cout << "Could not open or find the image" << endl;
        return -1;
    }
    imshow("original",img);
    waitKey(0);
    Tradition_Detector detector(mode,tradition_config_folder,armor_color,roi_shape);
    flag = detector(img,big_rec_list,roi_list);
    if (flag == IF_SUCCESS::SUCCESS)
    {
        std::cout << "big_rec_list size:" << big_rec_list.size() << std::endl;
        auto int_contours = trans_float_contours_to_int(big_rec_list);
        cv::drawContours(img, int_contours, -1, cv::Scalar(0, 255, 0), 2);
        imshow("result",img);
        waitKey(0);
    }
    else
    {
        std::cout << "no armor found" << std::endl;
    }
    return 0;
    
}

