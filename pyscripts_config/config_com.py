from autoaim_alpha.port_slavedevice.com_tools import *
from autoaim_alpha.port_slavedevice.port import *
import serial
from autoaim_alpha.os_op.basic import *
import time
mode = 'Dbg'
port_yaml_path = '../autoaim_alpha/config/other_config/port_params.yaml'

ser = serial.Serial(
                    port='/dev/ttyTHS0',
                    baudrate=115200,
                    bytesize=8,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_ONE,
                    xonxoff=False,
                    rtscts=False,
                    write_timeout=1,
                    dsrdtr=None,
                    inter_byte_timeout=0.1,
                    exclusive=1, # 1 is ok for one time long communication, but None is not, I dont know why!!!!
                    timeout=1
                    ) 

p = pos_data()
try:
    while True:
        # WARNING: CANNOT USE read(16) when you use time.sleep, exactly when sleep time is too long
        # WARNING: BETTER USE read_all() instead of read(16)
        #time.sleep(0.1)
        #b = ser.read(16)
        b = ser.read_all()
        if_error = p.convert_pos_bytes_to_data(b,if_part_crc=False)
        if if_error:
            print(f'Error in data conversion,ori_bytes:{b}')
            p.show()
            print(f"crc_get: {p.crc_get},crc_calc: {p.crc_v}")
        else:
            print(f'ori_bytes:{b}')
            p.show()
        
except Exception as e:
    print(f'Error:{e}')
    ser.close()
    


