from . import *
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import TransformStamped
from tf2_msgs.msg import TFMessage


from .decision_maker.decision_maker import *
from .decision_maker.observer import *
from .decision_maker.ballistic_predictor import *
from .port_slavedevice.port import *



predict_time_offset = 1.0
class Node_Observer(Node,Custom_Context_Obj):

    def __init__(self,
                 name,
                ):
        super().__init__(name)
        self.observer =  Observer(node_observer_mode,
                                        observer_config_yaml_path,
                                        enemy_car_list) 
        
        self.sub = self.create_subscription(
                                            topic_detect_result['type'],
                                            topic_detect_result['name'],
                                            self.detect_sub_callback,
                                            topic_detect_result['qos_profile'])
        
        self.pub = self.create_publisher(topic_armor_pos_list['type'],
                                          topic_armor_pos_list['name'],
                                          topic_armor_pos_list['qos_profile'])

        
        self.observer.set_armor_initial_state(armor_name_to_init_state)
        
        for armor_name in armor_name_to_init_state.keys():
            CHECK_INPUT_VALID(armor_name,*[car_dict['armor_name'] for car_dict in enemy_car_list])
            armor_state_list = self.observer.get_armor_latest_state()
            
            for armor_state in armor_state_list:
                self.get_logger().warn(f"Init Armor: {armor_state.name} id {armor_state.id}")
        
        
        if node_observer_mode == 'Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)
        
        
    def detect_sub_callback(self, msg:DetectResult):
        all_target_list = []
        for each_detect_result in msg.detect_result:
            
            armor_name = each_detect_result.armor_name
            tvec = np.array([each_detect_result.pose.pose.position.x,
                             each_detect_result.pose.pose.position.y,
                             each_detect_result.pose.pose.position.z])
            '''q = Quaternion(each_detect_result.pose.pose.orientation.w,
                           each_detect_result.pose.pose.orientation.x,
                           each_detect_result.pose.pose.orientation.y,
                           each_detect_result.pose.pose.orientation.z)'''
            #rvec = q.get_axis() * q.angle
            rvec = np.array([0.0,0.0,1.0])
            all_target_list.append({'armor_name':armor_name,'tvec':tvec,'rvec':rvec,'time':0.0})

        self.observer.update_by_detection_list(all_target_list)
        armor_params_detect_list = self.observer.get_armor_latest_state()
        
        msg2 = ArmorPosList()
        for armor_params in armor_params_detect_list:
            armor_pos = ArmorPos()
            armor_pos.armor_name = armor_params.name
            armor_pos.armor_id = armor_params.id
            armor_pos.confidence = armor_params.confidence
            armor_pos.pose.pose.position.x = armor_params.tvec[0]
            armor_pos.pose.pose.position.y = armor_params.tvec[1]
            armor_pos.pose.pose.position.z = armor_params.tvec[2]
            armor_pos.continuous_detected_num = armor_params.continuous_detected_num
            armor_pos.continuous_lost_num = armor_params.continuous_lost_num
            armor_pos.if_update = armor_params.if_update
            msg2.armor_pos_list.append(armor_pos)
            
        self.pub.publish(msg2)

    

    
    
    
    def _start(self):
        
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):

        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
        

def main(args=None):
    rclpy.init(args=args)
    my_node = Node_Observer(node_decision_maker_name)
    
    with Custome_Context(node_observer_name,my_node,[KeyboardInterrupt]):
        rclpy.spin(my_node)
        
    rclpy.shutdown()
