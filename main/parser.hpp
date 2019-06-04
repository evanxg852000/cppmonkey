#if !defined(PARSER_H)
#define PARSER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

class Token;
enum class TokenType;
class Lexer;
class Program;
class ExpressionNode;

enum class PrecedenceLevel {
    _, //0
    LOWEST,
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // + or -
    PRODUCT,     // * or /
    PREFIX,      // -x or !x
    CALL,        // myFunc(x)
    INDEX,       // index
};

class Parser {
public:
    Parser(Lexer&);
    virtual ~Parser() = default;
    void nextToken();
    std::shared_ptr<Program> parseProgram();
    std::vector<std::string> getErrors();

private:
    // type alias
    using ExprNode = std::shared_ptr<ExpressionNode>;
    using ExprNodeList = std::vector<ExprNode>;
    using PrefixParseFn = std::function<ExprNode()>;
    using InfixParseFn = std::function<ExprNode(ExprNode)>;

    Lexer lexer;
    Token currToken;
    Token peekToken;
    std::vector<std::string> errors;
    std::unordered_map<TokenType, PrefixParseFn> prefixParseFuncs;
    std::unordered_map<TokenType, InfixParseFn> infixParseFuncs;
    std::unordered_map<TokenType, int> precedences;

    std::shared_ptr<StatementNode> parseStatement();
    std::shared_ptr<LetStatement> parseLetStatement();
    std::shared_ptr<ReturnStatement> parseReturnStatement();
    std::shared_ptr<ExpressionStatement> parseExpressionStatement();
    std::shared_ptr<ExpressionNode> parseExpression(int);
    std::shared_ptr<BlockStatement> parseBlockStatement();
    std::shared_ptr<std::vector<Identifier>> parseFunctionParameters();
    std::shared_ptr<ExprNodeList> parseCallArguments();

    
    //pratt functions
    std::shared_ptr<ExpressionNode> parseIdentifier();
    std::shared_ptr<ExpressionNode> parseNumberLiteral();
    std::shared_ptr<ExpressionNode> parseStringLiteral();
    std::shared_ptr<ExpressionNode> parseBooleanLiteral();
    std::shared_ptr<ExpressionNode> parseArrayLiteral();
    std::shared_ptr<ExpressionNode> parseHashLiteral();
    std::shared_ptr<ExpressionNode> parsePrefixExpression();
    std::shared_ptr<ExpressionNode> parseInfixExpression(std::shared_ptr<ExpressionNode>);
    std::shared_ptr<ExpressionNode> parseGroupedExpression();
    std::shared_ptr<ExpressionNode> parseIfExpression();
    std::shared_ptr<ExpressionNode> parseFunctionLiteral();
    std::shared_ptr<ExpressionNode> parseCallExpression(std::shared_ptr<ExpressionNode>);
    std::shared_ptr<ExpressionNode> parseIndexExpression(std::shared_ptr<ExpressionNode>);

    void registerPrefix(TokenType, PrefixParseFn);
    void registerInfix(TokenType, InfixParseFn);
    int peekPrecedence();
    int currentPrecedence();
    bool currentTokenIs(TokenType);
    bool peekTokenIs(TokenType);
    bool expectPeek(TokenType);
    void putError(std::string);
    void peekError(TokenType);

};

#endif // PARSER_H