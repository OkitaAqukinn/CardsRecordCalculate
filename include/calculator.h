#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <algorithm>
#include <iostream>
#include <vector>

#define MAX_CARD_RANK (13)
#define MAX_CARD_PATTERN (4)
#define MIN_POP_CARDS_NUM (4)
#define MAX_POP_CARDS_NUM (6)
#define SINGLE_TURN_POP_CARDS (2)

enum class PairCardsCalcType : uint8_t {
    kNoneType = 0,
    kNormalType = 1,
    kContinueType = 2
};

class CardsBase {
   public:
    CardsBase(int suit_num) {
        suit_num_ = suit_num;
        reset();
    };
    CardsBase(){};
    ~CardsBase(){};

   public:
    void reset() {
        cards_.clear();
        for (int i = 0; i < suit_num_; i++) {
            for (int j = 0; j < MAX_CARD_PATTERN; j++) {
                for (int k = 1; k <= MAX_CARD_RANK; k++) {
                    addCard(k);
                }
            }
        }
        total_cards_num_ = cards_.size();
        remain_cards_num_ = total_cards_num_;
        std::cout << "Cards reset successfully, total_cards_num_:"
                  << total_cards_num_
                  << "remain_cards_num_:" << remain_cards_num_ << std::endl;
    };
    void addCard(int card) {
        cards_.push_back(card);
        remain_cards_num_++;
    };
    void addCards(std::vector<int> cards) {
        cards_.insert(cards_.end(), cards.begin(), cards.end());
        remain_cards_num_ += cards.size();
    };
    bool removeCard(int card) {
        std::vector<int>::iterator it =
            std::find(cards_.begin(), cards_.end(), card);
        if (it != cards_.end()) {
            cards_.erase(it);
            remain_cards_num_--;
            return true;
        } else {
            std::cout << "Card not found:" << card
                      << ". Please check the integrity of current cards"
                      << std::endl;
            return false;
        }
    };
    bool removeCards(const std::vector<int>& cards) {
        for (auto card : cards) {
            auto it = std::find(cards_.begin(), cards_.end(), card);
            if (it != cards_.end()) {
                std::cout << "removeCards: " << *it << std::endl;
                cards_.erase(it);
                remain_cards_num_--;
            } else {
                std::cout << "Card not found:" << card
                          << ". Please check the integrity of current cards"
                          << std::endl;
                continue;
            }
        }
        return true;
    };
    int getSuitNum() const { return suit_num_; };
    void setSuitNum(int suit_num) { suit_num_ = suit_num; };
    int getTotalCardsNum() const { return total_cards_num_; };
    int getRemainCardsNum() const { return remain_cards_num_; };
    std::vector<int>& getCards() { return cards_; };
    void setCards(const std::vector<int>& cards, int suit_num) {
        cards_ = cards;
        setSuitNum(suit_num);
        total_cards_num_ = suit_num_ * MAX_CARD_RANK;
        remain_cards_num_ = cards_.size();
    }

   private:
    int suit_num_;
    int total_cards_num_;
    int remain_cards_num_;
    std::vector<int> cards_;
};
class CardsEventCalculator {
   public:
    CardsEventCalculator(int suit_num, int index, std::string id)
        : croupier_pair_probability_(0.0), player_pair_probability_(0.0) {
        reset(suit_num);
        index_ = index;
        id_ = id;
    }
    ~CardsEventCalculator(){};

   public:
    CardsBase& getCardsBase() { return current_cards_; }
    void setCardsIndex(int index) { index_ = index; }
    int getCardsIndex() const { return index_; }
    void setCardsId(const std::string& id) { id_ = id; }
    std::string getCardsId() const { return id_; }

   public:
    void reset(int suit_num) {
        current_cards_.setSuitNum(suit_num);
        current_cards_.reset();
    }

    void update() {
        updatePairEventProbability(PairCardsCalcType::kNormalType);
        std::cout << "current id: " << id_.c_str() << std::endl;
        std::cout << "current index: " << index_ << std::endl;
        int tmp_multi[MAX_CARD_RANK] = {0};
        for (uint32_t i = 1; i <= MAX_CARD_RANK; i++) {
            tmp_multi[i - 1] = countDuplicates(current_cards_.getCards(), i);
            std::cout << "current cards [" << i << "]: " << tmp_multi[i - 1]
                      << std::endl;
        }
        std::cout << "current cards next croupier_pair_probability: "
                  << croupier_pair_probability_ << std::endl;
        std::cout << "current cards next player_pair_probability: "
                  << player_pair_probability_ << std::endl;
    }

   private:
    uint64_t permutationCombinationFormula(uint64_t n, uint64_t m) {
        // 组合公式C(n,m) = n!/(m!(n-m)!)
        uint64_t tmp_multiplicate = combinator(n, (n - m + 1));
        return (tmp_multiplicate / factorial(m));
    }

    uint64_t factorial(int n) {
        if (n == 0 || n == 1) {
            return 1;
        } else {
            return n * factorial(n - 1);
        }
    }

    uint64_t combinator(int n, int m) {
        if (n == m) {
            return n;
        }
        if (n == 0 || n == 1 || n < m) {
            return 1;
        } else {
            return n * combinator((n - 1), m);
        }
    }

    uint64_t decreaseSum(int n) {
        if (n <= 0) {
            return 0;
        } else {
            return n + decreaseSum(n - 1);
        }
    }

    int countDuplicates(const std::vector<int>& vec, int target) {
        int count = 0;
        for (int num : vec) {
            if (num == target) {
                count++;
            }
        }
        return count;
    }

    void updatePairEventProbability(PairCardsCalcType type) {
        if (type == PairCardsCalcType::kNormalType) {
            updateCroupierPairProbability();
            updatePlayerPairProbability();
        } else if (type == PairCardsCalcType::kContinueType) {
        } else {
            std::cout
                << "nextPairProbabilityCalc failed for received unknown type: "
                << static_cast<int>(type) << std::endl;
        }
    }

    void updateCroupierPairProbability() {
        croupier_pair_probability_ = calcPairProbability(current_cards_);
    }

    void updatePlayerPairProbability() {
        int tmp_multi[MAX_CARD_RANK] = {0};
        double tmp_pair_probability = 0.0;
        for (uint32_t i = 1; i <= MAX_CARD_RANK; i++) {
            tmp_multi[i - 1] = countDuplicates(current_cards_.getCards(), i);
        }
        for (int j = 0; j < MAX_CARD_RANK; j++) {
            CardsBase tmp_cards1 = current_cards_;
            tmp_cards1.removeCard(j + 1);
            double ovo = calcPairProbability(tmp_cards1);
            tmp_pair_probability +=
                (static_cast<double>(tmp_multi[j]) /
                 static_cast<double>(tmp_cards1.getRemainCardsNum())) *
                calcPairProbability(tmp_cards1);
        }
        player_pair_probability_ = tmp_pair_probability;
    }

    double calcPairProbability(const CardsBase& operate_cards) {
        // 庄对子 = P(第一第三张牌重复); 闲对子 = P(第二第四张牌重复)
        if (operate_cards.getRemainCardsNum() <= (SINGLE_TURN_POP_CARDS + 1)) {
            std::cout << "nextPairProbabilityCalc failed 1: "
                      << operate_cards.getRemainCardsNum() << std::endl;
            return 0.0;
        }
        uint64_t total_count = (operate_cards.getRemainCardsNum() - 2) *
                               (operate_cards.getRemainCardsNum() - 1) *
                               operate_cards.getRemainCardsNum();
        std::cout << "nextPairProbabilityCalc current total_count: "
                  << total_count << std::endl;
        if (total_count <= 1) {
            std::cout << "nextPairProbabilityCalc failed 2: "
                      << operate_cards.getRemainCardsNum() << std::endl;
            return 0.0;
        }

        int total_choice_count = MAX_CARD_RANK;
        int tmp_multi[MAX_CARD_RANK] = {0};
        CardsBase tmp_cards1 = operate_cards;
        for (uint32_t i = 1; i <= MAX_CARD_RANK; i++) {
            tmp_multi[i - 1] = countDuplicates(tmp_cards1.getCards(), i);
            if (tmp_multi[i - 1] <= 0) total_choice_count--;
        }
        if (total_choice_count < 1) return 0.0;

        uint64_t pair_count = 0;
        for (int j = 0; j < MAX_CARD_RANK; j++) {
            if (tmp_multi[j] < 1) continue;
            CardsBase tmp_cards2 = tmp_cards1;
            tmp_cards2.removeCard(j + 1);
            pair_count +=
                tmp_multi[j] * calcPairCountsContinuous(tmp_cards2, j + 1);
        }
        std::cout << "pair_count: " << pair_count << std::endl;
        if (total_count < pair_count) {
            std::cout << "nextPairProbabilityCalc failed for "
                         "total_count"
                      << total_count << std::endl;
            return 0.0;
        }
        double pair_probability =
            static_cast<double>(pair_count) / static_cast<double>(total_count);
        return pair_probability;
    }

    uint64_t calcPairCountsContinuous(const CardsBase& operate_cards,
                                      int pop_card) {
        int total_choice_count = MAX_CARD_RANK;
        int tmp_multi[MAX_CARD_RANK] = {0};
        CardsBase tmp_cards1 = operate_cards;
        for (uint32_t i = 1; i <= MAX_CARD_RANK; i++) {
            tmp_multi[i - 1] = countDuplicates(tmp_cards1.getCards(), i);
            if (tmp_multi[i - 1] <= 0) total_choice_count--;
        }
        if (total_choice_count < 1) return 0;

        uint64_t pair_count = 0;
        for (int j = 0; j < MAX_CARD_RANK; j++) {
            CardsBase tmp_cards2 = tmp_cards1;
            if (tmp_multi[j] < 1) continue;
            tmp_cards2.removeCard(j + 1);
            pair_count += (tmp_multi[j] *
                           countDuplicates(tmp_cards2.getCards(), pop_card));
        }
        return pair_count;
    }

    double nextMultiProbabilityCalc() {
        uint64_t total_count = permutationCombinationFormula(
            current_cards_.getRemainCardsNum(), MIN_POP_CARDS_NUM);
        std::cout
            << "nextMultiProbabilityCalc current total combination counts: "
            << total_count << std::endl;
        if (total_count <= 1) {
            std::cout << "nextMultiProbabilityCalc failed: "
                      << current_cards_.getRemainCardsNum() << std::endl;
            return 0.0;
        }

        int total_choice_count = MAX_CARD_RANK;
        int tmp_multi[MAX_CARD_RANK] = {0};
        for (uint32_t i = 1; i <= MAX_CARD_RANK; i++) {
            tmp_multi[i - 1] = countDuplicates(current_cards_.getCards(), i);
            if (tmp_multi[i - 1] <= 0) total_choice_count--;
        }
        if (total_choice_count < 1) return 0.0;
        uint64_t unmatched_count = 0;
        uint64_t unique_choice_count = 0;
        for (int i = 0; i + 3 < MAX_CARD_RANK; i++) {
            for (int j = i + 1; j + 2 < MAX_CARD_RANK; j++) {
                for (int k = j + 1; k + 1 < MAX_CARD_RANK; k++) {
                    for (int l = k + 1; l < MAX_CARD_RANK; l++) {
                        unmatched_count += (tmp_multi[i] * tmp_multi[j] *
                                            tmp_multi[k] * tmp_multi[l]);
                        unique_choice_count++;
                    }
                }
            }
        }
        std::cout << "unique_choice_count: " << unique_choice_count
                  << ", unmatched_count: " << unmatched_count << std::endl;
        if (total_count < unmatched_count) {
            std::cout << "nextMultiProbabilityCalc failed for unmatched_count "
                         "incorrect:"
                      << unmatched_count << std::endl;
            return 0.0;
        }
        return static_cast<double>(total_count - unmatched_count) /
               static_cast<double>(total_count);
    }

   private:
    CardsBase current_cards_;
    int index_;
    std::string id_;
    double croupier_pair_probability_;
    double player_pair_probability_;
};
#endif  // CALCULATOR_H
