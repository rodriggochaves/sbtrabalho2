#include <iostream>
#include <fstream>

class GeradorElf {
  std::ifstream file;
  public:
	  GeradorElf(std::string);
    void processFile();
};