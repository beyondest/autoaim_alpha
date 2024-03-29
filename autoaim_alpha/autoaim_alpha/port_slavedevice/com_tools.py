import struct
import numpy as np
from crcmod.predefined import mkPredefinedCrcFun
import serial

# Integer types:
# 'b': signed byte (1 byte)                 -128-127
# 'B': unsigned byte (1 byte)               0-255
# 'h': short integer (2 bytes)              -32768-32767
# 'H': unsigned short integer (2 bytes)     0-65535
# 'i': integer (4 bytes)                    
# 'I': unsigned integer (4 bytes)
# 'l': long integer (4 bytes)
# 'L': unsigned long integer (4 bytes)
# 'q': long long integer (8 bytes)
# 'Q': unsigned long long integer (8 bytes)

# Floating-point types:
# 'f': single-precision float (4 bytes)
# 'd': double-precision float (8 bytes)

# Character type:
# 'c': character (1 byte)   (notice that struct.pack will raise error when use ('c','A'),you should use ('B',ord('A') instead))

# String type:
# 's': string (must be followed by a number, e.g., '4s' means a string of length 4)

# Special type:
# '?': boolean (1 byte)

# Padding:
# 'x': padding byte

# Byte order:
# '<': little-endian
# '>': big-endian
# '!': network byte order (big-endian)


def send_data(ser:serial.Serial,msg:bytes)->None:
    if ser.is_open:
        if not isinstance(msg,bytes):

            raise TypeError('send_data msg wrong format')
        
        ser.write(msg)
    else:
        raise ValueError('serial port not open')
        

def read_data(ser:serial.Serial,byte_len:int = 16)->bytes:
    """read data from port:ser

    Args:
        ser (serial.Serial): _description_

    Returns:
        bytes: ori_byte
    """
    if ser.is_open:
        #10 byte works well
        com_input = ser.read(byte_len)
        return com_input
    
    else:
        raise ValueError('serial port not open')
           
def port_close(ser:serial.Serial):
    ser.close()          
   
def port_open(port_abs_path:str = '/dev/ttyUSB0',
              bytesize:int = 8,
              baudrate = 9600)->serial.Serial:
    """ Change port init settings here\n
        Init settings:\n
        port=port_abs_path,\n
        baudrate=baudrate,\n
        bytesize=bytesize,\n
        parity=serial.PARITY_NONE,\n
        stopbits=serial.STOPBITS_ONE,\n
        xonxoff=False,\n
        rtscts=False,\n
        write_timeout=1,\n
        dsrdtr=None,\n
        inter_byte_timeout=0.1,\n
        exclusive=None # 1 is ok for one time long communication\n
    """
    
    ser = serial.Serial(
        port=port_abs_path,
        baudrate=baudrate,
        bytesize=bytesize,
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
    
    return ser
    

def convert_to_bytes(value_tuple:tuple)->bytes:
    """(value,conver_type_str)
    e.g. (3.14,'<f')

    Args:
        value_tuple (tuple): _description_
    """
    if value_tuple[1][-1] == 'c':
        out = struct.pack(value_tuple[1][0]+'B',ord(value_tuple[0]))
    else:
        out = struct.pack(value_tuple[1],value_tuple[0])
    
    return out

def convert_to_data(byte_tuple:tuple):
    
    return struct.unpack(byte_tuple[1],byte_tuple[0])



def cal_crc(data:bytes)->int:
    """HOW TO GET RIGHT CRC:\n
       Input byte order same as memory in stm32\n, e.g.:0x44434241---b'ABCD'
       Make inverse of each 4 bytes of all_bytes, e.g.:b'abcd1234'----b'dcba4321'
       explain stm32 crc byte in little endian mode\n
       -> right result

    Args:
        data (bytes): _description_

    Raises:
        TypeError: _description_

    Returns:
        int: _description_
    """
    if not isinstance(data,bytes):
        raise TypeError('cal_crc must input bytes type data')
    crc_value = mkPredefinedCrcFun('crc-32-mpeg')(data)
    
    return crc_value



#upper part:   
'''
Action data: send | SOF: 'A'
By present_pitch and present_yaw send by stm32, calculte RELATIVE pitch and yaw angle and best reach time
By bollets left and some other info , decide the fire mode and fire times(default: fire x times from stm32 get msg to reach target pos, right closed)

Syn data: send | SOF: 'S'
Get present time minute and seconds and second.frac4*10000, send to stm32

Pos data: receive |SOF: 'P'
Receive present_yaw and present_pitch and present time from stm32 
'''
#stm32 part:
'''

Action data: receive |SOF : 'A'
By RELATIVE pitch and yaw sent by upper and reach target time, control 6020 to target pos on time
By fire times and reach target time, calculate uinform distributed firing time points within this time interval, and fire at each firing time point

Syn data: receive | SOF : 'S'
Once get present time from upper, stm32 will set RTC module to SYNCHRONIZE with upper pc
Once time gap between stm32 and upper is bigger than threshold , stm32 correct RTC or set bias by software code

Pos data: send | SOF : 'P'
By info from 6020 , send present yaw and pitch to upper
By info from RTC , send present time minute and second and second_frac4*10000 to upper
'''
'''
stm32 send format:


'''

class data_list:
    def __init__(self) -> None:
        """If use this as parent class, must cover len and list and labellist

        Args:
            length (_type_): _description_
        """
        self.count = 0
        self.len = 0
        self.list =[]
        self.label_list =[]
        self.crc_v = 0
        self.error = False
    def __iter__(self):
        return self
    def __len__(self):
        return self.len
    
    def __next__(self):
        if self.count < self.len:
            result = self.list[self.count]
            self.count+=1
            return result
        else:
            self.count = 0
            raise StopIteration
        
    def __getitem__(self,key):
        return self.list[key]
    
    def show(self):
        for i in range(self.len):
            print(f'{self.label_list[i]} = {self.list[i]}')
        
    def flip_4bytes_atime(self,bytes_12:bytes)->bytes:
        """return for_crc_cal, bytes_12

        Args:
            bytes_12 (bytes): _description_

        Raises:
            TypeError: _description_
            TypeError: _description_

        Returns:
            bytes: _description_
        """
        if not isinstance(bytes_12,bytes):
            raise TypeError('wrong input type, must be bytes')
        if len(bytes_12) != 12:
            raise TypeError('wrong bytes length, must be 16')
        out = list(bytes_12)
        out[0:4] = out[0:4][::-1]
        out[4:8] = out[4:8][::-1]
        out[8:12] = out[8:12][::-1]
        return bytes(out)

class syn_data(data_list):
    def __init__(self,
                 SOF:str = 'S',
                 present_minute:int=20,
                 present_second:int=30,
                 present_second_frac_10000:int=1234) -> None:
        """Null bytes will generate by convert_to_byte func
           

        Args:
            SOF (str, optional): _description_. Defaults to 'S'.
            present_minute (int, optional): _description_. Defaults to 0.
            present_second (int, optional): _description_. Defaults to 0.
            present_second_frac_10000 (int, optional): _description_. Defaults to 0.
        """
        super().__init__()
        
        self.SOF = SOF
        self.present_minute = present_minute
        self.present_second = present_second
        self.present_second_frac_10000 = present_second_frac_10000
        self.list = [self.SOF,self.present_minute,self.present_second,self.present_second_frac_10000]
        
        self.len = len(self.list)
        
        self.label_list = ['SOF','pre_min','pre_sec','pre_sec_frac']

    def convert_syn_data_to_bytes(self,if_crc:bool = True,if_revin_crc:bool = True, if_part_crc :bool =True)->bytes:
        """Calculate crc here if needed
        NO.0 (SOF:char , '<c')                             |     ('S')                      |byte0      bytes 1     total 1
        NO.1 (present_time_minute:int , '<B')              |     (0<=x<60)                  |byte1      bytes 1     total 2   
        NO.2 (present_time_second:int , '<B')              |     (0<=x<60)                  |byte2      bytes 1     total 3
        NO.3 (present_time_second_frac.4*10000:int, '<H')  |     (0<=x<=10000)              |byte3-4    bytes 2     total 5
        NO.4 (null_byte:b'1234567')   (auto add)(must add) |                                |byte5-11   bytes 7     total 12
        NO.5 (crc_value:int , '<I')  (auto add to end)     |     (return of cal_crc func)   |byte12-15  bytes 4     total 16
        PART_CRC:1-4 byte
        ALL: 6 elements,list has 4 elements
        """
        self.list = [self.SOF,self.present_minute,self.present_second,self.present_second_frac_10000]
        fmt_list = ['<c','<B','<B','<H']
        out = b''
        crc_v =b''
        null_bytes = b'1234567'
        for index,each in enumerate(self.list):
            out += convert_to_bytes((each,fmt_list[index]))
            
            
        out+=null_bytes
        if if_crc:
            if if_revin_crc:
                
                if if_part_crc:
                    for_crc_cal = out[1:5]
                    for_crc_cal = for_crc_cal[::-1]
                    crc_v = cal_crc(for_crc_cal)
                else:
                    for_crc_cal=self.flip_4bytes_atime(out)
                    crc_v = cal_crc(for_crc_cal)
            else:
                raise TypeError("This function isnot support yet")     
            self.crc_v = crc_v
            crc_v = convert_to_bytes((crc_v,'<I'))
        else:
            crc_v =b''
        out+= crc_v
        return out   
        
            
class action_data(data_list):
    
    def __init__(self,
                 SOF:str = 'A',
                 fire_times:int=2,
                 abs_pitch:float=20.0, #  degree
                 abs_yaw:float=90.0,  # degree
                 reserved_slot:float=0) -> None:
        
        super().__init__()
        
        self.label_list = ['SOF','ftimes','tarpitch','taryaw','reserved_slot']
        self.SOF = SOF
        self.fire_times = fire_times
        self.abs_pitch = abs_pitch
        self.abs_yaw = abs_yaw
        self.reserved_slot = reserved_slot
        self.crc_v = 0
        self.list = [self.SOF,
                     self.fire_times,
                     self.abs_pitch,
                     self.abs_yaw,
                     self.reserved_slot]
        self.len = len(self.list)
        
        
        
    def convert_action_data_to_bytes(self )->bytes:
        """Calculate crc here if needed
        NO.0 (SOF:char , '<c')                             |     ('A')                      |byte0      bytes 1     total 1
        NO.1 (fire_times:int , '<b')                       |     (-1<=x<=100)               |byte1      bytes 1     total 2 (-1:not control;0:control not fire) 
        NO.2 (target_pitch: float , '<h')                  |                                |byte2-5    bytes 4     total 6
        NO.3 (target_yaw: float , '<h')                    |                                |byte6-9    bytes 4     total 10
        NO.4 (reserved_slot:int, '<h')                     |                                |byte10-11  bytes 2     total 12
        NO.5 (crc_value:int , '<I')   (auto add to end)    |     (return of cal_crc func)   |byte12-15  bytes 4     total 16
        ALL: 5 elements 
        """
        self.list = [self.SOF,
                self.fire_times,
                self.abs_pitch,
                self.abs_yaw,
                self.reserved_slot]

        
        fmt_list = ['<c','<b','<f','<f','<h']
        out = b''
        for index,each in enumerate(self.list):
            out += convert_to_bytes((each,fmt_list[index]))
        for_crc_cal = self.flip_4bytes_atime(out)
        self.crc_v  = cal_crc(for_crc_cal)
        crc_b = convert_to_bytes((self.crc_v,'<I'))
        out+= crc_b
        return out  


class pos_data(data_list):
    
    def __init__(self,
                 SOF:str = 'P',
                 fire_times:int = 0,
                 present_pitch:float = -10.0,
                 present_yaw:float = 90.0,
                 reserved_slot:int = 0
                 ) -> None:
        super().__init__()
        
        self.label_list = ['SOF','fire_times','prepit','preyaw','reserved_slot']
        self.SOF =SOF
        self.fire_times = fire_times
        self.present_pitch = present_pitch
        self.present_yaw = present_yaw
        self.reserved_slot = reserved_slot
        self.error =False
        self.crc_v =0
        self.crc_get = 0
        self.list = [self.SOF,
                     self.fire_times,
                     self.present_pitch,
                     self.present_yaw,
                     self.reserved_slot]
        self.len = len(self.list)
        

    def convert_pos_bytes_to_data(self,ser_read:bytes)->bool:
        """Calculate crc here if needed
        NO.0 (SOF:char , '<c')                             |     ('A')                      |byte0      bytes 1     total 1
        NO.1 (fire_times:int , '<b')                       |     (-1<=x<=100)               |byte1      bytes 1     total 2 
        NO.2 (present_pitch: float , '<h')                 |                                |byte2-5    bytes 4     total 6
        NO.3 (present_yaw: float , '<h')                   |                                |byte6-9    bytes 4     total 10
        NO.4 (reserved_slot:int, '<h')                     |                                |byte10-11  bytes 2     total 12
        NO.5 (crc_value:int , '<I')   (auto add to end)    |     (return of cal_crc func)   |byte12-15  bytes 4     total 16
        ALL: 6 elements
        """
        self.fmt_list = ['<c','<b','<f','<f','<h','<I']
        self.range_list = [(0,1),(1,2),(2,6),(6,10),(10,12),(12,16)]
        self.frame_type_nums = 6
        out = []
        if ser_read == b'' or ser_read is None:
            self.error = True
            print(f"com error: recv is nullbyte")
            return self.error
        
        for i in range(self.frame_type_nums):
            out.append(struct.unpack(self.fmt_list[i],
                                    ser_read[self.range_list[i][0]:self.range_list[i][1]])[0])
            
        try:
            out[0] = out[0].decode('utf-8')
        except Exception as e:
            print(f'decode first byte failed: e:{e}, out[0]:{out[0]}')
            self.error = True
            return self.error
       
        if out[0] == self.SOF:

            for_crc_cal = ser_read[0:12]
            for_crc_cal = self.flip_4bytes_atime(for_crc_cal)
            self.crc_v = cal_crc(for_crc_cal)
            self.crc_get = out[5]
            self.error = not (self.crc_v == self.crc_get)

        else:
            self.error = True

        if not self.error:
            self.SOF = out[0]
            self.fire_times = out[1]
            self.present_pitch = out[2]
            self.present_yaw = out[3]
            self.reserved_slot = out[4]
            self.list = [self.SOF,
                        self.fire_times,
                        self.present_pitch,
                        self.present_yaw,
                        self.reserved_slot]
        

        return self.error


#first get reversed of data, then use crc-32-mpeg will get right crc same as stm32 
import numpy as np
if __name__ =="__main__":
    
    
    a = action_data()
    if 0:
        for yaw in np.arange(-180,180,10):
            
            a.abs_pitch = yaw
            
            r = a.convert_action_data_to_bytes()

            s = ''
            for i in r:
                
                add_ = hex(i)[2:]
                if len(add_) == 1:
                    add_ = '0'+add_
                s+= add_ + ' '
                
            print(s)
        
    if 0:
        for pitch in np.arange(-20,50,10):
            
            a.abs_pitch = pitch
            
            r = a.convert_action_data_to_bytes()

            s = ''
            for i in r:
                
                add_ = hex(i)[2:]
                if len(add_) == 1:
                    add_ = '0'+add_
                s+= add_ + ' '
                
            print(s)
    
    if 1:        
        p = pos_data()
        hex_string ='50 02 00 00 20 43 00 00 b4 42 00 00 3e c0 7e 2e '
        b = bytes.fromhex(hex_string)
        print(b)
        result = p.convert_pos_bytes_to_data(b)
        print(result)
        p.show()
        
        
        
    
    
        
        
    
    
    
    
    
    
    