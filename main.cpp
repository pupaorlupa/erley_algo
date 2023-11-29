#include "parser.h"
#include "earley.h"

int main() {
  char name[20] = "../grammar.txt";
  std::shared_ptr<FileReader> reader = std::make_shared<FileReader>(name);

  Grammar gr;
  EarleyParser parser;
  parser.Parse(reader, gr);
  Earley checker(gr);
  int iterations;
  std::cin >> iterations;
  std::string to_check;
  for (int i = 0; i < iterations; ++i) {
    std::cin >> to_check;
    std::cout << (checker.Predict(to_check) ? "YES" : "NO") << "\n";
  }
}

/* int main() { */
/*   Grammar gr; */
/*   std::shared_ptr<ConsoleReader> reader = std::make_shared<ConsoleReader>(); */
/*   EarleyParser parser; */
/*   parser.Parse(reader, gr); */
/*   Earley solver(gr); */
/*   std::cout << (solver.Predict("[][][][][]]") ? "YES" : "NO"); */
/*   std::cout << (solver.Predict("[[][][][][]]") ? "YES" : "NO"); */
/* } */
