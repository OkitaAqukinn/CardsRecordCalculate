#include "task_control_execute.h"

#include "thread_safe_queue.h"

threadsafe_queue<cards_receive_data_t, 1> cards_send_data_queue;

TaskControlExecute::TaskControlExecute() {}

void TaskControlExecute::update(const cards_receive_data_t &data) {
    std::cout << "TaskControlExecute receive subCmd: " << data.subCmd
              << std::endl;
    switch (static_cast<SubCmdType>(data.subCmd)) {
        case SubCmdType::kAddCards: {
            if (data.switch_type ==
                static_cast<int>(SwitchCardType::kSwitchByIndex)) {
                addCalculator(data.cards_index);
            } else if (data.switch_type ==
                       static_cast<int>(SwitchCardType::kSwitchById)) {
                addCalculator(data.cards_id);
            }
        } break;
        case SubCmdType::kDeleteCards: {
            if (data.switch_type ==
                static_cast<int>(SwitchCardType::kSwitchByIndex)) {
                del(data.cards_index, data.operate_cards);
                std::cout << "result: "
                          << getCalculatorPairProbability(data.cards_index)
                          << std::endl;
            } else if (data.switch_type ==
                       static_cast<int>(SwitchCardType::kSwitchById)) {
                del(data.cards_id, data.operate_cards);
                std::cout << "result: "
                          << getCalculatorPairProbability(data.cards_id)
                          << std::endl;
            }
        } break;
        case SubCmdType::kModifyCards: {
            if (data.switch_type ==
                static_cast<int>(SwitchCardType::kSwitchByIndex)) {
                modify(data.cards_index, data.operate_cards);
            } else if (data.switch_type ==
                       static_cast<int>(SwitchCardType::kSwitchById)) {
                modify(data.cards_id, data.operate_cards);
            }
        } break;
        case SubCmdType::kLoadCards: {
            cards_receive_data_t send_data = data;
            if (data.switch_type ==
                static_cast<int>(SwitchCardType::kSwitchByIndex)) {
                send_data.operate_cards =
                    load(data.cards_index).getCardsBase().getCards();
            } else if (data.switch_type ==
                       static_cast<int>(SwitchCardType::kSwitchById)) {
                send_data.operate_cards =
                    load(data.cards_id).getCardsBase().getCards();
            }
            cards_send_data_queue.push(send_data);
        } break;
        case SubCmdType::kResetCards: {
            if (data.switch_type ==
                static_cast<int>(SwitchCardType::kSwitchByIndex)) {
                delCalculator(data.cards_index);
            } else if (data.switch_type ==
                       static_cast<int>(SwitchCardType::kSwitchById)) {
                delCalculator(data.cards_id);
            }
        } break;
        case SubCmdType::kResetAllCards: {
            resetAllCalculator();
        } break;
        default: {
            std::cout << "Unknown subCmd: " << data.subCmd << std::endl;
        }
    }
}

bool TaskControlExecute::checkIfIndexValid(int index) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsIndex() == index) {
            return true;
        }
    }
    std::cout << "Check index failed: " << index << std::endl;
    return false;
}

bool TaskControlExecute::checkIfIdValid(std::string id) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsId() == id) {
            return true;
        }
    }
    std::cout << "Check id failed: " << id << std::endl;
    return false;
}

void TaskControlExecute::addCalculator(int index) {
    std::shared_ptr<CardsEventCalculator> tmp_calc_ptr =
        std::make_shared<CardsEventCalculator>(MAX_CARD_SUIT_NUM, index, "0");
    cards_calculator_.push_back(*tmp_calc_ptr);
    std::cout << index << " index already push back to vector" << std::endl;
}

void TaskControlExecute::addCalculator(std::string id) {
    std::shared_ptr<CardsEventCalculator> tmp_calc_ptr =
        std::make_shared<CardsEventCalculator>(MAX_CARD_SUIT_NUM, 0, id);
    cards_calculator_.push_back(*tmp_calc_ptr);
    std::cout << id << " id already push back to vector" << std::endl;
}

void TaskControlExecute::addCalculator(int index, std::string id) {
    std::shared_ptr<CardsEventCalculator> tmp_calc_ptr =
        std::make_shared<CardsEventCalculator>(MAX_CARD_SUIT_NUM, index, id);
    cards_calculator_.push_back(*tmp_calc_ptr);
}

void TaskControlExecute::delCalculator(int index) {
    if (!checkIfIndexValid(index)) {
        std::cout << "cannot found delCalculator, index: " << index
                  << std::endl;
        return;
    }
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsIndex() == index) {
            cards_calculator_.erase(it);
            std::cout << "delCalculator successfully, index: " << index
                      << std::endl;
        }
    }
}

void TaskControlExecute::delCalculator(std::string id) {
    if (!checkIfIdValid(id)) {
        std::cout << "cannot found delCalculator, index: " << id << std::endl;
        return;
    }
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsId() == id) {
            cards_calculator_.erase(it);
            std::cout << "delCalculator successfully, id: " << id << std::endl;
        }
    }
}

void TaskControlExecute::del(int index, const std::vector<int> &cards) {
    if (!checkIfIndexValid(index)) {
        addCalculator(index);
    }
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsIndex() == index) {
            bool result = it->getCardsBase().removeCards(cards);
            std::cout << "del result: " << result << ", index: " << index
                      << std::endl;
        }
    }
}

void TaskControlExecute::del(std::string id, const std::vector<int> &cards) {
    if (!checkIfIdValid(id)) {
        addCalculator(id);
    }
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsId() == id) {
            bool result = it->getCardsBase().removeCards(cards);
            std::cout << "del result: " << result << ", id: " << id
                      << std::endl;
        }
    }
}

void TaskControlExecute::modify(int index, const std::vector<int> &cards) {
    if (!checkIfIndexValid(index)) {
        addCalculator(index);
    }
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsIndex() == index) {
            it->getCardsBase().setCards(cards, MAX_CARD_SUIT_NUM);
            std::cout << "modify successfully, index: " << index << std::endl;
            return;
        }
    }
    std::cout << "modify failed, index: " << index << std::endl;
}
void TaskControlExecute::modify(std::string id, const std::vector<int> &cards) {
    if (!checkIfIdValid(id)) {
        addCalculator(id);
    }
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsId() == id) {
            it->getCardsBase().setCards(cards, MAX_CARD_SUIT_NUM);
            std::cout << "modify successfully, id: " << id << std::endl;
            return;
        }
    }
    std::cout << "modify failed, id: " << id << std::endl;
}

CardsEventCalculator TaskControlExecute::load(int index) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsIndex() == index) {
            return *it;
        }
    }
    std::cout << "load failed, index: " << index << std::endl;
    return CardsEventCalculator(0, 255, "-1");
}

CardsEventCalculator TaskControlExecute::load(std::string id) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsId() == id) {
            return *it;
        }
    }
    std::cout << ";oad failed, id: " << id << std::endl;
    return CardsEventCalculator(0, 255, "-1");
}

double TaskControlExecute::getCalculatorPairProbability(int index) {
    return load(index).nextPairProbabilityCalc();
}

double TaskControlExecute::getCalculatorPairProbability(std::string id) {
    return load(id).nextPairProbabilityCalc();
}
