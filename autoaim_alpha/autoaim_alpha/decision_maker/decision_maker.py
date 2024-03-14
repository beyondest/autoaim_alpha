from .ballistic_predictor import *
from .observer import *
from ..os_op.basic import *
from ..os_op.global_logger import *
from .ballistic_predictor import *

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
                 ballistic_predictor_params_yaml_path:Union[str,None] = None,
                 enemy_car_list:list = None
                 ) -> None:
        CHECK_INPUT_VALID(mode,"Dbg",'Rel')
        if enemy_car_list is None:
            lr1.critical("enemy_car_list is None")
            
        self.mode = mode
        self.params = Decision_Maker_Params()
        self.ballistic_predictor = Ballistic_Predictor(mode,
                                                       ballistic_predictor_params_yaml_path)
        
        self.enemy_car_list = enemy_car_list
        if decision_maker_params_yaml_path is not None:
            self.params.load_params_from_yaml(decision_maker_params_yaml_path)
        
        if ballistic_predictor_params_yaml_path is not None:
            self.ballistic_predictor.params.load_params_from_yaml(ballistic_predictor_params_yaml_path)
        
        else:
            lr1.warning("Ballistic_Predictor use default params")
            
        self.armor_state_list = [Armor_Params(enemy_car['armor_name'],armor_id) \
                                                        for enemy_car in self.enemy_car_list \
                                                            for armor_id in range(enemy_car['armor_nums'])]
        
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        self.remaining_health = 0.0
        self.remaining_ammo = 0
        self.electric_system_zero_unix_time = None
        self.electric_system_unix_time = time.time()
        self._init_yaw_pitch_search_data()
        

        
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
    
    def make_decision(self)->tuple:
        """

        Args:
            target_armor_params (Armor_Params): _description_

        Returns:
            tuple: next_yaw,next_pitch, fire_times
        """
        max_continous_detected_armor = max(self.armor_state_list,key=lambda x:x.continuous_detected_num)
        if max_continous_detected_armor.if_update:
            fire_yaw,fire_pitch,flight_time,if_success = self.ballistic_predictor.get_fire_yaw_pitch(max_continous_detected_armor.tvec,
                                                        self.cur_yaw,
                                                        self.cur_pitch)
            if if_success:
                next_yaw = fire_yaw
                next_pitch = fire_pitch
                fire_times = 1
                lr1.warn(f"Target Locked {max_continous_detected_armor.name} {max_continous_detected_armor.id} , d,l = {max_continous_detected_armor.continuous_detected_num}, {max_continous_detected_armor.continuous_lost_num}")
            else:
                next_yaw,next_pitch = self.cur_yaw,self.cur_pitch
                fire_times = 0
                lr1.info(f"Ballistic_Predictor failed, use current yaw and pitch, d,l = {max_continous_detected_armor.continuous_detected_num}, {max_continous_detected_armor.continuous_lost_num}")
        else:
            next_yaw,next_pitch = self._search_target()
            fire_times = 0
            lr1.warn(f'Target Lost {max_continous_detected_armor.name} {max_continous_detected_armor.id} , d,l = {max_continous_detected_armor.continuous_detected_num}, {max_continous_detected_armor.continuous_lost_num}')
            
        return next_yaw,next_pitch, fire_times
            
    
    def _search_target(self):
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
 