#include <cstdio>
#include <iostream>

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

std::string ConsoleReader::GetLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void FileReader::SetUpUsage() {
  ptr = fopen(filename, "r");
}

std::string FileReader::GetLine() {
  char* line = NULL;
  size_t len;
  if (getline(&line, &len, ptr) == -1) {
    throw std::runtime_error("end of file reached");
  }
  std::string answer(line);
  free(line);
  return answer;
}

void FileReader::FinishUsage() {
  fclose(ptr);
  ptr = NULL;
}

