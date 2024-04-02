import can
from autoaim_alpha.port_slavedevice.com_tools import *
# 设置CAN接口名称
interface_name = 'can0'

# 创建一个CAN Bus接口
bus = can.interface.Bus(channel=interface_name, bustype='socketcan_native')

try:
    while True:
        # 接收消息
        message = bus.recv(timeout=1.0)
        if message is not None:
            # 解析并打印消息
            received_data = message.data.decode()
            print(f"Received message: {received_data}")

except KeyboardInterrupt:
    print("\nExiting...")
finally:
    # 关闭CAN Bus接口
    bus.shutdown()
