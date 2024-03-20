#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    vector<vector<Point>> contours;
    Mat img = imread("/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/res/armorred.png");
    if(img.empty())
    {
        cout << "Could not open or find the image" << endl;
        return -1;
    }
    namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Display window", img);
    waitKey(0);
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    cv::threshold(img, img, 127, 255, cv::THRESH_BINARY);
    cv::findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for(unsigned int i = 0; i < contours.size(); i++)
    {
        cout << "Contour " << i << " has " << contours[i].size() << " points" << endl;
    }
    return 0;
}