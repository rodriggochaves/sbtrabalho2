#include <iostream>
#include <fstream>

struct dataNode {
  std::string symbol;
  std::string type;
  std::string value;
};

struct textNode {
  std::string label;
  std::string instruction;
  std::string op1;
  std::string op2;
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
    bool isString ( dataNode node );
    std::string getLabel( std::string line );
    textNode tokenize(std::string line);
    dataNode processDataNode( dataNode node );
    dataNode processDataLine( std::string line );
    std::string processTextLine(std::string line);
    std::string assembleMOV(textNode node);
    void createFile();
    std::string removeMultipleSpaces(std::string line);
};