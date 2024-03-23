#include "detector.hpp"
#include "basic.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#define DEBUG_TRADITION_DETECTOR
#define USE_TWO_LIGHTBAR


//**********************************************TOOLS********************************************
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

std::vector<cv::Point> trans_float_contour_to_int(std::vector<cv::Point2f>& contour)
{
    std::vector<cv::Point> int_contour;
    for (size_t i = 0; i < contour.size(); i++)
    {
        cv::Point p;
        p.x = std::round(contour[i].x);
        p.y = std::round(contour[i].y);
        int_contour.push_back(p);
    }
    return int_contour;
}




/**
 * @brief Get the corners of the rectangle from the contour
 * @param contour The contour of the rectangle
 * @param direction The direction of the rectangle (0: both side up_down strech, 1: strech to left, 2: strech to right)
*/
static std::vector<cv::Point2f> extendRectangle(const std::vector<cv::Point2f>& corners, const int& direction, const float& extendRatio) {
    std::vector<cv::Point2f> extendedCorners;
    assert (direction == 0 || direction == 1 || direction == 2 );
    if (direction == 0)
    {
        cv::Point2f topLeftExtended = corners[0] - extendRatio * (corners[3] - corners[0]);
        cv::Point2f bottomLeftExtended = corners[3] + extendRatio * (corners[3] - corners[0]);

        cv::Point2f topRightExtended = corners[1] - extendRatio * (corners[2] - corners[1]);
        cv::Point2f bottomRightExtended = corners[2] + extendRatio * (corners[2] - corners[1]);

        extendedCorners.push_back(topLeftExtended);
        extendedCorners.push_back(topRightExtended);
        extendedCorners.push_back(bottomRightExtended);
        extendedCorners.push_back(bottomLeftExtended);
    }
    else if(direction == 1)
    {
        float extend_dis = extendRatio * cv::norm(corners[1] - corners[2]);
        cv::Point2f topLeftExtended = corners[0] - extend_dis * (corners[1] - corners[0])/cv::norm(corners[1] - corners[0]);
        cv::Point2f bottomLeftExtended = corners[3] - extend_dis * (corners[1] - corners[0])/cv::norm(corners[1] - corners[0]);
        
        cv::Point2f topRightExtended = corners[1] ;
        cv::Point2f bottomRightExtended = corners[2];
        extendedCorners.push_back(topLeftExtended);
        extendedCorners.push_back(topRightExtended);
        extendedCorners.push_back(bottomRightExtended);
        extendedCorners.push_back(bottomLeftExtended);
    }
    else if (direction == 2)
    {
        float extend_dis = extendRatio * cv::norm(corners[1] - corners[2]);
        cv::Point2f topLeftExtended = corners[0] ;
        cv::Point2f bottomLeftExtended = corners[3];
        cv::Point2f topRightExtended = corners[1] + extend_dis * (corners[1] - corners[0])/cv::norm(corners[1] - corners[0]);
        cv::Point2f bottomRightExtended = corners[2] + extend_dis * (corners[1] - corners[0])/cv::norm(corners[1] - corners[0]);
        extendedCorners.push_back(topLeftExtended);
        extendedCorners.push_back(topRightExtended);
        extendedCorners.push_back(bottomRightExtended);
        extendedCorners.push_back(bottomLeftExtended);
    }
    return extendedCorners;
}

static int get_threshold(const cv::Mat& img) {

    std::vector<cv::Mat> imgs = {img};
    cv::Mat hist;

    cv::calcHist(imgs,{0},cv::Mat(), hist, {255}, {0, 255}, false);
    cv::normalize(hist, hist, 0, 255, cv::NORM_MINMAX);
    hist.convertTo(hist, CV_32S);
    int thresh = 0;
    cv::Point maxLoc;
    cv::minMaxLoc(hist, nullptr, nullptr, nullptr, &maxLoc);
    thresh = maxLoc.y;
    if (thresh != hist.rows - 1 && hist.at<int>(thresh + 1) > 30) thresh += 1;
    return thresh;
}
/**
 * @brief Order points into up_left, up_right, down_right, down_left order
 */
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

void static filter_overlap(std::vector<std::vector<cv::Point2f>>& big_rec_list)
{
    std::vector<std::vector<cv::Point2f>> results_list;
    bool found_overlap = false;
    for (auto& big_rec : big_rec_list)
    {
        for (auto& result : results_list)
        {   
            cv::RotatedRect rect1 = cv::minAreaRect(big_rec);
            cv::RotatedRect rect2 = cv::minAreaRect(result);
            cv::Point2f center1 = rect1.center;
            cv::Point2f center2 = rect2.center;
            cv::Size2f size1 = rect1.size;
            cv::Size2f size2 = rect2.size;
            if (  center1.x > center2.x - size2.width/2  && center1.x < center2.x + size2.width/2\
                &&center2.x > center1.x - size1.width/2  && center2.x < center1.x + size1.width/2\
                &&center1.y > center2.y - size2.height/2 && center1.y < center2.y + size2.height/2\
                &&center2.y > center1.y - size1.height/2 && center2.y < center1.y + size1.height/2)
            {
                result = (rect1.size.area() > rect2.size.area())? big_rec : result;
                found_overlap = true;
                break;
            }
            else continue;
        }
        if (!found_overlap) results_list.push_back(big_rec);
    }
    big_rec_list = results_list;
}


constexpr float sigmoid(float x) {
    return 1 / (1 + std::exp(-x));
}


//****************************************************Tradition Detector*************************************************

Tradition_Detector::Tradition_Detector(const Mode mode, 
                    const std::string& tradition_config_folder,
                    const std::string& armor_color_,
                    const std::vector<int>& roi_shape_) :
params(armor_color_),
mode(mode),
armor_color(armor_color_),
roi_shape(roi_shape_)
{   
    if (armor_color != "red" && armor_color != "blue") throw std::invalid_argument("Tradition_Detector : armor_color should be red or blue");
    std::string preprocess_params_path;
    preprocess_params_path = tradition_config_folder+"/" + armor_color + "/preprocess_params.yaml";
    YAML::Node filter1_config = YAML::LoadFile(tradition_config_folder+"/"+armor_color+"/filter1_params.yaml");
    accept_lightbar_ratio_range[0] = filter1_config["accept_aspect_ratio_range"][0].as<float>();
    accept_lightbar_ratio_range[1] = filter1_config["accept_aspect_ratio_range"][1].as<float>();

    YAML::Node filter2_config = YAML::LoadFile(tradition_config_folder+"/"+armor_color+"/filter2_params.yaml");
    accept_big_rec_ratio_range[0] = filter2_config["accept_aspect_ratio_range"][0].as<float>();
    accept_big_rec_ratio_range[1] = filter2_config["accept_aspect_ratio_range"][1].as<float>();

    this->params.load_params_from_yaml(preprocess_params_path);

    this->gauss_ksize = cv::Size(params.gauss_ksize[0], params.gauss_ksize[1]);
    this->close_ksize = cv::Size(params.close_ksize[0], params.close_ksize[1]);

    if (this->mode == Mode::Dbg)    
    {
        this->params.print_show_params();
        spdlog::warn("FUCK");
    }
    
}

IF_SUCCESS Tradition_Detector::operator()(const cv::Mat& img, 
                                          std::vector<std::vector<cv::Point2f>>& big_rec_list,
                                          std::vector<cv::Mat>& roi_list) const
{
    

    // preprocess img
    cv::Mat img_ ;
    std::vector<cv::Mat> img_split(3);
    cv::cvtColor(img, img_, cv::COLOR_BGR2YUV);
    cv::GaussianBlur(img_, img_, gauss_ksize, this->params.gauss_x);
    cv::split(img_, img_split);
    if (this->armor_color == "red") cv::inRange(img_split[2], this->params.yuv_range[0], this->params.yuv_range[1], img_split[0]);
    else                            cv::inRange(img_split[1], this->params.yuv_range[0], this->params.yuv_range[1], img_split[0]); 

#ifdef DEBUG_TRADITION_DETECTOR
    cv::imshow("img_split_before", img_split[0]);
    cv::waitKey(1);
#endif
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, this->close_ksize);
    cv::morphologyEx(img_split[0], img_split[0], cv::MORPH_CLOSE, kernel,cv::Point(-1,-1), 1);
    cv::morphologyEx(img_split[0], img_split[0], cv::MORPH_OPEN, kernel,cv::Point(-1,-1), 1);
#ifdef DEBUG_TRADITION_DETECTOR
    cv::imshow("img_split0", img_split[0]);
    cv::waitKey(1);

#endif
    // find big rec
    std::vector<std::vector<cv::Point>> contours;
    cv::Point2f vertices[4];
    cv::findContours(img_split[0], contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::RotatedRect rect1 = cv::minAreaRect(contours[i]);
        if (rect1.size.width == 0 || rect1.size.height == 0) continue;
        float aspect_ratio = rect1.size.width / rect1.size.height;
        if (aspect_ratio < this->accept_big_rec_ratio_range[0] || aspect_ratio > this->accept_big_rec_ratio_range[1]) continue;

#ifdef USE_TWO_LIGHTBAR
        if (contours.size() > 1) 
        {
            for (size_t j = i+1; j < contours.size(); j++) 
                {
                    cv::RotatedRect rect2 = cv::minAreaRect(contours[j]);
                    if (rect2.size.width == 0 || rect2.size.height == 0) continue;
                    std::vector<cv::Point2f> merged_contour;
                    merged_contour.reserve(contours[i].size() + contours[j].size()) ;
                    std::copy(contours[i].begin(), contours[i].end(), std::back_inserter(merged_contour));
                    std::copy(contours[j].begin(), contours[j].end(), std::back_inserter(merged_contour));
                    cv::RotatedRect big_rec = cv::minAreaRect(merged_contour);
                    aspect_ratio = big_rec.size.width / big_rec.size.height;
                    if (aspect_ratio < this->accept_big_rec_ratio_range[0] || aspect_ratio > this->accept_big_rec_ratio_range[1]) continue;
                    big_rec.points(vertices);
                    std::vector<cv::Point2f> big_rec_contour;
                    for (int k = 0; k < 4; k++) big_rec_contour.push_back(vertices[k]);
                    order_points(big_rec_contour);
                    big_rec_contour = extendRectangle(big_rec_contour,0,0.3);
                    big_rec_list.push_back(big_rec_contour);
                }
        }
#else
        std::vector<cv::Point2f> big_rec_contour;
        std::vector<cv::Point2f> left_right_extend_rec_contour;
        std::vector<cv::Point2f> up_down_extend_rec_contour;
        rect1.points(vertices);
        for (int k = 0; k < 4; k++) big_rec_contour.push_back(vertices[k]);
        order_points(big_rec_contour);
        left_right_extend_rec_contour = extendRectangle(big_rec_contour,1,3); // extend to left
        up_down_extend_rec_contour = extendRectangle(left_right_extend_rec_contour,0,0.3);
        big_rec_list.push_back(up_down_extend_rec_contour);
        left_right_extend_rec_contour = extendRectangle(big_rec_contour,2,3); // extend to right
        up_down_extend_rec_contour = extendRectangle(left_right_extend_rec_contour,0,0.3);
        big_rec_list.push_back(up_down_extend_rec_contour);
        filter_overlap(big_rec_list);
#endif
    }
    if (big_rec_list.empty()) return IF_SUCCESS::FAIL;
    cv::Mat roi;
    cv::RotatedRect big_rec;
    cv::Size roi_shape_ = {this->roi_shape[0], this->roi_shape[1]};

    // pick up roi and perspective transform
    for (size_t i = 0; i < big_rec_list.size(); i++)
    {   
        std::vector<cv::Point2f> dst_points;
        dst_points.push_back(cv::Point2f(0, 0));
        dst_points.push_back(cv::Point2f(this->roi_shape[0] - 1, 0));
        dst_points.push_back(cv::Point2f(this->roi_shape[0] - 1, this->roi_shape[1] - 1));
        dst_points.push_back(cv::Point2f(0, this->roi_shape[1] - 1));
        cv::Mat trans_mat = cv::getPerspectiveTransform(big_rec_list[i], dst_points);
        cv::warpPerspective(img, roi, trans_mat, roi_shape_, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        cv::cvtColor(roi, roi, cv::COLOR_BGR2GRAY);
        cv::Mat exclude_light_bar_region;
        if (this->roi_shape[0] == 32 && this->roi_shape[1] == 32){ 
            cv::Rect roi_(5,0,22,roi.rows);
            exclude_light_bar_region = roi(roi_);
        }
        else if (this->roi_shape[0] == 64 && this->roi_shape[1] == 64){
            cv::Rect roi_(10,0,44,roi.rows);
            exclude_light_bar_region = roi(roi_);
        }
        else throw std::runtime_error("roi_shape is not 32x32 or 64x64");
        int threshold = get_threshold(exclude_light_bar_region);
        //threshold = this->params.bin_thresh;
        cv::threshold(roi, roi, threshold, 255, cv::THRESH_BINARY);
        roi_list.push_back(roi);
    }
    return IF_SUCCESS::SUCCESS;;
}




bool Tradition_Detector_Params::load_params_from_yaml(const std::string& file_path)
{
    YAML::Node config = YAML::LoadFile(file_path);
    this->gauss_ksize[0] = config["gaussianblur_kernel_size"][0].as<int>(); 
    this->gauss_ksize[1] = config["gaussianblur_kernel_size"][1].as<int>(); 
    this->close_ksize[0] = config["close_kernel_size"][0].as<int>(); 
    this->close_ksize[1] = config["close_kernel_size"][1].as<int>(); 
    this->gauss_x = config["gaussianblur_x"].as<int>(); 
    //this->strech_max = config["strech_max"].as<int>(); 
    if (this->armor_color == "red")
    {
        this->bin_thresh = config["red_armor_binary_roi_threshold"].as<int>(); 
        this->yuv_range[0] = config["red_armor_yuv_range"][0].as<int>();
        this->yuv_range[1] = config["red_armor_yuv_range"][1].as<int>();
        this->bin_thresh = config["red_armor_binary_roi_threshold"].as<int>(); 
    }
    else
    {
        this->bin_thresh = config["blue_armor_binary_roi_threshold"].as<int>(); 
        this->yuv_range[0] = config["blue_armor_yuv_range"][0].as<int>();
        this->yuv_range[1] = config["blue_armor_yuv_range"][1].as<int>();
        this->bin_thresh = config["blue_armor_binary_roi_threshold"].as<int>(); 
    }
    
    return true;
    
}

void Tradition_Detector_Params::print_show_params()
{
    spdlog::info("gauss_ksize: {}, {}, close_ksize: {}, {}, bin_thresh: {}, gauss_x: {}, yuv_range: {}, {}",
                 this->gauss_ksize[0], this->gauss_ksize[1], this->close_ksize[0], this->close_ksize[1],
                 this->bin_thresh, this->gauss_x, this->yuv_range[0], this->yuv_range[1]);  
}




bool Net_Detector_Params::load_params_from_yaml(const std::string& file_path)
{
    YAML::Node config = YAML::LoadFile(file_path);
    for (auto i : config["enemy_car_list"])
    {
        Enemy_Car_Info info;
        info.armor_name = i["armor_name"].as<std::string>();
        info.armor_nums = i["armor_nums"].as<int>();
        this->enemy_car_list.push_back(info);
    }
    this->conf_thresh = config["conf_thresh"].as<float>();
    this->agnostic = config["agnostic"].as<bool>();
    this->ious_thresh = config["ious_thresh"].as<float>();
    this->max_det = config["max_det"].as<int>();
    return true;
}

void Net_Detector_Params::print_show_params()
{
    spdlog::info("enemy_car_list: {}, conf_thresh: {}, agnostic: {}, ious_thresh: {}, max_det: {}",
                 this->enemy_car_list, this->conf_thresh, this->agnostic, this->ious_thresh, this->max_det);
}

//***************************************************Net Detector****************************************************

Net_Detector::Net_Detector(Mode mode,
                 const std::string& net_config_folder,
                 bool if_yolov5):
                 mode(mode),
                 if_yolov5(if_yolov5),
                 engine(net_config_folder+"/classifier.trt",net_config_folder+"/net_params.yaml")
{
    this->params.load_params_from_yaml(net_config_folder+"/net_params.yaml");
    if (if_yolov5) throw std::invalid_argument("Net_Detector : yolov5 is not supported yet, if_yolov5 should be false");
    this->class_info = YAML::LoadFile(net_config_folder+"/classifier_class.yaml");
    this->class_num = this->class_info.size();
}


std::vector<detect_result_t> Net_Detector::operator()(const std::vector<cv::Mat>& bin_rois,const std::vector<std::vector<cv::Point2f>>& big_recs) const
{
    int batch_size = bin_rois.size();
    std::vector<detect_result_t> results;
    float conf = 0.0;
    std::string class_name = "";
    int max_idx = 0;
    float* output_buffer = this->engine(bin_rois);
    for (int i = 0; i < batch_size; i+=this->class_num)
    {  
        for (int j = i; j < this->class_num; j++){if (output_buffer[j] > output_buffer[max_idx])max_idx = j;}
        conf = sigmoid(output_buffer[max_idx]);
        class_name = this->class_info[std::to_string(max_idx)].as<std::string>();
        bool if_in_target_list = false;
        for (auto& enemy_car : this->params.enemy_car_list)if (enemy_car.armor_name == class_name) if_in_target_list = true;                
        if (conf > this->params.conf_thresh && if_in_target_list)
        {
            detect_result_t detect_result = {big_recs[i], conf, class_name};
            results.push_back(detect_result);
        }
    }
    return results;
}