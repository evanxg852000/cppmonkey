#if !defined(LEXER_H)
#define LEXER_H

#include <string>
#include <unordered_map>

class Token;
enum class TokenType;

class Lexer {
private:
    std::string input;
    std::unordered_map<std::string, TokenType> keywords;
    int pos; // current position in input
    int rpos; // current reading pos
    char ch; // current character

    
    void readChar();
    char peekChar();
    std::string readIdentifier();
    std::string readNumber();
    void skipWhitespace();
    TokenType getTokenType(std::string);
    
public:
    Lexer(std::string);
    ~Lexer();

    Token nextToken();    
};


#endif //  LEXER_H