#include "detector.hpp"
#include "basic.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
//#define DEBUG_TRADITION_DETECTOR
#define USE_TWO_LIGHTBAR


//**********************************************TOOLS********************************************
std::vector<std::vector<cv::Point>> trans_float_contours_to_int(const std::vector<std::vector<cv::Point2f>>& contours,const int& img_wid,const int& img_hei)
{
    std::vector<std::vector<cv::Point>> int_contours;

    for (size_t i = 0; i < contours.size(); i++)
    {
        std::vector<cv::Point> int_contour;
        for (size_t j = 0; j < contours[i].size(); j++)
        {
            cv::Point p;
            p.x = std::round(contours[i][j].x);
            p.x = p.x<0?0:p.x>img_wid?img_hei:p.x;
            p.y = std::round(contours[i][j].y);
            p.y = p.y<0?0:p.y>img_hei?img_hei:p.y;
            int_contour.push_back(p);
        }
        int_contours.push_back(int_contour);    
    }
    return int_contours;

}

std::vector<cv::Point> trans_float_contour_to_int(const std::vector<cv::Point2f>& contour,const int& img_wid,const int& img_hei)
{
    std::vector<cv::Point> int_contour;
    for (size_t i = 0; i < contour.size(); i++)
    {
        cv::Point p;
        p.x = std::round(contour[i].x);
        p.x = p.x<0?0:p.x>img_wid?img_wid:p.x;
        p.y = std::round(contour[i].y);
        p.y = p.y<0?0:p.y>img_hei?img_hei:p.y;
        int_contour.push_back(p);
    }
    return int_contour;
}
void visualize_results(cv::Mat& img_show, const std::vector<detect_result_t> results)
{
    for (auto& result : results)
    {
        auto rect_int = trans_float_contour_to_int(result.big_rec);
        std::vector<std::vector<cv::Point>> rect_int_list = {rect_int};
        cv::drawContours(img_show,rect_int_list,-1,cv::Scalar(255,0,0),2);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << result.conf;
        cv::putText(img_show,result.result+":"+ss.str(),cv::Point(rect_int[0].x,rect_int[0].y),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,0,0),2);
        std::string xyz = "xyz";
        for (size_t i = 0; i < 3; i++)
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << result.tvec[i];
            xyz+=stream.str() + "  ";
        }
        cv::putText(img_show,xyz,cv::Point(rect_int[2].x,rect_int[2].y),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,0,0),2);

        
    }
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

static inline float softmax(float* buffer, int& begin_idx, const int& class_num, int& actual_idx)
{
    float sum_exp = 0;
    for (int i = begin_idx; i <begin_idx + class_num; i++) sum_exp += std::exp(buffer[i]);
    return std::exp(buffer[actual_idx]) / sum_exp;
}




//****************************************************Tradition Detector*************************************************

Tradition_Detector::Tradition_Detector(const Mode mode, 
                    const std::string& tradition_config_folder,
                    const std::string& armor_color_) :
params(armor_color_),
mode(mode),
armor_color(armor_color_)
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
    accept_big_rec_angle_range[0] = filter2_config["accept_area_range"][0].as<int>();
    accept_big_rec_angle_range[1] = filter2_config["accept_area_range"][1].as<int>();
    this->params.load_params_from_yaml(preprocess_params_path);

    this->gauss_ksize = cv::Size(params.gauss_ksize[0], params.gauss_ksize[1]);
    this->close_ksize = cv::Size(params.close_ksize[0], params.close_ksize[1]);

    if (this->mode == Mode::Dbg)    
    {
        this->params.print_show_params();
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
    //cv::morphologyEx(img_split[0], img_split[0], cv::MORPH_OPEN, kernel,cv::Point(-1,-1), 1);
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
                    //if (big_rec.angle < this->accept_big_rec_angle_range[0] || big_rec.angle > this->accept_big_rec_angle_range[1]) continue;   
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
    this->conf_thres = config["conf_thres"].as<float>();
    this->agnostic = config["agnostic"].as<bool>();
    this->iou_thres = config["iou_thres"].as<float>();
    this->max_det = config["max_det"].as<int>();
    this->white_num_thresh = config["white_num_thresh"].as<int>();
    if (this->armor_color == "blue")
    {
    this->yuv_range[0] = config["blue_yuv_range"][0].as<int>();
    this->yuv_range[1] = config["blue_yuv_range"][1].as<int>();
    }
    else
    {
        this->yuv_range[0] = config["red_yuv_range"][0].as<int>();
        this->yuv_range[1] = config["red_yuv_range"][1].as<int>();
    }
    }
    return true;
}

void Net_Detector_Params::print_show_params()
{
    spdlog::info(" conf_thres: {}, agnostic: {}, iou_thres: {}, max_det: {} , white_num_thresh: {}",
                  this->conf_thres, this->agnostic, this->iou_thres, this->max_det,this->white_num_thresh);
    spdlog::info(" yuv_range: {}, {}", this->yuv_range[0], this->yuv_range[1]);
    for (auto& enemy_car : this->enemy_car_list)
    {
        spdlog::info(" enemy_car_info: armor_name: {}, armor_nums: {}", enemy_car.armor_name, enemy_car.armor_nums);
    }
}

//***************************************************Net Detector****************************************************

Net_Detector::Net_Detector(Mode mode,
                 const std::string& net_config_folder,
                 bool if_yolov5,
                 std::vector<Enemy_Car_Info> enemy_car_list_,
                 const std::string& armor_color_):
                 mode(mode),
                 if_yolov5(if_yolov5),
                 params(enemy_car_list_),
                 armor_color(armor_color_),
                 params(armor_color_)
{
    this->params.load_params_from_yaml(net_config_folder+"/net_params.yaml");
    if (if_yolov5) 
    {
        this->yolo_engine = new TRTModule(net_config_folder+"/opt4.onnx");
        this->class_info = YAML::LoadFile(net_config_folder+"/classifier_class.yaml");
        
    }
    else
    {
        this->engine = new TRT_Engine(net_config_folder+"/classifier.trt",net_config_folder+"/net_params.yaml");
        this->class_info = YAML::LoadFile(net_config_folder+"/classifier_class.yaml");
        this->class_num = this->class_info.size();
    } 
}



std::vector<detect_result_t> Net_Detector::operator()(const std::vector<cv::Mat>& bin_rois,const std::vector<std::vector<cv::Point2f>>& big_recs) const
{
    int batch_size ;
    std::vector<detect_result_t> results;
    float conf = 0.0;
    std::string class_name = "";
    float* output_buffer = (*engine)(bin_rois,batch_size);
    int i_max = (int)(batch_size * this->class_num);
    int count = 0;
    for (int i = 0; i < i_max ; i+=this->class_num)
    {  
        int max_idx = i;
        for (int j = i; j < i + this->class_num; j++){if (output_buffer[j] > output_buffer[max_idx])max_idx = j;}
        conf = softmax(output_buffer, i, this->class_num, max_idx);
        class_name = this->class_info[std::to_string(max_idx - i)].as<std::string>();
        bool if_in_target_list = false;
        for (auto& enemy_car : this->params.enemy_car_list)if (enemy_car.armor_name == class_name) if_in_target_list = true;                
        if (conf > this->params.conf_thres && if_in_target_list)
        {
            std::vector<float> tvec = {0, 0, 0};
            std::vector<float> rvec = {0, 0, 0};
            detect_result_t detect_result = {big_recs[count], conf, class_name,tvec,rvec};
            results.push_back(detect_result);
        }
        count++;
    }
    return results;
}
/**
 * @brief: yolov5 detector
 * @note: armor_name_string = color_id + tag_id, color_id 0 for blue, 1 for red,2 for gray, tag 0 for sentry, 1 for 1;
*/
std::vector<detect_result_t> Net_Detector::operator()(const cv::Mat& img_bgr) const
{
    std::vector<detect_result_t> results;
    auto yolo_results = (*yolo_engine)(img_bgr);
    for (auto& result : yolo_results)
    {
        bool if_in_target_list = false;
        std::string armor_name_string = std::to_string(result.color_id) + std::to_string(result.tag_id);
        if (result.confidence < this->params.conf_thres) continue;
        std::vector<cv::Point2f> big_rec;
        for (auto& point : result.pts) big_rec.push_back(cv::Point2f(point.x,point.y));
        
        for (auto& enemy_car : this->params.enemy_car_list)if (enemy_car.armor_name == armor_name_string) {if_in_target_list = true;break;}
        if (!if_in_target_list) continue;
        if (this->if_is_gray(img_bgr, big_rec)) continue;
        big_rec = extendRectangle(big_rec,0,0.3);
        std::vector<float> tvec = {0, 0, 0};
        std::vector<float> rvec = {0, 0, 0};
        results.push_back(detect_result_t{big_rec,result.confidence,armor_name_string,tvec,rvec});
    }
    return results;
}


bool Net_Detector::if_is_gray(const cv::Mat& img_bgr, std::vector<cv::Point2f>& big_rec) 
{
    cv::Mat roi;
    cv::Size roi_shape_ = {32,32};
    order_points(big_rec);
    // pick up roi and perspective transform
    std::vector<cv::Point2f> dst_points;
    dst_points.push_back(cv::Point2f(0, 0));
    dst_points.push_back(cv::Point2f(roi_shape[0] - 1, 0));
    dst_points.push_back(cv::Point2f(roi_shape[0] - 1, roi_shape[1] - 1));
    dst_points.push_back(cv::Point2f(0, roi_shape[1] - 1));
    cv::Mat trans_mat = cv::getPerspectiveTransform(big_rec, dst_points);
    cv::warpPerspective(img_bgr, roi, trans_mat, roi_shape_, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    std::vector<cv::Mat> img_split(3);
    cv::cvtColor(roi, roi, cv::COLOR_BGR2YUV);
    cv::split(roi, img_split);
    if (this->armor_color == "red") cv::inRange(img_split[2], this->params.yuv_range[0], this->params.yuv_range[1], img_split[0]);
    else                            cv::inRange(img_split[1], this->params.yuv_range[0], this->params.yuv_range[1], img_split[0]);
    int white_pixel_num = cv::countNonZero(img_split[0]);
    cv::imshow("split0", img_split[0]);
    cv::waitKey(1);
    std::cout << "white_pixel_num: " << white_pixel_num << std::endl;
    if (white_pixel_num < this->params.white_num_thresh) return true;
    else return false;
}

/*******************************PNP Solver********************************************************/

bool PNP_Params::load_params_from_yaml(const std::string& file_path)
{

    YAML::Node config = YAML::LoadFile(file_path);
    auto read_small_obj_points = config["small_obj_points"];
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[0][0].as<float>(), read_small_obj_points[0][1].as<float>(), read_small_obj_points[0][2].as<float>()));
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[1][0].as<float>(), read_small_obj_points[1][1].as<float>(), read_small_obj_points[1][2].as<float>()));
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[2][0].as<float>(), read_small_obj_points[2][1].as<float>(), read_small_obj_points[2][2].as<float>()));
    small_obj_points.push_back(cv::Point3f(read_small_obj_points[3][0].as<float>(), read_small_obj_points[3][1].as<float>(), read_small_obj_points[3][2].as<float>()));
    auto read_big_obj_points = config["big_obj_points"];
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[0][0].as<float>(), read_big_obj_points[0][1].as<float>(), read_big_obj_points[0][2].as<float>()));
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[1][0].as<float>(), read_big_obj_points[1][1].as<float>(), read_big_obj_points[1][2].as<float>()));
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[2][0].as<float>(), read_big_obj_points[2][1].as<float>(), read_big_obj_points[2][2].as<float>()));
    big_obj_points.push_back(cv::Point3f(read_big_obj_points[3][0].as<float>(), read_big_obj_points[3][1].as<float>(), read_big_obj_points[3][2].as<float>()));

    mtx.at<float>(0,0) = config["mtx"][0][0].as<float>();
    mtx.at<float>(0,1) = config["mtx"][0][1].as<float>();
    mtx.at<float>(0,2) = config["mtx"][0][2].as<float>();
    mtx.at<float>(1,0) = config["mtx"][1][0].as<float>();
    mtx.at<float>(1,1) = config["mtx"][1][1].as<float>();
    mtx.at<float>(1,2) = config["mtx"][1][2].as<float>();
    mtx.at<float>(2,0) = config["mtx"][2][0].as<float>();
    mtx.at<float>(2,1) = config["mtx"][2][1].as<float>();
    mtx.at<float>(2,2) = config["mtx"][2][2].as<float>();

    dist.at<float>(0,0) = config["dist"][0][0].as<float>();
    dist.at<float>(0,1) = config["dist"][0][1].as<float>();
    dist.at<float>(0,2) = config["dist"][0][2].as<float>();
    dist.at<float>(0,3) = config["dist"][0][3].as<float>();
    dist.at<float>(0,4) = config["dist"][0][4].as<float>();
    auto read_small_armor_name_list = config["small_armor_name_list"];
    for (size_t i = 0; i < read_small_armor_name_list.size(); i++)
    {
        small_armor_name_list.push_back(read_small_armor_name_list[i].as<std::string>());
    }
    
    return true;



}
void PNP_Params::print_show_params()
{
    /*

    spdlog::info("mtx: {}, {}, {}, dist: {}, {}, {}, {}, {}",
                 mtx.at<float>(0,0), mtx.at<float>(0,1), mtx.at<float>(0,2),
                 mtx.at<float>(1,0), mtx.at<float>(1,1), mtx.at<float>(1,2),
                 mtx.at<float>(2,0), mtx.at<float>(2,1), mtx.at<float>(2,2));
    spdlog::info("dist: {}, {}",
                 dist.at<float>(0,0), dist.at<float>(0,1), dist.at<float>(0,2),
                 dist.at<float>(0,3), dist.at<float>(0,4));
    spdlog::info("small_armor_name_list: {}", small_armor_name_list);
    */

    for (auto& point : small_obj_points)
    {
        spdlog::info("small_obj_points: x={}, y={}, z={}", point.x, point.y, point.z);
    }
    for (auto& small_armor_name : small_armor_name_list)
    {
        spdlog::info("small_armor_name: {}", small_armor_name);
    }
    for (auto& point : big_obj_points)
    {
        spdlog::info("big_obj_points: x={}, y={}, z={}", point.x, point.y, point.z);
    }
    
    spdlog::info("camera_center: x={}, y={}", camera_center.x, camera_center.y);
    std::cout << "[INFO] : mtx: "<<mtx<<std::endl;
    std::cout << "[INFO] : dist: "<<dist<<std::endl;
    return;
}


void PNP_Solver::operator()(std::vector<detect_result_t>& detect_results) const
{

    for (auto& detect_result : detect_results)
    {
        bool if_small_armor = false;
        std::vector<cv::Point2f> real_img_points;
        for (auto& armor_name : params.small_armor_name_list)if (armor_name == detect_result.result) {if_small_armor = true;break;}
        for (auto& point: detect_result.big_rec)
        {
            real_img_points.push_back(cv::Point2f(point.x/shrink_scale_x,point.y/shrink_scale_y));
        }
        if (if_small_armor)cv::solvePnP(params.small_obj_points,real_img_points,params.mtx,params.dist,detect_result.rvec,detect_result.tvec);
        else cv::solvePnP(params.big_obj_points,real_img_points,params.mtx,params.dist,detect_result.rvec,detect_result.tvec);

        cv::Point2f center = cv::Point2f(0,0);
        for (auto& point : detect_result.big_rec)
        {
            center.x += point.x/4.f;
            center.y += point.y/4.f;
        }
        float delta_x = center.x - params.camera_center.x;
        float delta_z = params.camera_center.y - center.y;

        // doing nothing
        detect_result.tvec[0] = detect_result.tvec[0]/1000.f;
        float tvec1 = detect_result.tvec[1];
        detect_result.tvec[1] = detect_result.tvec[2]/1000.f;
        detect_result.tvec[2] = -tvec1/1000.f;

        float rvec1 = detect_result.rvec[1];
        detect_result.rvec[1] = detect_result.rvec[2];
        detect_result.rvec[2] = -rvec1;
        // only use deep from pnp solver
        detect_result.tvec[0] = delta_x;
        detect_result.tvec[2] = delta_z;
    }
   
}
