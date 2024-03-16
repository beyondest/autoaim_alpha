from autoaim_alpha.port_slavedevice.com_tools import *
from autoaim_alpha.port_slavedevice.port import *
import serial
from autoaim_alpha.os_op.basic import *
mode = 'Dbg'
port_yaml_path = '../autoaim_alpha/config/other_config/port_params.yaml'

ser = serial.Serial(
                    port='/dev/ttyTHS0',
                    baudrate=9600,
                    bytesize=8,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_ONE,
                    xonxoff=False,
                    rtscts=False,
                    write_timeout=1,
                    dsrdtr=None,
                    inter_byte_timeout=0.1,
                    exclusive=None, # 1 is ok for one time long communication, but None is not, I dont know why!!!!
                    timeout=1
                    ) 

p = pos_data()
try:
    while True:
        
        b = ser.read(16)
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
    


