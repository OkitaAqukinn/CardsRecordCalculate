
#ifndef CALCULATOR_H
#define CALCULATOR_H
#include <algorithm>
#include <iostream>
#include <vector>
#define MAX_CARD_RANK (13)
#define MAX_CARD_PATTERN (4)
#define MIN_POP_CARDS_NUM (4)
#define MAX_POP_CARDS_NUM (6)
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
    void addCard(std::vector<int> cards) {
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
    void setCards(const std::vector<int>& cards) { cards_ = cards; }

   private:
    int suit_num_;
    int total_cards_num_;
    int remain_cards_num_;
    std::vector<int> cards_;
};
class CardsEventCalculator {
   public:
    CardsEventCalculator(int suit_num, int index, std::string id) {
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
    double nextPairProbabilityCalc() {
        // 组合公式C(n,m) = n!/(m!(n-m)!)
        uint64_t tmp_multiplicate = combinator(
            current_cards_.getRemainCardsNum(),
            (current_cards_.getRemainCardsNum() - MIN_POP_CARDS_NUM + 1));
        uint64_t total_count = tmp_multiplicate / factorial(MIN_POP_CARDS_NUM);
        std::cout
            << "nextPairProbabilityCalc current total combination counts: "
            << total_count << std::endl;
        if (total_count <= 0) {
            std::cout << "nextPairProbabilityCalc failed: "
                      << current_cards_.getRemainCardsNum() << std::endl;
            return 0.0;
        }

        int total_choice_count = MAX_CARD_RANK;
        int tmp_multi[MAX_CARD_RANK] = {0};
        for (uint32_t i = 1; i <= MAX_CARD_RANK; i++) {
            tmp_multi[i - 1] = countDuplicates(current_cards_.getCards(), i);
            if (tmp_multi[i - 1] <= 0) total_choice_count--;
        }
        if (total_choice_count < MIN_POP_CARDS_NUM) return 0.0;
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
                  << " unmatched_count: " << unmatched_count << std::endl;
        if (total_count < unmatched_count) {
            std::cout << "nextPairProbabilityCalc failed for unmatched_count "
                         "incorrect:"
                      << unmatched_count << std::endl;
            return 0.0;
        }
        return static_cast<double>(total_count - unmatched_count) /
               static_cast<double>(total_count);
    }

   private:
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

    int countDuplicates(const std::vector<int>& vec, int target) {
        int count = 0;
        for (int num : vec) {
            if (num == target) {
                count++;
            }
        }
        return count;
    }

   private:
    CardsBase current_cards_;
    int index_;
    std::string id_;
};
#endif  // CALCULATOR_H
