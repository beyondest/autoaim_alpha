#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main()
{
    cv::VideoCapture depthStream(cv::CAP_OPENNI2_ASUS);
    cv::namedWindow("Depth Stream");

    while (true)
    {
        cv::Mat frame;
        depthStream.read(frame);

        cv::imshow("Depth Stream", frame);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    cv::destroyAllWindows();

    return 0;
}


