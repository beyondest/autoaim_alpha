from typing import Any
from .com_tools import *
from ..os_op.basic import *
from ..os_op.global_logger import *

import can





class Port_Params(Params):
    
    def __init__(self) -> None:
        super().__init__()
        self.port_abs_path = '/dev/ttyUSB0'
        self.bitesize =8
        self.baudrate = 9600
        
        self.communication_delay = 0.05
        


class Port:
    def __init__(self,
                 mode:str = 'Dbg',
                 port_config_yaml_path:Union[str,None]=None) -> None:
        CHECK_INPUT_VALID(mode,'Dbg','Rel')
        self.params = Port_Params()
        if port_config_yaml_path is not None:
            self.params.load_params_from_yaml(port_config_yaml_path)
        
        self.action_data = action_data()
        self.syn_data = syn_data()
        self.pos_data = pos_data()
        self.mode = mode
        try:
            self.ser = serial.Serial(
                                    port=self.params.port_abs_path,
                                    baudrate=self.params.baudrate,
                                    bytesize=self.params.bitesize,
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
        
        except Exception as e:
            self.ser = None
            
            lr1.critical(f"Failed to open serial port {self.params.port_abs_path}")
            lr1.critical(f"Error message: {e}")
        
        
    def send_msg(self):
        if self.ser is not None:
                
            msg = self.action_data.convert_action_data_to_bytes()
            send_data(self.ser,msg)
            
            if self.mode == 'Dbg':
                lr1.debug(f"Send  {msg} tar_abs_yaw: {self.action_data.abs_yaw} tar_abs_pitch: {self.action_data.abs_pitch}")
            
        
    def recv_feedback(self)->tuple:
        """Only return True,0,0,0,0,0.0 if ser is None

        Returns:
            if_error:bool
            cur_yaw:float
            cur_pitch:float
        """
        
        if self.ser is not None:
            
            msg = self.ser.read_all()
            if msg == b'':
                lr1.debug(f"Recv empty msg")
                return True,0.0,0.0,0.0
            if_error = self.pos_data.convert_pos_bytes_to_data(msg)
            
            cur_yaw = self.pos_data.present_yaw
            cur_pitch = self.pos_data.present_pitch
            reserved_slog = self.pos_data.reserved_slot
            if self.mode == 'Dbg':
                lr1.debug(f"Recv {msg} cur_yaw: {cur_yaw} cur_pitch: {cur_pitch}")
            
            return if_error,cur_yaw,cur_pitch,reserved_slog
        
        else:
            return True,0.0,0.0,0.0
        
    def port_open(self):
        if self.ser is not None:
            self.ser.open()
    
    def port_close(self):  
        if self.ser is not None:
            self.ser.close()
        
    def save_params_to_yaml(self,yaml_path:str)->None:
        self.params.save_params_to_yaml(yaml_path)
        
class Can_Port:
    def __init__(self,
                 mode:str = 'Dbg',
                 port_config_yaml_path:Union[str,None]=None) -> None:
        CHECK_INPUT_VALID(mode,'Dbg','Rel')
        self.params = Port_Params()
        if port_config_yaml_path is not None:
            self.params.load_params_from_yaml(port_config_yaml_path)
        
        self.mode = mode
        self.bro_data_send = bro_data()
        self.bro_data_recv = bro_data()
        try:
            self.can = can.interface.Bus(bustype='socketcan',channel=self.params.port_abs_path)
        except Exception as e:
            self.can = None
            lr1.critical(f"Failed to open can port {self.params.port_abs_path}")
            lr1.critical(f"Error message: {e}")
    
    def send_msg(self):
        if self.can is not None:
            msg = self.bro_data_send.convert_bro_data_to_bytes()
            can_msg = can.Message(arbitration_id=0x123, data=msg, extended_id=False)
            self.can.send(can_msg)
            if self.mode == 'Dbg':
                lr1.debug(f"Send  {msg} find_enemy_yaw: {self.bro_data_send.find_enemy_yaw} cur_big_gimbal_yaw: {self.bro_data_send.cur_big_gimbal_yaw}")
    
    def recv_feedback(self)->tuple:
        """
        Returns:
            find_enemy_yaw: float, if > 999, means no enemy found
            cur_big_gimbal_yaw: float
        """
        if self.can is not None:
            can_msg = self.can.recv(timeout=0.1)
            if can_msg is not None:
                msg = can_msg.data
                if_error = self.bro_data_recv.convert_bro_bytes_to_data(msg)
                if self.mode == 'Dbg':
                    lr1.debug(f"Recv {msg} ,if_error: {if_error}  find_enemy_yaw: {self.bro_data_recv.find_enemy_yaw} cur_big_gimbal_yaw: {self.bro_data_recv.cur_big_gimbal_yaw}")
                return if_error, self.bro_data_recv.find_enemy_yaw, self.bro_data_recv.cur_big_gimbal_yaw
            else: 
                if self.mode == 'Dbg':
                    lr1.debug(f"Can Recv None")
                return True, 0.0, 0.0
        else:
            return True, 0.0, 0.0
        
        