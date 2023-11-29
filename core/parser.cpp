#include "parser.h"

EarleyParser::EarleyParser() {
  for (auto ch : kPossibleTerminals) {
    possible_terminals.insert(ch);
  }
}

void EarleyParser::Parse(std::shared_ptr<Reader> reader, Grammar& grammar) {
  reader->SetUpUsage();

  std::string line = reader->GetLine();
  int cursor = 0;
  int nont_size = ReadInt(line, cursor);
  int t_size = ReadInt(line, cursor);
  int rules_size = ReadInt(line, cursor);
  GetNonterminals(nont_size, reader);
  GetTerminals(t_size, reader);
  GetRules(rules_size, reader);
  GetStartingNonterm(reader);

  reader->FinishUsage();

  grammar = Grammar(terminals, nonterminals, rules, start_terminal);
  Clear();
}

void EarleyParser::GetNonterminals(int nont_size, std::shared_ptr<Reader> reader) {
  std::string line = reader->GetLine();
  int cursor = 0;
  std::string alphabet;
  alphabet = ReadString(line, cursor);
  VerifyLineEnd(line, cursor);
  int minim = kMinimNonterm;
  int maxim = kMaximNonterm;
  for (auto ch : alphabet) {
    if (static_cast<int>(ch) < minim || static_cast<int>(ch) > maxim) {
      throw ParsingErrors::INAPPROPRIATE_NONTERMINAL;
    }
    nonterminals.insert(ch);
  }
  if (static_cast<int>(nonterminals.size()) != nont_size) {
    throw ParsingErrors::WRONG_SIZE;
  }
}

void EarleyParser::GetTerminals(int t_size, std::shared_ptr<Reader> reader) {
  std::string line = reader->GetLine();
  int cursor = 0;
  std::string alphabet;
  alphabet = ReadString(line, cursor);
  VerifyLineEnd(line, cursor);
  for (auto ch : alphabet) {
    if (possible_terminals.find(ch) == possible_terminals.end()) {
      throw ParsingErrors::INAPPROPRIATE_TERMINAL;
    }
    terminals.insert(ch);
  }
  if (static_cast<int>(terminals.size()) != t_size) {
    throw ParsingErrors::WRONG_SIZE;
  }
}

void EarleyParser::GetRules(int rules_size, std::shared_ptr<Reader> reader) {
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
        throw "1";
      }
      arrow = ReadString(line, cursor);
      if (arrow != "->") {
        throw "1";
      }
      try {
        expression = ReadString(line, cursor);
      } catch (...) {
        expression = "";
      }
      VerifyLineEnd(line, cursor);
      rules[nonterminal[0]].push_back(expression);
    } catch (...) {
      char buf[kBuffSize];
      sprintf(buf, ParsingErrors::WRONG_RULE, i + 1);
      throw std::runtime_error(std::string(buf));
    }
  }
}

void EarleyParser::GetStartingNonterm(std::shared_ptr<Reader> reader) {
  std::string line = reader->GetLine();
  int cursor = 0;
  std::string value = ReadString(line, cursor);
  VerifyLineEnd(line, cursor);
  if (value.size() != 1 ||
      nonterminals.find(value[0]) == nonterminals.end()) {
    throw ParsingErrors::PARSING_ERROR;
  }
  start_terminal = value[0];
}

void EarleyParser::Clear() {
  terminals.clear();
  nonterminals.clear();
  rules.clear();
}

std::string EarleyParser::ReadString(std::string& given, int& cursor) {
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
    throw ParsingErrors::PARSING_ERROR;
  }
  return answer;
}

void EarleyParser::VerifyLineEnd(std::string& given, int cursor) {
  for (; cursor < given.size(); ++cursor) {
    if (given[cursor] != ' ' && given[cursor] != '\n') {
      throw ParsingErrors::PARSING_ERROR;
    }
  }
}

int EarleyParser::ReadInt(std::string& given, int& cursor) {
  try {
    return stoi(ReadString(given, cursor));
  } catch (...) {
    throw ParsingErrors::PARSING_ERROR;
  }
}
