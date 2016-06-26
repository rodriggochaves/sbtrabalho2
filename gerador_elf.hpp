#include <iostream>
#include <fstream>
#include <stdint.h>

struct dataNode {
  std::string symbol;
  std::string type;
  std::string value;
  int64_t position;
};

struct textNode {
  std::string label;
  std::string instruction;
  std::string op1;
  std::string op2;
  int64_t code;
};

class GeradorElf {
  std::ifstream file;
  std::string data;
  std::string text;
  std::vector<dataNode> symbols;
  std::vector<textNode> instructions;
  long long int currentDataPosition;
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
    std::string undercase( std::string );
    std::string filterMemory( std::string op );
    dataNode findSymbol( std::string op );
    void assemble( textNode& node );
    void createFile();
    std::string removeMultipleSpaces(std::string line);
    std::string convertInstructions(std::string result);
    long long int reverseNumber ( long long int number );
};