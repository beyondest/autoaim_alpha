
import cv2

from autoaim_alpha.utils_network.data import *
from autoaim_alpha.utils_network.actions import *    

from autoaim_alpha.utils_network.api_for_yolov5 import Yolov5_Post_Processor  

img_path = '../res/roi_tmp.jpg'
trt_path = '../autoaim_alpha/config/net_config/classifier.trt'
class_yaml = '../autoaim_alpha/config/net_config/classifier_class.yaml'


cur_batch_size = 2
input_shape = (1,32,32)
max_batch_size = 10


img = cv2.imread(img_path,cv2.IMREAD_GRAYSCALE)
_,img = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
img = cv2.resize(img,input_shape[-2:])


cv2.imshow('img',img)
cv2.waitKey(0)
cv2.destroyAllWindows()

real_time_input = normalize_to_nparray([img],np.float32)
class_info = Data.get_file_info_from_yaml(class_yaml)

Data.show_nplike_info([real_time_input])


engine = TRT_Engine_2(trt_path,
                    max_batchsize=max_batch_size
                    )

out,t = engine.run({0:real_time_input})

logits = out[0].reshape(-1,len(class_info))


p_list,index_list =trans_logits_in_batch_to_result(logits)
result_list = [class_info[i] for i in index_list]

print(p_list,result_list)
print('spent time:')
print(t)




