from .com_tools import *
from ..os_op.basic import *
from ..os_op.global_logger import *







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
        
        
    def send_msg(self,sof:str = 'A'):
        if self.ser is not None:
            if sof == 'A':
                
                msg = self.action_data.convert_action_data_to_bytes(if_part_crc=False)
                send_data(self.ser,msg)
                
            elif sof == 'S':
                
                msg = self.syn_data.convert_syn_data_to_bytes(if_part_crc=False)
                send_data(self.ser,msg)
            if self.mode == 'Dbg':
                lr1.debug(f"Send {sof} {msg} yaw_10000: {self.action_data.abs_yaw_10000}, target_second: {self.action_data.target_second}")
            
        
    def recv_feedback(self)->tuple:
        """Only return True,0,0,0,0,0.0 if ser is None

        Returns:
            if_error:bool
            cur_yaw:float
            cur_pitch:float
            cur_time_minute:int
            cur_time_second:int
            cur_time_second_frac:float
        """
        
        if self.ser is not None:
            
            msg = self.ser.read_all()
            if_error = self.pos_data.convert_pos_bytes_to_data(msg,if_part_crc=False)
            
            cur_yaw = self.pos_data.present_yaw
            cur_pitch = self.pos_data.present_pitch
            cur_time_minute = self.pos_data.stm_minute
            cur_time_second = self.pos_data.stm_second
            cur_time_second_frac = self.pos_data.stm_second_frac 
            if self.mode == 'Dbg':
                lr1.debug(f"Recv {msg} yaw_10000: {cur_yaw} stm_second: {cur_time_second}")
            
            return if_error,cur_yaw,cur_pitch,cur_time_minute,cur_time_second,cur_time_second_frac
        
        else:
            
            return True,0,0,0,0,0.0
        
    def port_open(self):
        if self.ser is not None:
            self.ser.open()
    
    def port_close(self):  
        if self.ser is not None:
            self.ser.close()
        
    def save_params_to_yaml(self,yaml_path:str)->None:
        self.params.save_params_to_yaml(yaml_path)
        
    