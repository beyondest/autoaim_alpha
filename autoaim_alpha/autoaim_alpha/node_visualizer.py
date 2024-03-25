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
        
        
        self.sub_img_for_vis = self.create_subscription(topic_img_for_visualize['type'],
                                                        topic_img_for_visualize['name'],
                                                        self.img_for_vis_callback,
                                                        topic_img_for_visualize['qos_profile'])
            
        self.sub_ele_sys_com = self.create_subscription(topic_electric_sys_com['type'],
                                                        topic_electric_sys_com['name'],
                                                        self.ele_sys_com_callback,
                                                        topic_electric_sys_com['qos_profile'])

        self.sub_ele_sys_state = self.create_subscription(topic_electric_sys_state['type'],
                                                        topic_electric_sys_state['name'],
                                                        self.ele_sys_state_callback,
                                                        topic_electric_sys_state['qos_profile'])

        self.detect_window_name = 'detect_img'

        self.cur_yaw = 0
        self.cur_pitch = 0
        self.target_yaw = 0
        self.target_pitch = 0
        
        self.bridge = CvBridge()
        self.get_logger().warn(f"!!!You have started visualizer, armor_name_to_draw_pos need to be set if armor_name is new!!!")
        
        
        if node_visualizer_mode =='Dbg':
            self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)

    def img_for_vis_callback(self, msg:Image):
        
        self.get_logger().debug(f"Received image for visualization")
        img = self.bridge.imgmsg_to_cv2(msg, desired_encoding=camera_output_format)
        
        add_text(   img,
                    f'cur_yaw',
                    value=f'{self.cur_yaw:.3f}',
                    pos=(20,50),
                    color=(0,0,0),
                    scale_size=0.7)
        add_text(   img,
                    f'cur_pitch',
                    value=f'{self.cur_pitch:.3f}',
                    pos=(20,80),
                    color=(0,0,0),
                    scale_size=0.7)
        add_text(   img,
                    f'target_yaw',
                    value=f'{self.target_yaw:.3f}',
                    pos=(20,110),
                    color=(0,0,0),
                    scale_size=0.7)
        add_text(   img,
                    f'target_pitch',
                    value=f'{self.target_pitch:.3f}',
                    pos=(20,140),
                    color=(0,0,0),
                    scale_size=0.7)
        
        cv2.imshow(self.detect_window_name, img)
        cv2.waitKey(1)
    
    
    
    def ele_sys_com_callback(self, msg:ElectricsysCom):
        self.target_yaw = msg.target_abs_yaw
        self.target_pitch = msg.target_abs_pitch

    def ele_sys_state_callback(self, msg:ElectricsysState):
        self.cur_yaw = msg.cur_yaw
        self.cur_pitch = msg.cur_pitch
 
    


    
    def _start(self):
        
        cv2.namedWindow(self.detect_window_name, cv2.WINDOW_AUTOSIZE)
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
