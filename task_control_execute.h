#ifndef TASK_CONTROL_EXECUTE_H
#define TASK_CONTROL_EXECUTE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "calculator.h"

#define MAX_CARD_SUIT_NUM (8)

enum class SwitchCardType : uint8_t {
    kUnknowType = 0,
    kSwitchByIndex = 1,
    kSwitchById = 2
};

enum class SubCmdType : uint8_t {
    kNoneType = 0,
    kAddCards = 1,
    kDeleteCards = 2,
    kModifyCards = 3,
    kLoadCards = 4,
    kResetAllCards = 5
};

typedef struct {
    uint8_t subCmd;
    uint8_t switch_type;
    uint8_t cards_index;
    std::string cards_id;
    std::vector<uint8_t> operate_cards;
} cards_receive_data_t;

class TaskControlExecute {
   public:
    TaskControlExecute();

   public:
    static TaskControlExecute &getIns() {
        static TaskControlExecute m_TaskControlExecute;
        return m_TaskControlExecute;
    }

   public:
    void update(const cards_receive_data_t &data);

   private:
    bool checkIfIndexValid(uint8_t index);
    bool checkIfIdValid(std::string id);
    void add(uint8_t index, std::string id, const std::vector<uint8_t> &cards);
    void del(uint8_t index, const std::vector<uint8_t> &cards);
    void del(std::string id, const std::vector<uint8_t> &cards);
    void modify(uint8_t index, const std::vector<uint8_t> &cards);
    void modify(std::string id, const std::vector<uint8_t> &cards);
    CardsEventCalculator load(uint8_t index);
    CardsEventCalculator load(std::string id);
    void addCalculator(uint8_t index);
    void addCalculator(std::string id);
    void addCalculator(uint8_t index, std::string id);
    void resetAllCalculator() { cards_calculator_.clear(); };
    double getCalculatorPairProbability(uint8_t index);
    double getCalculatorPairProbability(std::string id);

   private:
    std::vector<CardsEventCalculator> cards_calculator_;
};

#endif  // TASK_CONTROL_EXECUTE_H
