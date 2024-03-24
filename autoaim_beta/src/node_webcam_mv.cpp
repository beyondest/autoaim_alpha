#include <chrono>
#include <functional>
#include <memory>
#include <string>


#include "rclcpp/rclcpp.hpp"

#include "std_msgs/msg/string.hpp"
#include "autoaim_interface/msg/detect_result.hpp"
using namespace std::chrono_literals;

class Node_Webcam_MV : public rclcpp::Node
{

private:

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_ ;
    rclcpp::Publisher<autoaim_interface::msg::DetectResult>::SharedPtr pub_detect_result_;
    rclcpp::TimerBase::SharedPtr timer_;
    int count_;
    void timer_callback()
    {
       auto msg = autoaim_interface::msg::DetectResult();
       auto msg_each = autoaim_interface::msg::EachDetectResult();
       
    }
public:

    Node_Webcam_MV()
    : Node("node_webcam_mv"),count_(0)
    {
        pub_= this->create_publisher<std_msgs::msg::String>("topic", 10);
        timer_ = this->create_wall_timer(500ms, std::bind(&Node_Webcam_MV::timer_callback, this));
    }
    
};



int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Node_Webcam_MV>());
    rclcpp::shutdown();
    return 0;
}




