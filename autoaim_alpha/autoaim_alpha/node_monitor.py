import rclpy
from rclpy.node import Node
from rclpy.executors import SingleThreadedExecutor
from rclpy.exceptions import InvalidNodeNameException
from launch import LaunchDescription
from launch.actions import EmitEvent
from launch.event_handlers import OnProcessExit
from launch_ros.actions import Node as LaunchNode
from launch import LaunchService
from launch.event_handlers import OnShutdown
from launch.actions import RegisterEventHandler

class NodeMonitor(Node):

    def __init__(self):
        super().__init__('node_monitor')
        self.node_names = set()
        self.node_to_restart = 'node_webcam'  # Replace with your node name
    
        self.create_timer(3.0, self.check_node_status)

    def check_node_status(self):
        try:
            node_names = self.get_node_names()
            if self.node_to_restart not in node_names:
                self.get_logger().error(f"Node {self.node_to_restart} has crashed. Restarting...")
                self.restart_node()
        except Exception as e:
            self.get_logger().error(f"Node {self.node_to_restart} has crashed error { e}. Restarting...")
            self.restart_node()

    def restart_node(self):
        ld = LaunchDescription([
            LaunchNode(
                package='autoaim_beta',
                node_executable=self.node_to_restart,
                node_name=self.node_to_restart,
                output='log'
            )
        ])
        ls = LaunchService(argv=[], debug=True)
        ls.include_launch_description(ld)
        ls.run()

def main(args=None):
    rclpy.init(args=args)
    monitor = NodeMonitor()
    try:
        rclpy.spin(monitor)
    except KeyboardInterrupt:
        pass

    monitor.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
