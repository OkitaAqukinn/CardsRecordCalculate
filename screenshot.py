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

image_match_probability = 0.85

screenshot_ready_flag = False
def screenshot( image_path ):
    global screenshot_ready_flag
    t = time.localtime()
    b = str(t.tm_year) + str(t.tm_mon) + str(t.tm_mday) + str(t.tm_hour) + str(t.tm_min) + str(t.tm_sec)
    pyautogui.FAILSAFE = True
    pyautogui.PAUSE = 0.1
    recording = pyautogui.screenshot()   #截屏
    print("screenshot success " + b)
    recording.save(image_path)           #保存图片

    image = cv2.imread(image_path)
    template_path = str(src) + '/' + 'background.png'
    if not os.path.exists(template_path):
        print("something wrong while loading: ", template_path)
        return False
    template = cv2.imread(template_path)
    result = cv2.matchTemplate(image, template, cv2.TM_CCOEFF_NORMED)
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(result)
    if(max_val > image_match_probability and screenshot_ready_flag == False):
        print("background match, time: ", b)
        print("Ready to prepare screenshot.")
        screenshot_ready_flag = True
    if(max_val < image_match_probability and screenshot_ready_flag == True):
        print("screenshot prepared, time: ", b)
        recording.save(image_path)           #保存图片
        screenshot_ready_flag = False
        return True
    os.remove(image_path)            #删除截屏
    return False

#开始截屏
while True:
    fn = 'screenshot' + '.png'
    image_path = os.path.join(fp, fn)
    if(screenshot(image_path) == False):
        time.sleep(0.5)                    #设置截屏时间间隔
        continue

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
        if(max_val > image_match_probability):
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
 