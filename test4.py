import cv2
import numpy as np

from autoaim_alpha.autoaim_alpha.utils_network.data import *

info = Data.get_file_info_from_yaml('./autoaim_alpha/config/net_config/yolov5_class.yaml')
print(info)

info = {v:k for k,v in info.items()}
print(info)


