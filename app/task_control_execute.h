#ifndef TASK_CONTROL_EXECUTE_H
#define TASK_CONTROL_EXECUTE_H
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "calculator.h"

#define MAX_CARD_SUIT_NUM (8)
enum class SwitchCardType : int {
    kUnknowType = 0,
    kSwitchByIndex = 1,
    kSwitchById = 2
};
enum class SubCmdType : int {
    kNoneType = 0,
    kAddCards = 1,
    kDeleteCards = 2,
    kModifyCards = 3,
    kLoadCards = 4,
    kResetAllCards = 5
};
typedef struct {
    int subCmd;
    int switch_type;
    int cards_index;
    std::string cards_id;
    std::vector<int> operate_cards;
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
    bool checkIfIndexValid(int index);
    bool checkIfIdValid(std::string id);
    void addCalculator(int index);
    void addCalculator(std::string id);
    void addCalculator(int index, std::string id);
    void delCalculator(int index);
    void delCalculator(std::string id);
    void del(int index, const std::vector<int> &cards);
    void del(std::string id, const std::vector<int> &cards);
    void modify(int index, const std::vector<int> &cards);
    void modify(std::string id, const std::vector<int> &cards);
    CardsEventCalculator load(int index);
    CardsEventCalculator load(std::string id);
    void resetAllCalculator() { cards_calculator_.clear(); };
    double getCalculatorPairProbability(int index);
    double getCalculatorPairProbability(std::string id);

   private:
    std::vector<CardsEventCalculator> cards_calculator_;
};
#endif  // TASK_CONTROL_EXECUTE_H