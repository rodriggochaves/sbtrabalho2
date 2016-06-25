#include <iostream>
#include <fstream>

struct dataNode {
  std::string symbol;
  std::string type;
  std::string value;
};

class GeradorElf {
  std::ifstream file;
  std::string data;
  std::string text;
  std::vector<dataNode> symbols;
  public:
	  GeradorElf(std::string);
    void processFile();
    void readFile();
    char convertToHex(char c);
    std::vector<std::string> tokenize(std::string line);
    dataNode processDataNode( dataNode node );
    dataNode processDataLine(std::string line);
    std::string processTextLine(std::string line);
    std::string assembleMOV(std::vector<std::string> line);
    void createFile();
    std::string removeMultipleSpaces(std::string line);
};