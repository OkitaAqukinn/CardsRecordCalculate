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

image_match_probability = 0.68
middle1_top_left = 0
middle1_top_right = 0
middle1_bottom_left = 0
middle1_bottom_right = 0
middle2_top_left = 0
middle2_top_right = 0
middle2_bottom_left = 0
middle2_bottom_right = 0

standard_screenshot_width = 1920
standard_screenshot_height = 1080
standard_width_height_scale = standard_screenshot_width / standard_screenshot_height

def clipImage(image_path):
    global middle1_top_left
    global middle1_top_right
    global middle1_bottom_left
    global middle1_bottom_right
    global middle2_top_left
    global middle2_top_right
    global middle2_bottom_left
    global middle2_bottom_right
    image = cv2.imread(image_path)
    logo_path = str(src) + '/' + 'logo.png'
    if not os.path.exists(logo_path):
        print("something wrong while clipping image: ", logo_path)
        return False
    logo = cv2.imread(logo_path)
    logo_result = cv2.matchTemplate(image, logo, cv2.TM_CCOEFF_NORMED)
    logo_min_val, logo_max_val, logo_min_loc, logo_max_loc = cv2.minMaxLoc(logo_result)
    print("clipImage match logo probability: ", logo_max_val)
    if(logo_max_val > image_match_probability):
        logo_h, logo_w = logo.shape[:2]
        logo_top_left = logo_max_loc
        logo_top_right = (logo_top_left[0] + logo_w, logo_top_left[1])
        logo_middle_x = (int)(logo_top_left[0] + logo_w / 2)
        cropped_top_left = (middle1_top_left[0] - 2 * (middle2_top_left[0] - middle1_top_left[0]), middle1_top_left[1])
        cropped_bottom_left = (cropped_top_left[0], middle1_bottom_left[1])
        count = 0
        gain = 0
        last_gain = 0
        margin = (int)(logo_w / 2)
        max_image_count = 6
        while count < max_image_count:
            if(count == 0):
                last_gain = cropped_top_left[0]
                gain = middle1_top_left[0] + margin
            elif (count == 1):
                last_gain = middle1_top_left[0] - margin
                gain = middle2_top_left[0] + margin
            elif (count == 2):
                last_gain = middle2_top_left[0] - margin
                gain = logo_top_left[0] + margin
            elif (count == 3):
                last_gain = logo_top_right[0] - margin
                gain = logo_middle_x + (logo_middle_x - middle2_top_left[0]) + margin
            elif (count == 4):
                last_gain = logo_middle_x + (logo_middle_x - middle2_top_left[0]) - margin
                gain = logo_middle_x + (logo_middle_x - middle1_top_left[0]) + margin
            elif (count == 5):
                last_gain = logo_middle_x + (logo_middle_x - middle1_top_left[0]) - margin
                gain = logo_middle_x + (logo_middle_x - cropped_top_left[0])
            print("cropped image count: ", count, "cropped_top_left[1]: ", cropped_top_left[1], "cropped_bottom_left[1]: ", cropped_bottom_left[1])
            print("(last_gain): ", (last_gain), "(gain) ", (gain))
            cropped = image[cropped_top_left[1]:cropped_bottom_left[1], 
                            last_gain:gain]  # 裁剪坐标为[y0:y1, x0:x1]
            cv2.imwrite(str(fp) + '/' + str(count) + '.png', cropped)
            count += 1

screenshot_ready_flag = False
def screenshot(image_path):
    global screenshot_ready_flag
    global middle1_top_left
    global middle1_top_right
    global middle1_bottom_left
    global middle1_bottom_right
    global middle2_top_left
    global middle2_top_right
    global middle2_bottom_left
    global middle2_bottom_right
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
    print("screenshot match background probability: ", max_val)
    if(max_val > image_match_probability and screenshot_ready_flag == False):
        time.sleep(1.3)
        image = cv2.imread(image_path)
        print("background match, time: ", b)
        print("Ready to prepare screenshot.")
        middle1_path = str(src) + '/' + 'middle1.png'
        if not os.path.exists(middle1_path):
            print("something wrong while loading image: ", middle1_path)
            return False
        middle1 = cv2.imread(middle1_path)
        middle1_result = cv2.matchTemplate(image, middle1, cv2.TM_CCOEFF_NORMED)
        middle1_min_val, middle1_max_val, middle1_min_loc, middle1_max_loc = cv2.minMaxLoc(middle1_result)
        middle1_h, middle1_w = middle1.shape[:2]
        middle1_top_left = middle1_max_loc
        middle1_top_right = (middle1_top_left[0] + middle1_w, middle1_top_left[1])
        middle1_bottom_left = (middle1_top_left[0], middle1_top_left[1] + middle1_h)
        middle1_bottom_right = (middle1_top_right[0] + middle1_w, middle1_top_right[1] + middle1_h)
        middle2_path = str(src) + '/' + 'middle2.png'
        if not os.path.exists(middle2_path):
            print("something wrong while loading image: ", middle2_path)
            return False
        middle2 = cv2.imread(middle2_path)
        middle2_result = cv2.matchTemplate(image, middle2, cv2.TM_CCOEFF_NORMED)
        middle2_min_val, middle2_max_val, middle2_min_loc, middle2_max_loc = cv2.minMaxLoc(middle2_result)
        middle2_h, middle2_w = middle2.shape[:2]
        middle2_top_left = middle2_max_loc
        middle2_top_right = (middle2_top_left[0] + middle2_w, middle2_top_left[1])
        middle2_bottom_left = (middle2_top_left[0], middle2_top_left[1] + middle2_h)
        middle2_bottom_right = (middle2_top_right[0] + middle2_w, middle2_top_right[1] + middle2_h)
        screenshot_ready_flag = True
    if(max_val < image_match_probability and screenshot_ready_flag == True):
        os.remove(image_path)
        time.sleep(0.8)
        print("screenshot prepared, time: ", b)
        pyautogui.FAILSAFE = True
        pyautogui.PAUSE = 0.1
        recording = pyautogui.screenshot()   #截屏
        recording.save(image_path)
        image = cv2.imread(image_path)
        current_screenshot_h, current_screenshot_w = image.shape[:2]
        current_width_height_scale = current_screenshot_w / current_screenshot_h
        width_scale = standard_screenshot_width / current_screenshot_w
        modify_screenshot_width = int(current_screenshot_w * width_scale)
        modify_screenshot_height = int(modify_screenshot_width / current_width_height_scale)
        image_resize = cv2.resize(image, (modify_screenshot_width, modify_screenshot_height))
        cv2.imwrite(image_path, image_resize)           #保存图片
        clipImage(image_path)
        screenshot_ready_flag = False
        return True
    os.remove(image_path)            #删除截屏
    return False

def getImageMatchResult(source_path, template_path):
    if not os.path.exists(source_path):
        print("template_path_name load failed in getImageMatchResult: ", source_count)
        return False
    if not os.path.exists(template_path):
        print("source_file_name load failed in getImageMatchResult: ", template_path)
        return False
    
    source = cv2.imread(source_path)
    template = cv2.imread(template_path)
    result = cv2.matchTemplate(source, template, cv2.TM_CCOEFF_NORMED)
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(result)
    return max_val

def debugImage(image, top_left, bottom_right):
    # 在被识别图片上标记匹配位置
    cv2.rectangle(image, top_left, bottom_right, (0, 0, 255), 2)

    # 显示结果
    cv2.imshow('Result', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

game_id = input("please press in current game id:")
while True:
    fn = 'screenshot' + '.png'
    image_path = os.path.join(fp, fn)
    if(screenshot(image_path) == False):
        time.sleep(0.2)                    #设置截屏时间间隔
        continue

    # 加载被识别图片和模板图片
    cache_count = 0
    max_cache_file_count = 6
    kConfirmMatchCardsProbability = 0.45
    result_list = [0, 0, 0, 0, 0, 0]
    while cache_count < max_cache_file_count:
        cache_path = str(fp) + '/' + str(cache_count) + '.png'
        source_count = 0
        if(cache_count == 0 or cache_count == 5):
            source_count = 1
        max_source_file_count = 103
        current_max_match_probability = 0.0
        current_max_match_index = -1
        while source_count < max_source_file_count:
            template_path = str(src) + '/' + str(source_count) + '.png'
            current_match_probability = getImageMatchResult(cache_path, template_path)
            if(current_match_probability > kConfirmMatchCardsProbability and current_match_probability > current_max_match_probability):
                current_max_match_probability = current_match_probability
                current_max_match_index = source_count
            source_count += 2
        if(current_max_match_probability > kConfirmMatchCardsProbability):
            result_list[cache_count] = int(1 + (current_max_match_index / 8))
            print("cache_count: ", cache_count, "current_max_match_index: ", current_max_match_index, "current_max_match_probability: ", current_max_match_probability)
        else:
            print("cache_count: ", cache_count, "failed to match any source cards")
        cache_count += 1

    result_path = str(fp) + '/' + str(game_id) + '.txt'
    if not os.path.exists(result_path):
        with open(result_path, 'w') as f:
            result_str = ''
            for i in range(0, len(result_list)):
                if(i == len(result_list) - 1):
                    result_str = result_str + str(result_list[i])
                else:
                    result_str = result_str + str(result_list[i]) + ','
            f.write(result_str)
    else:
        print("last result has not updated into cpp file yet, do not update again.")
    os.remove(image_path)            #删除截屏
 