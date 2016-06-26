#include <iostream>
#include <fstream>
#include <stdint.h>

struct labelNode {
  std::string label;
  long long int address;
};

struct dataNode {
  std::string symbol;
  std::string type;
  std::string value;
  std::string times;
  int64_t position;
};

struct textNode {
  std::string label;
  std::string instruction;
  std::string op1;
  std::string op2;
  int64_t code;
  bool valid;
};

class GeradorElf {
  std::ifstream file;
  std::string data;
  std::string text;
  std::vector<dataNode> symbols;
  std::vector<textNode> instructions;
  std::vector<labelNode> labels;
  long long int currentDataPosition;
  long long int currentTextPosition;
  std::string currentLabel;
  labelNode entryPoint;
  public:
	  GeradorElf(std::string);
    void processFile();
    void readFile();
    char convertToHex(char c);
    bool isString ( dataNode node );
    bool isRegister ( std::string name );
    bool isMemory ( std::string name );
    bool isImmediate ( std::string name );
    std::string getLabel( std::string& line );
    std::string getInstruction( std::string& line );
    std::string getOp1( std::string& line );
    std::string getOp2( std::string& line );
    std::string getToken( std::string& line );
    void storeLabel( std::string line );
    long long int getRegistersNumber ( std::string reg );
    textNode tokenize(std::string line);
    dataNode processDataNode( dataNode node );
    dataNode processDataLine( std::string line );
    textNode processTextLine( std::string line );
    std::string undercase( std::string );
    std::string filterMemory( std::string op );
    dataNode findSymbol( std::string op );
    int numberOfDigits( long long int number, int base );
    void assemble( textNode& node );
    long long int hexParamater( std::string op );
    void createFile();
    std::string removeMultipleSpaces(std::string line);
    std::string convertInstructions(std::string result);
    long long int reverseNumber ( long long int number );
};