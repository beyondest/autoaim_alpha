from .ballistic_predictor import *
from .observer import *
from ..os_op.basic import *
from ..os_op.global_logger import *


class Decision_Maker_Params(Params):
    def __init__(self) -> None:
        super().__init__()
        
        self.yaw_left_degree = -100
        self.yaw_right_degree = 100
        self.pitch_down_degree = 10
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
                 enemy_car_list:list = None
                 ) -> None:
        CHECK_INPUT_VALID(mode,"Dbg",'Rel')
        if enemy_car_list is None:
            lr1.critical("enemy_car_list is None")
            
        self.mode = mode
        self.params = Decision_Maker_Params()
        self.enemy_car_list = enemy_car_list
        if decision_maker_params_yaml_path is not None:
            self.params.load_params_from_yaml(decision_maker_params_yaml_path)
            
        self.armor_state_predict_list = [Armor_Params(enemy_car['armor_name'],armor_id) \
                                                        for enemy_car in self.enemy_car_list \
                                                            for armor_id in range(enemy_car['armor_nums'])]
        
        self.cur_yaw = 0.0
        self.cur_pitch = 0.0
        self.remaining_health = 0.0
        self.remaining_ammo = 0
        self.electric_system_zero_unix_time = None
        self.electric_system_unix_time = time.time()
        
        

        
    def update_enemy_side_info(self,
                      armor_name:str,
                      armor_id:int,
                      armor_tvec:np.ndarray,
                      armor_rvec:np.ndarray,
                      armor_confidence:float = 0.5,
                      armor_time:float = 0.0)->None:
        
        for armor_params in self.armor_state_predict_list:
            
            if armor_params.name == armor_name and armor_params.id == armor_id:
                armor_params.tvec = armor_tvec
                armor_params.rvec = armor_rvec
                armor_params.confidence = armor_confidence
                armor_params.time = armor_time
                
                
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

    def choose_target(self)->Armor_Params:
        """Choose the nearest enemy car to shoot.

        Returns:
            Armor_Params: _description_
        """
        
        nearest_armor_params = min(self.armor_state_predict_list, key=lambda x: x.tvec[2])
        #latest is meaningless cause we update correct params together
        
        
        if self.mode == 'Dbg':
            pass
            #for armor_params in self.armor_state_predict_list:
                #lr1.debug(f"Decision_Maker : For chosen : armor {armor_params.name} id {armor_params.id} : tvec {armor_params.tvec}, t : {armor_params.time}")
                
        #lr1.debug(f"Decision_Maker : Choosed latest_armor_state: {latest_armor_params.name} id {latest_armor_params.id} : tvec {latest_armor_params.tvec}, t : {latest_armor_params.time}")
        
        return nearest_armor_params
    
    def save_params_to_yaml(self,yaml_path:str)->None:
        self.params.save_params_to_yaml(yaml_path)
    
    
    def search_target(self):
        yaw,pitch = self._get_next_yaw_pitch()
        return yaw,pitch
    
    def _init_yaw_pitch_search_data(self):
        self.search_index = 0
        # mode 0 : not in search mode, mode 1 : search from right to left, mode 2 : search from left to right 
        self.search_mode = 1
        
        self.yaw_left = -self.params.yaw_left_degree/180 * np.pi
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
 