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
