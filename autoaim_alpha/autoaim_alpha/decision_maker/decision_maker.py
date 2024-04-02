from .ballistic_predictor import *
from .observer import *
from ..os_op.basic import *
from ..os_op.global_logger import *
from .ballistic_predictor import *
from ..utils_network.data import Data_Recoreder
from ..os_op.keyboad_and_mouse_control import *

class Decision_Maker_Params(Params):
    def __init__(self) -> None:
        super().__init__()
        
        # for abs search
        self.big_gimbal_major_yaw = 0.0
        self.pitch_min = -10
        self.pitch_max = 15
        self.yaw_search_step = 0.01
        self.pitch_search_step = 0.01
        
        # for yaw_limit
        self.yaw_mechanical_positive_limit_switch = 999
        self.yaw_mechanical_negative_limit_switch = -999
        
        
        # for track and lock
        self.continuous_detected_num_for_track = 2
        self.continuous_lost_num_max_threshold = 4
        
        # for data record
        self.tvec_history_length = 10
        self.record_data_path = None

        
        # mouse control
        self.screen_width = 1920
        self.screen_height = 1080
        self.x_axis_sensitivity = 0.5        # 0.0 - 1.0   
        self.y_axis_sensitivity = 0.5
        self.if_enable_mouse_control = False
        
        # general config
        self.if_use_pid_control = True
        self.fire_mode = 0
        """fire mode:
            0: only track target, not shooting, no pitch compensation
            1: track target and shooting, apply pitch compensation
            2: track target and shooting, apply pitch compensation with gravity compensation
        """
        self.choose_mode = 0
        """choose mode:
            0: precision_first
            1: distance_first
            2: balanced
        """
        
        # for ballistic
        self.min_img_x_for_locked = 10.0    # only work for fire_mode == 1
        self.min_continuous_detected_num_for_lock = 2
        self.manual_pitch_compensation = 0.0
        self.manual_yaw_compensation = 0.0
        # for predict
        self.target_history_lenght = 20
        # for event
        self.auto_bounce_back_period = 5 #if < 0, disable auto bounce back
        self.direction_accept_error = 0.03
        self.auto_bounce_back_continue_frames = 40
        self.sentry_go_supply_health_threshold = 250
        self.search_friend_yaw = 0.0
        self.search_friend_pitch = 0.0
        
        self.strategy_0_event_flag_to_arg_list = []
        self.strategy_1_event_flag_to_arg_list = []
        self.strategy_2_event_flag_to_arg_list = []
        
        
        self.primary_target = "0"

class Decision_Maker:
    def __init__(self,
                 mode:str,
                 decision_maker_params_yaml_path:Union[str,None] = None,
                 yaw_pid_controller_params_yaml_path:Union[str,None] = None,
                 pitch_pid_controller_params_yaml_path:Union[str,None] = None,
                 ballistic_predictor:Union[Ballistic_Predictor,None] = None,
                 enemy_car_list:list = None,
                 if_ignore_brother:bool = True
                 ) -> None:
        CHECK_INPUT_VALID(mode,"Dbg",'Rel')
        if enemy_car_list is None:
            lr1.critical("enemy_car_list is None")
            
        self.mode = mode
        self.params = Decision_Maker_Params()
        self.ballistic_predictor = ballistic_predictor
        self.yaw_pid_controller = PID_Controller()
        self.pitch_pid_controller = PID_Controller()
        
        self.yaw_pid_controller.load_params_from_yaml(yaw_pid_controller_params_yaml_path)
        self.pitch_pid_controller.load_params_from_yaml(pitch_pid_controller_params_yaml_path)
        self.enemy_car_list = enemy_car_list
        if decision_maker_params_yaml_path is not None:
            self.params.load_params_from_yaml(decision_maker_params_yaml_path)
        self._trans_params_degree_to_radian()
        if self.params.if_use_pid_control == False:
            lr1.warning("PID control is disabled, fire mode ignored, apply pitch compensation forced")
        
        self.armor_state_list = [Armor_Params(enemy_car['armor_name'],armor_id) \
                                                        for enemy_car in self.enemy_car_list \
                                                            for armor_id in range(enemy_car['armor_nums'])]
        self.target = self.armor_state_list[0]
        self.next_yaw = 0.0
        self.next_pitch = 0.0
        self.fire_times = 0
        self.reserved_slot = 10 # tens digit is 1 means not bounce back, 2 means bounce back; digit is 0 means stay, 2 means go forward towards gimbal direction, 1 means go backward 
        self.pitch_compensation = 0.0
        
        
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        self.cur_big_gimbal_yaw = 0.0
        self.remaining_health = 0
        self.remaining_ammo = 750
        self.sentry_state = 0
        self.pre_sentry_state = 0
        self.electric_system_zero_unix_time = None
        self.electric_system_unix_time = time.time()
        
        self.bro_target = None
        self.i_found = False 
        self.bro_found = True if if_ignore_brother else False
        
        # if -1, means first enter action func, then set to continue frame count, if == 0, means action finished, if > 0, means not finished
        self.action_count = -1
       
        # this is for predict
        self.target_list = [Armor_Params('None',0) for i in range(self.params.target_history_lenght)]
        
        # this is for record
        if self.params.record_data_path is not None:
            self.data_recorder = Data_Recoreder(self.params.record_data_path,
                                            (self.params.tvec_history_length,3),
                                            (2,),
                                            np.float32,
                                            np.float32)
            
            self.tvec_history_list = [np.zeros(3) for i in range(self.params.tvec_history_length)]
        
        if self.params.if_enable_mouse_control:
            self.mouse_control = KeyboardAndMouseControl('Rel',if_enable_key_board=False,if_enable_mouse_control=True)
            self.mouse_pos_prior = (0,0)
        
        self.yaw_search_add = True
        self.pitch_search_add = True
        
    def _trans_params_degree_to_radian(self):
        self.params.yaw_mechanical_negative_limit_switch = self.params.yaw_mechanical_negative_limit_switch/180*np.pi
        self.params.yaw_mechanical_positive_limit_switch = self.params.yaw_mechanical_positive_limit_switch/180*np.pi
        self.params.pitch_max = self.params.pitch_max/180*np.pi
        self.params.pitch_min = self.params.pitch_min/180*np.pi
        
    def update_enemy_side_info(self,
                      armor_name:str,
                      armor_id:int,
                      armor_tvec:np.ndarray,
                      armor_rvec:np.ndarray,
                      armor_confidence:float = 0.5,
                      armor_time:float = 0.0,
                      continuous_detected_num:int = 0,
                      continuous_lost_num:int = 0,
                      if_update:bool = False)->None:
        if not self.i_found and armor_name.split('_')[0] == 'friend':
            self.i_found = True
            
        for armor_params in self.armor_state_list:
            if armor_params.name == armor_name and armor_params.id == armor_id:
                armor_params.tvec = armor_tvec
                armor_params.rvec = armor_rvec
                armor_params.confidence = armor_confidence
                armor_params.time = armor_time
                armor_params.continuous_detected_num = continuous_detected_num
                armor_params.continuous_lost_num = continuous_lost_num
                armor_params.if_update = if_update
        
                
                
    def update_small_gimbal_info(self,
                             cur_yaw:float,
                             cur_pitch:float
                             )->None:
        
        self.cur_yaw = cur_yaw
        self.cur_pitch = cur_pitch
    
        
    def update_big_gimbal_info(self,
                               cur_big_gimbal_yaw:float,
                               remaining_health:int):
        
        self.cur_big_gimbal_yaw = cur_big_gimbal_yaw
        if self.remaining_health == 0 and remaining_health == 0:
            self.sentry_state = 0 # race not start yet
        elif self.remaining_health == 0 and remaining_health >= 600:
            self.sentry_state = 1 # race start
        elif self.remaining_health > 0 and remaining_health < self.params.sentry_go_supply_health_threshold:
            self.sentry_state = 2 # go supply
        elif self.sentry_state == 2 and remaining_health >= 600:
            self.sentry_state = 3 # supply finished
        self.remaining_health = remaining_health
        

    def save_params_to_yaml(self,yaml_path:str)->None:
        self.params.save_params_to_yaml(yaml_path)
    
    
    def search_and_fire(self,arg)->bool:
        """
        Args:
            target_armor_params (Armor_Params): _description_
        Returns:
            if action finished, return True, else return False
        """
            
        if self.action_count == -1:
            self.action_count = arg
            if self.mode == 'Dbg':
                lr1.debug("Start Search and Fire")
            return False
        
        elif self.action_count > 0:
            if self.action_count == 999:
                pass
            else:
                self.action_count -= 1
            last_update_target_list = self._find_last_update_target()
            self._choose_target(last_update_target_list)
            if self.target.if_update and self.target.continuous_detected_num >= self.params.continuous_detected_num_for_track:
                if not self.params.if_use_pid_control:
                    # abs pitch, apply pitch compensation automatically
                    self._get_next_yaw_pitch_by_ballistic()
                else:
                    if self.params.fire_mode == 0:
                        self._get_next_yaw_pitch_by_pid(yaw_pid_target=0.0,pitch_pid_target=0.0)
                    elif self.params.fire_mode == 1:
                        gun_aim_minus_camera_aim_pitch_diff = self.ballistic_predictor.get_pitch_diff(self.target.tvec[1])
                        self.pitch_compensation = gun_aim_minus_camera_aim_pitch_diff + self.params.manual_pitch_compensation
                        self._get_next_yaw_pitch_by_pid(yaw_pid_target=self.params.manual_yaw_compensation,
                                                        pitch_pid_target=self.pitch_compensation)
                    elif self.params.fire_mode == 2:
                        gun_aim_minus_camera_aim_pitch_diff = self.ballistic_predictor.get_pitch_diff(self.target.tvec[1])
                        gravity_compensation = self.ballistic_predictor.get_gravity_compensation(self.target.tvec[1])
                        self.pitch_compensation = gun_aim_minus_camera_aim_pitch_diff +gravity_compensation + self.params.manual_pitch_compensation
                        self._get_next_yaw_pitch_by_pid(yaw_pid_target=self.params.manual_yaw_compensation,
                                                        pitch_pid_target=self.pitch_compensation)
            else:
                self._get_next_yaw_pitch_by_stay_or_search()
            if self.params.if_enable_mouse_control:
                self.next_yaw,self.next_pitch = self.__trans_mouse_pos_to_next_yaw_pitch(self.mouse_control.get_mouse_position())
            
            SHIFT_LIST_AND_ASSIG_VALUE(self.target_list,self.target)
            
            if_locked = self._if_target_locked()
            if if_locked:
                self.fire_times = 1
                lr1.warn("Target Locked, FIRE")
                if self.mode == 'Dbg':
                    lr1.debug(f"Target Locked, FIRE, img_x: {self.target.tvec[0]:.2f}, img_y: {self.target.tvec[2]}")
                    
            else:
                self.fire_times = 0
                if self.mode == 'Dbg':
                    if self.params.fire_mode != 0:
                        lr1.debug(f"Target Not Locked, NOT FIRE, img_x: {self.target.tvec[0]:.2f}, img_y: {self.target.tvec[2]}")
            
            self.reserved_slot = 10
            if self.params.record_data_path is not None:
                SHIFT_LIST_AND_ASSIG_VALUE(self.tvec_history_list,self.target.tvec)
                self.data_recorder.record_data(np.array(self.tvec_history_list).reshape(-1,3),np.array([self.next_yaw,self.next_pitch]))
            if self.mode == 'Dbg':
                lr1.debug("Doing Search and Fire")
            return self._check_if_sentry_state_changed()
        
        elif self.action_count == 0:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Search and Fire Finished")
            return True

    def auto_bounce_back(self,arg):
        if self.action_count == -1:
            self.action_count = arg
            if self.mode == 'Dbg':
                lr1.debug("Start Auto Bounce Back")
            return False
        elif self.action_count > 0:
            self.action_count -= 1
            self.next_pitch = self.cur_pitch
            self.next_yaw = self.cur_yaw
            self.fire_times = 0
            self.reserved_slot = 20
            if self.mode == 'Dbg':
                lr1.debug("Doing Auto Bounce Back")
            return False
        elif self.action_count == 0:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Auto Bounce Back Finished")
            return True
            
        
    def doing_nothing(self,arg):
        if self.action_count == -1:
            self.action_count = arg 
            if self.mode == 'Dbg':
                lr1.debug("Start Doing Nothing")
            return False
        elif self.action_count > 0:
            self.action_count -= 1
            self.next_pitch = self.cur_pitch
            self.next_yaw = self.cur_yaw
            self.fire_times = 0
            self.reserved_slot = 10
            if self.mode == 'Dbg':
                lr1.debug("Doing Doing Nothing")
            return False
        elif self.action_count == 0:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Doing Nothing Finished")
            return True
    
    
    def go_right(self,arg):
        if self.action_count == -1:
            self.action_count = arg
            if self.mode == 'Dbg':
                lr1.debug("Start Go Right")
            return False
        elif self.action_count > 0:
            if abs(self.cur_yaw - (-1.5708)) > self.params.direction_accept_error:
                self.next_yaw = -1.5708
                self.next_pitch = 0.0
                self.fire_times = 0
                self.reserved_slot = 10
                if self.mode == 'Dbg':
                    lr1.debug("Turn Right")
                return False
            else:
                self.action_count -= 1
                self.next_yaw = self.cur_yaw
                self.next_pitch = 0.0
                self.fire_times = 0
                self.reserved_slot = 12
                if self.mode == 'Dbg':
                    lr1.debug("Go Right")
                return False
            
        elif self.action_count == 0:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Go Right Finished")
            return True
        
    def go_left(self,arg):
        if self.action_count == -1:
            self.action_count = arg
            if self.mode == 'Dbg':
                lr1.debug("Start Go Left")
            return False
        elif self.action_count > 0:
            if abs(self.cur_yaw - 1.5708) > self.params.direction_accept_error:
                self.next_yaw = 1.5708
                self.next_pitch = 0.0
                self.fire_times = 0
                self.reserved_slot = 10
                if self.mode == 'Dbg':
                    lr1.debug("Turn Left")
                return False
            else:
                self.action_count -= 1
                self.next_yaw = self.cur_yaw
                self.next_pitch = 0.0
                self.fire_times = 0
                self.reserved_slot = 12
                if self.mode == 'Dbg':
                    lr1.debug("Go Left")
                return False
            
        elif self.action_count == 0:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Go Left Finished")
            return True
    
    def go_forward(self,arg):
        if self.action_count == -1:
            self.action_count = arg
            if self.mode == 'Dbg':
                lr1.debug("Start Go Forward")
            return False
        elif self.action_count > 0:
            if abs(self.cur_yaw - 0.0) > self.params.direction_accept_error:
                self.next_pitch = 0.0
                self.next_yaw = 0.0
                self.fire_times = 0
                self.reserved_slot = 10
                if self.mode == 'Dbg':
                    lr1.debug("Turn to 0.0")
                return False
            else:
                self.action_count -= 1
                self.next_pitch = 0.0
                self.next_yaw = self.cur_yaw
                self.fire_times = 0
                self.reserved_slot = 12
                if self.mode == 'Dbg':
                    lr1.debug("Go Forward")
                return False
            
        elif self.action_count == 0:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Go Forward Finished")
            return True
    
    def go_backward(self,arg):
        
        if self.action_count == -1:
            self.action_count = arg
            if self.mode == 'Dbg':
                lr1.debug("Start Go Backward")
            return False
        elif self.action_count > 0:
            if abs(self.next_yaw - 0.0) > self.params.direction_accept_error:
                self.next_pitch = 0.0
                self.next_yaw = 0.0
                self.fire_times = 0
                self.reserved_slot = 10
                if self.mode == 'Dbg':
                    lr1.debug("Turn to 0.0")
                return False
            else:
                self.action_count -= 1
                self.next_pitch = 0.0
                self.next_yaw = self.cur_yaw
                self.fire_times = 0
                self.reserved_slot = 11
                if self.mode == 'Dbg':
                    lr1.debug("Go Backward")
                return False
            
        elif self.action_count == 0:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Go Backward Finished")
            return True
        
    def search_friend(self,arg):
        if self.action_count == -1:
            self.action_count = 999
            if self.mode == 'Dbg':
                lr1.debug("Start Search Friend")
            return False
        elif self.action_count > 0:
            if self.bro_target:
                if self.bro_target.name.split('_')[0] == 'friend':
                    self.bro_found = True
                    lr1.warn(f"Bro Found Friend {self.bro_target.name}")
            if self.i_found and self.bro_found:
                self.action_count = -1
                if self.mode == 'Dbg':
                    lr1.debug("Search Friend Finished")
                return True
            else:
                last_update_target_list = self._find_last_update_target()
                if len(last_update_target_list) != 0:
                    for target in last_update_target_list:
                        if target.name.split('_')[0] == 'friend':
                            self.i_found = True
                            self.next_yaw = self.params.search_friend_yaw
                            self.next_pitch = np.pi/2
                            lr1.warn(f"I Found Friend {target.name}")
                            break
                else:
                    self.next_yaw = self.params.search_friend_yaw
                    self.next_pitch = self.params.search_friend_pitch
                self.fire_times = 0
                self.reserved_slot = 10
                return False
        else:
            self.action_count = -1
            if self.mode == 'Dbg':
                lr1.debug("Search Friend Finished")
            return True
        
    def _search_target(self):
        """
        Returns:
            yaw, pitch
        """
        if self.yaw_search_add:
            next_yaw = self.cur_yaw + self.params.yaw_search_step
            if self.params.yaw_mechanical_positive_limit_switch != 0:
                if next_yaw > self.cur_big_gimbal_yaw + self.params.yaw_mechanical_positive_limit_switch:
                    next_yaw = self.cur_big_gimbal_yaw + self.params.yaw_mechanical_positive_limit_switch - self.params.yaw_search_step
                    self.yaw_search_add = False
        else:
            next_yaw = self.cur_yaw - self.params.yaw_search_step
            if self.params.yaw_mechanical_negative_limit_switch != 0:
                if next_yaw < self.cur_big_gimbal_yaw + self.params.yaw_mechanical_negative_limit_switch:
                    next_yaw = self.cur_big_gimbal_yaw + self.params.yaw_mechanical_negative_limit_switch + self.params.yaw_search_step
                    self.yaw_search_add = True
        if self.pitch_search_add:
            next_pitch = self.cur_pitch + self.params.pitch_search_step
            if next_pitch > self.params.pitch_max:
                next_pitch = self.params.pitch_max - self.params.pitch_search_step
                self.pitch_search_add = False
        else:
            next_pitch = self.cur_pitch - self.params.pitch_search_step
            if next_pitch < self.params.pitch_min:
                next_pitch = self.params.pitch_min + self.params.pitch_search_step
                self.pitch_search_add = True
                
        return next_yaw,next_pitch
                
        
    def __trans_mouse_pos_to_next_yaw_pitch(self,cur_mouse_pos:tuple):
        delta_x = cur_mouse_pos[0] - self.mouse_pos_prior[0]
        delta_y = cur_mouse_pos[1] - self.mouse_pos_prior[1]
        self.mouse_pos_prior = cur_mouse_pos
        
        move_yaw = -delta_x / self.params.screen_width * 100/180 * np.pi
        move_pitch = -delta_y / self.params.screen_height * 100/180 * np.pi
        next_yaw = move_yaw * self.params.x_axis_sensitivity
        next_pitch = move_pitch * self.params.y_axis_sensitivity
        
        return next_yaw,next_pitch
        

    def _find_last_update_target(self)->list:
        last_update_target = []
        for armor_params in self.armor_state_list:
            if armor_params.if_update:
                last_update_target.append(armor_params)
        return last_update_target
    
    def _choose_target(self, last_update_target_list:list):
        
        if len(last_update_target_list) == 0:
            self.target.if_update = False
        else:
            for target in last_update_target_list:
                if target.name[1] == self.params.primary_target:
                    self.target = target
                    return
            if self.params.choose_mode == 0:
                self.target = max(last_update_target_list,key=lambda x:x.continuous_detected_num)
            elif self.params.choose_mode == 1:
                self.target = min(last_update_target_list,key=lambda x:x.tvec[1])
            elif self.params.choose_mode == 2:
                last_update_target_list.sort(key = lambda x : x.tvec[1])
                for target in last_update_target_list:
                    if target.continuous_detected_num >= int(self.params.continuous_detected_num_for_track/2)+1:
                        self.target = target
                        break
                else:
                    self.target = last_update_target_list[0]
            for target in last_update_target_list:
                if target.name == self.target.name and target.id != self.target.id:
                    if target.tvec[0] < self.target.tvec[0]:
                        self.target = target
                        break
    
    def _get_next_yaw_pitch_by_stay_or_search(self):
        
        if self.target.continuous_lost_num < self.params.continuous_lost_num_max_threshold:
            self.next_yaw =  self.cur_yaw
            self.next_pitch =  self.cur_pitch
            lr1.warn(f"Stay cause blink {self.target.name} {self.target.id} , d,l = {self.target.continuous_detected_num}, {self.target.continuous_lost_num}")
        
        else: 
            self.next_yaw,self.next_pitch = self._search_target()
            if self.params.if_use_pid_control:
                self.yaw_pid_controller.reset()
                self.pitch_pid_controller.reset()
            lr1.warn(f'Search {self.target.name} {self.target.id} , d,l = {self.target.continuous_detected_num}, {self.target.continuous_lost_num}')
    
    def _get_next_yaw_pitch_by_ballistic(self):
        
        fire_yaw,fire_pitch,flight_time,if_success = self.ballistic_predictor.get_fire_yaw_pitch(self.target.tvec,
                                                    self.cur_yaw,
                                                    self.cur_pitch)
        if if_success:
            self.next_yaw = fire_yaw
            self.next_pitch = fire_pitch
            lr1.warn(f"Track {self.target.name} {self.target.id} , d,l = {self.target.continuous_detected_num}, {self.target.continuous_lost_num}")
            if self.mode == 'Dbg':
                lr1.debug(f"cy = {self.cur_yaw:.3f}, cp = {self.cur_pitch:.3f}, fy = {fire_yaw:.3f}, fp = {fire_pitch:.3f}, ny = {self.next_yaw:.3f}, np = {self.next_pitch:.3f}, x = {self.target.tvec[0]:.3f}, y = {self.target.tvec[1]:.3f}, z = {self.target.tvec[2]:.3f}")
            
        else:
            self.next_yaw = self.cur_yaw
            self.next_pitch =  self.cur_pitch
            lr1.warn(f"Stay cause fail to solve , d,l = {self.target.continuous_detected_num}, {self.target.continuous_lost_num}")
    
    def _get_next_yaw_pitch_by_pid(self,yaw_pid_target:float = 0.0,pitch_pid_target:float = 0.0):
        
        img_x = self.target.tvec[0]
        img_y = self.target.tvec[2]
        relative_yaw = -np.arctan(img_x / 651.7) 
        pid_rel_yaw = -self.yaw_pid_controller.get_output(yaw_pid_target,relative_yaw)
        relative_pitch = -np.arctan(img_y /581.3) # 581.3 = 384/2/np.tan(0.319),384 is 
        pid_rel_pit = self.pitch_pid_controller.get_output(pitch_pid_target,relative_pitch) 
        self.next_yaw = self.cur_yaw + pid_rel_yaw
        self.next_pitch = self.cur_pitch + pid_rel_pit
            
        lr1.warn(f"Track {self.target.name} {self.target.id} , d,l = {self.target.continuous_detected_num}, {self.target.continuous_lost_num}")
        if self.mode == 'Dbg':  
            lr1.debug(f"cy = {self.cur_yaw:.3f}, cp = {self.cur_pitch:.3f}, ry = {relative_yaw:.3f}, rp = {relative_pitch:.3f}, ny = {self.next_yaw:.3f}, np = {self.next_pitch:.3f}, x = {self.target.tvec[0]:.3f}, y = {self.target.tvec[1]:.3f}, z = {self.target.tvec[2]:.3f}")
    
    
    def _if_target_locked(self):
        if self.params.fire_mode == 1 or self.params.fire_mode == 2:
            if self.target.continuous_detected_num >= self.params.continuous_detected_num_for_track\
            and self.target.if_update \
            and abs(self.target.tvec[0]) < self.params.min_img_x_for_locked\
            and self.__find_update_target_num() >= self.params.min_continuous_detected_num_for_lock\
            and self.__find_continuous_track_armor_num() >= self.params.min_continuous_detected_num_for_lock:
                return True
        else:
            return False

    def __find_update_target_num(self)->int:
        for i, target in enumerate(self.target_list):
            if target.if_update:
                continue
            else:
                return i
        return len(self.target_list)
    
    def __find_continuous_track_armor_num(self):
        for i in range(len(self.target_list)-1):
            if self.target_list[i].name == self.target_list[i+1].name and self.target_list[i].id == self.target_list[i+1].id:
                continue
            else:
                return i + 1
        return len(self.target_list)
    
                
    def force_enable_mouse_control(self,
                                   data_path:Union[str,None]=None):
        self.params.if_enable_mouse_control = True
        
        if data_path is not None:
            self.data_recorder = Data_Recoreder(data_path,
                                            (self.params.tvec_history_length,3),
                                            (2,),
                                            np.float32,
                                            np.float32)
            
            self.tvec_history_list = [np.zeros(3) for i in range(self.params.tvec_history_length)]
        
        self.mouse_control = KeyboardAndMouseControl('Rel',if_enable_key_board=False,if_enable_mouse_control=True)
        self.mouse_pos_prior = (0,0)
    
    
    def _check_if_sentry_state_changed(self)->bool:
        if self.sentry_state == self.pre_sentry_state + 1:
            self.pre_sentry_state = self.sentry_state
            if self.mode == 'Dbg':
                lr1.debug(f"Sentry state changed to {self.sentry_state}")
            self.action_count = -1
            return True
        else:
            return False
        
        
            