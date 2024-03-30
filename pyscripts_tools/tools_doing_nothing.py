from autoaim_alpha.port_slavedevice.port import *


if __name__ == '__main__':
    p = Port('Dbg',
             '/home/rcclub/ggbond/autoaim_ws/src/autoaim_alpha/config/other_config/port_params.yaml')
    p.action_data.abs_pitch = 0.0
    p.action_data.abs_yaw = 0.0
    p.action_data.fire_times = 0
    p.action_data.reserved_slot = 10
    times = 10
    
    while times > 0:
        try:
            p.send_msg()
            times -= 1
        except Exception as e:
            print("port error:",e)
    
        
    