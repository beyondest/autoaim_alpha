#include <opencv2/opencv.hpp>
#include <iostream>
cv::RotatedRect expand_big_rec(const cv::RotatedRect& rotatedRect) 
{
    cv::Point2f center = rotatedRect.center;
    float width = rotatedRect.size.width;
    float height = rotatedRect.size.height;
    float angle = rotatedRect.angle;
    //float minLength = std::min(width, height);
    //float maxLength = std::max(width, height);
    cv::RotatedRect squareRect(center, cv::Size2f(width*1.5, height),angle);
    return squareRect;
}
std::vector<std::vector<cv::Point>> trans_float_contours_to_int(std::vector<std::vector<cv::Point2f>>& contours)
{
    std::vector<std::vector<cv::Point>> int_contours;

    for (size_t i = 0; i < contours.size(); i++)
    {
        std::vector<cv::Point> int_contour;
        for (size_t j = 0; j < contours[i].size(); j++)
        {
            cv::Point p;
            p.x = std::round(contours[i][j].x);
            p.y = std::round(contours[i][j].y);
            int_contour.push_back(p);
        }
        int_contours.push_back(int_contour);    
    }
    return int_contours;

}

static void order_points(std::vector<cv::Point2f>& pts) 
{
    // Check and reshape the input points if needed
    std::vector<cv::Point2f> recPointsCopy = {pts[0],pts[1],pts[2],pts[3]};
    std::vector<cv::Point2f> orderedPoints(4);

    // Find up_left and bottom_right
    // sum = x + y
    std::vector<float> sums(4);
    for (int i = 0; i < 4; ++i) {
        sums[i] = recPointsCopy[i].x + recPointsCopy[i].y;
    }
    auto minmaxSum = std::minmax_element(sums.begin(), sums.end());
    orderedPoints[0] = recPointsCopy[std::distance(sums.begin(), minmaxSum.first)];
    orderedPoints[2] = recPointsCopy[std::distance(sums.begin(), minmaxSum.second)];

    // Find up_right and bottom_left
    // diff = y - x
    std::vector<float> diffs(4);
    for (int i = 0; i < 4; ++i) {
        diffs[i] = recPointsCopy[i].y - recPointsCopy[i].x;
    }
    auto minmaxDiff = std::minmax_element(diffs.begin(), diffs.end());
    orderedPoints[1] = recPointsCopy[std::distance(diffs.begin(), minmaxDiff.first)];
    orderedPoints[3] = recPointsCopy[std::distance(diffs.begin(), minmaxDiff.second)];

    pts = orderedPoints; // Update the original points vector
}
std::vector<cv::Point2f> extendRectangle(const std::vector<cv::Point2f>& corners) {
    // 创建一个新的vector来存储延长后的角点
    std::vector<cv::Point2f> extendedCorners;
    float extendRatio = 1.5; // 延长比例
    // 左上角点和左下角点延长
    cv::Point2f topLeftExtended = corners[0] - extendRatio * (corners[3] - corners[0]);
    cv::Point2f bottomLeftExtended = corners[3] + extendRatio * (corners[3] - corners[0]);

    // 右上角点和右下角点延长
    cv::Point2f topRightExtended = corners[1] - extendRatio * (corners[2] - corners[1]);
    cv::Point2f bottomRightExtended = corners[2] + extendRatio * (corners[2] - corners[1]);

    // 将延长后的角点添加到vector中
    extendedCorners.push_back(topLeftExtended);
    extendedCorners.push_back(topRightExtended);
    extendedCorners.push_back(bottomRightExtended);
    extendedCorners.push_back(bottomLeftExtended);

    return extendedCorners;
}
int main() {
    // 创建一个轮廓（这里仅作示例）
    std::vector<cv::Point> contour = {
        cv::Point(50, 25),
        cv::Point(25, 100),
        cv::Point(120, 150),
        cv::Point(100, 40)
    };

    // 计算最小包围旋转矩形
    cv::RotatedRect rotatedRect = cv::minAreaRect(contour);
    // 获取旋转矩形的角点
    cv::Point2f vertices[4];
    rotatedRect.points(vertices);
    
    // 打印角点坐标
    for (int i = 0; i < 4; i++) {
        std::cout << "Vertex " << i + 1 << ": " << vertices[i] << std::endl;
    }
    std::cout << rotatedRect.angle << std::endl;
    std::cout << rotatedRect.size.width << " " << rotatedRect.size.height << std::endl;

    //rotatedRect = expand_big_rec(rotatedRect);
    cv::Point2f vertices2[4];   
    rotatedRect.points(vertices2);

    // 打印扩大后的角点坐标
    for (int i = 0; i < 4; i++) {
        std::cout << "Vertex2 " << i + 1 << ": " << vertices2[i] << std::endl;
    }
    std::cout << rotatedRect.angle << std::endl;
    std::cout << rotatedRect.size.width << " " << rotatedRect.size.height << std::endl;


    std::vector<std::vector<cv::Point>> contours = {contour};

    std::vector<std::vector<cv::Point2f>> contours2 ;
    std::vector<cv::Point2f> c_;
    for (int i = 0; i < 4; i++) {
        c_.push_back(vertices[i]);
    }
    order_points(c_);
    for (int i = 0; i < 4; i++) {
        std::cout << "Vertex " << i + 1 << ": " << c_[i] << std::endl;
    }
    contours2.push_back(c_);
    std::vector<std::vector<cv::Point2f>> contours3 ;
    std::vector<cv::Point2f> c2_;
    for (int i = 0; i < 4; i++) {
        c2_.push_back(vertices2[i]);
    }
    order_points(c2_);
    c2_ = extendRectangle(c2_);
    for (int i = 0; i < 4; i++) {
        std::cout << "Vertex2 " << i + 1 << ": " << c2_[i] << std::endl;
    }
    contours3.push_back(c2_);

    std::vector<std::vector<cv::Point>> int_contours2 = trans_float_contours_to_int(contours2);
    std::vector<std::vector<cv::Point>> int_contours3 = trans_float_contours_to_int(contours3);

    cv::Mat img(500,500,CV_8UC3,cv::Scalar(255,255,255));
    cv::drawContours(img,contours, -1, cv::Scalar(0,0,255), 2);
    cv::drawContours(img, int_contours2, -1, cv::Scalar(0,255,0), 2);
    cv::drawContours(img,int_contours3, -1, cv::Scalar(255,0,0), 2);
    cv::imshow("img", img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
