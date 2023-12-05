#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum SymbType { Undefined, Terminal, Nonterminal };
class Grammar {
 public:
  Grammar() = default;

  Grammar(std::unordered_set<char> terminals,
          std::unordered_set<char> nonterminals,
          std::unordered_map<char, std::list<std::string>> rules,
          char start_terminal);

  SymbType GetSymbType(char symb);
  std::unordered_set<char> terminals;
  std::unordered_set<char> nonterminals;
  std::unordered_map<char, std::list<std::string>> rules;
  // по нетерминалу получаем все правила, где он стоит слева.
  char start_terminal;
};

