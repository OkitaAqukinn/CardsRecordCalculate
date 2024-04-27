import os
import pyautogui
import time

import cv2
import numpy as np

fp = r'./cache' #指定保存截图文件夹
src = r'./source' #指定被模板图片文件夹
 
# 创建文件夹
if not os.path.exists(fp):
    os.makedirs(fp)
if not os.path.exists(src):
    os.makedirs(src)
 
#开始截屏
while True:
    t = time.localtime()
    b = str(t.tm_year) + str(t.tm_mon) + str(t.tm_mday) + str(t.tm_hour) + str(t.tm_min) + str(t.tm_sec)
    pyautogui.FAILSAFE = True
    pyautogui.PAUSE = 0.1
    recording = pyautogui.screenshot()  #截屏
    fn = b + '.png'
    print("screenshot success " + fn)
    image_path = os.path.join(fp, fn)    #截屏截到的图片保存地址，这里保存到了刚才创建的文件夹中
    recording.save(image_path)           #保存图片

    # 加载被识别图片和模板图片
    image = cv2.imread(image_path)
    count = -1
    max_source_file_count = 103
    while count < max_source_file_count:
        count += 1
        template_path = str(src) + '/' + str(count) + '.png'
        if not os.path.exists(template_path):
            print("source_file_name: ", count)
            continue
        template = cv2.imread(template_path)

        # 模板匹配
        result = cv2.matchTemplate(image, template, cv2.TM_CCOEFF_NORMED)

        # 找到最大值和最大值的位置
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(result)
        if(max_val > 0.9):
            print("source_file_name: ", int(1+(count/8)), "match probability: ", max_val)

        # # 找到匹配位置的左上角和右下角坐标
        # w, h = template.shape[:2]
        # top_left = max_loc
        # bottom_right = (top_left[0] + h, top_left[1] + w)

        # # 在被识别图片上标记匹配位置
        # cv2.rectangle(image, top_left, bottom_right, (0, 0, 255), 2)

        # # 显示结果
        # cv2.imshow('Result', image)
        # cv2.waitKey(0)
        # cv2.destroyAllWindows()

    os.remove(image_path)            #删除截屏
 