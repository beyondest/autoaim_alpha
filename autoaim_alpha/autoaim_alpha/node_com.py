from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from .port_slavedevice.port import *
import time

# when first recv from ele_sys, set zero_unix_time = current time - ele_sys_time_already_run

class Node_Com(Node,Custom_Context_Obj):

    def __init__(self,
                 name
                 ):
        
        super().__init__(name)
        
        self.ele_sys_state_pub = self.create_publisher(topic_electric_sys_state['type'],
                                                topic_electric_sys_state['name'],
                                                topic_electric_sys_state['qos_profile'])
        
        self.subscription_ = self.create_subscription(topic_electric_sys_com['type'],
                                                      topic_electric_sys_com['name'],
                                                      self.listener_callback,
                                                      topic_electric_sys_com['qos_profile'])

        #self.timer_send_msg = self.create_timer(1/send_msg_freq, self.timer_send_msg_callback)
        self.timer_recv_msg = self.create_timer(1/recv_from_ele_sys_freq, self.timer_recv_msg_callback)
        
        self.port = Port(node_com_mode,
                         port_config_yaml_path)
        
        if node_com_mode == 'Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)

        #self.init_synchronization_time()
        self.last_sub_topic_time = 0
        self.if_first_recv_from_ele_sys = True
        self.zero_unix_time = None
        
        # When init, yaw is 0 in ele_sys, so we need to set it to 0 in this node
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        
    def listener_callback(self, msg: ElectricsysCom):
        if self.zero_unix_time is None:
            self.get_logger().debug(f"No zero_unix_time, waiting for connection")
            return
        
        if msg.sof == 'A' :
            self.last_sub_topic_time = time.time()
            self.port.action_data.fire_times = msg.fire_times
            self.port.action_data.abs_pitch_10000 = int(msg.target_abs_pitch * 10000)
            self.port.action_data.abs_yaw_10000 = int(msg.target_abs_yaw * 10000)  # due to int16 is from -32768 to 32767, so we need to convert angle to this range
            self.port.action_data.reserved_slot = msg.reserved_slot
            
            if msg.reach_unix_time == 0:
                self.port.action_data.target_minute = 0
                self.port.action_data.target_second = 0
                self.port.action_data.target_second_frac_10000 = 0
                
            else:
                minute, second, second_frac = TRANS_UNIX_TIME_TO_T(msg.reach_unix_time, self.zero_unix_time)
                self.port.action_data.target_minute = minute
                self.port.action_data.target_second = second
                self.port.action_data.target_second_frac_10000 = int(second_frac * 10000)
            
            if node_com_mode == 'Dbg':
                self.get_logger().debug(f"SOF A from Decision maker : abs_pitch {msg.target_abs_pitch:.3f}, abs_yaw {msg.target_abs_yaw:.3f}, reach at time {msg.reach_unix_time:.3f}")
            self.port.send_msg(msg.sof)
            self.port.action_data.abs_pitch_10000 = 0
            self.port.action_data.abs_yaw_10000 = 0 
            self.port.send_msg('A')

        elif msg.sof == 'S':
            cur_unix_time = time.time()
            self.last_sub_topic_time = cur_unix_time
            minute, second, second_frac = TRANS_UNIX_TIME_TO_T(cur_unix_time, self.zero_unix_time)
            self.port.syn_data.present_minute = minute
            self.port.syn_data.present_second = second
            self.port.syn_data.present_second_frac_10000 = int(second_frac * 10000)
            self.port.send_msg(msg.sof)
            
            if node_com_mode == 'Dbg':
                self.get_logger().debug(f"Sync time : {cur_unix_time:.3f}")
                self.get_logger().debug(f"SOF S from Decision maker")
            
        else:
            self.get_logger().error(f"Unknown sof {msg.sof}")
        
    def init_synchronization_time(self,ele_time_minute:int, ele_time_second:int, ele_time_second_frac:float):
        zero_unix_time = time.time()
        zero_unix_time -= ele_time_minute * 60 + ele_time_second + ele_time_second_frac 
        
        self.declare_parameter('zero_unix_time',zero_unix_time)
        self.zero_unix_time = zero_unix_time
        if node_com_mode == 'Dbg':
            self.get_logger().debug(f"First Recv from electric sys, init synchronization time : zero_unix_time {self.zero_unix_time:.3f}")
        
    def timer_send_msg_callback(self):
        if self.zero_unix_time is None:
            self.get_logger().debug(f"No zero_unix_time, waiting for connection")
            return
        cur_time = time.time()
        if cur_time - self.last_sub_topic_time > 0.5:
            
            # gimbal just has to stay location, so next_time = cur_time + communication_delay
            next_time = cur_time + self.port.params.communication_delay
            self.port.action_data.fire_times = 0
            self.port.action_data.abs_pitch_10000 = int(0 * 10000)
            self.port.action_data.abs_yaw_10000 = int(0 * 10000)  # due to int16 is from -32768 to 32767, so we need to convert angle to this range
            self.port.action_data.reserved_slot = 0
            minute, second, second_frac = TRANS_UNIX_TIME_TO_T(next_time, self.zero_unix_time)
            self.port.action_data.target_minute = minute
            self.port.action_data.target_second = second
            self.port.action_data.target_second_frac_10000 = int(second_frac * 10000)
            self.port.send_msg('A')
            if node_com_mode == 'Dbg':
                self.get_logger().warn(f"Decision too old, you should control gimbal all time, last sub time {self.last_sub_topic_time:.3f}, cur_time {cur_time:.3f}, remain cur pos")
            
        else:
            
            self.port.send_msg('A')
            if node_com_mode == 'Dbg':
                self.get_logger().debug(f"Obey , cur pos(p,y) {self.cur_pitch:.3f}, {self.cur_yaw:.3f}, target_pos : {self.port.action_data.abs_pitch_10000/10000:.3f}, {self.port.action_data.abs_yaw_10000/10000:.3f}")
            
    def timer_recv_msg_callback(self):
        
        if_error, current_yaw, current_pitch, cur_time_minute, cur_time_second, cur_time_second_frac = self.port.recv_feedback()
        if if_error:
            if self.port.ser is not None:
                self.get_logger().error(f"Com receive crc error")
            else:
                pass 
                #self.get_logger().error(f"Com port {self.port.params.port_abs_path} cannot open")
        else:
            if self.if_first_recv_from_ele_sys:
                self.if_first_recv_from_ele_sys = False
                self.init_synchronization_time(cur_time_minute, cur_time_second, cur_time_second_frac)
                msg = ElectricsysState()
            
                msg.unix_time = self.zero_unix_time
                msg.cur_pitch = current_pitch
                msg.cur_yaw = current_yaw
                self.ele_sys_state_pub.publish(msg)
                
            else:
                msg = ElectricsysState()
                msg.unix_time = TRANS_T_TO_UNIX_TIME(cur_time_minute, cur_time_second, cur_time_second_frac, self.zero_unix_time)
                msg.cur_pitch = current_pitch
                msg.cur_yaw = current_yaw
                self.cur_yaw = current_yaw
                self.cur_pitch = current_pitch
                
                self.ele_sys_state_pub.publish(msg)
            
            
            #if node_com_mode == 'Dbg':
            #   self.get_logger().debug(f"Recv from electric sys state p: {msg.cur_pitch:.3f}, y: {msg.cur_yaw:.3f}, t:{msg.unix_time:.3f}")
        
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
