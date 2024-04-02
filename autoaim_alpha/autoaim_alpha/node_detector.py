
from . import *
from rclpy.node import Node
from cv_bridge import CvBridge
import cv2
import numpy as np
import time
from .img.detector import Armor_Detector
from visualization_msgs.msg import Marker
from pyquaternion import Quaternion



class Node_Detector(Node,Custom_Context_Obj):
    
    def __init__(self,
                 name,
                 ):
        
        super().__init__(name)
        self.sub_img_raw = self.create_subscription(
                                            topic_img_raw['type'],
                                            topic_img_raw['name'],
                                            self.sub_img_raw_callback,
                                            topic_img_raw['qos_profile']
                                            )
        self.pub_detect_result = self.create_publisher(
                                            topic_detect_result['type'],
                                            topic_detect_result['name'],
                                            topic_detect_result['qos_profile']
                                            )
        if mode == 'Dbg':
            self.sub_ele_sys_com = self.create_subscription(topic_electric_sys_com['type'],
                                            topic_electric_sys_com['name'],
                                            self.sub_ele_sys_com_callback,
                                            topic_electric_sys_com['qos_profile'])
            
            self.sub_ele_sys_state = self.create_subscription(topic_electric_sys_state['type'],
                                                    topic_electric_sys_state['name'],
                                                    self.sub_ele_sys_state_callback,
                                                    topic_electric_sys_state['qos_profile'])
            
            if if_show_img_remote:
                self.pub_img_for_visualize = self.create_publisher(topic_img_for_visualize['type'],
                                                    topic_img_for_visualize['name'],
                                                    topic_img_for_visualize['qos_profile'])
                
        self.fire_times = 0
        self.target_abs_pitch = 0.0
        self.target_abs_yaw = 0.0
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        self.cv_bridge = CvBridge()
        self.armor_detector = Armor_Detector(
                                            armor_color=armor_color,
                                            mode=node_detector_mode,
                                            tradition_config_folder=tradition_config_folder,
                                            net_config_folder=net_config_folder,
                                            depth_estimator_config_yaml=depth_estimator_config_yaml_path,
                                            if_yolov5=if_yolov5,
                                            if_show_img_local = if_show_img_local,
                                            enemy_car_list = enemy_car_list
                                             )
        
        #self.armor_detector.tradition_detector.enable_preprocess_config()
        #self.armor_detector.tradition_detector.filter1.enable_trackbar_config()
        #self.armor_detector.tradition_detector.filter2.enable_trackbar_config()
        
        if node_detector_mode == 'Dbg':

            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)

        self.pre_time = time.perf_counter()
        self.fps = 0
        self.window_name = 'show_local'
        
        if node_detector_mode == 'Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)
        
         
    def sub_img_raw_callback(self,data):
        
        img = self.cv_bridge.imgmsg_to_cv2(data,camera_output_format)
        
        result , find_time = self.armor_detector.get_result(img)
        self.cur_time = time.perf_counter()
        if self.cur_time - self.pre_time  == 0:
            self.fps = 0
        else:
            self.fps = round(1/(self.cur_time-self.pre_time))
            
        self.pre_time = self.cur_time
        
        if if_show_img_remote or if_show_img_local:
            img_for_visualize = self.armor_detector.visualize(img,
                                          fps=self.fps,
                                          cur_yaw=self.cur_yaw,
                                          tar_yaw=self.target_abs_yaw,
                                          cur_pitch=self.cur_pitch,
                                          tar_pitch=self.target_abs_pitch)
            
            if if_show_img_remote:
                self.pub_img_for_visualize.publish(self.cv_bridge.cv2_to_imgmsg(img_for_visualize,camera_output_format))
            if if_show_img_local:
                cv2.imshow(self.window_name,img_for_visualize)
                cv2.waitKey(1)
                
        if mode == 'Dbg':
            self.get_logger().info(f"DETECT_FPS:{self.fps}")
        
        if result is not None:
            msg = DetectResult()
            
            t = self.get_clock().now().to_msg()
            log_info = ''
            for each_result in result:
                ed = EachDetectResult()
                
                ed.armor_name = each_result['result']
                ed.confidence = float(each_result['probability'])
                ed.pose.header.stamp = t
                ed.pose.header.frame_id = 'camere_frame'
                ed.pose.pose.position.x = each_result['pos'][0]
                ed.pose.pose.position.y = each_result['pos'][1]
                ed.pose.pose.position.z = each_result['pos'][2]
                
                q = Quaternion(axis=each_result['rvec'],radians=np.linalg.norm(each_result['rvec']))
                ed.pose.pose.orientation.w = q.w
                ed.pose.pose.orientation.x = q.x
                ed.pose.pose.orientation.y = q.y
                ed.pose.pose.orientation.z = q.z
                msg.detect_result.append(ed)
                
                log_info += f"\narmor_name:{ed.armor_name},conf:{ed.confidence:.2f}, x:{each_result['pos'][0]:.2f},y:{each_result['pos'][1]:.2f},z:{each_result['pos'][2]:.2f}"
            if mode == 'Dbg':
                self.get_logger().warn(f"Find target : {log_info} spend time:{find_time}s")
            
            self.pub_detect_result.publish(msg)
            
        else:
            self.pub_detect_result.publish(DetectResult())
            if mode == 'Dbg':
                self.get_logger().warn(f"No target found")
    
    def sub_ele_sys_com_callback(self,msg:ElectricsysCom):
        
        self.target_abs_yaw = msg.target_abs_yaw
        
        self.target_abs_pitch = msg.target_abs_pitch
        self.fire_times = msg.fire_times
        self.target_abs_pitch = msg.target_abs_pitch
        
    def sub_ele_sys_state_callback(self,msg:ElectricsysState):
        
        self.cur_pitch = msg.cur_pitch
        self.cur_yaw = msg.cur_yaw
    def _start(self):
        if if_show_img_local:
            try:
                cv2.namedWindow(self.window_name,cv2.WINDOW_AUTOSIZE)
            except Exception as e:
                self.get_logger().error(f"cv2.namedWindow error {e}")
        
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        cv2.destroyAllWindows()
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):

        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
        
        
def main(args = None):
    
    rclpy.init(args=args)
    node = Node_Detector(node_detector_name)
    with Custome_Context(node_detector_name,node,[KeyboardInterrupt]):
        rclpy.spin(node)
    rclpy.shutdown()
        
        

















