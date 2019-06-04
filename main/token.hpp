#if !defined(TOKEN_H)

#include <string>
#include <variant>
#include <unordered_map>

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, COLON, SEMICOLON, SLASH, STAR,

    // One or two character tokens
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

    // Literals
    IDENT, // add, foo, x, y ...
    STRING, NUMBER, INT,

    // Keywords
    FUNC, LET, TRUE, FALSE, IF, ELSE, RETURN,


    // Utils
    ILLEGAL,
    EOS
};

struct Token {
    TokenType type;
    std::string literal;
    std::variant<std::string, double, bool> value;
    int line;
    int column;
};

std::string to_string(const TokenType&);

Token makeToken(TokenType, std::string);

Token makeToken(TokenType, char);

std::string tokenToString(const Token&);

std::unordered_map<std::string, TokenType> makeKeywords();
 
#endif // TOKEN_H