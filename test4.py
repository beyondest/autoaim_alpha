import numpy as np
from autoaim_alpha.decision_maker.decision_maker import *
class AutoAim:
    def __init__(self):
        self._init_yaw_pitch_search_data()
        
    def _init_yaw_pitch_search_data(self):
        self.search_index = 0
        # mode 0 : not in search mode, mode 1 : search from right to left, mode 2 : search from left to right 
        self.search_mode = 1
        
        self.yaw_left = -np.pi/3 *2
        self.yaw_right = np.pi/3 * 2
        self.pitch_down = 10/180 * np.pi
        self.pitch_up = 20/180 * np.pi
        self.yaw_search_step = 0.02
        self.yaw_search_data = np.round(np.arange(self.yaw_left,self.yaw_right,self.yaw_search_step),3)
        
        self.pitch_search_left = -5 * np.pi
        self.pitch_search_right = 6* np.pi
        # 1 wave down, 1 wave up, 1 wave down, 1 wave up, 1 wave down
        self.pitch_search_step = ((self.pitch_search_right - self.pitch_search_left)/len(self.yaw_search_data))
        self.pitch_search_data =np.round(np.sin(np.arange(self.pitch_search_left, self.pitch_search_right, self.pitch_search_step)),3) 
        for i, pitch in enumerate(self.pitch_search_data):
            if pitch < 0:
                self.pitch_search_data[i] = pitch * self.pitch_down
            else:
                self.pitch_search_data[i] = pitch * self.pitch_up
  
a = AutoAim()  
print(a.pitch_search_data)
print(a.yaw_search_data)
print(len(a.pitch_search_data))
print(len(a.yaw_search_data))

b = Decision_Maker('Dbg','./autoaim_alpha/config/other_config/decision_maker_params.yaml',)
print(b.yaw_search_data)


