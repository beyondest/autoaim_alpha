from autoaim_alpha.port_slavedevice.com_tools import *
from autoaim_alpha.port_slavedevice.port import *

from autoaim_alpha.os_op.basic import *
mode = 'Dbg'
port_yaml_path = '../autoaim_alpha/config/other_config/port_params.yaml'
p = Port(mode,port_yaml_path)

#p.port_open()

p.action_data.abs_pitch_10000
try:
    while True:
        p.action_data.fire_times = 0
        p.action_data.abs_yaw_10000 = -1745
        p.send_msg('A')
        e,c_y,c_p,c_t1,c_t2,c_t3 = p.recv_feedback()
        print(e,c_y,c_p,c_t1,c_t2,c_t3)
        print('send A')
        
except:
    p.port_close()
    


