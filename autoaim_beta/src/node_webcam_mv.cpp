#include <chrono>
#include <functional>
#include <memory>
#include <string>


#include "rclcpp/rclcpp.hpp"

#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class Node_Webcam_MV : public rclcpp::Node
{
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_ ;
    rclcpp::TimerBase::SharedPtr timer_;
    int count_;
    
public:
    Node_Webcam_MV()
    : Node("node_webcam_mv"),count_(0)
    {
        
        pub_= this->create_publisher<std_msgs::msg::String>("topic", 10);
        timer_ = this->create_wall_timer(500ms, std::bind(&Node_Webcam_MV::timer_callback, this));
    }

private:
    void timer_callback()
    {
        auto msg = std_msgs::msg::String();
        msg.data = "Hello, world! " + std::to_string(count_);
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", msg.data.c_str());
        pub_->publish(msg);
        count_++;
    }
    
};


int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Node_Webcam_MV>());
    rclcpp::shutdown();
    return 0;
}




