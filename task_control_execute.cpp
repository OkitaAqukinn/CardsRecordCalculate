#include "task_control_execute.h"

TaskControlExecute::TaskControlExecute() {}

void TaskControlExecute::update(const cards_send_data_t &data) {
    std::cout << "TaskControlExecute receive subCmd: " << data.subCmd
              << std::endl;
    switch (static_cast<SubCmdType>(data.subCmd)) {
        case SubCmdType::kAddCards: {
        } break;
        case SubCmdType::kDeleteCards: {
        } break;
        case SubCmdType::kModifyCards: {
        } break;
        case SubCmdType::kLoadCards: {
        } break;
        default: {
            std::cout << "Unknown subCmd: " << data.subCmd << std::endl;
        }
    }
}

void TaskControlExecute::add(uint8_t index, std::string id,
                             const std::vector<uint8_t> &cards) {}

void TaskControlExecute::del(uint8_t index) {}
void TaskControlExecute::del(std::string id) {}
void TaskControlExecute::modify(uint8_t index,
                                const std::vector<uint8_t> &cards) {}
void TaskControlExecute::modify(std::string id,
                                const std::vector<uint8_t> &cards) {}
CardsEventCalculator TaskControlExecute::load(uint8_t index) const {}
CardsEventCalculator TaskControlExecute::load(std::string id) const {}
