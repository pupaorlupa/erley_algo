#include "grammar.h"

Grammar::Grammar(std::unordered_set<char> terminals,
                 std::unordered_set<char> nonterminals,
                 std::unordered_map<char, std::list<std::string>> rules,
                 char start_terminal)
    : terminals(terminals),
      nonterminals(nonterminals),
      rules(rules),
      start_terminal(start_terminal) {}

SymbType Grammar::GetSymbType(char symb) {
  if (nonterminals.find(symb) != nonterminals.end()) {
    return SymbType::Nonterminal;
  } else if (terminals.find(symb) != terminals.end()) {
    return SymbType::Terminal;
  }
  return SymbType::Undefined;
}

void Grammar::DestroyUseless() {
  std::unordered_set<char> useful;
  std::unordered_set<char> useful_nont;
  for (auto ch : terminals) {
    useful.insert(ch);
  }
  size_t last_size;
  do {
    last_size = useful.size();
    for (auto& rules_by_nont : rules) {
      if (useful.count(rules_by_nont.first) == 0) {
        for (auto& right_side : rules_by_nont.second) {
          bool flag = true;
          for (auto ch : right_side) {
            if (useful.count(ch) == 0) {
              flag = false;
              break;
            }
          }
          if (flag) {
            useful.insert(rules_by_nont.first);
            useful_nont.insert(rules_by_nont.first);
          }
        }
      }
    }
  } while (last_size < useful.size());
  nonterminals = useful_nont;
}
