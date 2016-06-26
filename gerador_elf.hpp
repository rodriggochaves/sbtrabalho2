#include <iostream>
#include <fstream>

struct dataNode {
  std::string symbol;
  std::string type;
  std::string value;
  int position;
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
  std::vector<textNode> instructions;
  int currentDataPosition;
  public:
	  GeradorElf(std::string);
    void processFile();
    void readFile();
    char convertToHex(char c);
    bool isString ( dataNode node );
    bool isRegister ( std::string name );
    bool isMemory ( std::string name );
    std::string getLabel( std::string& line );
    std::string getInstruction( std::string& line );
    std::string getOp1( std::string& line );
    std::string getOp2( std::string& line );
    textNode tokenize(std::string line);
    dataNode processDataNode( dataNode node );
    dataNode processDataLine( std::string line );
    textNode processTextLine( std::string line );
    void assemble( textNode node );
    void createFile();
    std::string removeMultipleSpaces(std::string line);
};