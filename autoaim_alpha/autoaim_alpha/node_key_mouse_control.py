from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from autoaim_interface.msg import *
import pynput

class Node_Key_Mouse_Control(Node,Custom_Context_Obj):

    def __init__(self,
                 name,
                 topic:dict,
                 frequency:Union[float,None] = None):
        super().__init__(name)
        
        
        self.publisher_ = self.create_publisher(topic['type'],
                                                topic['name'],
                                                topic['qos_profile'])
        
        
        
        self.subscription_ = self.create_subscription(topic_armor_pos_without_correct['type'],
                                                      topic['name'],
                                                      self.listener_callback,
                                                      topic['qos_profile'])
                                        

        

        self.timer = self.create_timer(1/frequency, self.timer_callback)

        self.mouse_listener = pynput.mouse.Listener(on_move=self.__on_move,
                                                    on_click=self.__on_click,
                                                    on_scroll=self.__on_scroll)
        self.mouse_listener.start()

        self.keyboard_listener = pynput.keyboard.Listener(on_press=self.__on_press,
                                                          on_release=self.__on_release)
        self.keyboard_listener.start()



    def listener_callback(self, msg:TFMessage):
        for tf in msg.transforms:
            self.get_logger().info(tf)


    def timer_callback(self):
        msg = ...

        self.publisher_.publish(msg)
        self.get_logger().info(f"Published msg: {msg}")
    
    
    def _start(self):
        
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):
        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
        
     


def main(args=None):
    rclpy.init(args=args)
    
    my_node = Node_Key_Mouse_Control(node_publish_name,topic_pos,node_publish_frequency)
    
    with Custome_Context(node_publish_name,my_node):
        rclpy.spin(my_node)
        
    
    rclpy.shutdown()

if __name__ == '__main__':
    main()
