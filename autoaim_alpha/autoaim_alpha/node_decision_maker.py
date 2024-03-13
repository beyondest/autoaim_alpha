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
        
        self.sub_predict_pos = self.create_subscription(topic_armor_pos_corrected['type'],
                                                      topic_armor_pos_corrected['name'],
                                                      self.sub_predict_pos_callback,
                                                      topic_armor_pos_corrected['qos_profile'])
                       
        self.ballestic = Ballistic_Predictor(node_decision_maker_mode,
                                             ballistic_predictor_config_yaml_path)
        
        self.decision_maker = Decision_Maker(node_decision_maker_mode,
                                             None,
                                             enemy_car_list)

        action_mode_to_callback = {0:self.make_decision_callback,
                                1:self.repeat_recv_from_ele_callback,
                                2:self.test_yaw_callback,
                                3:self.test_pitch_callback,
                                4:self.doing_nothing_callback}
        
        
        
        action_mode_to_note = {0:"Make decision",
                                1:"Repeat recv from ele",
                                2:"Test yaw",
                                3:"Test pitch",
                                4:"Doing nothing"}
        
        
        
        self.timer = self.create_timer(1/make_decision_freq, action_mode_to_callback[gimbal_action_mode])
        self.get_logger().warn(f"Use gimbal_action_mode {action_mode_to_note[gimbal_action_mode]}")
        
        if if_ignore_elesys:
            self.decision_maker.params.electric_system_zero_unix_time = time.time()
            self.get_logger().warn(f"Ignore electric system, init zero_unix_time {self.decision_maker.params.electric_system_zero_unix_time}")
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
            self.decision_maker.params.electric_system_zero_unix_time = msg.unix_time
            self.get_logger().info(f"Connect to electric system, zero_unix_time {msg.unix_time}, cur_time {time.time()}")
            
        self.ballestic._update_camera_pos_in_gun_pivot_frame(msg.cur_yaw,msg.cur_pitch)

        minute,second,second_frac = TRANS_UNIX_TIME_TO_T(msg.unix_time,self.decision_maker.params.electric_system_zero_unix_time)
        self.decision_maker.update_our_side_info(msg.cur_yaw,
                                                  msg.cur_pitch,
                                                  minute,
                                                  second,
                                                  second_frac)
        
        
    def sub_predict_pos_callback(self, msg:ArmorPos):
        target_pos_in_camera_frame = np.array([msg.pose.pose.position.x,
                                                msg.pose.pose.position.y,
                                                msg.pose.pose.position.z])
        
        q = Quaternion(msg.pose.pose.orientation.w,
                       msg.pose.pose.orientation.x,
                       msg.pose.pose.orientation.y,
                       msg.pose.pose.orientation.z)
        rvec = q.get_axis() * q.angle
        self.decision_maker.update_enemy_side_info(msg.armor_name,
                                                   msg.armor_id,
                                                   target_pos_in_camera_frame,
                                                   rvec,
                                                   msg.confidence,
                                                   msg.pose.header.stamp.sec + msg.pose.header.stamp.nanosec/1e9
                                                   )
        
        
    def make_decision_callback(self):
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        target_armor = self.decision_maker.choose_target()
        
        abs_yaw,abs_pitch, flight_time, if_success = self.ballestic.get_fire_yaw_pitch(target_armor.tvec,
                                                                                       self.decision_maker.params.cur_yaw,
                                                                                       self.decision_maker.params.cur_pitch)
        
        if not if_success:
            self.get_logger().info(f"Ballistic predict fail, bad target, target pos: {target_armor.tvec}")
            return
            
        if target_armor.confidence == 0.75:
            com_msg.fire_times = 2
            
            self.get_logger().warn(f"Target {target_armor.name} id {target_armor.id} {target_armor.confidence} locked, FIRE {com_msg.fire_times}")
            
        elif target_armor.confidence == 0.5:
            com_msg.fire_times = 1
            self.get_logger().warn(f"Target {target_armor.name} id {target_armor.id} {target_armor.confidence} locked , FIRE {com_msg.fire_times}")
        
        
        elif target_armor.confidence ==0.4:
            com_msg.fire_times = 0
            self.get_logger().info(f"Target {target_armor.confidence} blink {target_armor.name} id {target_armor.id} , Only follow")
        
        elif target_armor.confidence == 0.3:
            self.get_logger().info(f"Target {target_armor.confidence} {target_armor.name} id {target_armor.id} first show, not follow ")
            return
        
        elif target_armor.confidence == 0.2:
            self.get_logger().info(f"Target {target_armor.confidence} {target_armor.name} id {target_armor.id} over 0.1s, not follow ")
            return
        
        else:
            self.get_logger().info(f"Target {target_armor.confidence} {target_armor.name} id {target_armor.id} Lost, not follow ")
            return
            
            
        com_msg.reach_unix_time = target_armor.time
        com_msg.target_abs_pitch = abs_pitch
        com_msg.target_abs_yaw = abs_yaw
        com_msg.sof = 'A'
        com_msg.reserved_slot = 0
        
        self.pub_ele_sys_com.publish(com_msg)
        
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().debug(f"Choose Target {target_armor.name} id {target_armor.id} tvec {target_armor.tvec} rvec {target_armor.rvec} time {target_armor.time} ")
            self.get_logger().debug(f"Make decision : fire_times {com_msg.fire_times}  target_abs_pitch {com_msg.target_abs_pitch:.3f} target_abs_yaw {com_msg.target_abs_yaw:.3f} reach_unix_time {com_msg.reach_unix_time:.3f}")
                
       
    def repeat_recv_from_ele_callback(self):
        
        
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        self.get_logger().debug(f"Get : {self.decision_maker.params.electric_system_unix_time}, {type(self.decision_maker.params.electric_system_unix_time)}")
        self.get_logger().debug(f"Get : {self.decision_maker.params.cur_pitch}, {type(self.decision_maker.params.cur_pitch)}")
        self.get_logger().debug(f"Get : {self.decision_maker.params.cur_yaw}, {type(self.decision_maker.params.cur_yaw)}")

        com_msg.reach_unix_time = self.decision_maker.params.electric_system_unix_time
        com_msg.target_abs_pitch = self.decision_maker.params.cur_pitch
        com_msg.target_abs_yaw = self.decision_maker.params.cur_yaw
        com_msg.sof = 'A'
        com_msg.reserved_slot = 0
        com_msg.fire_times = 0
        
        
        self.pub_ele_sys_com.publish(com_msg)
        
    def test_yaw_callback(self):
        
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        com_msg.reach_unix_time = self.decision_maker.params.electric_system_unix_time
        com_msg.target_abs_pitch = 0.0
        com_msg.target_abs_yaw = yaw_test_data[self.yaw_test_idx]
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
        
        com_msg.reach_unix_time = self.decision_maker.params.electric_system_unix_time
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
        
        abs_yaw,abs_pitch, flight_time, if_success = self.ballestic.get_fire_yaw_pitch(target_armor.tvec,
                                                                                       self.decision_maker.params.cur_yaw,
                                                                                       self.decision_maker.params.cur_pitch)
        
        if not if_success:
            self.get_logger().info(f"Ballistic predict fail, bad target, target pos: {target_armor.tvec}")
            return
            
        if target_armor.confidence == 0.75:
            self.get_logger().warn(f"Target {target_armor.name} id {target_armor.id} {target_armor.confidence} locked, FIRE {com_msg.fire_times}")
            
        elif target_armor.confidence == 0.5:
            self.get_logger().warn(f"Target {target_armor.name} id {target_armor.id} {target_armor.confidence} locked , FIRE {com_msg.fire_times}")
        
        
        elif target_armor.confidence ==0.4:

            self.get_logger().info(f"Target {target_armor.confidence} blink {target_armor.name} id {target_armor.id} , Only follow")
        
        elif target_armor.confidence == 0.3:
            self.get_logger().info(f"Target {target_armor.confidence} {target_armor.name} id {target_armor.id} first show, not follow ")
            return
        
        elif target_armor.confidence == 0.2:
            self.get_logger().info(f"Target {target_armor.confidence} {target_armor.name} id {target_armor.id} over 0.1s, not follow ")
            return
        
        else:
            self.get_logger().info(f"Target {target_armor.confidence} {target_armor.name} id {target_armor.id} Lost, not follow ")
            return
            
        com_msg.cur_yaw = abs_yaw
        com_msg.cur_pitch = abs_pitch
        com_msg.unix_time = time.time()
        
        self.pub_show.publish(com_msg)

        
        if node_decision_maker_mode == 'Dbg':
            self.get_logger().debug(f"Choose Target {target_armor.name} id {target_armor.id} tvec {target_armor.tvec} rvec {target_armor.rvec} time {target_armor.time} ")
            self.get_logger().debug(f"Make decision : fire_times {com_msg.fire_times}  target_abs_pitch {com_msg.target_abs_pitch:.3f} target_abs_yaw {com_msg.target_abs_yaw:.3f} reach_unix_time {com_msg.reach_unix_time:.3f}")
                
       
    def repeat_recv_from_ele_callback(self):
        
        
        if self.if_connetect_to_ele_sys == False:
            self.get_logger().warn(f"Not connect to electric system, cannot make decision")
            return
        
        com_msg = ElectricsysCom()
        
        self.get_logger().debug(f"Get : {self.decision_maker.params.electric_system_unix_time}, {type(self.decision_maker.params.electric_system_unix_time)}")
        self.get_logger().debug(f"Get : {self.decision_maker.params.cur_pitch}, {type(self.decision_maker.params.cur_pitch)}")
        self.get_logger().debug(f"Get : {self.decision_maker.params.cur_yaw}, {type(self.decision_maker.params.cur_yaw)}")

        com_msg.reach_unix_time = self.decision_maker.params.electric_system_unix_time
        com_msg.target_abs_pitch = self.decision_maker.params.cur_pitch
        com_msg.target_abs_yaw = self.decision_maker.params.cur_yaw
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

