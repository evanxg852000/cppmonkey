#if !defined(AST_H)
#define AST_H

#include <memory>
#include <string>
#include <vector>

class Token;


class AstNode {
public:
    virtual ~AstNode() = default;
    virtual std::string tokenLiteral() = 0;
    virtual std::string toString() = 0;
};

class ExpressionNode: public virtual AstNode {
public:
    ExpressionNode() = default;
    ExpressionNode(Token token):token{token} {};
    virtual ~ExpressionNode() = default;
    virtual std::string tokenLiteral(){return token.literal;};
    virtual void expressionNode() = 0;
    virtual std::string toString() = 0;
protected:
    Token token;
};

class StatementNode: public virtual AstNode {
public:
    StatementNode() = default;
    StatementNode(Token token):token{token} {};
    virtual ~StatementNode() = default;
    virtual std::string tokenLiteral(){return token.literal;};
    virtual void statementNode() = 0;
    virtual std::string toString() = 0;

    Token token;
};

class Program: public virtual AstNode {
public:
    Program(): statements{} {};
    virtual ~Program() = default;
    virtual std::string tokenLiteral();
    virtual std::string toString();

    std::vector<std::shared_ptr<StatementNode>> statements;
};

class Identifier: public virtual ExpressionNode {
public:
    Identifier() = default;
    Identifier(Token token, std::string val): ExpressionNode{token}, value{val} {};
    virtual ~Identifier() = default;
    virtual void expressionNode();
    virtual std::string toString();
    
    std::string value;
};

class LetStatement: public virtual StatementNode {
public:
    LetStatement() = default;
    LetStatement(Token token): StatementNode{token} {};
    virtual ~LetStatement() = default;
    virtual void statementNode();
    virtual std::string toString();

    Identifier name;
    std::shared_ptr<ExpressionNode> value;
};

class ReturnStatement: public virtual StatementNode {
public:
    ReturnStatement() = default;
    ReturnStatement(Token token): StatementNode{token} {};
    virtual ~ReturnStatement() = default;
    virtual void statementNode();
    virtual std::string toString();

    std::shared_ptr<ExpressionNode> value;
};

class ExpressionStatement: public virtual StatementNode {
public:
    ExpressionStatement() = default;
    ExpressionStatement(Token token): StatementNode{token} {};
    virtual ~ExpressionStatement() = default;
    virtual void statementNode();
    virtual std::string toString();

    std::shared_ptr<ExpressionNode> expression;
};

class BlockStatement: public virtual StatementNode {
public:
    BlockStatement() = default;
    BlockStatement(Token token): StatementNode{token} {};
    virtual ~BlockStatement() = default;
    virtual void statementNode();
    virtual std::string toString();

    std::vector<std::shared_ptr<StatementNode>> statements;
};

class NumberLiteral: public virtual ExpressionNode {
public:
    NumberLiteral() = default;
    NumberLiteral(Token token, double val): ExpressionNode{token}, value{val} {};
    virtual ~NumberLiteral() = default;
    virtual void expressionNode();
    virtual std::string toString();
    
    double value;
};

class BooleanLiteral: public virtual ExpressionNode {
public:
    BooleanLiteral() = default;
    BooleanLiteral(Token token, bool val): ExpressionNode{token}, value{val} {};
    virtual ~BooleanLiteral() = default;
    virtual void expressionNode();
    virtual std::string toString();
    
    bool value;
};

class PrefixExpression: public virtual ExpressionNode {
public:
    PrefixExpression() = default;
    PrefixExpression(Token token, std::string op): ExpressionNode{token}, oprator{op} {};
    virtual ~PrefixExpression() = default;
    virtual void expressionNode();
    virtual std::string toString();

    std::string oprator;
    std::shared_ptr<ExpressionNode> right;
};

class InfixExpression: public virtual ExpressionNode {
public:
    using ExprNode = std::shared_ptr<ExpressionNode>;

    InfixExpression() = default;
    InfixExpression(Token token, std::string op, ExprNode l): ExpressionNode{token}, oprator{op}, left{l} {};
    virtual ~InfixExpression() = default;
    virtual void expressionNode();
    virtual std::string toString();

    std::shared_ptr<ExpressionNode> left;
    std::string oprator;
    std::shared_ptr<ExpressionNode> right;
};

class IfExpression: public virtual ExpressionNode {
public:
    IfExpression() = default;
    IfExpression(Token token): ExpressionNode{token} {};
    virtual ~IfExpression() = default;
    virtual void expressionNode();
    virtual std::string toString();

    std::shared_ptr<ExpressionNode> condition;
    std::shared_ptr<BlockStatement> consequence;
    std::shared_ptr<BlockStatement> alternative;
};

class FunctionLiteral: public virtual ExpressionNode {
public:
    FunctionLiteral() = default;
    FunctionLiteral(Token token): ExpressionNode{token} {};
    virtual ~FunctionLiteral() = default;
    virtual void expressionNode();
    virtual std::string toString();

    std::shared_ptr<std::vector<Identifier>> params;
    std::shared_ptr<BlockStatement> body;
};

class CallExpression: public virtual ExpressionNode {
public:
    using ExprNode = std::shared_ptr<ExpressionNode>;
    using ExprNodeList = std::vector<ExprNode>;
    
    CallExpression() = default;
    CallExpression(Token token, ExprNode func): ExpressionNode{token}, function{func} {};
    virtual ~CallExpression() = default;
    virtual void expressionNode();
    virtual std::string toString();

    ExprNode function;
    std::shared_ptr<ExprNodeList> arguments;
};

#endif // AST_H
