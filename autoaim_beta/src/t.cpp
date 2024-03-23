#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat concatenateMats(const std::vector<cv::Mat>& mats) {
    // Check if the vector is empty
    if (mats.empty()) {
        return cv::Mat();
    }

    // Calculate the total number of rows
    int totalRows = mats.size() * mats[0].rows;


    // Create the output matrix
    std::cout <<"type"<<mats[0].type()<<std::endl;
    cv::Mat concatenatedMat(totalRows, mats[0].cols, mats[0].type());
    cv::vconcat(mats, concatenatedMat);


    /*
    int currentRow = 0;
    for (const auto& mat : mats) {
        cv::Mat subMat = concatenatedMat.rowRange(currentRow, currentRow + mat.rows);
        mat.copyTo(subMat);
        currentRow += mat.rows;
    }*/

    return concatenatedMat;
}

int main() {
    // Create some sample Mats
    cv::Mat mat1 = cv::Mat::ones(3, 3, CV_8UC1) * 1;
    cv::Mat mat2 = cv::Mat::ones(3, 3, CV_8UC1) * 2;
    cv::Mat mat3 = cv::Mat::ones(3, 3, CV_8UC1) * 3;

    // Create a vector of Mats
    std::vector<cv::Mat> mats = {mat1, mat2, mat3};

    // Concatenate the Mats
    cv::Mat concatenatedMat = concatenateMats(mats);

    // Print the concatenated Mat
    std::cout << "Concatenated Mat:\n" << concatenatedMat << std::endl;

    return 0;
}
