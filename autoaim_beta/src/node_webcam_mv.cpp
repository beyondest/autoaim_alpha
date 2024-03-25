#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "autoaim_interface/msg/detect_result.hpp"
#include "basic.hpp"
#include <yaml-cpp/yaml.h>
#include "detector.hpp"
#include "mv_class.hpp"

#include <sensor_msgs/msg/image.hpp>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>

std::string general_config_yaml_path = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/general_params.yaml";
std::string camera_config_folder = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/camera_config";
std::string log_save_folder = "/home/rcclub/.ros/log/custom_log";
std::string tradition_config_folder = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/tradition_config";
std::string net_config_folder = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/net_config";
std::string pnp_config_params_path = "/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/other_config/pnp_params.yaml";
using namespace std::chrono_literals;

class Node_Webcam_MV : public rclcpp::Node
{

private:
    int fps = 0;
    Mode mode = Mode::Rel;
    bool if_reverse = false;
    bool if_yolov5 = false;
    bool if_show_img_local = false;
    bool if_show_img_remote = false;
    int img_show_wid = 640;
    int img_show_hei = 384;
    cv::Mat* img = nullptr;
    cv::Mat* img_show = nullptr;
    rclcpp::Publisher<autoaim_interface::msg::DetectResult>::SharedPtr pub_detect_result_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_img_for_visualize_;
    rclcpp::TimerBase::SharedPtr timer_;
    Tradition_Detector* tradition_detector = nullptr;
    Mindvision_Camera* mv = nullptr;
    Net_Detector* net_detector = nullptr;
    PNP_Solver* pnp_solver = nullptr;
    std::chrono::time_point<std::chrono::high_resolution_clock> pre_t = std::chrono::high_resolution_clock::now();

public:
    Node_Webcam_MV()
    : Node("node_webcam_mv")
    {
        YAML::Node general_config = YAML::LoadFile(general_config_yaml_path);
        std::string mode_ = general_config["mode"].as<std::string>();
        if (mode_ == "Dbg") mode = Mode::Dbg;
        auto armor_color = general_config["armor_color"].as<std::string>();
        RCLCPP_WARN(this->get_logger(), "ENEMY_CAR_COLOR: %s", armor_color.c_str());
        this->if_yolov5 = general_config["if_yolov5"].as<bool>();
        this->if_reverse = general_config["if_reverse_img"].as<bool>();
        this->img_show_wid = general_config["img_show_wid"].as<int>();
        this->img_show_hei = general_config["img_show_hei"].as<int>();
        int node_webcam_mv_ms = general_config["node_webcam_mv_ms"].as<int>();
        this->if_show_img_local = general_config["if_show_img_local"].as<bool>();
        this->if_show_img_remote = general_config["if_show_img_remote"].as<bool>();
        
        std::vector<Enemy_Car_Info> enemy_car_info_list;
        for (size_t i = 0; i < general_config["enemy_car_list"].size(); i++)
        {
            Enemy_Car_Info enemy_car_info;
            enemy_car_info.armor_name = general_config["enemy_car_list"][i]["armor_name"].as<std::string>();
            enemy_car_info.armor_nums = general_config["enemy_car_list"][i]["armor_nums"].as<int>();
            enemy_car_info_list.push_back(enemy_car_info);
            RCLCPP_WARN(this->get_logger(), "ENEMY_CAR_INFO: %s %d", enemy_car_info.armor_name.c_str(), enemy_car_info.armor_nums);
        }
        this->img = new cv::Mat(1024,1280,CV_8UC3);
        this->img_show = new cv::Mat(img_show_wid,img_show_hei,CV_8UC3);
        this->mv = new Mindvision_Camera(mode,camera_config_folder,false,armor_color,if_yolov5);
        if (!if_yolov5)
        {
            RCLCPP_WARN(this->get_logger(), " USE TRADITION METHOD");
            this->tradition_detector = new Tradition_Detector(mode,tradition_config_folder,armor_color);
        }
        else RCLCPP_WARN(this->get_logger(), "USE YOLOV5 METHOD");
        mv->open();
        this->net_detector = new Net_Detector(mode,net_config_folder,if_yolov5,enemy_car_info_list);
        this->pnp_solver = new PNP_Solver(mode,pnp_config_params_path,img_show_wid,img_show_hei);
        this->timer_ = this->create_wall_timer(std::chrono::milliseconds(node_webcam_mv_ms), std::bind(&Node_Webcam_MV::timer_callback, this));
        this->pub_detect_result_ = this->create_publisher<autoaim_interface::msg::DetectResult>("detect_result", 5); // must same as in __init__.py topic_detect_result
        this->pub_img_for_visualize_ = this->create_publisher<sensor_msgs::msg::Image>("img_for_visualize", 10); // must same as in __init__.py topic_img_for_visualize
        

        RCLCPP_INFO(this->get_logger(), "Node_Webcam_MV Start");

    }
    ~Node_Webcam_MV()
    {
        if (tradition_detector) delete tradition_detector;
        delete mv;
        delete net_detector;
        delete pnp_solver;
        delete img;
        delete img_show;
    }
private:
    void timer_callback()
    {
        auto msg = autoaim_interface::msg::DetectResult();
        auto msg_each = autoaim_interface::msg::EachDetectResult();
        mv->get_img(*img);
        cv::resize(*img,*img_show,cv::Size(img_show_wid,img_show_hei));
        if(if_reverse) cv::flip(*img_show,*img_show,-1);
        if(!if_yolov5)
        {
            std::vector<std::vector<cv::Point2f>> big_rec_list;
            std::vector<cv::Mat> roi_list;
            (*tradition_detector)(*img_show,big_rec_list,roi_list);
            if (big_rec_list.size() > 0) 
            {
                auto d_rsts =(*net_detector)(roi_list,big_rec_list);
                (*pnp_solver)(d_rsts);
                if(d_rsts.size() == 0) RCLCPP_INFO(this->get_logger(), "No Target Found");
                for (auto& rst : d_rsts)
                {
                    msg_each.armor_name = rst.result;
                    msg_each.confidence = rst.conf;
                    msg_each.pose.pose.position.x = rst.tvec[0];
                    msg_each.pose.pose.position.y = rst.tvec[1];
                    msg_each.pose.pose.position.z = rst.tvec[2];
                    msg.detect_result.push_back(msg_each);
                    RCLCPP_INFO(this->get_logger(), "Find Target : %s, conf: %.2f, x: %.3f, y: %.3f, z: %.3f", rst.result.c_str(), rst.conf, rst.tvec[0], rst.tvec[1], rst.tvec[2]);
                }
                if (mode == Mode::Dbg) visualize_results(*img_show,d_rsts);
            }
            else RCLCPP_INFO(this->get_logger(), "No Target Found");
        }
        else
        {
            auto d_rsts = (*net_detector)(*img_show);
            (*pnp_solver)(d_rsts);
            if(d_rsts.size() == 0) RCLCPP_INFO(this->get_logger(), "No Target Found");
            for (auto& rst : d_rsts)
            {
                msg_each.armor_name = rst.result;
                msg_each.confidence = rst.conf;
                msg_each.pose.pose.position.x = rst.tvec[0];
                msg_each.pose.pose.position.y = rst.tvec[1];
                msg_each.pose.pose.position.z = rst.tvec[2];
                msg.detect_result.push_back(msg_each);
                RCLCPP_INFO(this->get_logger(), "Find Target : %s, conf: %.2f, x: %.3f, y: %.3f, z: %.3f", rst.result.c_str(), rst.conf, rst.tvec[0], rst.tvec[1], rst.tvec[2]);
            }
            if (mode == Mode::Dbg) visualize_results(*img_show,d_rsts);

        }
        this->pub_detect_result_->publish(msg);
        auto cur_t = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(cur_t - pre_t).count();
        this->fps = 1000 / dur;
        pre_t = cur_t;
        if (mode == Mode::Dbg) RCLCPP_INFO(this->get_logger(), "FPS: %d", this->fps);
        if (if_show_img_local) 
        {
            cv::imshow("local_img",*img_show);
            cv::waitKey(1);
        }
        if (if_show_img_remote) 
        {
            sensor_msgs::msg::Image::SharedPtr msg_img = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", *img_show).toImageMsg();
            this->pub_img_for_visualize_->publish(*msg_img);
        }
    }
};



int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Node_Webcam_MV>());
    rclcpp::shutdown();
    spdlog::get("lr1")->flush();
    return 0;
}




