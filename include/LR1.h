#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "grammar.h"

const char kStartNonterm = '@';
const char kLastSymb = '$';

class LR1 {
 public:
  LR1(Grammar grammar);

  bool Predict(std::string word);

 private:

  enum ActionTypes { Shift, Reduce, Accept };

  struct Act {
    ActionTypes type;
    int uni;  // в случае shift это номер состояния
              // а в случае reduce это длина правой части правила

    char res;  // обозначает нетерминал из которого вывелась последовательность
               // в случае reduce

    Act(ActionTypes type, int uni, char res = 'a');
    Act() = default;
    bool operator==(const Act& other) const;
    bool operator!=(const Act& other) const;
  };

  struct Situation {
    Situation() = default;
    Situation(char left_side, std::string right_side, int position,
              char preview);

    bool operator==(const Situation& other) const;

    bool operator<(const Situation& other) const;

    char left_side;
    std::string right_side;
    int position;
    char preview;
  };

  using Condition = std::unordered_map<char, std::set<Situation>>;

  void TableAdd(int layer, Act possible_act, char next);
  void BuildTable();
  void BuildFirst();
  std::set<char> find_first(std::string given);
  void Closure(Condition& cond);
  int FindCondIndex(const Condition& cond); // -1, если нет
                                            // иначе индекс вхождения
  void BuildNextConditions(int old_ind);
  void BuildConditions();

  Grammar grammar;
  std::vector<std::unordered_map<char, int>> goto_;
  std::vector<Condition> conditions;
  std::unordered_map<char, std::set<char>> first;
  std::vector<std::unordered_map<char, Act>> table;
};

