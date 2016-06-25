#include <iostream>
#include <fstream>

class GeradorElf {
  std::ifstream file;
  std::string data;
  std::string text;
  public:
	  GeradorElf(std::string);
    void processFile();
    void readFile();
    char convertToHex(char c);
    std::vector<std::string> tokenize(std::string line);
    std::string processDataLine(std::string line);
    std::string processTextLine(std::string line);
    void createFile();
    std::string removeMultipleSpaces(std::string line);
};