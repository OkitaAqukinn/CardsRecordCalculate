#include "task_control_execute.h"

#include "thread_safe_queue.h"

threadsafe_queue<cards_receive_data_t, 1> cards_send_data_queue;

TaskControlExecute::TaskControlExecute() {}

void TaskControlExecute::update(const cards_receive_data_t &data) {
    std::cout << "TaskControlExecute receive subCmd: " << data.subCmd
              << std::endl;
    switch (static_cast<SubCmdType>(data.subCmd)) {
        case SubCmdType::kAddCards: {
            add(data.cards_index, data.cards_id, data.operate_cards);
        } break;
        case SubCmdType::kDeleteCards: {
            if (data.switch_type ==
                static_cast<uint8_t>(SwitchCardType::kSwitchByIndex)) {
                del(data.cards_index, data.operate_cards);
                std::cout << "result: "
                          << getCalculatorPairProbability(data.cards_index)
                          << std::endl;
            } else if (data.switch_type ==
                       static_cast<uint8_t>(SwitchCardType::kSwitchById)) {
                del(data.cards_id, data.operate_cards);
                std::cout << "result: "
                          << getCalculatorPairProbability(data.cards_id)
                          << std::endl;
            }
        } break;
        case SubCmdType::kModifyCards: {
            if (data.switch_type ==
                static_cast<uint8_t>(SwitchCardType::kSwitchByIndex)) {
                modify(data.cards_index, data.operate_cards);
            } else if (data.switch_type ==
                       static_cast<uint8_t>(SwitchCardType::kSwitchById)) {
                modify(data.cards_id, data.operate_cards);
            }
        } break;
        case SubCmdType::kLoadCards: {
            cards_receive_data_t send_data = data;
            if (data.switch_type ==
                static_cast<uint8_t>(SwitchCardType::kSwitchByIndex)) {
                send_data.operate_cards =
                    load(data.cards_index).getCardsBase().getCards();
            } else if (data.switch_type ==
                       static_cast<uint8_t>(SwitchCardType::kSwitchById)) {
                send_data.operate_cards =
                    load(data.cards_id).getCardsBase().getCards();
            }
            cards_send_data_queue.push(send_data);
        } break;
        case SubCmdType::kResetAllCards: {
            resetAllCalculator();
        } break;
        default: {
            std::cout << "Unknown subCmd: " << data.subCmd << std::endl;
        }
    }
}

bool TaskControlExecute::checkIfIndexValid(uint8_t index) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsIndex() == index) {
            return true;
        }
    }
    return false;
}

bool TaskControlExecute::checkIfIdValid(std::string id) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsId() == id) {
            return true;
        }
    }
    return false;
}

void TaskControlExecute::add(uint8_t index, std::string id,
                             const std::vector<uint8_t> &cards) {}

void TaskControlExecute::del(uint8_t index, const std::vector<uint8_t> &cards) {
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

void TaskControlExecute::del(std::string id,
                             const std::vector<uint8_t> &cards) {
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

void TaskControlExecute::modify(uint8_t index,
                                const std::vector<uint8_t> &cards) {}
void TaskControlExecute::modify(std::string id,
                                const std::vector<uint8_t> &cards) {}

CardsEventCalculator TaskControlExecute::load(uint8_t index) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsIndex() == index) {
            return *it;
        }
    }
    std::cout << "del failed, index: " << index << std::endl;
    return CardsEventCalculator(0, 255, "-1");
}

CardsEventCalculator TaskControlExecute::load(std::string id) {
    std::vector<CardsEventCalculator>::iterator it = cards_calculator_.begin();
    for (it; it != cards_calculator_.end(); it++) {
        if (it->getCardsId() == id) {
            return *it;
        }
    }
    std::cout << "del failed, id: " << id << std::endl;
    return CardsEventCalculator(0, 255, "-1");
}

void TaskControlExecute::addCalculator(uint8_t index) {
    std::shared_ptr<CardsEventCalculator> tmp_calc_ptr =
        std::make_shared<CardsEventCalculator>(MAX_CARD_SUIT_NUM, index, "0");
    cards_calculator_.push_back(*tmp_calc_ptr);
}

void TaskControlExecute::addCalculator(std::string id) {
    std::shared_ptr<CardsEventCalculator> tmp_calc_ptr =
        std::make_shared<CardsEventCalculator>(MAX_CARD_SUIT_NUM, 0, id);
    cards_calculator_.push_back(*tmp_calc_ptr);
}

void TaskControlExecute::addCalculator(uint8_t index, std::string id) {
    std::shared_ptr<CardsEventCalculator> tmp_calc_ptr =
        std::make_shared<CardsEventCalculator>(MAX_CARD_SUIT_NUM, index, id);
    cards_calculator_.push_back(*tmp_calc_ptr);
}

double TaskControlExecute::getCalculatorPairProbability(uint8_t index) {
    return load(index).nextPairProbabilityCalc();
}
double TaskControlExecute::getCalculatorPairProbability(std::string id) {
    return load(id).nextPairProbabilityCalc();
}
