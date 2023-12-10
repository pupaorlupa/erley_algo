#include <iostream>
#include <memory>

#include "LR1.h"
#include "parser.h"

int main() {
  char name[1024] = GRAMMAR_PATH;
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

/* int main() { */
/*   Grammar gr; */
/*   std::shared_ptr<ConsoleReader> reader = std::make_shared<ConsoleReader>(); */
/*   Parser parser; */
/*   parser.Parse(reader, gr); */
/*   LR1 checker(gr); */
/*   int iterations; */
/*   std::cin >> iterations; */
/*   std::string to_check; */
/*   for (int i = 0; i < iterations; ++i) { */
/*     std::cin >> to_check(); */
/*     std::cout << (checker.Predict(to_check) ? "YES" : "NO") << "\n"; */
/*   } */
/* } */
