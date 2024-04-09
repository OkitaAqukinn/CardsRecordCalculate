#ifndef TASK_CONTROL_EXECUTE_H
#define TASK_CONTROL_EXECUTE_H

#include <iostream>
#include <string>
#include <vector>

#include "calculator.h"

enum class SubCmdType : uint8_t {
    kNoneType = 0,
    kAddCards = 1,
    kDeleteCards = 2,
    kModifyCards = 3,
    kLoadCards = 4
};

typedef struct {
    uint8_t subCmd;
    uint8_t cards_index;
    std::string cards_id;
    std::vector<uint8_t> operate_cards;
} cards_send_data_t;

class TaskControlExecute {
   public:
    TaskControlExecute();

   public:
    static TaskControlExecute &getIns() {
        static TaskControlExecute m_TaskControlExecute;
        return m_TaskControlExecute;
    }

   public:
    void update(const cards_send_data_t &data);

   private:
    void add(uint8_t index, std::string id, const std::vector<uint8_t> &cards);
    void del(uint8_t index);
    void del(std::string id);
    void modify(uint8_t index, const std::vector<uint8_t> &cards);
    void modify(std::string id, const std::vector<uint8_t> &cards);
    CardsEventCalculator load(uint8_t index) const;
    CardsEventCalculator load(std::string id) const;

   private:
    std::vector<CardsEventCalculator> cards_calculator_;
};

#endif  // TASK_CONTROL_EXECUTE_H
