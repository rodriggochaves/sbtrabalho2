#include <iostream>
#include <fstream>

class GeradorElf {
  std::ifstream file;
  std::string data;
  public:
	  GeradorElf(std::string);
    void processFile();
    void readFile();
    char convertToHex(char c);
    std::string processDataLine(std::string line);
    void createFile(std::string text);
};