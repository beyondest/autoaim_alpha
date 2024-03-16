from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
import cv2
def for_trackbar(x):
    pass

class Node_Parameter(Node,Custom_Context_Obj):

    def __init__(self,
                 name):
        
        
        super().__init__(name)
        


        self.window_name = 'config'
        cv2.createTrackbar('kp_1000',self.window_name,-2000,2000,for_trackbar)
        cv2.createTrackbar('ki_1000',self.window_name,-2000,2000,for_trackbar)
        cv2.createTrackbar('kd_1000',self.window_name,-2000,2000,for_trackbar)
        cv2.setTrackbarPos('kp_1000',self.window_name,int(self.kp*1000))
        cv2.setTrackbarPos('ki_1000',self.window_name,int(self.ki*1000))
        cv2.setTrackbarPos('kd_1000',self.window_name,int(self.kd*1000))
        
        self.get_logger().info(f"kp:{self.kp},ki:{self.ki},kd:{self.kd}")
    
        self.create_timer(0.01,self.update_parameter)
    
    def update_parameter(self):
        kp_1000 = cv2.getTrackbarPos('kp_1000',self.window_name)
        ki_1000 = cv2.getTrackbarPos('ki_1000',self.window_name)
        kd_1000 = cv2.getTrackbarPos('kd_1000',self.window_name)
        self.kp = kp_1000/1000
        self.ki = ki_1000/1000
        self.kd = kd_1000/1000
        
        self.set_parameters([
            ('kp', self.kp),
            ('ki', self.ki),
            ('kd', self.kd)
            ])
        
        
        if node_parameter_mode == 'Dbg':
            self.get_logger().debug(f"kp:{self.kp},ki:{self.ki},kd:{self.kd}")

    def _start(self):
        cv2.namedWindow(self.window_name,cv2.WINDOW_AUTOSIZE)
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        cv2.destroyAllWindows()
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):
        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
        
        

def main(args=None):
    rclpy.init(args=args)
    
    node = Node_Parameter(node_parameter_name)
    
    with Custome_Context(node):
        rclpy.spin(node)
        
    
    rclpy.shutdown()

if __name__ == '__main__':
    main()
