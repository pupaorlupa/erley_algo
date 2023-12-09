#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "grammar.h"

const char kStartNonterm = '@';
const char kLastSymb = '$';

class LR1 {
 private:
  enum ActionTypes { Shift, Reduce, Accept };

  struct Act {
    ActionTypes type;
    int uni;  // в случае shift это номер состояния
              // а в случае reduce это длина правой части правила

    char res;  // обозначает нетерминал из которого вывелась последовательность
               // в случае reduce

    Act(ActionTypes type, int uni, char res = 'a')
        : type(type), uni(uni), res(res) {}
    Act() = default;
    bool operator==(const Act& other) const {
      return (type == other.type && uni == other.uni && res == other.res);
    }
    bool operator!=(const Act& other) const { return !(*this == other); }
  };

  struct Situation {
    Situation() = default;
    Situation(char left_side, std::string right_side, int position,
              char preview)
        : left_side(left_side),
          right_side(right_side),
          position(position),
          preview(preview) {}

    bool operator==(const Situation& other) const {
      return left_side == other.left_side && right_side == other.right_side &&
             position == other.position && preview == other.preview;
    }

    bool operator<(const Situation& other) const {
      return left_side < other.left_side ||
             (left_side == other.left_side && right_side < other.right_side) ||
             (left_side == other.left_side && right_side == other.right_side &&
              position < other.position) ||
             (left_side == other.left_side && right_side == other.right_side &&
              position == other.position && preview < other.preview);
    }

    char left_side;
    std::string right_side;
    int position;
    char preview;
  };
  using Condition = std::unordered_map<char, std::set<Situation>>;

 public:
  LR1(Grammar grammar) : grammar(grammar) {
    grammar.DestroyUseless();
    std::string start;
    grammar.nonterminals.insert(kStartNonterm);
    start += grammar.start_terminal;
    start += kLastSymb;
    grammar.rules[kStartNonterm].push_back(start);
    BuildFirst();
    BuildConditions();
    BuildTable();
  }

  bool Predict(std::string word) {
    word.push_back(kLastSymb);
    std::stack<int> cur_conds;
    cur_conds.push(0);
    size_t ind = 0;
    while (ind < word.size()) {
      int cur_cond = cur_conds.top();
      if (table[cur_cond].find(word[ind]) != table[cur_cond].end()) {
        Act cur_act = table[cur_cond][word[ind]];
        if (cur_act.type == ActionTypes::Accept) {
          return true;
        } else if (cur_act.type == ActionTypes::Shift) {
          cur_conds.push(word[ind]);
          cur_conds.push(cur_act.uni);
          ++ind;
        } else if (cur_act.type == ActionTypes::Reduce) {
          for (int i = 0; i < cur_act.uni; ++i) {
            cur_conds.pop();
            cur_conds.pop();
          }
          cur_cond = cur_conds.top();
          if (table[cur_cond].find(cur_act.res) == table[cur_cond].end()) {
            return false;
          } else {
            cur_conds.push(cur_act.res);
            cur_conds.push(table[cur_cond][cur_act.res].uni);
          }
        }
      } else {
        return false;
      }
    }
    return false;
  }

 private:
  void TableAdd(int layer, Act possible_act, char next) {
    if (table[layer].find(next) != table[layer].end()) {
      if (table[layer][next] != possible_act) {
        throw std::runtime_error("bad grammar");
      }
      return;
    }
    table[layer][next] = possible_act;
  }

  void BuildTable() {
    std::string init_str;
    init_str.push_back(grammar.start_terminal);
    init_str.push_back(kLastSymb);
    Situation start_sit{kStartNonterm, init_str, 1, kLastSymb};

    for (int i = 0; i < table.size(); ++i) {
      for (auto cond : conditions[i]) {
        char next_symb = cond.first;
        if (grammar.GetSymbType(next_symb) == SymbType::Nonterminal) {
          Act possible_act{ActionTypes::Shift, goto_[i][next_symb]};
          TableAdd(i, possible_act, next_symb);
        } else {
          for (auto& sit : cond.second) {
            if (next_symb == kLastSymb) {
              if (sit == start_sit) {
                TableAdd(i, Act(ActionTypes::Accept, 0), sit.preview);
              } else {
                TableAdd(i,
                         Act(ActionTypes::Reduce, sit.right_side.size() - 1,
                             sit.left_side),
                         sit.preview);
              }
            } else {
              TableAdd(i, Act(ActionTypes::Shift, goto_[i][next_symb]),
                       next_symb);
            }
          }
        }
      }
    }
  }

  void BuildFirst() {
    for (auto nonter : grammar.nonterminals) {
      first[nonter] = std::set<char>();
    }
    bool changed = true;
    while (changed) {
      changed = false;
      for (auto nonter : grammar.nonterminals) {
        std::set<char>& first_nont = first[nonter];
        for (auto right_side : grammar.rules[nonter]) {
          if (grammar.GetSymbType(right_side[0]) == SymbType::Terminal ||
              right_side[0] == kLastSymb) {
            auto it = first_nont.find(right_side[0]);
            if (it == first_nont.end()) {
              changed = true;
              first_nont.insert(right_side[0]);
            }
          } else {
            bool next = true;
            size_t index = 0;
            while (next && index < right_side.size()) {
              next = false;
              std::set<char>& first_next = first[right_side[index]];
              for (auto it = first_next.begin(); it != first_next.end(); ++it) {
                if (*it != kLastSymb) {
                  auto iter2 = first_nont.find(*it);
                  if (iter2 == first_nont.end()) {
                    changed = true;
                    first_nont.insert(*it);
                  }
                }
              }
              auto iter3 = first_next.find(kLastSymb);
              if (iter3 != first_next.end()) {
                next = true;
                ++index;
              }
            }
          }
        }
      }
    }
  }

  std::set<char> find_first(std::string given) {
    std::set<char> ans;
    for (auto ch : given) {
      if (grammar.GetSymbType(ch) == SymbType::Nonterminal) {
        ans.insert(first[ch].begin(), first[ch].end());
        if (first[ch].find(kLastSymb) != first[ch].end()) {
          break;
        }
      } else {
        if (ch != kLastSymb) {
          ans.insert(ch);
          break;
        }
      }
    }
    if (ans.empty()) {
      ans.insert(kLastSymb);
    }
    return ans;
  }

  void Closure(Condition& cond) {
    bool changed = true;
    while (changed) {
      changed = false;
      for (auto& sit_by_nont : cond) {
        if (grammar.GetSymbType(sit_by_nont.first) == SymbType::Nonterminal) {
          for (auto& sit : sit_by_nont.second) {
            std::string suff =
                sit.right_side.substr(sit.position + 1,
                                      sit.right_side.size() - 1) +
                sit.preview;
            std::set<char> cur_first = find_first(suff);
            for (auto& right : grammar.rules[sit_by_nont.first]) {
              for (auto next : cur_first) {
                Situation new_sit(sit_by_nont.first, right, 0, next);
                char cur_symb = new_sit.right_side[new_sit.position];
                if (cond.find(cur_symb) == cond.end() ||
                    cond[cur_symb].find(new_sit) == cond[cur_symb].end()) {
                  changed = true;
                  cond[cur_symb].insert(new_sit);
                }
              }
            }
          }
        }
      }
    }
  }

  int FindCondIndex(const Condition& cond) {  // -1, если нет
                                              // иначе индекс вхождения
    for (int i = 0; i < conditions.size(); ++i) {
      if (cond == conditions[i]) {
        return i;
      }
    }
    return -1;
  }

  void BuildNextConditions(int old_ind) {
    auto old_cond = conditions[old_ind];
    for (auto& sit_by_nont : old_cond) {
      if (sit_by_nont.first != kLastSymb) {
        Condition new_cond;

        for (auto& old_sit : sit_by_nont.second) {
          Situation new_sit(old_sit);
          ++new_sit.position;
          new_cond[new_sit.right_side[new_sit.position]].insert(new_sit);
        }
        Closure(new_cond);
        int new_cond_index = FindCondIndex(new_cond);
        if (new_cond_index > -1) {
          goto_[old_ind][sit_by_nont.first] = new_cond_index;
        } else {
          conditions.push_back(std::move(new_cond));
          goto_[old_ind][sit_by_nont.first] = conditions.size() - 1;
          goto_.emplace_back();
          table.emplace_back();
        }
      }
    }
  }

  void BuildConditions() {
    conditions.resize(1);
    table.resize(1);
    goto_.resize(1);
    std::string init_str;
    init_str.push_back(grammar.start_terminal);
    init_str.push_back(kLastSymb);
    conditions[0][grammar.start_terminal].emplace(kStartNonterm, init_str, 0,
                                                  kLastSymb);
    Closure(conditions[0]);
    size_t ind = 0;
    while (ind < conditions.size()) {
      BuildNextConditions(ind);
      ++ind;
    }
  }

  Grammar grammar;
  std::vector<std::unordered_map<char, int>> goto_;
  std::vector<Condition> conditions;
  std::unordered_map<char, std::set<char>> first;
  std::vector<std::unordered_map<char, Act>> table;
};

int main() {
  char name[20] = "../grammar.txt";
  std::shared_ptr<FileReader> reader = std::make_shared<FileReader>(name);

  Grammar gr;
  Parser parser;
  parser.Parse(reader, gr);
  LR1 checker(gr);
  int iterations;
  std::cin >> iterations;
  std::string to_check;
  for (int i = 0; i < iterations; ++i) {
    std::cin >> to_check;
    std::cout << (checker.Predict(to_check) ? "YES" : "NO") << "\n";
  }
}
