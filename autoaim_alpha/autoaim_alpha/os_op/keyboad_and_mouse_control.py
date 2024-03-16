import pynput

class KeyboardAndMouseControl:
    def __init__(self):
        self.listener = pynput.keyboard.Listener(on_press=self.__on_press, on_release=self.__on_release)
        self.listener.start()
        
        self.mouse_listener = pynput.mouse.Listener(on_move=self.__on_move, on_click=self.__on_click, on_scroll=self.__on_scroll)
        self.mouse_listener.start()
    
    
    
    def __on_move(x, y):
        print('mouse moved to ({0}, {1})'.format(x, y))

    def __on_click(x, y, button, pressed):
        if pressed:
            print('mouse click at ({0}, {1}) pressed {2}'.format(x, y, button))
        else:
            print('mouse click at ({0}, {1}), released {2}'.format(x, y, button))

    def __on_scroll(x, y, dx, dy):
        print('mouse scrolled at ({0}, {1}) ({2}, {3})'.format(x, y, dx, dy))


    def __on_press(key):
        print('{0} pressed'.format(key))
 
    def __on_release(key):
        print('{0} released'.format(key))
        if key == pynput.keyboard.Key.esc:
            return False
 
            