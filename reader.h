#include <cstdio>
#include <iostream>

#pragma once

class Reader {
 public:
  virtual void SetUpUsage() = 0;
  virtual std::string GetLine() = 0;
  virtual void FinishUsage() = 0;
};

class ConsoleReader: public Reader {
 public:
  ConsoleReader() = default;
  virtual void SetUpUsage() {}
  virtual void FinishUsage() {}
  virtual std::string GetLine();
};

class FileReader: public Reader {
 public:
  FileReader(char* filename): filename(filename) {}
  virtual void SetUpUsage();
  virtual std::string GetLine();
  virtual void FinishUsage();
 private:
  FILE* ptr;
  char* filename;
};
