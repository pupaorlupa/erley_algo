#include "reader.h"

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
