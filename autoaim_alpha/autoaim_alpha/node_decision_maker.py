from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from .decision_maker.ballistic_predictor import *
from .decision_maker.decision_maker import *

yaw_left = -np.pi
yaw_right = np.pi
yaw_step = 0.01
pitch_down = -0.3491    # -20
pitch_up = 1.2227       # +70
pitch_step = 0.01

yaw_test_data = np.round(np.arange(yaw_left,yaw_right,yaw_step),3)

pitch_test_data = np.r_[(np.round(np.arange(pitch_down,pitch_up,pitch_step),3)),np.round(np.arange(pitch_up,pitch_down,-pitch_step)),np.round(np.arange(pitch_down,0,pitch_step),3)]

class Node_Decision_Maker(Node,Custom_Context_Obj):

    def __init__(self,
                 name):
        super().__init__(name)
        self.action_mode_to_callback = {0:self.make_decision_callback,
                                1:self.repeat_recv_from_ele_callback,
                                2:self.test_yaw_callback,
                                3:self.test_pitch_callback,
                                4:self.doing_nothing_callback}
        
        self.action_mode_to_note = {0:"Make decision",
                                1:"Repeat recv from ele",
                                2:"Test yaw",
                                3:"Test pitch",
                                4:"Doing nothing"}
        
        self.pub_ele_sys_com = self.create_publisher(topic_electric_sys_com['type'],
                                                topic_electric_sys_com['name'],
                                                topic_electric_sys_com['qos_profile'])

        self.pub_show = self.create_publisher(topic_show['type'],
                                        topic_show['name'],
                                        topic_show['qos_profile'])

        self.sub_ele_sys_state = self.create_subscription(topic_electric_sys_state['type'],
                                                      topic_electric_sys_state['name'],
                                                      self.recv_from_ele_sys_callback,
                                                      topic_electric_sys_state['qos_profile'])
        
        self.sub_armor_pos_list = self.create_subscription(topic_armor_pos_corrected_list['type'],
                                                        topic_armor_pos_corrected_list['name'],
                                                        self.sub_armor_pos_list_callback,
                                                        topic_armor_pos_corrected_list['qos_profile'])

        self.decision_maker = Decision_Maker(node_decision_maker_mode,
                                             decision_maker_params_yaml_path,
                                             ballistic_predictor_config_yaml_path,
                                             enemy_car_list)


        
        self.get_logger().warn(f"Use gimbal_action_mode {self.action_mode_to_note[gimbal_action_mode]}")
        
        if if_ignore_elesys:
            self.decision_maker.electric_system_zero_unix_time = time.time()
            self.get_logger().warn(f"Ignore electric system, init zero_unix_time {self.decision_maker.electric_system_zero_unix_time}")
            self.if_connetect_to_ele_sys = True
            
        else:
            self.if_connetect_to_ele_sys = False
        
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)
            
        self.yaw_test_idx = 0
        self.pitch_test_idx = 0

    def recv_from_ele_sys_callback(self, msg:ElectricsysState):
        
        if self.if_connetect_to_ele_sys == False:
            self.if_connetect_to_ele_sys = True
            self.decision_maker.electric_system_zero_unix_time = msg.unix_time
            self.get_logger().info(f"Connect to electric system, zero_unix_time {msg.unix_time}, cur_time {time.time()}")
            
        self.decision_maker.update_our_side_info(
                                                 cur_yaw=msg.cur_yaw,
                                                 cur_pitch=msg.cur_pitch,
                                                 ele_unix_time=msg.unix_time,
                                                 remaining_health=None,
                                                 remaining_ammo=None)
        
        
    def sub_armor_pos_list_callback(self, msg:ArmorPosList):
        for armor_pos in msg.armor_pos_list:
            target_pos_in_camera_frame = np.array([armor_pos.pose.pose.position.x,
                                                    armor_pos.pose.pose.position.y,
                                                    armor_pos.pose.pose.position.z])
            
            q = Quaternion(armor_pos.pose.pose.orientation.w,
                        armor_pos.pose.pose.orientation.x,
                        armor_pos.pose.pose.orientation.y,
                        armor_pos.pose.pose.orientation.z)
            
            rvec = q.get_axis() * q.angle
            
            self.decision_maker.update_enemy_side_info(armor_pos.armor_name,
                                                    armor_pos.armor_id,
                                                    target_pos_in_camera_frame,
                                                    rvec,
                                                    armor_pos.confidence,
                                                    armor_pos.pose.header.stamp.sec + armor_pos.pose.header.stamp.nanosec/1e9,
                                                    armor_pos.continuous_detected_num,
                                                    armor_pos.continuous_lost_num,
                                                    armor_pos.if_update
                                                    )
            
        self.action_mode_to_callback[gimbal_action_mode]()
        
        
        
    def make_decision_callback(self):
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        next_yaw,next_pitch,fire_times = self.decision_maker.make_decision() 

        com_msg.reach_unix_time = self.decision_maker.electric_system_unix_time
        com_msg.target_abs_pitch = next_pitch
        com_msg.target_abs_yaw = next_yaw
        com_msg.sof = 'A'
        com_msg.reserved_slot = 0
        com_msg.fire_times = fire_times
        
        self.pub_ele_sys_com.publish(com_msg)
        
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().debug(f"Make decision : fire_times {com_msg.fire_times}  target_abs_yaw {com_msg.target_abs_yaw:.3f},target_abs_pitch {com_msg.target_abs_pitch:.3f} reach_unix_time {com_msg.reach_unix_time:.3f}")
       
    def repeat_recv_from_ele_callback(self):
        
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        self.get_logger().debug(f"Get : {self.decision_maker.electric_system_unix_time}, {type(self.decision_maker.electric_system_unix_time)}")
        self.get_logger().debug(f"Get : {self.decision_maker.cur_pitch}, {type(self.decision_maker.cur_pitch)}")
        self.get_logger().debug(f"Get : {self.decision_maker.cur_yaw}, {type(self.decision_maker.cur_yaw)}")

        com_msg.reach_unix_time = self.decision_maker.electric_system_unix_time
        com_msg.target_abs_pitch = self.decision_maker.cur_pitch
        com_msg.target_abs_yaw = self.decision_maker.cur_yaw
        com_msg.sof = 'A'
        com_msg.reserved_slot = 0
        com_msg.fire_times = 0
        
        self.pub_ele_sys_com.publish(com_msg)
        
    def test_yaw_callback(self):
        
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        abs_yaw, abs_pitch = self.decision_maker._search_target()
        com_msg.reach_unix_time = self.decision_maker.electric_system_unix_time
        com_msg.target_abs_pitch = abs_pitch
        com_msg.target_abs_yaw = abs_yaw
        com_msg.sof = 'A'
        com_msg.reserved_slot = 0
        com_msg.fire_times = 0
        self.yaw_test_idx += 1
        if self.yaw_test_idx >= len(yaw_test_data):
            self.yaw_test_idx = 0
        
        self.pub_ele_sys_com.publish(com_msg)
        
    def test_pitch_callback(self):
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        com_msg.reach_unix_time = self.decision_maker.electric_system_unix_time
        com_msg.target_abs_pitch = pitch_test_data[self.pitch_test_idx]
        com_msg.target_abs_yaw = 0.0
        com_msg.sof = 'A'
        com_msg.reserved_slot = 0
        com_msg.fire_times = 0
        self.pitch_test_idx += 1
        if self.pitch_test_idx >= len(pitch_test_data):
            self.pitch_test_idx = 0
        
        self.pub_ele_sys_com.publish(com_msg)
    
    def doing_nothing_callback(self):
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysState()
        
        target_armor = self.decision_maker.choose_target()
        next_yaw,next_pitch,fire_times = self.decision_maker.make_decision(target_armor) 
       
        com_msg.cur_yaw = next_yaw
        com_msg.cur_pitch = next_pitch
        com_msg.unix_time = time.time()
        
        self.pub_show.publish(com_msg)

        if node_decision_maker_mode == 'Dbg':
            self.get_logger().debug(f"Choose Target {target_armor.name} id {target_armor.id} tvec {target_armor.tvec} rvec {target_armor.rvec} time {target_armor.time} ")
        self.get_logger().debug(f"Make decision : target_abs_pitch {com_msg.cur_pitch:.3f} target_abs_yaw {com_msg.cur_yaw:.3f} reach_unix_time {com_msg.unix_time:.3f}")
       
    def repeat_recv_from_ele_callback(self):
        
        
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        self.get_logger().debug(f"Get : {self.decision_maker.electric_system_unix_time}, {type(self.decision_maker.electric_system_unix_time)}")
        self.get_logger().debug(f"Get : {self.decision_maker.cur_pitch}, {type(self.decision_maker.cur_pitch)}")
        self.get_logger().debug(f"Get : {self.decision_maker.cur_yaw}, {type(self.decision_maker.cur_yaw)}")

        com_msg.reach_unix_time = self.decision_maker.electric_system_unix_time
        com_msg.target_abs_pitch = self.decision_maker.cur_pitch
        com_msg.target_abs_yaw = self.decision_maker.cur_yaw
        com_msg.sof = 'A'
        com_msg.reserved_slot = 0
        com_msg.fire_times = 0
        
        self.pub_ele_sys_com.publish(com_msg)
    
    def _start(self):
        
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):

        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
        
def main(args=None):
    rclpy.init(args=args)
    
    node = Node_Decision_Maker(node_decision_maker_name)
    
    with Custome_Context(node_decision_maker_name,node,[KeyboardInterrupt]):
        rclpy.spin(node)
        
    rclpy.shutdown()

if __name__ == '__main__':
    main()

