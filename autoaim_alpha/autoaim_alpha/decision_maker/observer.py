from ..os_op.basic import *
from ..os_op.global_logger import *
import numpy as np
import time
from .tools import *


class Score_Obj_for_automatic_matching(Score_Obj):
    def __init__(self,
                 name:str,
                 attributes_list:list,
                 score_accumulation_method_list:list,
                 weights_list:list,
                 reference_list:list):
        
        """attribute_id (int):
                0: armor_name:str
                1: tvec_at_specific_time:np.ndarray, (3,)
                2: rvec_at_specific_time:np.ndarray, (3,)
                3: armor_confidence:float
            

        Args:
            name (str): _description_
            attributes_list (list): _description_
            score_accumulation_method_list (list): _description_
            reference_list (list): _description_
        """
        super().__init__(name,
                         attributes_list,
                         score_accumulation_method_list,
                         weights_list,
                         reference_list)
        
        
    
    def turn_attribute_to_score(self, attribute_id, attribute_value, reference_value: Union[float, None]) -> np.float:
        
        """
        Returns:
            float: _description_
        """
        
        if attribute_id == 0:
            return attribute_value == reference_value 
        
        if attribute_id == 1 :
            dis_criterion = 1
            min_dis = 0.01
            dis = np.linalg.norm(attribute_value - reference_value)
            if dis < min_dis:
                dis = min_dis
            return dis_criterion/dis
        
        if attribute_id == 2 :
            dis_criterion = 1
            min_dis = 0.01
            dis = np.linalg.norm(attribute_value - reference_value)
            if dis < min_dis:
                dis = min_dis
            return dis_criterion/dis
        

       

class Armor_Params(Params):
    
    def __init__(self,
                 name:str,
                 id :int
                 ):
        super().__init__()
        
        self.tvec = np.zeros(3)                             # in camera frame
        self.rvec = np.array([0.0,0.0,1.0])                 # in camera frame
        self.tvec_in_car_frame = np.zeros(3)                # in car frame
        self.rvec_in_car_frame = np.array([0.0,0.0,1.0])    # in car frame
        
        self.time = time.time()     
        self.confidence = 0.0
        self.id = id 
        self.name = name 
        self.continuous_detected_num = 0
        self.continuous_lost_num = 0
        self.if_update = False
        


class Car_Params(Params):
    def __init__(self,
                 armor_distance:list,
                 armor_name:str,
                 Q:np.ndarray,
                 R:np.ndarray,
                 H:np.ndarray,
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
            armor_distance_in_x_axis (float): _description_
            armor_name (str, optional): _description_. Defaults to '3x'.
            history_depth (int, optional): _description_. Defaults to 10.
            armor_nums (int, optional): _description_. Defaults to 4.
        """
        
        super().__init__()
        
        self.armor_distance = armor_distance
        
        self.armor_name = armor_name
        self.history_depth = history_depth
        self.armor_nums = armor_nums
        
        self.car_tvec_history_list = [np.zeros(3) for i in range(self.history_depth)]   # history of car center position in camera frame
        self.car_tv_vec_history_list = [np.zeros(3) for i in range(self.history_depth)]   # history of car center velocity in camera frame
        self.car_ta_vec_history_list = [np.zeros(3) for i in range(self.history_depth)]   # history of car center acceleration in camera frame
        self.car_rvec_history_list = [np.array([0.0,0.0,1.0]) for i in range(self.history_depth)]   # history of car rotation position in camera frame for i in range(self.history_depth)]   # history of armor_id 0 rotation position in camera frame
        self.car_rv_vec_history_list = [np.zeros(3) for i in range(self.history_depth)]   # history of armor_id 0 rotation velocity in camera frame
        self.car_ra_vec_history_list = [np.zeros(3) for i in range(self.history_depth)]   # history of armor_id 0 rotation acceleration in camera frame
        self.car_rotation_speed_history_list = [0.0 for i in range(self.history_depth)]   # history of car rotation speed in car frame
        
        self.car_confidence_history_list = [0.0 for i in range(self.history_depth)]     # history of all armors confidence
        self.car_time_history_list = [0.0 for i in range(self.history_depth)]     # history of car time      

        self.armor_idx_to_correct_history = {}
        self.armor_idx_to_detect_history = {}
        self.armor_idx_to_tvec_kf = {}
        self.armor_idx_to_rvec_kf = {}
        
        for i in range(self.armor_nums):
            
            self.armor_idx_to_correct_history.update({i :  [Armor_Params(armor_name,i) for j in range(self.history_depth)]})
            self.armor_idx_to_detect_history.update({i :  [Armor_Params(armor_name,i) for j in range(self.history_depth)]})
            self.armor_idx_to_tvec_kf.update({i :  Kalman_Filter(Q,R,H)})
            self.armor_idx_to_rvec_kf.update({i :  Kalman_Filter(Q,R,H)})
            
            
class Observer_Params(Params):
    
    def __init__(self):
        
        super().__init__()
        

        
        self.Q_scale = 0.1
        self.R_scale_dis_diff_denominator = 0.1 # unit: meter; R_scale = dis_diff / R_scale_dis_diff_denominator
        
        self.H = [[1.0,0.0,0.0],
                  
                  [0.0,1.0,0.0],
                  
                  [0.0,0.0,1.0]]
        
        self.predict_offset_time = 1.0
        self.history_depth = 5
        self.min_continuous_num_to_apply_predict = 3
        self.min_dis_between_continous_detection = 0.1 # unit: meter
        self.min_time_between_continous_detection = 0.1 # unit: second 
        
        # [score_weight_for_armor_name, score_weight_for_tvec, score_weight_for_rvec]
        self.score_weights_list_for_automatic_matching = [100,10,10]
        self.score_accumulation_method_list_for_automatic_matching = ['add','add','add']
        
        # confidence = score/score_criterion
        self.score_criterion_for_automatic_matching = 200
        self.wrongpic_threshold_dis = 0.2 # unit: meter
        self.armor_name_to_car_params = {}

        
    def init_each_car_params(self,enemy_car_list:list):
        self.H = np.array(self.H)
        for i in enemy_car_list:
            if i['armor_name'] in self.armor_name_to_car_params.keys():
                lr1.critical(f"Observer: armor_name {i['armor_name']} already exists in car_params")
                raise ValueError("armor_name already exists in car_params")
                
                
            dic = {i['armor_name']:Car_Params(i['armor_distance'],
                                              i['armor_name'],
                                              self.Q_scale * np.eye(3),
                                              np.eye(3),
                                              self.H,
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
        self.latest_focus_armor_name = None
        self.latest_focus_armor_index = None
        self.latest_focus_time = None
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
        target_name_list = [i['armor_name'] for i in all_targets_list]
        for car_params in self.observer_params.armor_name_to_car_params.values():
            if car_params.armor_name in target_name_list:
                target_index = target_name_list.index(car_params.armor_name)
                tvec = all_targets_list[target_index]['tvec']
                rvec = all_targets_list[target_index]['rvec']
                t = all_targets_list[target_index]['time']
                self.update_by_detection(car_params.armor_name,
                                         tvec,
                                         rvec,
                                         t ,
                                         True
                                         )
            else:
                self.update_by_detection(car_params.armor_name,
                                         np.zeros(3),
                                         np.array([0.0,0.0,1.0]),
                                         0.0,
                                         False
                                         )
        
        
 
    
    def update_by_detection(self,
                            armor_name:str,
                            tvec:np.ndarray,
                            rvec:np.ndarray,
                            t:float,
                            if_update:bool):
        
        if if_update:
            right_armor_name,right_armor_idx,confidence = self._automatic_matching(armor_name, tvec, rvec)
            armor_idx_to_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history
            
            continuous_detected_num = armor_idx_to_list[right_armor_idx][0].continuous_detected_num + 1
            continuous_detected_num = CLAMP(continuous_detected_num, [0,3])
            continuous_lost_num = armor_idx_to_list[right_armor_idx][0].continuous_lost_num - 1
            continuous_lost_num = CLAMP(continuous_lost_num, [0,3])
            
            armor_idx_to_list = self.observer_params.armor_name_to_car_params[right_armor_name].armor_idx_to_detect_history
            self._update_all_id_armor_by_detection(right_armor_name, 
                                                    right_armor_idx, 
                                                    tvec, 
                                                    rvec, 
                                                    t, 
                                                    confidence, 
                                                    continuous_detected_num,
                                                    continuous_lost_num
                                                        )

            armor_idx_to_list[right_armor_idx][0].if_update = True
            if self.mode == 'Dbg':
                lr1.info(f"Observer: Update armor detect params {right_armor_name} at t {t} with confidence {confidence}")
            self.latest_focus_armor_name = right_armor_name
            self.latest_focus_armor_index = right_armor_idx
            self.latest_focus_t = t
                   
            
        else:
            armor_idx_to_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history
            for armor_idx in range(self.observer_params.armor_name_to_car_params[armor_name].armor_nums):
                new_armor_params = Armor_Params(armor_name,armor_idx)
                new_armor_params.tvec = armor_idx_to_list[armor_idx][0].tvec
                new_armor_params.rvec = armor_idx_to_list[armor_idx][0].rvec
                new_armor_params.time = armor_idx_to_list[armor_idx][0].time
                new_armor_params.confidence = 0.0
                continuous_detected_num = armor_idx_to_list[armor_idx][0].continuous_detected_num - 1
                continuous_detected_num = CLAMP(continuous_detected_num, [0,3])
                new_armor_params.continuous_detected_num = continuous_detected_num
                continuous_lost_num = armor_idx_to_list[armor_idx][0].continuous_lost_num + 1
                continuous_lost_num = CLAMP(continuous_lost_num, [0,3])
                new_armor_params.continuous_lost_num = continuous_lost_num
                new_armor_params.if_update = False
                SHIFT_LIST_AND_ASSIG_VALUE(armor_idx_to_list[armor_idx],new_armor_params)
            if self.mode == 'Dbg':
                lr1.info(f"Observer: Update armor detect params {armor_name} at t {t} with confidence {0.0}")
            self.latest_focus_armor_name = armor_name
            self.latest_focus_armor_index = 0
            self.latest_focus_t = t
            
   
        
    def update_by_correct_all(self)->dict:
        """
        Args:
            all_targets_list (list): list of all targets, each target is a dict, including:
                'armor_name':str
        Returns:
            dict: armor_name_to_idx, this is the most latest armor id for each armor name
        """
        armor_name_to_idx = {}
        for car_params in self.observer_params.armor_name_to_car_params.values():
            
            armor_name  = car_params.armor_name
            latest_update_armor_id = self.__get_latest_update_armor_id(armor_name)
            if latest_update_armor_id is not None:
                self.update_by_correct(armor_name, latest_update_armor_id)
                
                t = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history[latest_update_armor_id][0].time
                tvec = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history[latest_update_armor_id][0].tvec
                rvec = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history[latest_update_armor_id][0].rvec
                confidence = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history[latest_update_armor_id][0].confidence
                
                self._update_all_id_armor_by_detection(armor_name, 
                                                            latest_update_armor_id, 
                                                            tvec, 
                                                            rvec, 
                                                            t, 
                                                            confidence, 
                                                            self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history)
            else:
                for armor_idx in range(car_params.armor_nums):
                    self.update_by_correct(armor_name, armor_idx)
            armor_name_to_idx.update({armor_name:latest_update_armor_id})
            
        return armor_name_to_idx   
        
    def update_by_correct(  self,
                            armor_name:str,
                            armor_idx:int):
        
        self._update_car_params_and_armor_relative_params()
        car_params = self.observer_params.armor_name_to_car_params[armor_name]
        correct_history_list = car_params.armor_idx_to_correct_history[armor_idx]
        detect_history_list = car_params.armor_idx_to_detect_history[armor_idx]
        if detect_history_list[0].if_update:
            continuous_detected_num = correct_history_list[0].continuous_detected_num + 1
            continuous_lost_num = correct_history_list[0].continuous_lost_num - 1
            detect_history_list[0].if_update = False
        else:
            continuous_lost_num = correct_history_list[0].continuous_lost_num + 1
            continuous_detected_num = correct_history_list[0].continuous_detected_num - 1
            detect_history_list[0].if_update = False
            
        continuous_detected_num = CLAMP(continuous_detected_num, [0,3])
        continuous_lost_num = CLAMP(continuous_lost_num, [0,3])
        tvec_correct, rvec_correct, cur_time, confidence = self.__cal_correct_params(armor_name, armor_idx,continuous_detected_num,continuous_lost_num)
        
        self._update_armor_history_params(correct_history_list,
                                          tvec_correct, 
                                          rvec_correct,
                                          cur_time,
                                          confidence,
                                          continuous_detected_num,
                                          continuous_lost_num,
                                          False)
                
        if self.mode == 'Dbg':
            
            lr1.debug(f"Observer: Update armor correct params {armor_name} id {armor_idx} at time {cur_time:.3f} with confidence {confidence}") 
    
            
    def get_car_latest_state(self)->list:
        """
        
        Returns:
            list of dict:
                armor_name:str
                car_center_tvec : (armor id 0 tvec) in camera frame 
                car_center_rvec : (armor id 0 rvec) in camera frame
                car_center_tv_vec : (armor id 0 tv_vec) in camera frame
                car_rotation_speed : in car frame
                car_time:float
                confidence : average confidence of all armors
        """
        car_list = []
        for armor_name in self.observer_params.armor_name_to_car_params.keys():
            car_center_tvec = self.observer_params.armor_name_to_car_params[armor_name].car_tvec_history_list[0]
            car_center_rvec = self.observer_params.armor_name_to_car_params[armor_name].car_rvec_history_list[0]
            car_center_tv_vec = self.observer_params.armor_name_to_car_params[armor_name].car_tv_vec_history_list[0]
            car_rotation_speed = self.observer_params.armor_name_to_car_params[armor_name].car_rotation_speed_history_list[0] 
            car_time = self.observer_params.armor_name_to_car_params[armor_name].car_time_history_list[0]
            car_confidence = np.mean(self.observer_params.armor_name_to_car_params[armor_name].car_confidence_history_list)
            
            car_list.append({'armor_name':armor_name,
                             'car_center_tvec':car_center_tvec,
                             'car_center_rvec':car_center_rvec,
                             'car_center_tv_vec':car_center_tv_vec,
                             'car_rotation_speed':car_rotation_speed,
                             'car_time': car_time ,
                             'car_confidence':car_confidence})
        return car_list
    
    def get_armor_latest_state(self,
                               if_correct_state:bool = True)->list:
        """

        Args:
            if_correct_state : 
                True: get correct state
                False: get detect state
        Returns:
            list of Armor_Params

        """
        if if_correct_state:
            armor_list = []
            for armor_name in self.observer_params.armor_name_to_car_params.keys():
                for armor_id,armor_correct_history_list in self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history.items():
                    armor_correct_latest_params = armor_correct_history_list[0]
                    
                    armor_list.append({'armor_name':armor_name,
                                    'armor_id':armor_id,
                                    'armor_tvec':armor_correct_latest_params.tvec,
                                    'armor_rvec':armor_correct_latest_params.rvec,
                                    'armor_confidence':armor_correct_latest_params.confidence,
                                    'armor_time':armor_correct_latest_params.time})
            return armor_list
        else:
            armor_list = []
            for armor_name in self.observer_params.armor_name_to_car_params.keys():
                for armor_id,armor_detect_history_list in self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history.items():
                    armor_detect_latest_params = armor_detect_history_list[0]
                    armor_list.append(armor_detect_latest_params)

            return armor_list
        
        
    
    def predict_armor_state_by_car(self,
                                    armor_name:str,
                                    armor_index:int,
                                    specific_time:float)->list:
        """get the specific armor(id) state at specific time

        Args:
            armor_name (str): _description_
            armor_index (int): _description_
            specific_time (float): _description_

        Returns:
            armor_predict_tvec (np.ndarray): _description_
            armor_predict_rvec (np.ndarray): _description_
        """
        
        car_params = self.observer_params.armor_name_to_car_params[armor_name]
        armor_latest_params = car_params.armor_idx_to_detect_history[armor_index][0]
        
        
        period = specific_time - armor_latest_params.time
        armor_relative_tvec = armor_latest_params.tvec - car_params.car_tvec
        armor_relative_rvec = armor_latest_params.rvec - car_params.car_rvec
        
        car_predict_tvec = car_params.car_tvec + car_params.car_tv_vec * period + 0.5 * car_params.car_ta_vec * period**2
        car_predict_rvec = car_params.car_rvec + car_params.car_rv_vec * period + 0.5 * car_params.car_ra_vec * period**2
        
        armor_predict_tvec = car_predict_tvec + armor_relative_tvec
        armor_predict_rvec = car_predict_rvec + armor_relative_rvec
        
        return armor_predict_tvec, armor_predict_rvec

    def predict_armor_state_by_itself(self,
                                      armor_name:str,
                                      armor_index:int,
                                      specific_time:float):
        """get the specific armor(id) state at specific time

        Args:
            armor_name (str): _description_
            armor_index (int): _description_
            specific_time (float): _description_

        Returns:
            armor_predict_tvec (np.ndarray): _description_
            armor_predict_rvec (np.ndarray): _description_
        """
        
        correct_history_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history[armor_index]
        continuous_num = self.__find_continuous_num(correct_history_list,if_check_dis_continuous=True)
        if continuous_num < self.observer_params.min_continuous_num_to_apply_predict:
            tvec = correct_history_list[0].tvec
            rvec = correct_history_list[0].rvec
            
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Predict get not enough continuous history : {continuous_num}, use the last correct state")
            return tvec,rvec
        else:
        
            predict_period = specific_time - correct_history_list[0].time
            
            tv_vec = (correct_history_list[0].tvec - correct_history_list[1].tvec) / predict_period if predict_period > 0 else np.zeros(3)
            tv_vec_old = (correct_history_list[1].tvec - correct_history_list[2].tvec) / predict_period if predict_period > 0 else np.zeros(3)
            #ta_vec = (tv_vec - tv_vec_old) / predict_period if predict_period > 0 else np.zeros(3)
            
            tvec = correct_history_list[0].tvec + tv_vec * predict_period 
            
            rv_vec = (correct_history_list[0].rvec - correct_history_list[1].rvec) / predict_period if predict_period > 0 else np.zeros(3)
            rv_vec_old = (correct_history_list[1].rvec - correct_history_list[2].rvec) / predict_period if predict_period > 0 else np.zeros(3)
            #ra_vec = (rv_vec - rv_vec_old) / predict_period if predict_period > 0 else np.zeros(3)
            
            rvec = correct_history_list[0].rvec + rv_vec * predict_period  
            
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Continuous num : {continuous_num}")
                lr1.debug(f"Observer: Predict armor {armor_name} id {armor_index} state at time {specific_time} , tvec {tvec} rvec {rvec}")
            
            return tvec,rvec
    
    def _automatic_matching(self,
                           detect_armor_name:str,
                           tvec:np.ndarray,
                           rvec:np.ndarray
                           )->list:
        """The armor that is closest to the given position at the given time will be selected as the target.

        Args:
            tvec (np.ndarray): _description_
            rvec (np.ndarray): _description_
            time (float): _description_

        Returns:
            armor_name:str, armor_idx:int, confidence:float
        """
        
        score_keeper = Score_Keeper()
        
        for armor_name,car_params in self.observer_params.armor_name_to_car_params.items():
            for armor_id,armor_correct_history_list in car_params.armor_idx_to_correct_history.items():
                
                armor_correct_latest_params = armor_correct_history_list[0]
                
                score_obj_name = armor_name + '_' + str(armor_id)
                
                attributes_list = [ armor_name,
                                    armor_correct_latest_params.tvec,
                                    armor_correct_latest_params.rvec]
                
                
                reference_list =  [ detect_armor_name,
                                    tvec,
                                    rvec
                                    ]
                
                score_obj_for_automatic_matching = Score_Obj_for_automatic_matching(score_obj_name,
                                                                                    attributes_list,
                                                                                    self.observer_params.score_accumulation_method_list_for_automatic_matching,
                                                                                    self.observer_params.score_weights_list_for_automatic_matching,
                                                                                    reference_list
                                                                                    )
                
                score_keeper.add_score_obj(score_obj_for_automatic_matching)
                
                if self.mode == 'Dbg':
                    score_obj_for_automatic_matching.show_each_attribute_score()
                    
        score_keeper.update_rank()
        best_name ,best_score = score_keeper.get_name_and_score_of_score_by_rank(0)
        right_armor_name = best_name.split('_')[0]
        right_armor_idx = int(best_name.split('_')[1])
        confidence = best_score/self.observer_params.score_criterion_for_automatic_matching
        
        if self.mode == 'Dbg':
            
            lr1.debug(f"Observer : Automatic matching : detection result: {detect_armor_name} \nautomatic matching result: {right_armor_name} {right_armor_idx} with confidence {confidence}")
           
            
        return right_armor_name,right_armor_idx,confidence

    def _if_wrong_pic(self,
                    detect_armor_name:str,
                    tvec:np.ndarray,
                    rvec:np.ndarray)->bool:
        if detect_armor_name == 'wrongpic':
            for car_params in self.observer_params.armor_name_to_car_params.values():
                for armor_idx in car_params.armor_idx_to_correct_history:
                    if np.linalg.norm(tvec - car_params.armor_idx_to_correct_history[armor_idx][0].tvec) < self.observer_params.wrongpic_threshold_dis:
                        return False
                    
            return True
        
        else:
            return False
                    

        
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

    def _update_car_params_and_armor_relative_params(self):
        """Will update all car params, including armor tvec,rvec in car frame too
        """
        for armor_name,car_params in self.observer_params.armor_name_to_car_params.items():
            
            tvec_list = []
            rvec_list = []
            confidence_list = []
            
            cur_time = max([car_params.armor_idx_to_correct_history[idx][0].time for idx in car_params.armor_idx_to_correct_history.keys()])
            
            period = cur_time - car_params.car_time_history_list[0]
            for armor_idx in car_params.armor_idx_to_correct_history:
                    
                latest_correct_armor_params = car_params.armor_idx_to_correct_history[armor_idx][0] 
                tvec_list.append(latest_correct_armor_params.tvec)
                rvec_list.append(latest_correct_armor_params.rvec)
                confidence_list.append(latest_correct_armor_params.confidence)
                    
            car_tvec = np.mean(np.array(tvec_list),axis=0)
            car_tv_vec = (car_tvec - car_params.car_tvec_history_list[0]) / period if period > 0 else np.zeros(3)
            car_ta_vec = (car_tv_vec - car_params.car_tvec_history_list[0]) / period if period > 0 else np.zeros(3)
            
            car_rvec = rvec_list[0]
            car_rv_vec = (car_rvec - car_params.car_rvec_history_list[0])/ period if period > 0 else np.zeros(3)
            car_ra_vec = (car_rv_vec - car_params.car_rv_vec_history_list[0]) / period if period > 0 else np.zeros(3)
            
            for armor_idx in car_params.armor_idx_to_correct_history:
                
                latest_correct_armor_params = car_params.armor_idx_to_correct_history[armor_idx][0] 
                latest_correct_armor_params.tvec_in_car_frame = latest_correct_armor_params.tvec - car_tvec
                latest_correct_armor_params.rvec_in_car_frame = latest_correct_armor_params.rvec - car_rvec
            
            car_rotation_speed = self.__cal_car_rotation_speed(armor_name)
            car_confidence = np.mean(np.array(confidence_list))
            car_time = cur_time
            
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_tvec_history_list,car_tvec)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_tv_vec_history_list,car_tv_vec)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_ta_vec_history_list,car_ta_vec)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_rvec_history_list,car_rvec)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_rv_vec_history_list,car_rv_vec)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_ra_vec_history_list,car_ra_vec)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_rotation_speed_history_list,car_rotation_speed)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_confidence_history_list,car_confidence)
            SHIFT_LIST_AND_ASSIG_VALUE(car_params.car_time_history_list,car_time)
            
            if self.mode == 'Dbg':
                
                lr1.debug(f"Observer: Update car {armor_name} params at time {cur_time}")
            

            
    def __cal_correct_params(self,
                    armor_name:str,
                    armor_idx:int,
                    continuous_detected_num:int,
                    continuous_lost_num:int
                    )->list:
        """Use Kalman Filter to get best predict params from detect_history_list and correct_history_list

        Args:
            detect_history_list (list): _description_
            correct_history_list (list): _description_

        Returns:
            list: _description_
                tvec_correct (np.ndarray): _description_
                rvec_correct (np.ndarray): _description_
                cur_time (float): detect_history_list[0].time
                confidence (float): 
                    if confidence == 0, means the armor is not focused, so we only use correct_history_list to predict the armor state.
                    if confidence == 0.25, continuous_num = 2
                    if confidence == 0.5, continuous_num = 3
                    if confidence == 0.75, continuous_num >3
        """
        detect_history_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history[armor_idx]
        correct_history_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history[armor_idx]
        cur_time = time.time()
        
        # all conditions : detected,lost : 00,01,02,03,10,20,30,12,21
        # target lost
        if continuous_lost_num == 3:
            tvec_correct = correct_history_list[0].tvec
            rvec_correct = correct_history_list[0].rvec
            confidence = 0.1  # search
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Lost Armor {armor_name} id {armor_idx}, detect,lost = 0,3")
                
        elif continuous_lost_num == 2 and continuous_detected_num == 0:
            tvec_correct = correct_history_list[0].tvec
            rvec_correct = correct_history_list[0].rvec
            confidence = 0.2 # search
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Lost Armor {armor_name} id {armor_idx}, detect,lost = 0,2")
                
        elif continuous_lost_num == 1 and continuous_detected_num == 0:
            tvec_correct = correct_history_list[0].tvec
            rvec_correct = correct_history_list[0].rvec
            confidence = 0.3 # search
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Lost Armor {armor_name} id {armor_idx}, detect,lost = 0,1")
                
        elif continuous_detected_num == 0 and continuous_lost_num == 0:
            tvec_correct = correct_history_list[0].tvec
            rvec_correct = correct_history_list[0].rvec
            confidence = 0.4 # search
            

        # target focused
        elif continuous_detected_num== 3:
            tvec_correct = detect_history_list[0].tvec
            rvec_correct = detect_history_list[0].rvec
            confidence = 0.9 # fire
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Focus Armor {armor_name} id {armor_idx}, detect,lost = 3,0")
        
        elif continuous_detected_num == 2 and continuous_lost_num == 0:
            tvec_correct = detect_history_list[0].tvec
            rvec_correct = detect_history_list[0].rvec
            confidence = 0.8 # fire
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Focus Armor {armor_name} id {armor_idx}, detect,lost = 2,0")
        
        elif continuous_detected_num == 2 and continuous_lost_num == 1:
            tvec_correct = detect_history_list[0].tvec
            rvec_correct = detect_history_list[0].rvec
            confidence = 0.7 # follow but not fire
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Focus Armor {armor_name} id {armor_idx}, detect,lost = 1,2")
        
        elif continuous_detected_num == 1 and continuous_lost_num == 0:
            tvec_correct = detect_history_list[0].tvec
            rvec_correct = detect_history_list[0].rvec
                
            confidence = 0.6 # stay
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Focus Armor {armor_name} id {armor_idx}, detect,lost = 1,0")
                
        elif continuous_lost_num == 2 and continuous_detected_num == 1:
            tvec_correct = detect_history_list[0].tvec
            rvec_correct = detect_history_list[0].rvec
            confidence = 0.5 # follow but not fire
            if self.mode == 'Dbg':
                lr1.debug(f"Observer: Lost Armor {armor_name} id {armor_idx}, detect,lost = 1,2")


            
        return tvec_correct,rvec_correct,cur_time,confidence
          
    def __cal_car_rotation_speed(self,
                            armor_name:str)->float:
        
        each_armor_rotation_speed_list = []
        each_armor_confidence_list = []
        
        for each_armor_correct_history_list in self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_correct_history.values():
            
            tvec_in_car_frame_latest = each_armor_correct_history_list[0].tvec_in_car_frame
            tvec_in_car_frame_old = each_armor_correct_history_list[1].tvec_in_car_frame
            period = each_armor_correct_history_list[0].time - each_armor_correct_history_list[1].time
            
            each_armor_rotation_speed = get_rotation_speed_in_xoy_plane(tvec_in_car_frame_latest, tvec_in_car_frame_old, period)
            each_armor_confidence = each_armor_correct_history_list[0].confidence
            
            each_armor_rotation_speed_list.append(each_armor_rotation_speed)
            each_armor_confidence_list.append(each_armor_confidence)
        
 
        if all(e == 0 for e in each_armor_confidence_list):
            car_rotation_speed = np.average(each_armor_rotation_speed_list)
            
        else:
            car_rotation_speed = np.average(each_armor_rotation_speed_list,weights=each_armor_confidence_list)
        
        #car_rotation_speed = each_armor_confidence.index(max(each_armor_confidence))
        
      
        return car_rotation_speed
 
 
    def __find_continuous_num(self,
                          armor_history_list:list,
                          if_check_dis_continuous:bool=False)->int:
        """Find how many times the armor(id) has been detected continuously latest
        Warning: only detect the continuity of time, not the continuity of position.\n
        Args:
            armor_name (str): _description_
            armor_idx (int): _description_

        Returns:
            int: 1 < continuous_num <= history_depth
        """
        
        continuous_num = 1
        for i in range(self.observer_params.history_depth - 1):
            dt = armor_history_list[i].time - armor_history_list[i+1].time
            
            if dt > self.observer_params.min_time_between_continous_detection:
                
                break
            else:
                if if_check_dis_continuous:
                    dis = np.linalg.norm(armor_history_list[i].tvec - armor_history_list[i+1].tvec)
                    if dis > self.observer_params.min_dis_between_continous_detection:
                        break
                    
                continuous_num += 1
                
        return continuous_num
                
    def __get_armor_last_focus_period(self,
                                        armor_name:str,
                                        armor_idx:int
                                        ):
        
        car_params = self.observer_params.armor_name_to_car_params[armor_name]
        armor_latest_params = car_params.armor_idx_to_detect_history[armor_idx][0]
        cur_time = time.time()
        period = cur_time - armor_latest_params.time
        
        return period
    
    def __get_latest_update_armor_id(self,
                                     armor_name:str):
        
        car_params = self.observer_params.armor_name_to_car_params[armor_name]
        armor_idx_to_detect_history_list = car_params.armor_idx_to_detect_history
        for armor_idx,history_list in armor_idx_to_detect_history_list.items():
            if history_list[0].if_update:
                return armor_idx
        
        return None
                
                
            
            
 
    def _update_all_id_armor_by_detection(self,
                                        armor_name:str,
                                        armor_idx:int,
                                        tvec:np.ndarray,
                                        rvec:np.ndarray,
                                        t,
                                        confidence:float,
                                        continuous_detected_num:int,
                                        continuous_lost_num:int
                                        ):
        
        armor_nums = self.observer_params.armor_name_to_car_params[armor_name].armor_nums
        tvec_list,rvec_list= get_other_face_center_pos( tvec,
                                                        rvec,
                                                        self.observer_params.armor_name_to_car_params[armor_name].armor_distance[0],
                                                        self.observer_params.armor_name_to_car_params[armor_name].armor_distance[1],
                                                        armor_nums)
        
        for i in range(armor_nums):
            armor_index = i + armor_idx
            if armor_index >= armor_nums:
                armor_index -= armor_nums
            history_list = self.observer_params.armor_name_to_car_params[armor_name].armor_idx_to_detect_history[armor_index]
            new_armor_params = Armor_Params(armor_name,armor_index)
            new_armor_params.tvec = tvec_list[i]
            new_armor_params.rvec = rvec_list[i]
            new_armor_params.time = t
            new_armor_params.confidence = confidence
            new_armor_params.continuous_detected_num = continuous_detected_num
            new_armor_params.continuous_lost_num = continuous_lost_num
            new_armor_params.if_update = False
            SHIFT_LIST_AND_ASSIG_VALUE(history_list,new_armor_params)
            
            
    
    
    def save_params_to_yaml(self,yaml_path:str)->None:
        self.observer_params.armor_name_to_car_params = {}
        self.observer_params.H = self.observer_params.H.tolist()
        self.observer_params.save_params_to_yaml(yaml_path)
 
    
    def set_armor_initial_state(self,
                                 armor_name_to_init_state:dict):
        
        for car_params in self.observer_params.armor_name_to_car_params.values():
            armor_name = car_params.armor_name
            
            t = time.time()
            confidence =0.0
            
            tvec = np.array(armor_name_to_init_state[armor_name]['tvec'])
            rvec = np.array(armor_name_to_init_state[armor_name]['rvec'])
            

            '''self._update_all_id_armor_by_detection(car_params.armor_name, 
                                                        0, 
                                                        tvec, 
                                                        rvec, 
                                                        t, 
                                                        confidence, 
                                                        car_params.armor_idx_to_detect_history)'''
            
            self._update_all_id_armor_by_detection(car_params.armor_name, 
                                                        0, 
                                                        tvec, 
                                                        rvec, 
                                                        t, 
                                                        confidence, 
                                                        0,
                                                        0)
    