#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <algorithm>
#include <iostream>
#include <vector>

#define MAX_CARD_RANK 13
#define MIN_POP_CARDS_NUM 4
#define MAX_POP_CARDS_NUM 6

class CardsBase {
   public:
    CardsBase(uint8_t suit_num) {
        suit_num_ = suit_num;
        reset();
    };
    CardsBase(){};
    ~CardsBase(){};

   public:
    void reset() {
        cards_.clear();
        for (uint8_t i = 0; i < suit_num_; i++) {
            for (uint8_t j = 1; j <= MAX_CARD_RANK; j++) {
                addCard(j);
            }
        }
        total_cards_num_ = cards_.size();
        remain_cards_num_ = total_cards_num_;
        std::cout << "Cards reset successfully, total_cards_num_:"
                  << total_cards_num_
                  << "remain_cards_num_:" << remain_cards_num_ << std::endl;
    };
    void addCard(uint8_t card) {
        cards_.push_back(card);
        remain_cards_num_++;
    };
    void addCard(std::vector<uint8_t> cards) {
        cards_.insert(cards_.end(), cards.begin(), cards.end());
        remain_cards_num_ += cards.size();
    };
    bool removeCard(uint8_t card) {
        std::vector<uint8_t>::iterator it =
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
    uint8_t getSuitNum() const { return suit_num_; };
    void setSuitNum(uint8_t suit_num) { suit_num_ = suit_num; };
    uint16_t getTotalCardsNum() const { return total_cards_num_; };
    uint16_t getRemainCardsNum() const { return remain_cards_num_; };
    std::vector<uint8_t> getCards() const { return cards_; };
    void setCards(const std::vector<uint8_t>& cards) { cards_ = cards; }

   private:
    uint8_t suit_num_;
    uint16_t total_cards_num_;
    uint16_t remain_cards_num_;
    std::vector<uint8_t> cards_;
};

class CardsEventCalculator {
   public:
    CardsEventCalculator(uint8_t suit_num, uint8_t index, std::string id) {
        reset(suit_num);
        index_ = index;
        id_ = id;
    }
    ~CardsEventCalculator(){};

   public:
    void setCardsIndex(uint8_t index) { index_ = index; }
    uint8_t getCardsIndex() const { return index_; }
    void setCardsId(const std::string& id) { id_ = id; }
    std::string getCardsId() const { return id_; }

   public:
    void reset(uint8_t suit_num) {
        current_cards_.setSuitNum(suit_num);
        current_cards_.reset();
    }
    double nextPairProbabilityCalc() {
        // 组合公式C(n,m) = n!/(m!(n-m)!)
        uint8_t kPickCardsNum = 4;
        uint16_t total_count =
            factorial(current_cards_.getRemainCardsNum()) /
            (factorial(kPickCardsNum) *
             factorial(current_cards_.getRemainCardsNum() - kPickCardsNum));

        std::vector<uint8_t>::iterator it = current_cards_.getCards().begin();

        uint32_t pair_count = 0;
        for (auto i = it; i != current_cards_.getCards().end(); i) {
            for (auto j = i + 1; j != current_cards_.getCards().end(); j++) {
                for (auto k = j + 1; k != current_cards_.getCards().end();
                     k++) {
                    for (auto l = k + 1; l != current_cards_.getCards().end();
                         l++) {
                        if (*i != *j && *i != *k && *i != *l && *j != *k &&
                            *j != *l && *k != *l) {
                            continue;
                        } else {
                            pair_count++;
                        }
                    }
                }
            }
        }
        return static_cast<double>(pair_count) /
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

   private:
    CardsBase current_cards_;
    uint8_t index_;
    std::string id_;
};

#endif  // CALCULATOR_H
