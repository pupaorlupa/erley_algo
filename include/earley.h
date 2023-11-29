#include "grammar.h"

const char kStartNonterm = '$';
const char kSpecSymb = '@';
const char kLastSymb = '|';

template <typename T>
void hash_combine(std::size_t& seed, T const& key);

class Earley {
 private:
  struct Situation {
    Situation() = default;
    bool operator==(const Situation& other) const;
    Situation(char left_side, int before, int position, std::string right_side);

    char left_side;
    int before;
    int position;
    std::string right_side;
  };

 public:
  Earley(Grammar grammar);

  bool Predict(std::string word);

 private:
  void Clear();
  void Complete(int layer, const std::string& word, bool& changed);
  void PredictInside(int layer, std::string& word, bool& changed);
  bool AddSituation(int layer, Situation& sit);
  void Scan(int layer, std::string& word);
  char FindNextSymb(const Situation& sit);

  struct sit_hasher {
    std::size_t operator()(const Situation& sit) const;
  };

  Grammar grammar;
  std::vector<std::unordered_set<Situation, sit_hasher>> layers;
  // все ситуации на слое
  std::vector<std::unordered_map<char, std::list<Situation>>> layers_by_nonterm;
  // по нетерминалу получаем все ситуации на слое, где указатель стоит перед
  // этим нетерминалом
  // либо же по спец. символу получаем все ситуации, где указатель стоит перед
  // каким-то терминалом
  std::vector<Situation> temp_for_predict;
  size_t predict_cursor;
  std::vector<Situation> temp_for_complete;
  size_t comp_cursor;
  // будем перезаписывать этот vector на каждом слое, чтобы предикт не ходил по
  // одним и тем же ситуациям несколько раз в случае, если что-то поменяется в
  // результате комплита или предикта
};
