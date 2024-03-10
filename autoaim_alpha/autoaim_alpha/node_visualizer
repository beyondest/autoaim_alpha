from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from cv_bridge import CvBridge
import cv2

class Node_Visualizer(Node,Custom_Context_Obj):

    def __init__(self,
                 name,
                ):
        super().__init__(name)
        
        self.subscription_ = self.create_subscription(topic_img_for_visualize['type'],
                                                      topic_img_for_visualize['name'],
                                                      self.img_for_vis_callback,
                                                      topic_img_for_visualize['qos_profile'])
                                        
        self.bridge = CvBridge()
        self.visualize_window_name = 'visualize_img'



    def img_for_vis_callback(self, msg:Image):
        
        self.get_logger().info(f"Received image for visualization")
        img = self.bridge.imgmsg_to_cv2(msg, desired_encoding=camera_output_format)
        cv2.imshow(self.visualize_window_name, img)
        cv2.waitKey(1)
        




    
    def _start(self):
        cv2.namedWindow(self.visualize_window_name, cv2.WINDOW_NORMAL)
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):
        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
        
        

def main(args=None):
    rclpy.init(args=args)
    
    node = Node_Visualizer(node_visualizer_name)
    
    with Custome_Context(node_visualizer_name,node,[KeyboardInterrupt]):
        rclpy.spin(node)
    
    
    rclpy.shutdown()

if __name__ == '__main__':
    main()
