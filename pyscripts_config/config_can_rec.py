import can
import time
from autoaim_alpha.port_slavedevice.com_tools import *
from autoaim_alpha.port_slavedevice.port import *
# 设置CAN接口名称
interface_name = 'can0'
# 创建一个CAN Bus接口
bus = can.interface.Bus(channel=interface_name, bustype='socketcan_native')
can_port = Can_Port('Dbg','/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/other_config/port2_params.yaml')

try:
    while True:
        
        i_error,f_yaw, c_yaw = can_port.recv_feedback()
        # 等待1秒
        print("i_error:{},f_yaw:{},c_yaw:{}".format(i_error,f_yaw,c_yaw))   
        time.sleep(1)

except KeyboardInterrupt:
    print("\nExiting...")
finally:
    # 关闭CAN Bus接口
    bus.shutdown()





