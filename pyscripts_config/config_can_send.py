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
        can_port.bro_data_send.cur_big_gimbal_yaw = 5.0
        can_port.bro_data_send.find_enemy_yaw = 2.0
        print(can_port.bro_data_send.convert_bro_data_to_bytes())
        can_port.send_msg()
        # 等待1秒
        time.sleep(1)

except KeyboardInterrupt:
    print("\nExiting...")
finally:
    # 关闭CAN Bus接口
    bus.shutdown()



