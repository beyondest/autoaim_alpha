import numpy as np

yaw_left = -np.pi
yaw_right = np.pi
yaw_search_step = 0.05

yaw_search_data = np.round(np.arange(yaw_left,yaw_right,yaw_search_step),3)
        
        # 1 wave down, 1 wave up, 1 wave down, 1 wave up, 1 wave down
        
        
nums = len(yaw_search_data)
step = ((2 * np.pi + 3* np.pi)/(nums))
pitch_search_data = np.round(np.sin(np.arange(-3 * np.pi, 2 * np.pi, step)) * 10/180 * np.pi,3)
        
index_yaw = int((-3.0423 - yaw_left)/yaw_search_step)

print(pitch_search_data)
print(yaw_search_data)

print(len(pitch_search_data))
print(len(yaw_search_data))

print(index_yaw)
print(yaw_search_data[index_yaw])