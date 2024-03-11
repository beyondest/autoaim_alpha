from . import *
from .os_op.basic import *
import rclpy
from rclpy.node import Node
from cv_bridge import CvBridge
import cv2
from .img.tools import *
from .decision_maker.observer import Armor_Params

# if armor.t - cur_t > expired_time, then this is an expired info; 
# WARNING : YOU MUST CONSIDER LATENCE OF RECEIVING REMOTE INFO
expired_time = 1.0
wid_step_add_by_attr = 150
wid_step_cal_by_id = 100

refresh_freq = 30
# h,w,c
canvas_shape = (600,800,3)
armor_name_to_pos_y_start = {'1d': 100, '2x': 0, '3d': 0, '3x': 20, '4d': 0, '4x': 40, '5x': 0, 'based': 0, 'outpost': 0, 'sentry': 60, 'wrongpic': 80}\
    if not if_yolov5 else \
        {'B1': 100, 'B2': 0, 'B3': 20, 'B4': 40, 'B5': 0, 'BBb': 0, 'BG': 0, 'BO': 0,\
            'R1': 100, 'R2': 0, 'R3': 20, 'R4': 40, 'R5': 0, 'RBb': 0, 'RG': 0, 'RO': 0}
x_start = 20



class Node_Visualizer(Node,Custom_Context_Obj):

    def __init__(self,
                 name,
                ):
        super().__init__(name)
        
        
        if if_show_img_remote:
            self.sub_img_for_vis = self.create_subscription(topic_img_for_visualize['type'],
                                                        topic_img_for_visualize['name'],
                                                        self.img_for_vis_callback,
                                                        topic_img_for_visualize['qos_profile'])
            
        if if_pub_armor_state_without_correct:
        
            self.sub_armor_state_without_corrected = self.create_subscription(  topic_armor_pos_without_correct['type'],
                                                                                topic_armor_pos_without_correct['name'],
                                                                                self.sub_armor_pos_without_correct_callback,
                                                                                topic_armor_pos_without_correct['qos_profile'])
            
            self.timer_refresh_armor_state_without_correct = self.create_timer(1/refresh_freq,
                                                                    self.refresh_armor_state_without_correct)
            
            
            
        if if_pub_armor_state_corrected:
            self.sub_armor_state_corrected = self.create_subscription(topic_armor_pos_corrected['type'],
                                                            topic_armor_pos_corrected['name'],
                                                            self.sub_armor_pos_corrected_callback,
                                                            topic_armor_pos_corrected['qos_profile'])
            
            self.timer_refresh_armor_state_corrected = self.create_timer(1/refresh_freq,
                                                                    self.refresh_armor_state_corrected)
        
        if if_pub_armor_state_predicted:
            self.sub_armor_state_predicted = self.create_subscription(topic_armor_pos_predicted['type'],
                                                            topic_armor_pos_predicted['name'],
                                                            self.sub_armor_pos_predicted_callback,
                                                            topic_armor_pos_predicted['qos_profile'])
            
            self.timer_refresh_armor_state_predicted = self.create_timer(1/refresh_freq,
                                                                    self.refresh_armor_state_predicted)

        


        self.armor_state_without_correct_list = [Armor_Params(enemy_car['armor_name'],armor_id) \
                                                for enemy_car in enemy_car_list \
                                                    for armor_id in range(enemy_car['armor_nums'])]
        
        self.armor_state_corrected_list = [Armor_Params(enemy_car['armor_name'],armor_id) \
                                                for enemy_car in enemy_car_list \
                                                    for armor_id in range(enemy_car['armor_nums'])]
        self.armor_state_predicted = [Armor_Params(enemy_car['armor_name'],armor_id) \
                                                for enemy_car in enemy_car_list \
                                                    for armor_id in range(enemy_car['armor_nums'])]
        self.detect_window_name = 'detect_img'
        self.window_armor_state_without_correct = 'armor_state_without_correct'
        self.window_armor_state_corrected = "window_armor_state_corrected"
        self.window_armor_state_predicted = "window_armor_state_predicted"
        
        self.bridge = CvBridge()
        self.get_logger().warn(f"!!!You have started visualizer, armor_name_to_draw_pos need to be set if armor_name is new!!!")
        
        
        if node_visualizer_mode =='Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)

    def img_for_vis_callback(self, msg:Image):
        
        self.get_logger().debug(f"Received image for visualization")
        img = self.bridge.imgmsg_to_cv2(msg, desired_encoding=camera_output_format)
        cv2.imshow(self.detect_window_name, img)
        cv2.waitKey(1)
    
        
        
 
    
    def sub_armor_pos_without_correct_callback(self, msg:ArmorPos):
        self.get_logger().debug(f"Recv armorpos without corrected")
        self._update_armor_state_list(self.armor_state_without_correct_list, msg)
        
    def sub_armor_pos_corrected_callback(self,msg:ArmorPos):
        self.get_logger().debug(f"Recv armorpos corrected")
        self._update_armor_state_list(self.armor_state_corrected_list,msg)            
          
    def sub_armor_pos_predicted_callback(self,msg:ArmorPos):
        self.get_logger().debug(f"Recv armorpos predicted")
        self._update_armor_state_list(self.armor_state_predicted,msg)            
    
    
    
    def refresh_armor_state_without_correct(self):
        c = canvas(canvas_shape,'white')
        for armor_state in self.armor_state_without_correct_list:
            self.draw_armor_state_to_img(c.img,armor_state)
        cv2.imshow(self.window_armor_state_without_correct,c.img)
        cv2.waitKey(1)
        
    def refresh_armor_state_corrected(self):
        c = canvas(canvas_shape,'white')
        for armor_state in self.armor_state_corrected_list:
            self.draw_armor_state_to_img(c.img,armor_state)
        cv2.imshow(self.window_armor_state_corrected,c.img)
        cv2.waitKey(1)
        
          
    def refresh_armor_state_predicted(self):
        c = canvas(canvas_shape,'white')
        for armor_state in self.armor_state_predicted:
            self.draw_armor_state_to_img(c.img,armor_state)
        cv2.imshow(self.window_armor_state_predicted,c.img)
        cv2.waitKey(1)
        
        
    def _update_armor_state_list(self,armor_state_list:list,msg:ArmorPos):
        
        target_pos_in_camera_frame = np.array([msg.pose.pose.position.x,
                                                msg.pose.pose.position.y,
                                                msg.pose.pose.position.z])
        
        q = Quaternion(msg.pose.pose.orientation.w,
                       msg.pose.pose.orientation.x,
                       msg.pose.pose.orientation.y,
                       msg.pose.pose.orientation.z)
        rvec = q.get_axis() * q.angle
        
        for armor_params in armor_state_list:
            if armor_params.name == msg.armor_name and armor_params.id == msg.armor_id:
                armor_params.tvec = target_pos_in_camera_frame
                armor_params.rvec = rvec
                armor_params.confidence = msg.confidence
                armor_params.time = msg.pose.header.stamp.sec + msg.pose.header.stamp.nanosec/1e9
    

    
    def _start(self):
        if if_show_img_remote:
            cv2.namedWindow(self.detect_window_name, cv2.WINDOW_AUTOSIZE)
        if if_pub_armor_state_without_correct:
            cv2.namedWindow(self.window_armor_state_without_correct,  cv2.WINDOW_AUTOSIZE)
            
        if if_pub_armor_state_corrected:
            cv2.namedWindow(self.window_armor_state_corrected,  cv2.WINDOW_AUTOSIZE)
        
        if if_pub_armor_state_predicted:
            cv2.namedWindow(self.window_armor_state_predicted,  cv2.WINDOW_AUTOSIZE)
            
            
        self.get_logger().info(f"Node {self.get_name()} start success")
    
    def _end(self):
        
        cv2.destroyAllWindows()
        self.get_logger().info(f"Node {self.get_name()} end success")
        self.destroy_node()

    def _errorhandler(self,exc_value):
        self.get_logger().error(f"Node {self.get_name()} get error {exc_value}")
    
    
    def draw_armor_state_to_img(self,
                        img:np.ndarray,
                        armor_state:Armor_Params):
        cur_time = time.time()
        if_latest = cur_time - armor_state.time < expired_time
        
        pos = ( int(x_start + armor_state.id * wid_step_cal_by_id) , int(armor_name_to_pos_y_start[armor_state.name]))
        
        add_text(   img,
                    f'name',
                    value=f'{armor_state.name}:{armor_state.id}',
                    pos=pos,
                    color=(0,0,0),
                    scale_size=0.7)
        
        add_text(   img,
                    f'if_latest',
                    value=f'{if_latest}',
                    pos=(int(pos[0] + wid_step_add_by_attr * 1), pos[1]),
                    color=(0,0,0),
                    scale_size=0.7)
        
        
        if 0:
            add_text(   img,
                        f'pos',
                        value=f'x:{armor_state.tvec[0]:.3f}y:{armor_state.tvec[1]:.3f} z:{armor_state.tvec[2]:.3f}',
                        pos=(int(pos[0] + wid_step_add_by_attr * 2),pos[1]),
                        color=(0,0,0),
                        scale_size=0.7)
            add_text(   img,
                        f'rot',
                        value=f'x:{armor_state.rvec[0]:.3f} y:{armor_state.rvec[1]:.3f} z:{armor_state.rvec[2]:.3f}',
                        pos=(int(pos[0] + wid_step_add_by_attr * 3),pos[1]),
                        color=(0,0,0),
                        scale_size=0.7)
            add_text(   img,
                        f'latest_time:',
                        value=f'{armor_state.time}:.3f',
                        pos=(int(pos[0] + wid_step_add_by_attr * 4),pos[1]),
                        color=(0,0,0),
                        scale_size=0.7)
            add_text(   img,
                        f'confidence',
                        value=f'{armor_state.confidence:.2f}',
                        pos=(int(pos[0] + wid_step_add_by_attr * 5),pos[1]),
                        color=(0,0,0),
                        scale_size=0.7)
            

def main(args=None):
    rclpy.init(args=args)
    
    node = Node_Visualizer(node_visualizer_name)
    
    with Custome_Context(node_visualizer_name,node,[KeyboardInterrupt]):
        rclpy.spin(node)
    
    
    rclpy.shutdown()

if __name__ == '__main__':
    main()
