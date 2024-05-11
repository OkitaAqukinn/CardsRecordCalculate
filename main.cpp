#include <direct.h>
#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "task_control_execute.h"
#include "thread_safe_queue.h"

threadsafe_queue<cards_receive_data_t, 5> cards_receive_data_queue;

std::vector<std::string> getFilesCardsId(const std::string& directory) {
    std::vector<std::string> txtFiles;
    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) {
        std::cerr << "Failed to open directory while getFilesCardsId: "
                  << directory << std::endl;
        return txtFiles;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename.size() >= 4 &&
            filename.substr(filename.size() - 4) == ".txt") {
            txtFiles.push_back(filename);
        }
    }

    closedir(dir);
    return txtFiles;
}

std::string removeSubstring(const std::string& str, const std::string& substr) {
    std::string result = str;
    size_t pos = result.find(substr);
    while (pos != std::string::npos) {
        result.erase(pos, substr.length());
        pos = result.find(substr, pos);
    }
    return result;
}

void manualDebugCardsTask() {
    for (;;) {
        // 传入字符串格式: cmd;type;index;id;card1,card2,card3,card4,card5,card6
        std::string input_card;
        std::cout << "press in the new cards manually here :" << std::endl;
        std::cin >> input_card;
        cards_receive_data_t tmp_data = {0};
        std::string tmp_cards;
        sscanf((const char *)input_card.c_str(), "%d;%d;%d;%[^;];%[^;]",
               &tmp_data.subCmd, &tmp_data.switch_type, &tmp_data.cards_index,
               tmp_data.cards_id.data(), tmp_cards.data());
        int card1 = 0, card2 = 0, card3 = 0, card4 = 0, card5 = 0, card6 = 0;
        sscanf((const char *)tmp_cards.c_str(), "%d,%d,%d,%d,%d,%d", &card1,
               &card2, &card3, &card4, &card5, &card6);
        tmp_data.operate_cards.push_back(card1);
        tmp_data.operate_cards.push_back(card2);
        tmp_data.operate_cards.push_back(card3);
        tmp_data.operate_cards.push_back(card4);
        tmp_data.operate_cards.push_back(card5);
        tmp_data.operate_cards.push_back(card6);
        cards_receive_data_queue.push(tmp_data);
        std::cout << "manualDebugCardsTask input_card:" << tmp_cards.c_str()
                  << " has already updated to cards" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void changeCardsCalculateTask() {
    for (;;) {
        cards_receive_data_t tmp_data = {0};
        if (cards_receive_data_queue.try_pop(tmp_data)) {
            TaskControlExecute::getIns().execute(tmp_data);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void detectImageChangesTask() {
    char szBuf[512] = {0};
    getcwd(szBuf, sizeof(szBuf) - 1);
    std::string result_path = szBuf;
    std::string current_file_name = "main.cpp";
    result_path = removeSubstring(result_path, current_file_name);
    result_path = result_path + "/" + "cache";
    for (int16_t i = 0; i < result_path.size(); i++) {
        if (result_path[i] == '\\') {
            result_path[i] = '/';
        }
    }
    for (;;) {
        // 组包字符串格式: cmd;type;index;id;card1,card2,card3,card4,card5,card6
        std::vector<std::string> result = getFilesCardsId(result_path);
        if (result.size() > 0) {
            std::cout << "Found " << result.size()
                      << " .txt files in directory " << result_path << ":"
                      << std::endl;
            for (const auto& file_name : result) {
                std::string content;
                std::cout << "Found file: " << file_name << std::endl;
                opendir(result_path.c_str());
                std::ifstream file(result_path + "/" + file_name);
                if (file.is_open()) {
                    std::string line;
                    while (std::getline(file, line)) {
                        content += line + '\n';
                    }
                    std::cout << "Content is: " << content << std::endl;
                    file.close();

                } else {
                    std::cerr << "Failed to open file " << file_name
                              << std::endl;
                }
                cards_receive_data_t tmp_data = {0};
                std::string detect_cards;
                tmp_data.subCmd = static_cast<int>(SubCmdType::kDeleteCards);
                tmp_data.switch_type =
                    static_cast<int>(SwitchCardType::kSwitchById);
                tmp_data.cards_id = file_name;
                int card1 = 0, card2 = 0, card3 = 0, card4 = 0, card5 = 0,
                    card6 = 0;
                sscanf((const char*)content.c_str(), "%d,%d,%d,%d,%d,%d",
                       &card1, &card2, &card3, &card4, &card5, &card6);
                tmp_data.operate_cards.push_back(card1);
                tmp_data.operate_cards.push_back(card2);
                tmp_data.operate_cards.push_back(card3);
                tmp_data.operate_cards.push_back(card4);
                tmp_data.operate_cards.push_back(card5);
                tmp_data.operate_cards.push_back(card6);
                cards_receive_data_queue.push(tmp_data);
                std::cout << "detectImageChangesTask input_card:" << 1
                          << " has already updated to cards" << std::endl;
                if (std::remove((result_path + "/" + file_name).c_str()) == 0) {
                    std::cerr << "Delete file " << file_name
                              << " successfully after loading its content"
                              << std::endl;
                } else {
                    std::cerr << "Failed to delete file " << file_name
                              << ", please check if project is running normally"
                              << std::endl;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::thread calculator(changeCardsCalculateTask);
    std::cout << "changeCardsCalculateTask created!!!" << std::endl;
    std::thread detector(detectImageChangesTask);
    std::cout << "detectImageChangesTask created!!!" << std::endl;
    std::thread debugger(manualDebugCardsTask);
    std::cout << "manualDebugCardsTask created!!!" << std::endl;

    calculator.join();
    std::cout << "changeCardsCalculateTask quit!!!" << std::endl;
    detector.join();
    std::cout << "detectImageChangesTask quit!!!" << std::endl;
    debugger.join();
    std::cout << "manualDebugCardsTask quit!!!" << std::endl;

    return 0;
}