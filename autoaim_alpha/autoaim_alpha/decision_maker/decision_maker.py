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
        
        self.yaw_left_degree = -100
        self.yaw_right_degree = 100
        self.pitch_down_degree = -10
        self.pitch_up_degree = 15
        self.yaw_search_step = 0.01
        self.pitch_search_left_waves = 5
        self.pitch_search_right_waves = 6
        self.sleep_time_after_move_command = 0.1
        self.repeat_times_for_move_command = 3
        
        self.yaw_pitch_history_length = 20
        
        self.continuous_detected_num_min_threshold = 2
        self.continuous_lost_num_max_threshold = 4
        
        self.relative_yaw_move_step = 0.05
        self.relative_pitch_move_step = 0.05
        self.yaw_idx_max = 100
        self.pitch_idx_max = 10
        
        self.tvec_history_length = 10
        self.screen_width = 1920
        self.screen_height = 1080
        
        # 0.0 - 1.0
        self.x_axis_sensitivity = 0.5
        self.y_axis_sensitivity = 0.5
        self.if_enable_mouse_control = False
        self.record_data_path = None
        
        self.fire_mode = 1
        """fire mode:
            0: firepower priority
            1: accuracy priority
            2: fixed angle interval shooting
        """
        

class Decision_Maker:
    def __init__(self,
                 mode:str,
                 decision_maker_params_yaml_path:Union[str,None] = None,
                 pid_controller_params_yaml_path:Union[str,None] = None,
                 ballistic_predictor:Union[Ballistic_Predictor,None] = None,
                 enemy_car_list:list = None,
                 if_relative: bool = False
                 ) -> None:
        CHECK_INPUT_VALID(mode,"Dbg",'Rel')
        if enemy_car_list is None:
            lr1.critical("enemy_car_list is None")
            
        self.mode = mode
        self.params = Decision_Maker_Params()
        self.ballistic_predictor = ballistic_predictor
        self.pid_controller = PID_Controller()
        self.pid_controller.load_params_from_yaml(pid_controller_params_yaml_path)
        self.enemy_car_list = enemy_car_list
        if decision_maker_params_yaml_path is not None:
            self.params.load_params_from_yaml(decision_maker_params_yaml_path)
        if pid_controller_params_yaml_path is None:
            lr1.warning("pid_controller_params_yaml_path is None, use default params")
        else:
            self.pid_controller.load_params_from_yaml(pid_controller_params_yaml_path)

        self.armor_state_list = [Armor_Params(enemy_car['armor_name'],armor_id) \
                                                        for enemy_car in self.enemy_car_list \
                                                            for armor_id in range(enemy_car['armor_nums'])]
        
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        self.remaining_health = 0.0
        self.remaining_ammo = 200
        self.electric_system_zero_unix_time = None
        self.electric_system_unix_time = time.time()
        self._init_yaw_pitch_search_data()
        
        self.yaw_idx = 0
        self.yaw_add = True
        self.pitch_idx = 0
        self.pitch_add = True
        self.if_relative = if_relative
        
       
        # this is for predict
        self.next_yaw_history_list = [0.0 for i in range(self.params.yaw_pitch_history_length)]
        self.next_pitch_history_list = [0.0 for i in range(self.params.yaw_pitch_history_length)]
        
        
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
        
        for armor_params in self.armor_state_list:
            
            if armor_params.name == armor_name and armor_params.id == armor_id:
                armor_params.tvec = armor_tvec
                armor_params.rvec = armor_rvec
                armor_params.confidence = armor_confidence
                armor_params.time = armor_time
                armor_params.continuous_detected_num = continuous_detected_num
                armor_params.continuous_lost_num = continuous_lost_num
                armor_params.if_update = if_update
                
                
    def update_our_side_info(self,
                             cur_yaw:float,
                             cur_pitch:float,
                             ele_unix_time:float,
                             remaining_health:Union[float,None] = None,
                             remaining_ammo:Union[float,None] = None,
                             fire_mode:Union[int,None] = None)->None:
        
        self.cur_yaw = cur_yaw
        self.cur_pitch = cur_pitch
        self.electric_system_unix_time = ele_unix_time
        
        if remaining_health is not None:
            self.remaining_health = remaining_health
        if remaining_ammo is not None:
            self.remaining_ammo = remaining_ammo
        if fire_mode is not None:
            self.params.fire_mode = fire_mode    


    def save_params_to_yaml(self,yaml_path:str)->None:
        self.params.save_params_to_yaml(yaml_path)
    
    def make_decision(self,if_use_pid:bool = False)->tuple:
        """
        Args:
            target_armor_params (Armor_Params): _description_
        Returns:
            tuple: next_yaw,next_pitch, fire_times
        """
        
        target = max(self.armor_state_list,key=lambda x:x.continuous_detected_num)
        if target.if_update and target.continuous_detected_num >= self.params.continuous_detected_num_min_threshold:
            
            if not if_use_pid:
                fire_yaw,fire_pitch,flight_time,if_success = self.ballistic_predictor.get_fire_yaw_pitch(target.tvec,
                                                            self.cur_yaw,
                                                            self.cur_pitch)
                if if_success:
                    next_yaw = fire_yaw if not self.if_relative else fire_yaw - self.cur_yaw
                    next_pitch = fire_pitch if not self.if_relative else fire_pitch - self.cur_pitch
                    fire_times = 1
                    lr1.warn(f"Target Locked {target.name} {target.id} , d,l = {target.continuous_detected_num}, {target.continuous_lost_num}")
                    if self.mode == 'Dbg':
                        lr1.debug(f"cur_yaw = {self.cur_yaw}, cur_pitch = {self.cur_pitch}, fire_yaw = {fire_yaw}, fire_pitch = {fire_pitch}")
                    
                else:
                    next_yaw = 0.0 if self.if_relative else self.cur_yaw
                    next_pitch = 0.0 if self.if_relative else self.cur_pitch
                    fire_times = 0
                    lr1.warn(f"Bad Target, Stay, d,l = {target.continuous_detected_num}, {target.continuous_lost_num}")
            
            else:
                relative_yaw = -np.arctan2(target.tvec[0],target.tvec[1]) 
                next_yaw = -self.pid_controller.get_output(0.0,relative_yaw) if self.if_relative else self.cur_yaw + next_yaw
                relative_pitch = -np.arctan2(target.tvec[2],target.tvec[1])
                next_pitch = -self.pid_controller.get_output(0.0,relative_pitch) if self.if_relative else self.cur_pitch + next_pitch
                fire_times = 1
                lr1.warn(f"Target Locked {target.name} {target.id} , d,l = {target.continuous_detected_num}, {target.continuous_lost_num}")
                if self.mode == 'Dbg':  
                    lr1.debug(f"cur_yaw = {self.cur_yaw}, cur_pitch = {self.cur_pitch}, relative_yaw = {relative_yaw}, relative_pitch = {relative_pitch}")
        
        else:
            if target.continuous_lost_num < self.params.continuous_lost_num_max_threshold:
                next_yaw = 0.0 if self.if_relative else self.cur_yaw
                next_pitch = 0.0 if self.if_relative else self.cur_pitch
                fire_times = 0
                lr1.warn(f"Target Blink, Stay {target.name} {target.id} , d,l = {target.continuous_detected_num}, {target.continuous_lost_num}")
            
            else: 
                next_yaw,next_pitch = self._search_target()
                if if_use_pid:
                    self.pid_controller.reset()
                fire_times = 0
                lr1.warn(f'Target Lost {target.name} {target.id} , d,l = {target.continuous_detected_num}, {target.continuous_lost_num}')
        
        if self.params.if_enable_mouse_control:
            next_yaw,next_pitch = self.__trans_mouse_pos_to_next_yaw_pitch(self.mouse_control.get_mouse_position())
            
        SHIFT_LIST_AND_ASSIG_VALUE(self.next_yaw_history_list,next_yaw)
        SHIFT_LIST_AND_ASSIG_VALUE(self.next_pitch_history_list,next_pitch) 
        
        
        if self.if_relative:
            next_yaw = CIRCLE(next_yaw, [-np.pi, np.pi])
            next_pitch = CIRCLE(next_pitch, [-np.pi, np.pi])
        else:
            next_yaw = CLAMP(next_yaw, [-np.pi, np.pi])
            next_pitch = CLAMP(next_pitch, [-np.pi, np.pi])
            
        
        
        if self.params.record_data_path is not None:
            SHIFT_LIST_AND_ASSIG_VALUE(self.tvec_history_list,target.tvec)
            self.data_recorder.record_data(np.array(self.tvec_history_list).reshape(-1,3),np.array([next_yaw,next_pitch]))
        
        
        return next_yaw,next_pitch, fire_times

    
    def _search_target(self):
        if self.if_relative:
            
            if self.yaw_add:
                yaw = self.params.relative_yaw_move_step
                self.yaw_idx += 1
                if self.yaw_idx >= self.params.yaw_idx_max:
                    self.yaw_add = False
                    
            else:
                yaw = -self.params.relative_yaw_move_step
                self.yaw_idx -= 1
                if self.yaw_idx < 0:
                    self.yaw_add = True
                    
            if self.pitch_add:
                pitch = self.params.relative_pitch_move_step
                self.pitch_idx += 1
                if self.pitch_idx >= self.params.pitch_idx_max:
                    self.pitch_add = False
                    
            else:
                pitch = -self.params.relative_pitch_move_step
                self.pitch_idx -= 1
                if self.pitch_idx < 0:
                    self.pitch_add = True
                    
        else:
            yaw,pitch = self._get_next_yaw_pitch()
        
        return yaw,pitch
    
    def _init_yaw_pitch_search_data(self):
        self.search_index = 0
        # mode 0 : not in search mode, mode 1 : search from right to left, mode 2 : search from left to right 
        self.search_mode = 1
        self.yaw_left = self.params.yaw_left_degree/180 * np.pi
        self.yaw_right = self.params.yaw_right_degree/180 * np.pi
        self.pitch_down = self.params.pitch_down_degree/180 * np.pi
        self.pitch_up = self.params.pitch_up_degree/180 * np.pi
        self.yaw_search_step = self.params.yaw_search_step
        
        self.yaw_search_data = np.round(np.arange(self.yaw_left,self.yaw_right,self.yaw_search_step),3)
        
        self.pitch_search_left = -self.params.pitch_search_left_waves * np.pi
        self.pitch_search_right = self.params.pitch_search_right_waves * np.pi
        # 1 wave down, 1 wave up, 1 wave down, 1 wave up, 1 wave down
        
        self.pitch_search_step = ((self.pitch_search_right - self.pitch_search_left)/len(self.yaw_search_data))
        self.pitch_search_data =np.round(np.sin(np.arange(self.pitch_search_left, self.pitch_search_right, self.pitch_search_step)),3) 
        for i, pitch in enumerate(self.pitch_search_data):
            if pitch < 0:
                self.pitch_search_data[i] = pitch * self.pitch_down
            else:
                self.pitch_search_data[i] = pitch * self.pitch_up
                
                
                
    def _get_next_yaw_pitch(self):
        if self.search_mode:
            next_yaw = self.yaw_search_data[self.search_index]
            next_pitch = self.pitch_search_data[self.search_index]
            
            if self.search_mode == 1:
                self.search_index += 1
            else:
                self.search_index -= 1
        
        else:
            self.search_mode = 1
            self.search_index = int((self.cur_yaw - self.yaw_left)/self.yaw_search_step)
            if self.search_index >= len(self.yaw_search_data):
                self.search_index = len(self.yaw_search_data) - 1
            elif self.search_index < 0:
                self.search_index = 0
            next_yaw = self.yaw_search_data[self.search_index]
            next_pitch = self.pitch_search_data[self.search_index]
        
        if self.search_index >= len(self.yaw_search_data):
                
            self.search_mode = 2
            self.search_index = len(self.yaw_search_data) - 1
            
        elif self.search_index < 0:
            self.search_mode = 1 
            self.search_index = 0
            
        return next_yaw,next_pitch


        
    def __trans_mouse_pos_to_next_yaw_pitch(self,cur_mouse_pos:tuple):
        delta_x = cur_mouse_pos[0] - self.mouse_pos_prior[0]
        delta_y = cur_mouse_pos[1] - self.mouse_pos_prior[1]
        self.mouse_pos_prior = cur_mouse_pos
        
        move_yaw = -delta_x / self.params.screen_width * 100/180 * np.pi
        move_pitch = -delta_y / self.params.screen_height * 100/180 * np.pi
        
        if self.if_relative:
        
            next_yaw = self.cur_yaw + move_yaw * self.params.x_axis_sensitivity
            next_pitch = self.cur_pitch + move_pitch * self.params.y_axis_sensitivity
            
        else:
            next_yaw = move_yaw * self.params.x_axis_sensitivity
            next_pitch = move_pitch * self.params.y_axis_sensitivity
        
        return next_yaw,next_pitch
        
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
            