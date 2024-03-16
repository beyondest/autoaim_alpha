import pynput
from .basic import *
from .global_logger import lr1
class KeyboardAndMouseControl:
    def __init__(self,mode='Dbg',if_enable_key_board = False,if_enable_mouse_control = True):
        CHECK_INPUT_VALID(mode,'Dbg','Rel')
        self.mode = mode
        
        if if_enable_key_board:
            self.listener = pynput.keyboard.Listener(on_press=self.__on_press, on_release=self.__on_release)
            self.listener.start()
        
        
        if if_enable_mouse_control:
            self.mouse_listener = pynput.mouse.Listener(on_move=self.__on_move, on_click=self.__on_click, on_scroll=self.__on_scroll)
            self.mouse_listener.start()
            self.mouse_position = (0,0)
        
    
    def get_mouse_position(self):
        
        return self.mouse_position
    
    def __on_move(self,x, y):
        
        self.mouse_position = (x,y)
        if self.mode == 'Dbg':
            lr1.debug('mouse moved to ({0}, {1})'.format(x, y))

    def __on_click(self,x, y, button, pressed):
        if pressed:
            print('mouse click at ({0}, {1}) pressed {2}'.format(x, y, button))
        else:
            print('mouse click at ({0}, {1}), released {2}'.format(x, y, button))

    def __on_scroll(self,x, y, dx, dy):
        print('mouse scrolled at ({0}, {1}) ({2}, {3})'.format(x, y, dx, dy))


    def __on_press(self,key):
        print('{0} pressed'.format(key))
 
    def __on_release(self,key):
        print('{0} released'.format(key))
        if key == pynput.keyboard.Key.esc:
            return False
 
            