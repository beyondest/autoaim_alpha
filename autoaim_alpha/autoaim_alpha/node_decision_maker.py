from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from .decision_maker.ballistic_predictor import *
from .decision_maker.decision_maker import *

DOING_NOTHING = 0
SEARCH_AND_FIRE = 1
AUTO_BOUNCE_BACK = 2
GO_RIGHT = 3
GO_LEFT = 4
GO_FORWARD = 5
GO_BACKWARD = 6
SEARCH_FRIEND = 7


class Node_Decision_Maker(Node,Custom_Context_Obj):

    def __init__(self,
                 name):
        super().__init__(name)
        self.action_mode_to_callback = {0:self.make_decision_callback,
                                       1:self.test_fire_callback}
        
        self.action_mode_to_note = {0:"Make decision mode",
                                    1:"Test fire , auto track"}

        
        self.pub_ele_sys_com = self.create_publisher(topic_electric_sys_com['type'],
                                                topic_electric_sys_com['name'],
                                                topic_electric_sys_com['qos_profile'])

        self.pub_show = self.create_publisher(topic_show['type'],
                                        topic_show['name'],
                                        topic_show['qos_profile'])

        
        self.ballistic_predictor = Ballistic_Predictor(node_decision_maker_mode,
                                                       ballistic_predictor_config_yaml_path,
                                                       False)
        
        self.decision_maker = Decision_Maker(node_decision_maker_mode,
                                             decision_maker_params_yaml_path,
                                             yaw_pid_path,
                                             pitch_pid_path,
                                             self.ballistic_predictor,
                                             enemy_car_list,
                                             if_ignore_brother,
                                             if_main_head)
        if strategy == 1:
            self.event_flag_to_arg_list = self.decision_maker.params.strategy_1_event_flag_to_arg_list
            self.get_logger().warn(f"Use strategy 1, GO LEFT")
        elif strategy == 2:
            self.event_flag_to_arg_list = self.decision_maker.params.strategy_2_event_flag_to_arg_list
            self.get_logger().warn(f"Use strategy 2, GO RIGHT")
        elif strategy == 0:
            self.event_flag_to_arg_list = self.decision_maker.params.strategy_0_event_flag_to_arg_list
            self.get_logger().warn(f"Use strategy 0, KEEP_SEARCHING_ALL_TIME")
        if if_auto_recognize_friend:
            self.event_flag_to_arg_list.insert(0, [SEARCH_FRIEND, 999])
            self.get_logger().warn(f"Auto recognize friend")
        else:
            self.get_logger().warn(f"Disable auto recognize friend, Indiscriminate Attack")
            
        self.event_flat_to_callback = { DOING_NOTHING:self.decision_maker.doing_nothing,
                                        SEARCH_AND_FIRE:self.decision_maker.search_and_fire,
                                        AUTO_BOUNCE_BACK:self.decision_maker.auto_bounce_back,
                                        GO_RIGHT:self.decision_maker.go_right,
                                        GO_LEFT:self.decision_maker.go_left,
                                        GO_FORWARD:self.decision_maker.go_forward,
                                        GO_BACKWARD:self.decision_maker.go_backward,
                                        SEARCH_FRIEND:self.decision_maker.search_friend}
                
        self.get_logger().warn(f"Use gimbal_action_mode {self.action_mode_to_note[gimbal_action_mode]}")
        
        if if_ignore_elesys:
            self.decision_maker.electric_system_zero_unix_time = time.time()
            self.get_logger().warn(f"Ignore electric system, init zero_unix_time {self.decision_maker.electric_system_zero_unix_time}")
            self.if_connetect_to_ele_sys = True
        else:
            self.if_connetect_to_ele_sys = False
        if if_ignore_brother:
            self.if_connect_to_brother = True
            self.get_logger().warn(f"Ignore brother ")
        else:
            self.if_connect_to_brother = False
        if if_main_head:
            self.get_logger().warn(f"Main head, use big gimbal")
        else:
            self.get_logger().warn(f"Not main head, use small gimbal only")
            
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)
            
        self.rel_yaw = 0.0
        self.rel_pitch = 0.0
        self.fire_times = 0
        
        self.event_index = 0
        self.cur_event = self.event_flag_to_arg_list[self.event_index][0]
        self.pre_event = DOING_NOTHING
        self.cur_event_arg = self.event_flag_to_arg_list[self.event_index][1]

        self.sub_armor_pos_list = self.create_subscription(topic_armor_pos_list['type'],
                                                        topic_armor_pos_list['name'],
                                                        self.sub_armor_pos_list_callback,
                                                        topic_armor_pos_list['qos_profile'])
        self.sub_ele_sys_state = self.create_subscription(topic_electric_sys_state['type'],
                                                      topic_electric_sys_state['name'],
                                                      self.recv_from_small_gimbal_callback,
                                                      topic_electric_sys_state['qos_profile'])
        self.sub_bro_info = self.create_subscription(topic_bro_info['type'],
                                                    topic_bro_info['name'],
                                                    self.sub_bro_info_callback,
                                                    topic_bro_info['qos_profile'])
        if if_main_head:
            self.sub_big_gimbal_info = self.create_subscription(topic_elebig_gimbal['type'],
                                                        topic_elebig_gimbal['name'],
                                                        self.sub_big_gimbal_state_callback,
                                                        topic_elebig_gimbal['qos_profile'])
            
        if self.decision_maker.params.auto_bounce_back_period > 0:
            self.auto_bounce_back_timer = self.create_timer(self.decision_maker.params.auto_bounce_back_period, 
                                                            self.auto_bounce_callback)
            self.get_logger().warn(f"Auto bounce back period {self.decision_maker.params.auto_bounce_back_period} s")
            
        else:
            self.get_logger().warn(f"Disable auto bounce back")
            
            
            
        if mode == 'Dbg':
            self.sub_pid_config = self.create_subscription(topic_pid_config['type'],
                                                        topic_pid_config['name'],
                                                        self.sub_pid_config_callback,
                                                        topic_pid_config['qos_profile'])
            
            self.pub_debug_fire = self.create_publisher(topic_debug_fire['type'],
                                                    topic_debug_fire['name'],
                                                    topic_debug_fire['qos_profile'])
            
            self.sub_gimbal_action = self.create_subscription(topic_gimbal_action['type'],
                                                        topic_gimbal_action['name'],
                                                        self.sub_gimbal_action_callback,
                                                        topic_gimbal_action['qos_profile'])
        
    def recv_from_small_gimbal_callback(self, msg:ElectricsysState):
        
        if self.if_connetect_to_ele_sys == False:
            self.if_connetect_to_ele_sys = True
            self.decision_maker.electric_system_zero_unix_time = time.time()
            self.get_logger().info(f"Connect to small gimbal, zero_unix_time {self.decision_maker.electric_system_zero_unix_time} ")
            
        self.decision_maker.update_small_gimbal_info(
                                                 cur_yaw=msg.cur_yaw,
                                                 cur_pitch=msg.cur_pitch)
        
    def sub_armor_pos_list_callback(self, msg:ArmorPosList):
        for armor_pos in msg.armor_pos_list:
            target_pos_in_camera_frame = np.array([armor_pos.pose.pose.position.x,
                                                    armor_pos.pose.pose.position.y,
                                                    armor_pos.pose.pose.position.z])
            

            self.decision_maker.update_enemy_side_info(armor_pos.armor_name,
                                                    armor_pos.armor_id,
                                                    target_pos_in_camera_frame,
                                                    np.array([0.0,0.0,1.0]),
                                                    armor_pos.confidence,
                                                    time.time(),
                                                    armor_pos.continuous_detected_num,
                                                    armor_pos.continuous_lost_num,
                                                    armor_pos.if_update
                                                    )
            
        self.action_mode_to_callback[gimbal_action_mode]()
        com_msg = ElectricsysCom()
        com_msg.sof = 'A'
        com_msg.target_abs_pitch = self.decision_maker.next_pitch
        com_msg.target_abs_yaw = self.decision_maker.next_yaw
        com_msg.fire_times = self.decision_maker.fire_times
        com_msg.big_gimbal_yaw = self.decision_maker.next_big_gimbal_yaw
        com_msg.reserved_slot = self.decision_maker.reserved_slot
        self.pub_ele_sys_com.publish(com_msg)
            
    
    def sub_bro_info_callback(self, msg:BroInfo):
        self.decision_maker.update_brother_info(msg.find_enemy_yaw,
                                                msg.cur_big_gimbal_yaw)
                
    def sub_big_gimbal_state_callback(self, msg:EleBigGimbalState):
        self.decision_maker.update_big_gimbal_info(msg.cur_big_gimbal_yaw,
                                                   msg.sentry_health)
        
    def make_decision_callback(self):
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to small gimbal, cannot make decision")
            self.decision_maker.doing_nothing(999)
            return
        if self.if_connect_to_brother == False:
            self.get_logger().warn(f"Not connect to brother, cannot make decision")
            self.decision_maker.doing_nothing(999)
            return
        
        t1 = time.time()
        if_action_finished = self.event_flat_to_callback[self.cur_event](self.cur_event_arg)
        if if_action_finished:
            if self.cur_event == AUTO_BOUNCE_BACK:
                self.cur_event = self.pre_event
            else:
                self.pre_event = self.cur_event
                if self.event_index == len(self.event_flag_to_arg_list)-1:
                    self.event_index = self.event_index
                else:
                    self.event_index += 1
                self.cur_event = self.event_flag_to_arg_list[self.event_index][0]
                self.cur_event_arg = self.event_flag_to_arg_list[self.event_index][1]
        else:
            self.cur_event = self.cur_event
            self.cur_event_arg = self.cur_event_arg    
            
        t2 = time.time()
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().debug(f"Make decision : time cost {t2-t1:.3f}")
            self.get_logger().debug(f"Make decision : event_flag: {self.cur_event} , event arg: {self.event_flag_to_arg_list[self.event_index][1]} fire_times {self.decision_maker.fire_times}  target_abs_yaw {self.decision_maker.next_yaw:.3f},target_abs_pitch {self.decision_maker.next_pitch:.3f}, reserved_slot {self.decision_maker.reserved_slot}, next_big_gimbal_yaw: {self.decision_maker.next_big_gimbal_yaw:.3f}")
       
    def auto_bounce_callback(self):
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot auto bounce back")
        else:
            if self.cur_event == SEARCH_AND_FIRE or self.cur_event == DOING_NOTHING:
                self.pre_event = self.cur_event
                self.cur_event = AUTO_BOUNCE_BACK
                self.cur_event_arg = self.decision_maker.params.auto_bounce_back_continue_frames
                self.decision_maker.action_count = -1
                if node_decision_maker_mode == 'Dbg':
                    self.get_logger().debug(f"Auto bounce back")
            else:
                if node_decision_maker_mode == 'Dbg':
                    self.get_logger().debug(f"Cannot auto bounce back, current event {self.cur_event}")
        
    def sub_pid_config_callback(self,msg:PidConfig):
        
        self.decision_maker.yaw_pid_controller.params.kp = msg.yaw_kp
        self.decision_maker.yaw_pid_controller.params.ki = msg.yaw_ki
        self.decision_maker.yaw_pid_controller.params.kd = msg.yaw_kd
        self.decision_maker.pitch_pid_controller.params.kp = msg.pitch_kp
        self.decision_maker.pitch_pid_controller.params.ki = msg.pitch_ki
        self.decision_maker.pitch_pid_controller.params.kd = msg.pitch_kd
        
    def test_fire_callback(self):
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        if self.fire_times == -1:
            self.decision_maker.params.manual_pitch_compensation = self.rel_pitch
            self.decision_maker.params.manual_yaw_compensation = self.rel_yaw
        
        com_msg = ElectricsysCom()
        t1 = time.time()
        self.decision_maker.search_and_fire() 
        t2 = time.time()
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().debug(f"Make decision : time cost {t2-t1:.3f}")
        
        com_msg.target_abs_pitch = self.decision_maker.next_pitch
        com_msg.target_abs_yaw = self.decision_maker.next_yaw
        com_msg.sof = 'A'
        com_msg.reserved_slot = 10
        com_msg.fire_times = self.fire_times if self.fire_times != -1 else 0

        if mode == 'Dbg':
            debug_fire_msg = DebugFire()
            debug_fire_msg.img_x = float(self.decision_maker.target.tvec[0])
            debug_fire_msg.img_y = float(self.decision_maker.target.tvec[2])
            debug_fire_msg.depth = float(self.decision_maker.target.tvec[1])
            self.pub_debug_fire.publish(debug_fire_msg)
        
        self.pub_ele_sys_com.publish(com_msg)
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().debug(f"Make decision : fire_times {com_msg.fire_times}  target_abs_yaw {com_msg.target_abs_yaw:.3f},target_abs_pitch {com_msg.target_abs_pitch:.3f}")

    def sub_gimbal_action_callback(self, msg:GimbalAction):
        self.fire_times = msg.fire_times
        self.rel_pitch = msg.rel_pitch
        self.rel_yaw = msg.rel_yaw
        
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

