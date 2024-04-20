import os
import pyautogui
import time

fp = r'./cache' #指定文件夹
 
# 创建文件夹
if not os.path.exists(fp):
    os.makedirs(fp)
 
#开始截屏
i = 1
while True:
    b = str(i)
    pyautogui.FAILSAFE = True
    pyautogui.PAUSE = 0.1
    recording = pyautogui.screenshot()  #截屏
    fn = b + '.png'
    print("screenshot success" + fn)
    file_path = os.path.join(fp, fn)    #截屏截到的图片保存地址，这里保存到了刚才创建的文件夹中
    recording.save(file_path)           #保存图片
    time.sleep(1)                    #设置截屏时间间隔
 