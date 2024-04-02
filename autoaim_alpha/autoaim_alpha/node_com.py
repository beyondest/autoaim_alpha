from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from .port_slavedevice.port import *
import time

# 1000 means no meaning data, 2000 means i find friend, < 999 means meaningful data

class Node_Com(Node,Custom_Context_Obj):

    def __init__(self,
                 name
                 ):
        
        super().__init__(name)
        self.port = Port(node_com_mode,
                         port_config_yaml_path)
        if not if_ignore_brother:
            self.bro_port = Can_Port(node_com_mode,
                                    port2_config_yaml_path)
        if if_main_head:
            self.big_gimbal_port = Port(node_com_mode,
                            port3_config_yaml_path)
       
        if node_com_mode == 'Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)

        self.last_sub_topic_time = 0
        self.if_first_recv_from_ele_sys = True
        self.if_first_recv_from_big_gimbal = True
        self.if_first_recv_from_bro = True
        self.zero_unix_time = None
        
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        
        self.ele_sys_state_pub = self.create_publisher(topic_electric_sys_state['type'],
                                        topic_electric_sys_state['name'],
                                        topic_electric_sys_state['qos_profile'])
        self.bro_info_pub = self.create_publisher(topic_bro_info['type'],
                                        topic_bro_info['name'],
                                        topic_bro_info['qos_profile'])
        self.sub_ele_sys_com = self.create_subscription(topic_electric_sys_com['type'],
                                                      topic_electric_sys_com['name'],
                                                      self.sub_ele_sys_com_callback,
                                                      topic_electric_sys_com['qos_profile'])
        if if_main_head:
            self.elebig_gimbal_pub = self.create_publisher(topic_elebig_gimbal['type'],
                                            topic_elebig_gimbal['name'],
                                            topic_elebig_gimbal['qos_profile'])
            
        self.timer_recv_msg = self.create_timer(1/recv_from_ele_sys_freq, self.timer_recv_msg_callback)
        
    def sub_ele_sys_com_callback(self, msg: ElectricsysCom):
        if self.zero_unix_time is None and gimbal_action_mode != 4:
            self.get_logger().debug(f"No zero_unix_time, waiting for connection")
            return
        
        if msg.sof == 'A' :
            # Small Gimbal Action
            self.last_sub_topic_time = time.time()
            self.port.action_data.fire_times = msg.fire_times
            # Call Brother I Found Friend
            if not if_ignore_brother:
                if msg.target_abs_yaw > 2000:
                    self.bro_port.bro_data_send.find_enemy_yaw = msg.target_abs_yaw
                    self.bro_port.bro_data_send.cur_big_gimbal_yaw = 1000.0
                    self.bro_port.send_msg()
                    msg.target_abs_yaw = 0.0
            
            self.port.action_data.abs_yaw = msg.target_abs_yaw / np.pi * 180
            self.port.action_data.abs_pitch = msg.target_abs_pitch / np.pi * 180
            
            if node_com_mode == 'Dbg':
                self.get_logger().debug(f"SOF A from Decision maker : abs_pitch {msg.target_abs_pitch:.3f}, abs_yaw {msg.target_abs_yaw:.3f}")
            
            self.port.send_msg()
            
            # Call Brother Where Enemy is
            if not if_ignore_brother:
                if msg.fire_times != 0:
                    self.bro_port.bro_data_send.find_enemy_yaw = msg.target_abs_yaw
                else:
                    self.bro_port.bro_data_send.find_enemy_yaw = 1000.0
                self.bro_port.bro_data_send.cur_big_gimbal_yaw = 1000.0
                self.bro_port.send_msg()
            
            # Big Gimbal Action
            if if_main_head:
                self.big_gimbal_port.action_data.abs_yaw = msg.big_gimbal_yaw / np.pi * 180
                self.big_gimbal_port.action_data.reserved_slot = msg.reserved_slot
                self.big_gimbal_port.send_msg()
                
        else:
            self.get_logger().error(f"Unknown sof {msg.sof}")
        
    def init_synchronization_time(self):
        self.zero_unix_time = time.time()
        self.declare_parameter('zero_unix_time',self.zero_unix_time)
        if node_com_mode == 'Dbg':
            self.get_logger().debug(f"Connected to Small Gimbal, init synchronization time : zero_unix_time {self.zero_unix_time:.3f}")
        
    
    def timer_recv_msg_callback(self):
        ele_state_msg = ElectricsysState()
        bro_info_msg = BroInfo()
        big_gimbal_msg = EleBigGimbalState()
        
        
        # Small Gimbal Feedback
        if_error, cur_yaw, cur_pitch, reserved_slot = self.port.recv_feedback()
        cur_yaw = cur_yaw / 180 * np.pi
        cur_pitch = cur_pitch / 180 * np.pi
        if if_error:
            if self.port.ser is not None:
                self.get_logger().debug(f"Com receive error, may be nullbyte or crc error")
            else:
                pass 
        else:
            if self.if_first_recv_from_ele_sys:
                self.if_first_recv_from_ele_sys = False
                self.init_synchronization_time()
                ele_state_msg.cur_pitch = cur_pitch
                ele_state_msg.cur_yaw = cur_yaw
                
            else:
                ele_state_msg.cur_pitch = cur_pitch
                ele_state_msg.cur_yaw = cur_yaw
            self.ele_sys_state_pub.publish(ele_state_msg)
        
        # Brother Feedback  
        if not if_ignore_brother:  
            if_error,find_enemy_yaw,cur_big_gimbal_yaw_ = self.bro_port.recv_feedback()
            if if_error:
                if self.bro_port.can is not None:
                    self.get_logger().debug(f"Bro com receive error, may be nullbyte or crc error")
                else:
                    pass
            else:
                if self.if_first_recv_from_bro == True:
                    self.if_first_recv_from_bro = False
                    lr1.info(f"Connected to Brother")
                else:
                    pass
                bro_info_msg.find_enemy_yaw = find_enemy_yaw
                bro_info_msg.cur_big_gimbal_yaw = cur_big_gimbal_yaw_ 
                self.bro_info_pub.publish(bro_info_msg)
        
        # Big Gimbal Feedback
        if if_main_head:
            if_error_, cur_yaw_, cur_pitch_, reserved_slot_ = self.big_gimbal_port.recv_feedback()
            if if_error_:
                if self.big_gimbal_port.ser is not None:
                    self.get_logger().debug(f"Big gimbal com receive error, may be nullbyte or crc error")
                else:
                    pass
            else:
                if self.if_first_recv_from_big_gimbal:
                    self.if_first_recv_from_big_gimbal = False
                    lr1.info(f"Connected to Big Gimbal")
                else:
                    pass
                big_gimbal_msg.cur_big_gimbal_yaw = cur_yaw_ / 180 * np.pi
                big_gimbal_msg.sentry_health = reserved_slot_
                self.elebig_gimbal_pub.publish(big_gimbal_msg)
                
                # Call Brother Where Big Gimbal is         
                self.bro_port.bro_data_send.find_enemy_yaw = 1000.0
                self.bro_port.bro_data_send.cur_big_gimbal_yaw = cur_yaw_ / 180 * np.pi
                self.bro_port.send_msg()
                
                    
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
