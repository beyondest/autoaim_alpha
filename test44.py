from pynput.mouse import Listener

# 定义回调函数，当鼠标移动时调用
def on_move(x, y):
    print('鼠标移动到 ({0}, {1})'.format(x, y))

# 定义回调函数，当鼠标点击时调用
def on_click(x, y, button, pressed):
    if pressed:
        print('鼠标点击在 ({0}, {1}) 上，按下 {2}'.format(x, y, button))
    else:
        print('鼠标点击在 ({0}, {1}) 上，释放 {2}'.format(x, y, button))

# 定义回调函数，当鼠标滚动时调用
def on_scroll(x, y, dx, dy):
    print('鼠标滚动 ({0}, {1}) ({2}, {3})'.format(x, y, dx, dy))

# 创建鼠标监听器，并设置回调函数
with Listener(on_move=on_move, on_click=on_click, on_scroll=on_scroll) as listener:
    listener.join()

