#include "rclcpp/rclcpp.hpp"

class HelloWorldNode : public rclcpp::Node
{
public:
    HelloWorldNode()
    : Node("hello_world_node")
    {
        // 在构造函数中打印消息
        int a = 10;
        RCLCPP_INFO(this->get_logger(), "Hello, ROS 2! a = %d", a);
        
    }
};

int main(int argc, char * argv[])
{
    // 初始化 ROS 2 节点
    rclcpp::init(argc, argv);

    // 创建一个 ROS 2 节点
    auto node = std::make_shared<HelloWorldNode>();

    // 进入 ROS 2 事件循环
    rclcpp::spin(node);

    // 关闭 ROS 2 节点
    rclcpp::shutdown();

    return 0;
}
