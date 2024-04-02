import can
import time
from autoaim_alpha.port_slavedevice.com_tools import *
# 设置CAN接口名称
interface_name = 'can0'
# 创建一个CAN Bus接口
bus = can.interface.Bus(channel=interface_name, bustype='socketcan_native')

try:
    while True:
        # 获取当前时间
        current_time = time.strftime("%H:%M:%S", time.localtime())

        # 构造消息
        message_data = f"hello"
        message = can.Message(arbitration_id=0x123, data=message_data.encode(), is_extended_id=False)
        # 发送消息
        bus.send(message)
        print(f"Sent message: {message_data}")
        # 等待1秒
        time.sleep(1)

except KeyboardInterrupt:
    print("\nExiting...")

finally:
    # 关闭CAN Bus接口
    bus.shutdown()



