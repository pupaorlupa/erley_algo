#include <memory>

#include "grammar.h"
#include "reader.h"

namespace ParsingErrors {
  const std::runtime_error PARSING_ERROR("inappropriate parsing format");
  const std::runtime_error INAPPROPRIATE_NONTERMINAL("inappropriate nonterminal value");
  const std::runtime_error INAPPROPRIATE_TERMINAL("inappropriate terminal value");
  const std::runtime_error WRONG_SIZE("expected size does not match real size");
  const char WRONG_RULE[] = "rule number %i is wrong";
}  // namespace ParsingErrors

const std::string kPossibleTerminals(
    "01234567890()[]{}abcdefghijklmnopqrstuvwxyz+-=/*");
const int kMinimNonterm = 'A';
const int kMaximNonterm = 'Z';
const int kBuffSize = 1024;


class EarleyParser {
 public:
  EarleyParser();
  void Parse(std::shared_ptr<Reader> reader, Grammar& grammar);

 private:
  void GetNonterminals(int nont_size, std::shared_ptr<Reader> reader);
  void GetTerminals(int t_size, std::shared_ptr<Reader> reader);
  void GetRules(int rules_size, std::shared_ptr<Reader> reader);
  void GetStartingNonterm(std::shared_ptr<Reader> reader);
  void Clear();
  std::string ReadString(std::string& given, int& cursor);
  void VerifyLineEnd(std::string& given, int cursor);
  int ReadInt(std::string& given, int& cursor);

  std::unordered_set<char> terminals;
  std::unordered_set<char> nonterminals;
  std::unordered_map<char, std::list<std::string>> rules;
  std::unordered_set<char> possible_terminals;
  char start_terminal;
};

