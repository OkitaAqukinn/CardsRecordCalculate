#include <iostream>
#include <thread>

#include "task_control_execute.h"
#include "thread_safe_queue.h"

threadsafe_queue<cards_receive_data_t, 5> cards_receive_data_queue;
void detectImageChangesTask() {
    for (;;) {
        // 传入字符串格式: cmd;type;index;id;card1,card2,card3,card4,card5,card6
        std::string input_card;
        std::cout << "press in the new cards here:" << std::endl;
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
        std::cout << "input_card:" << tmp_cards.c_str()
                  << " has already updated to cards" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
void changeCardsCalculateTask() {
    for (;;) {
        cards_receive_data_t tmp_data = {0};
        if (cards_receive_data_queue.try_pop(tmp_data)) {
            TaskControlExecute::getIns().update(tmp_data);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
int main() {
    std::thread detector(detectImageChangesTask);
    std::cout << "detectImageChangesTask created!!!" << std::endl;
    std::thread calculator(changeCardsCalculateTask);
    std::cout << "changeCardsCalculateTask created!!!" << std::endl;

    detector.join();
    std::cout << "detectImageChangesTask quit!!!" << std::endl;
    calculator.join();
    std::cout << "changeCardsCalculateTask quit!!!" << std::endl;
    return 0;
}