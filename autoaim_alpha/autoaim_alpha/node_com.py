from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from .port_slavedevice.port import *
import time


class Node_Com(Node,Custom_Context_Obj):

    def __init__(self,
                 name
                 ):
        
        super().__init__(name)
        

        self.port = Port(node_com_mode,
                         port_config_yaml_path)
        if if_main_head:
            self.port2 = Port(node_com_mode,
                            port2_config_yaml_path)
        self.bro_port = Port()
        if node_com_mode == 'Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)

        self.last_sub_topic_time = 0
        self.if_first_recv_from_ele_sys = True
        self.zero_unix_time = None
        
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        
        self.ele_sys_state_pub = self.create_publisher(topic_electric_sys_state['type'],
                                        topic_electric_sys_state['name'],
                                        topic_electric_sys_state['qos_profile'])
        
        self.subscription_ = self.create_subscription(topic_electric_sys_com['type'],
                                                      topic_electric_sys_com['name'],
                                                      self.listener_callback,
                                                      topic_electric_sys_com['qos_profile'])

        self.timer_recv_msg = self.create_timer(1/recv_from_ele_sys_freq, self.timer_recv_msg_callback)
        
        
    def listener_callback(self, msg: ElectricsysCom):
        if self.zero_unix_time is None and gimbal_action_mode != 4:
            self.get_logger().debug(f"No zero_unix_time, waiting for connection")
            return
        
        if msg.sof == 'A' :
            self.last_sub_topic_time = time.time()
            self.port.action_data.fire_times = msg.fire_times
            self.port.action_data.abs_pitch = msg.target_abs_pitch / np.pi * 180
            self.port.action_data.abs_yaw = msg.target_abs_yaw / np.pi * 180
            self.port.action_data.reserved_slot = msg.reserved_slot
            if node_com_mode == 'Dbg':
                self.get_logger().debug(f"SOF A from Decision maker : abs_pitch {msg.target_abs_pitch:.3f}, abs_yaw {msg.target_abs_yaw:.3f}")
            
            self.port.send_msg()
        else:
            self.get_logger().error(f"Unknown sof {msg.sof}")
        
    def init_synchronization_time(self):
        self.zero_unix_time = time.time()
        self.declare_parameter('zero_unix_time',self.zero_unix_time)
        if node_com_mode == 'Dbg':
            self.get_logger().debug(f"First Recv from electric sys, init synchronization time : zero_unix_time {self.zero_unix_time:.3f}")
        
    
    def timer_recv_msg_callback(self):
        
        if_error, cur_yaw, cur_pitch, reserved_slot = self.port.recv_feedback()
        cur_yaw = cur_yaw / 180 * np.pi
        cur_pitch = cur_pitch / 180 * np.pi
        if if_error:
            if self.port.ser is not None:
                self.get_logger().error(f"Com receive error, may be nullbyte or crc error")
            else:
                pass 
                #self.get_logger().error(f"Com port {self.port.params.port_abs_path} cannot open")
        else:
            if self.if_first_recv_from_ele_sys:
                self.if_first_recv_from_ele_sys = False
                self.init_synchronization_time()
                msg = ElectricsysState()
            
                msg.cur_pitch = cur_pitch
                msg.cur_yaw = cur_yaw
                msg.sentry_health = reserved_slot
                self.ele_sys_state_pub.publish(msg)
                
            else:
                msg = ElectricsysState()
                msg.cur_pitch = cur_pitch
                msg.cur_yaw = cur_yaw
                msg.sentry_health = reserved_slot
                self.ele_sys_state_pub.publish(msg)
            
            
        
    def _start(self):
        #self.port.port_open()
        
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        
        self.port.port_close()
        
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):

        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
        
    

def main(args=None):
    rclpy.init(args=args)
    
    node = Node_Com(node_com_name)
    with Custome_Context(node_com_name,node,[KeyboardInterrupt]):
        rclpy.spin(node=node)
        
    
    rclpy.shutdown()

if __name__ == '__main__':
    main()
