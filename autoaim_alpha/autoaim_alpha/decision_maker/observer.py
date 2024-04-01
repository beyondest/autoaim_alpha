from ..os_op.basic import *
from ..os_op.global_logger import *
import numpy as np
import time
from .tools import *



       

class Armor_Params(Params):
    
    def __init__(self,
                 name:str,
                 id :int
                 ):
        super().__init__()
        
        self.tvec = np.zeros(3)                             # in camera frame
        self.rvec = np.array([0.0,0.0,1.0])                 # in camera frame

        self.time = time.time()     
        self.confidence = 0.0
        self.id = id 
        self.name = name 
        self.continuous_detected_num = 0
        self.continuous_lost_num = 0
        self.if_update = False
        


class Car_Params(Params):
    def __init__(self,
                 armor_name:str,
                 history_depth:int=10,
                 armor_nums:int=4,
                 ):
        """Armor id:(counterclockwise)
            0: front
            1: right
            2: back
            3: left
            
          or 
            0: front
            1: back

        Args:
            armor_name (str, optional): _description_. Defaults to '3x'.
            history_depth (int, optional): _description_. Defaults to 10.
            armor_nums (int, optional): _description_. Defaults to 4.
        """
        
        super().__init__()
        
        
        self.armor_name = armor_name
        self.history_depth = history_depth
        self.armor_nums = armor_nums
        


        self.armor_idx_to_detect_history = {}
        
        for i in range(self.armor_nums):
            
            self.armor_idx_to_detect_history.update({i :  [Armor_Params(armor_name,i) for j in range(self.history_depth)]})
            
            
class Observer_Params(Params):
    
    def __init__(self):
        
        super().__init__()
        


        
        self.history_depth = 5
        self.continuous_detected_num_max = 3
        self.continuous_lost_num_max = 3
        self.armor_name_to_car_params = {}

        
    def init_each_car_params(self,enemy_car_list:list):
        for i in enemy_car_list:
            if i['armor_name'] in self.armor_name_to_car_params.keys():
                lr1.critical(f"Observer: armor_name {i['armor_name']} already exists in car_params")
                raise ValueError("armor_name already exists in car_params")
                
                
            dic = {i['armor_name']:Car_Params(i['armor_name'],
                                              self.history_depth,
                                              i['armor_nums'])}
            
            self.armor_name_to_car_params.update(dic)
            lr1.info(f"Observer: Init {i['armor_name']} car params")
        
        lr1.info(f"Observer: All Enemy Car Nums: {len(enemy_car_list)}  ")
        
       
    
    
class Observer:
    
    def __init__(self,
                 mode:str = 'Dbg',
                 observer_params_yaml_path:Union[str,None]=None,
                 enemy_car_list:list = None
                 ):
        
        CHECK_INPUT_VALID(mode,'Dbg','Rel')
        if enemy_car_list is None:
            lr1.critical(f"Observer: enemy_car_list is None")
        self.mode = mode
        self.observer_params = Observer_Params()
        self.enemy_car_list = enemy_car_list
        if observer_params_yaml_path is not None:
            self.observer_params.load_params_from_yaml(observer_params_yaml_path)
            self.observer_params.init_each_car_params(enemy_car_list)
        
        else:
            lr1.warning(f"Observer: observer_params_yaml_path is None, use default params")
            self.observer_params.init_each_car_params()
    
    def update_by_detection_list(self,
               all_targets_list:list):
        """
        Args:
            all_targets_list (list): list of all targets, each target is a dict, including:
                'armor_name':str,
                'tvec':np.ndarray, (3,)
                'rvec':np.ndarray, (3,)
                'time':float
        """
        self._assign_armor_idx(all_targets_list)
        target_name_list = [i['armor_name'] for i in all_targets_list]
        for car_params in self.observer_params.armor_name_to_car_params.values():
            for armor_id in range(car_params.armor_nums):
                if car_params.armor_name + str(armor_id) in target_name_list:
                    target_index = target_name_list.index(car_params.armor_name + str(armor_id))
                    tvec = all_targets_list[target_index]['tvec']
                    rvec = all_targets_list[target_index]['rvec']
                    t = all_targets_list[target_index]['time']
                    self.update_by_detection(car_params.armor_name,
                                            armor_id,
                                            tvec,
                                            rvec,
                                            t ,
                                            True
                                            )
                else:
                    self.update_by_detection(car_params.armor_name,
                                            armor_id,
                                            np.zeros(3),
                                            np.array([0.0,0.0,1.0]),
                                            0.0,
                                            False
                                            )
                
    def update_by_detection(self,
                            armor_name:str,
                            armor_id:int,
                            tvec:np.ndarray,
                            rvec:np.ndarray,
                            t:float,
                            if_update:bool):
        
        if if_update:
            armor_idx_to_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history
            continuous_detected_num = armor_idx_to_list[armor_id][0].continuous_detected_num + 1
            continuous_detected_num = CLAMP(continuous_detected_num, [0,self.observer_params.continuous_detected_num_max])
            continuous_lost_num = armor_idx_to_list[armor_id][0].continuous_lost_num * 0
            continuous_lost_num = CLAMP(continuous_lost_num, [0,self.observer_params.continuous_lost_num_max])
            new_armor_params = Armor_Params(armor_name,armor_id)
            new_armor_params.tvec = tvec
            new_armor_params.rvec = rvec
            new_armor_params.time = t
            new_armor_params.confidence = 1.0
            new_armor_params.if_update = True
            new_armor_params.continuous_detected_num = continuous_detected_num
            new_armor_params.continuous_lost_num = continuous_lost_num
            SHIFT_LIST_AND_ASSIG_VALUE(armor_idx_to_list[armor_id],new_armor_params)
            if self.mode == 'Dbg':
                lr1.info(f"Observer: Update armor detect params {armor_name} id {armor_id}")
                
        else:
            armor_idx_to_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history
            continuous_detected_num = armor_idx_to_list[armor_id][0].continuous_detected_num - 1
            continuous_detected_num = CLAMP(continuous_detected_num, [0,self.observer_params.continuous_detected_num_max])
            continuous_lost_num = armor_idx_to_list[armor_id][0].continuous_lost_num + 1
            continuous_lost_num = CLAMP(continuous_lost_num, [0,self.observer_params.continuous_lost_num_max])
            new_armor_params = Armor_Params(armor_name,armor_id)
            new_armor_params.tvec = armor_idx_to_list[armor_id][0].tvec
            new_armor_params.rvec = armor_idx_to_list[armor_id][0].rvec
            new_armor_params.time = armor_idx_to_list[armor_id][0].time
            new_armor_params.confidence = 0.0
            new_armor_params.continuous_detected_num = continuous_detected_num
            new_armor_params.continuous_lost_num = continuous_lost_num
            new_armor_params.if_update = False
            SHIFT_LIST_AND_ASSIG_VALUE(armor_idx_to_list[armor_id],new_armor_params)
            if self.mode == 'Dbg':
                lr1.info(f"Observer: Update armor detect params {armor_name} at t {t} with confidence {0.0}")

    def get_armor_latest_state(self)->list:
        """
        Args:
            if_correct_state : 
                True: get correct state
                False: get detect state
        Returns:
            list of Armor_Params

        """
        
        armor_list = []
        for armor_name in self.observer_params.armor_name_to_car_params.keys():
            for armor_id,armor_detect_history_list in self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history.items():
                armor_detect_latest_params = armor_detect_history_list[0]
                armor_list.append(armor_detect_latest_params)

        return armor_list
        
    

    def _update_armor_history_params(self,
                            armor_list:list,
                            tvec:np.ndarray,
                            rvec:np.ndarray,
                            time:float,
                            confidence:float,
                            continuous_detected_num:int,
                            continuous_lost_num:int,
                            if_update:bool):
        
        new_armor_params = Armor_Params(armor_list[0].name,armor_list[0].id)
        new_armor_params.tvec = tvec
        new_armor_params.rvec = rvec
        new_armor_params.time = time
        new_armor_params.confidence = confidence
        new_armor_params.continuous_detected_num = continuous_detected_num
        new_armor_params.continuous_lost_num = continuous_lost_num
        new_armor_params.if_update = if_update
        SHIFT_LIST_AND_ASSIG_VALUE(armor_list,new_armor_params)

            
            
 
    def _assign_armor_idx(self,all_targets_list:list):
        """Will assign armor_id to each target in all_targets_list
        
        If there are same armor_name targets, will assign armor_id based on img_x, the smaller the img_x the armor_id is 1, else 0
        
        If there is only one target for each armor_name , will assign armor_id 0

        Args:
            all_targets_list (list): _description_
        """
        for target_index,target in enumerate(all_targets_list):
            armor_name = target['armor_name']
            tar_img_x = target['tvec'][0]
            car_parmas = self.observer_params.armor_name_to_car_params[armor_name]
            if car_parmas.armor_nums == 2:
                id_0_dis = abs(tar_img_x - car_parmas.armor_idx_to_detect_history[0][0].tvec[0])
                id_1_dis = abs(tar_img_x - car_parmas.armor_idx_to_detect_history[1][0].tvec[0])
                if id_0_dis < id_1_dis:
                    target.update({'armor_name':armor_name + str(0)})
                else:
                    target.update({'armor_name':armor_name + str(1)})
            else:
                target.update({'armor_name':armor_name + str(0)})
            
            

            
    
    
    def save_params_to_yaml(self,yaml_path:str)->None:
        self.observer_params.armor_name_to_car_params = {}
        self.observer_params.save_params_to_yaml(yaml_path)
 
    
    def set_armor_initial_state(self):
        
        for car_params in self.observer_params.armor_name_to_car_params.values():
            armor_name = car_params.armor_name
            
            t = time.time()
            confidence =0.0
            
            tvec = np.array([0.0,0.0,0.0])
            rvec = np.array([0.0,0.0,1.0])
            

            for armor_id in range(car_params.armor_nums):
                armor_idx_to_list = car_params.armor_idx_to_detect_history
                new_armor_params = Armor_Params(armor_name,armor_id)
                new_armor_params.tvec = tvec
                new_armor_params.rvec = rvec
                new_armor_params.time = t
                new_armor_params.confidence = confidence
                new_armor_params.continuous_detected_num = 0
                new_armor_params.continuous_lost_num = 0
                new_armor_params.if_update = False
                SHIFT_LIST_AND_ASSIG_VALUE(armor_idx_to_list[armor_id],new_armor_params)
                
    