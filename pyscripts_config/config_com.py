from autoaim_alpha.port_slavedevice.com_tools import *
from autoaim_alpha.port_slavedevice.port import *

from autoaim_alpha.os_op.basic import *
mode = 'Dbg'
port_yaml_path = '../autoaim_alpha/config/other_config/port_params.yaml'
p = Port(mode,port_yaml_path)


p.port_open()


try:
    while True:
        p.action_data.fire_times = 2
        p.send_msg('A')
        print('send A')
except:
    p.port_close()
    


