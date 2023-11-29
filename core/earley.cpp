#include "earley.h"

template <typename T>
void hash_combine(std::size_t& seed, T const& key) {
  std::hash<T> hasher;
  seed ^= hasher(key) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

bool Earley::Situation::operator==(const Situation& other) const {
  return (left_side == other.left_side && before == other.before &&
          right_side == other.right_side && position == other.position);
}

Earley::Situation::Situation(char left_side, int before, int position, std::string right_side)
        : left_side(left_side),
          before(before),
          position(position),
          right_side(right_side) {}

Earley::Earley(Grammar grammar) : grammar(grammar) {}

bool Earley::Predict(std::string word) {
  Clear();
  for (auto ch : word) {
    if (grammar.GetSymbType(ch) != SymbType::Terminal) {
      return false;
    }
  }
  grammar.nonterminals.insert(kStartNonterm);
  layers.resize(word.size() + 1);
  layers_by_nonterm.resize(word.size() + 1);
  std::string starting_string;
  starting_string += grammar.start_terminal;
  Situation start_sit = Situation(kStartNonterm, 0, 0, starting_string);
  AddSituation(0, start_sit);
  bool changed;
  for (int i = 0; i <= word.size(); ++i) {
    Scan(i, word);

    temp_for_predict.clear();
    temp_for_complete.clear();
    for (auto ch : grammar.nonterminals) {
      for (auto sit : layers_by_nonterm[i][ch]) {
        temp_for_predict.push_back(sit);
      }
    }
    predict_cursor = 0;
    for (auto sit : layers_by_nonterm[i][kLastSymb]) {
      temp_for_complete.push_back(sit);
    };
    comp_cursor = 0;

    do {
      changed = false;
      Complete(i, word, changed);
      PredictInside(i, word, changed);
    } while (changed);
  }
  Situation sit(kStartNonterm, 0, 1, starting_string);
  return layers[word.size()].count(sit) > 0;
}

void Earley::Clear() {
  layers.clear();
  layers_by_nonterm.clear();
}

void Earley::Complete(int layer, const std::string& word, bool& changed) {
  for (; comp_cursor < temp_for_complete.size(); ++comp_cursor) {
    auto sit = temp_for_complete[comp_cursor];
    int before = sit.before;
    for (auto old_sit : layers_by_nonterm[before][sit.left_side]) {
      Situation new_sit(old_sit.left_side, old_sit.before,
                        old_sit.position + 1, old_sit.right_side);
      changed |= AddSituation(layer, new_sit);
    }
  }
}

void Earley::PredictInside(int layer, std::string& word, bool& changed) {
  for (; predict_cursor != temp_for_predict.size(); ++predict_cursor) {
    Situation sit = temp_for_predict[predict_cursor];
    char next_nonterm = sit.right_side[sit.position];
    for (auto right_size : grammar.rules[next_nonterm]) {
      // left side, before, position, right side
      Situation new_sit(next_nonterm, layer, 0, right_size);
      changed |= AddSituation(layer, new_sit);
    }
  }
}

bool Earley::AddSituation(int layer, Situation& sit) {
  if (layers[layer].count(sit) == 0) {
    layers[layer].insert(sit);
    layers_by_nonterm[layer][FindNextSymb(sit)].push_back(sit);
    if (grammar.GetSymbType(FindNextSymb(sit)) == SymbType::Nonterminal) {
      temp_for_predict.push_back(sit);
    }
    if (FindNextSymb(sit) == kLastSymb) {
      temp_for_complete.push_back(sit);
    }
    return true;
  }
  return false;
}

void Earley::Scan(int layer, std::string& word) {
  if (layer == 0) {
    return;
  }
  Situation new_one;
  for (auto sit : layers_by_nonterm[layer - 1][kSpecSymb]) {
    new_one = sit;
    new_one.position += 1;
    if (sit.right_side[sit.position] == word[layer - 1]) {
      AddSituation(layer, new_one);
    }
  }
}

char Earley::FindNextSymb(const Situation& sit) {
  if (sit.position == sit.right_side.size()) {
    return kLastSymb;
  }
  if (grammar.GetSymbType(sit.right_side[sit.position]) ==
      SymbType::Terminal) {
    return kSpecSymb;
  }
  return sit.right_side[sit.position];
}

std::size_t Earley::sit_hasher::operator()(const Situation& sit) const {
  std::size_t seed1(0);
  ::hash_combine(seed1, sit.left_side);
  ::hash_combine(seed1, sit.before);
  ::hash_combine(seed1, sit.position);
  ::hash_combine(seed1, sit.right_side);

  std::size_t seed2(0);
  ::hash_combine(seed2, sit.position);
  ::hash_combine(seed2, sit.right_side);
  ::hash_combine(seed2, sit.left_side);
  ::hash_combine(seed2, sit.before);

  std::size_t seed3(0);
  ::hash_combine(seed3, sit.right_side);
  ::hash_combine(seed3, sit.position);
  ::hash_combine(seed3, sit.left_side);
  ::hash_combine(seed3, sit.before);

  return std::min(seed3, std::min(seed1, seed2));
}
