#include <cstdio>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "reader.cpp"

enum SymbType { Undefined, Terminal, Nonterminal };
namespace Errors {
std::runtime_error PARSING_ERROR("inappropriate parsing format");
std::runtime_error INAPPROPRIATE_NONTERMINAL("inappropriate nonterminal value");
std::runtime_error INAPPROPRIATE_TERMINAL("inappropriate terminal value");
std::runtime_error WRONG_SIZE("expected size does not match real size");
const char WRONG_RULE[] = "rule number %i is wrong";
}  // namespace Errors

const std::string kPossibleTerminals(
    "01234567890()abcdefghijklmnopqrstuvwxyz+-=/*");
const int kMinimNonterm = 'A';
const int kMaximNonterm = 'Z';
const int kBuffSize = 1024;

class Grammar {
 public:

 private:
  SymbType CheckSymb(char symb) {
    if (nonterminals.find(symb) != nonterminals.end()) {
      return SymbType::Nonterminal;
    } else if (terminals.find(symb) != terminals.end()) {
      return SymbType::Terminal;
    }
    return SymbType::Undefined;
  }

  std::unordered_set<char> nonterminals;
  std::unordered_set<char> terminals;
};

class EarleyParser {
 public:
  EarleyParser() {
    for (auto ch : kPossibleTerminals) {
      possible_terminals.insert(ch);
    }
  }

  void Parse(std::shared_ptr<Reader> reader) {
    reader->SetUpUsage();

    Clear();
    std::string line = reader->GetLine();
    int cursor = 0;
    int nont_size = ReadInt(line, cursor);
    int t_size = ReadInt(line, cursor);
    int rules_size = ReadInt(line, cursor);
    GetNonterminals(nont_size, reader);
    GetTerminals(nont_size, reader);
    GetRules(rules_size, reader);

    reader->FinishUsage();
  }

  /* Grammar GetGrammar() { */
  /*   return Grammar(nonterminals, terminals, rules, start); */
  /* } */

 private:
  void GetNonterminals(int nont_size, std::shared_ptr<Reader> reader) {
    std::string line = reader->GetLine();
    int cursor = 0;
    std::string alphabet;
    alphabet = ReadString(line, cursor);
    int minim = kMinimNonterm;
    int maxim = kMaximNonterm;
    for (auto ch : alphabet) {
      if (static_cast<int>(ch) < minim || static_cast<int>(ch) > maxim) {
        throw Errors::INAPPROPRIATE_NONTERMINAL;
      }
      nonterminals.insert(ch);
    }
    if (static_cast<int>(nonterminals.size()) != nont_size) {
      throw Errors::WRONG_SIZE;
    }
  }

  void GetTerminals(int t_size, std::shared_ptr<Reader> reader) {
    std::string line = reader->GetLine();
    int cursor = 0;
    std::string alphabet;
    alphabet = ReadString(line, cursor);
    for (auto ch : alphabet) {
      if (possible_terminals.find(ch) == possible_terminals.end()) {
        throw Errors::INAPPROPRIATE_TERMINAL;
      }
      terminals.insert(ch);
    }
    if (static_cast<int>(terminals.size()) != t_size) {
      throw Errors::WRONG_SIZE;
    }
  }

  void GetRules(int rules_size, std::shared_ptr<Reader> reader) {
    std::string nonterminal;
    std::string arrow;
    std::string expression;
    for (int i = 0; i < rules_size; ++i) {
      try {
        std::string line = reader->GetLine();
        int cursor = 0;
        nonterminal = ReadString(line, cursor);
        if (nonterminal.size() != 1 ||
            nonterminals.find(nonterminal[0]) == nonterminals.end()) {
          throw;
        }
        arrow = ReadString(line, cursor);
        if (arrow != "->") {
          throw;
        }
        try {
          expression = ReadString(line, cursor);
        } catch (...) {
          expression = "";
        }
        rules[nonterminal[0]].insert(expression);
      } catch (...) {
        char buf[kBuffSize];
        sprintf(buf, Errors::WRONG_RULE, i);
        throw std::runtime_error(std::string(buf));
      }
    }
  }

  void Clear() {
    terminals.clear();
    nonterminals.clear();
    rules.clear();
  }

  std::string ReadString(std::string& given, int& cursor) {
    bool begin = false;
    std::string answer;
    for (; cursor < given.size(); ++cursor) {
      if (given[cursor] != ' ' && given[cursor] != '\n') {
        begin = true;
        answer.push_back(given[cursor]);
      } else if (begin) {
        return answer;
      }
    }
    if (!begin) {
      throw Errors::PARSING_ERROR;
    }
    return answer;
  }

  void VerifyLineEnd(std::string& given, int cursor) {
    for (; cursor < given.size(); ++cursor) {
      if (given[cursor] != ' ' && given[cursor != '\n']) {
        throw Errors::PARSING_ERROR;
      }
    }
  }

  int ReadInt(std::string& given, int& cursor) {
    try {
      return stoi(ReadString(given, cursor));
    } catch (...) {
      throw Errors::PARSING_ERROR;
    }
  }

  std::unordered_set<char> terminals;
  std::unordered_set<char> nonterminals;
  std::unordered_map<char, std::unordered_set<std::string>> rules;
  std::unordered_set<char> possible_terminals;
  char start;
};

int main() {
  char name[20] = "text.txt";
  std::shared_ptr<FileReader> reader = std::make_shared<FileReader>(name);
  
  EarleyParser parser;
  parser.Parse(reader);
}
