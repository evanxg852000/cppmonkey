#include <string>
#include <sstream>
#include <memory>

#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "utils.hpp"

using namespace std;

Parser::Parser(Lexer& lexer): lexer{lexer} {
    errors = {};
    nextToken();
    nextToken();

    // Pratt Parser (Operator Precedence parser)
    // setup precedence table
    precedences = {
        {TokenType::EQUAL_EQUAL, static_cast<int>(PrecedenceLevel::EQUALS)},
        {TokenType::BANG_EQUAL, static_cast<int>(PrecedenceLevel::EQUALS)},
        {TokenType::LESS, static_cast<int>(PrecedenceLevel::LESSGREATER)},
        {TokenType::GREATER, static_cast<int>(PrecedenceLevel::LESSGREATER)},
        {TokenType::PLUS, static_cast<int>(PrecedenceLevel::SUM)},
        {TokenType::MINUS, static_cast<int>(PrecedenceLevel::SUM)},
        {TokenType::STAR, static_cast<int>(PrecedenceLevel::PRODUCT)},
        {TokenType::SLASH, static_cast<int>(PrecedenceLevel::PRODUCT)},
        {TokenType::LEFT_PAREN, static_cast<int>(PrecedenceLevel::CALL)}
    };

    //register prefix functions 
    registerPrefix(TokenType::IDENT, std::bind(&Parser::parseIdentifier, this));
    registerPrefix(TokenType::NUMBER, std::bind(&Parser::parseNumberLiteral, this));
    registerPrefix(TokenType::BANG, std::bind(&Parser::parsePrefixExpression, this));
    registerPrefix(TokenType::MINUS, std::bind(&Parser::parsePrefixExpression, this));
    registerPrefix(TokenType::TRUE, std::bind(&Parser::parseBooleanLiteral, this));
    registerPrefix(TokenType::FALSE, std::bind(&Parser::parseBooleanLiteral, this));
    registerPrefix(TokenType::LEFT_PAREN, std::bind(&Parser::parseGroupedExpression, this));
    registerPrefix(TokenType::IF, std::bind(&Parser::parseIfExpression, this));
    registerPrefix(TokenType::FUNC, std::bind(&Parser::parseFunctionLiteral, this));

    // register infix functions
    registerInfix(TokenType::PLUS, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::MINUS, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::STAR, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::SLASH, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::EQUAL_EQUAL, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::BANG_EQUAL, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::LESS, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::GREATER, std::bind(&Parser::parseInfixExpression, this, std::placeholders::_1));
    registerInfix(TokenType::LEFT_PAREN, std::bind(&Parser::parseCallExpression, this, std::placeholders::_1));
}

void Parser::nextToken(){
    currToken = peekToken;
    peekToken = lexer.nextToken();
}

shared_ptr<Program> Parser::parseProgram(){
    auto program = Program();
    while(!currentTokenIs(TokenType::EOS)){
        auto stmt = parseStatement();
        if(stmt != nullptr)
            program.statements.push_back(stmt);
        nextToken();
    }
    return make_shared<Program>(program);
}

shared_ptr<StatementNode> Parser::parseStatement(){
    switch (currToken.type){
    case TokenType::LET:
        return parseLetStatement();
    case TokenType::RETURN:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

shared_ptr<LetStatement> Parser::parseLetStatement(){
    auto stmt = LetStatement{currToken};
    if(!expectPeek(TokenType::IDENT))
        return nullptr;

    stmt.name = Identifier{currToken, currToken.literal};
    if(!expectPeek(TokenType::EQUAL))
        return nullptr;

    nextToken();
    stmt.value = parseExpression(static_cast<int>(PrecedenceLevel::LOWEST));

    while(!currentTokenIs(TokenType::SEMICOLON))
        nextToken();
    return make_shared<LetStatement>(stmt);
}

shared_ptr<ReturnStatement> Parser::parseReturnStatement(){
    auto stmt = ReturnStatement{currToken};

    nextToken();

    stmt.value = parseExpression(static_cast<int>(PrecedenceLevel::LOWEST));

    while(!currentTokenIs(TokenType::SEMICOLON))
        nextToken();

    return make_shared<ReturnStatement>(stmt);
}

shared_ptr<ExpressionStatement> Parser::parseExpressionStatement(){
    auto stmt = ExpressionStatement{currToken};
    stmt.expression = parseExpression(static_cast<int>(PrecedenceLevel::LOWEST));
    if(peekTokenIs(TokenType::SEMICOLON))
        nextToken();
    return make_shared<ExpressionStatement>(stmt);
}

shared_ptr<ExpressionNode> Parser::parseExpression(int precedence){
    auto prefixFn = prefixParseFuncs[currToken.type];
    if(!prefixFn){
        putError(format("No prefix found for token '", ::to_string(currToken.type), "'"));
        return nullptr;
    }
    auto leftExpr = prefixFn();

    while(!peekTokenIs(TokenType::SEMICOLON) && precedence < peekPrecedence()){
        auto infixFn = infixParseFuncs[peekToken.type];
        if(!infixFn)
            return leftExpr;
        nextToken();
        leftExpr = infixFn(leftExpr);
    }

    return leftExpr;
}

shared_ptr<BlockStatement> Parser::parseBlockStatement(){
    auto block = BlockStatement{currToken};
    nextToken();
    while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::EOS)){
        auto stmt = parseStatement();
        if(stmt != nullptr)
            block.statements.push_back(stmt);
        nextToken();
    }
    return make_shared<BlockStatement>(block);
}

shared_ptr<vector<Identifier>> Parser::parseFunctionParameters(){
    vector<Identifier> params{};
    if(peekTokenIs(TokenType::RIGHT_PAREN)){
        nextToken();
        return make_shared<vector<Identifier>>(params);
    }

    nextToken();
    auto iden = Identifier{currToken, currToken.literal};
    params.push_back(iden);

    while(peekTokenIs(TokenType::COMMA)){
        nextToken();
        nextToken();
        iden =  Identifier{currToken, currToken.literal};
        params.push_back(iden);
    }

    if(!expectPeek(TokenType::RIGHT_PAREN))
        return nullptr;

    return make_shared<vector<Identifier>>(params);
}

std::shared_ptr<CallExpression::ExprNodeList> Parser::parseCallArguments(){
    CallExpression::ExprNodeList arguments{};
    if(peekTokenIs(TokenType::RIGHT_PAREN)){
        nextToken();
        return make_shared<CallExpression::ExprNodeList>(arguments);
    }

    nextToken();
    arguments.push_back(parseExpression(static_cast<int>(PrecedenceLevel::LOWEST)));
    while(peekTokenIs(TokenType::COMMA)){
        nextToken();
        nextToken();
        arguments.push_back(parseExpression(static_cast<int>(PrecedenceLevel::LOWEST)));
    }

    if(!expectPeek(TokenType::RIGHT_PAREN))
        return nullptr;

    return make_shared<CallExpression::ExprNodeList>(arguments);
}


// Partt functions
shared_ptr<ExpressionNode> Parser::parseIdentifier(){
    //auto expr = Identifier{currToken, currToken.literal};
    return make_shared<Identifier>(currToken, currToken.literal);
}

shared_ptr<ExpressionNode> Parser::parseNumberLiteral(){
    //auto expr = NumberLiteral{currToken, ::get<double>(currToken.value)};
    return make_shared<NumberLiteral>(currToken, std::get<double>(currToken.value));
}

shared_ptr<ExpressionNode> Parser::parseBooleanLiteral(){
    return make_shared<BooleanLiteral>(currToken, std::get<bool>(currToken.value));
}

shared_ptr<ExpressionNode> Parser::parsePrefixExpression(){
    auto expr = PrefixExpression{currToken, currToken.literal};
    nextToken();
    expr.right = parseExpression(static_cast<int>(PrecedenceLevel::PREFIX));

    return make_shared<PrefixExpression>(expr);
}

shared_ptr<ExpressionNode> Parser::parseInfixExpression(shared_ptr<ExpressionNode> left) {
    auto expr = InfixExpression{currToken, currToken.literal, left};
    int precedence = currentPrecedence();
    nextToken();
    expr.right = parseExpression(precedence);
    return make_shared<InfixExpression>(expr);
}

shared_ptr<ExpressionNode> Parser::parseGroupedExpression(){
    nextToken();
    auto expr = parseExpression(static_cast<int>(PrecedenceLevel::LOWEST));
    if(!expectPeek(TokenType::RIGHT_PAREN)) 
        return nullptr;
    return expr;
}

shared_ptr<ExpressionNode> Parser::parseIfExpression(){
    auto expr = IfExpression{currToken};
    if(!expectPeek(TokenType::LEFT_PAREN))
        return nullptr;

    nextToken();
    expr.condition = parseExpression(static_cast<int>(PrecedenceLevel::LOWEST));

    if(!expectPeek(TokenType::RIGHT_PAREN))
        return nullptr;
    if(!expectPeek(TokenType::LEFT_BRACE))
        return nullptr;

    expr.consequence = parseBlockStatement();

    if(peekTokenIs(TokenType::ELSE)){
        nextToken();
        if(!expectPeek(TokenType::LEFT_BRACE))
            return nullptr;
        expr.alternative = parseBlockStatement();
    }

    return make_shared<IfExpression>(expr);
}

shared_ptr<ExpressionNode> Parser::parseFunctionLiteral(){
    auto expr = FunctionLiteral{currToken};
    if(!expectPeek(TokenType::LEFT_PAREN))
        return nullptr;

    expr.params = parseFunctionParameters();

    if(!expectPeek(TokenType::LEFT_BRACE))
        return nullptr;

    expr.body = parseBlockStatement();

    return make_shared<FunctionLiteral>(expr);
}

shared_ptr<ExpressionNode> Parser::parseCallExpression(shared_ptr<ExpressionNode> func){
    auto expr = CallExpression{currToken, func};
    expr.arguments = parseCallArguments();
    return make_shared<CallExpression>(expr);
}

// helpers
void Parser::registerPrefix(TokenType type, PrefixParseFn fn){
    prefixParseFuncs[type] = fn;
}

void Parser::registerInfix(TokenType type, InfixParseFn fn){
    infixParseFuncs[type] = fn;
}

int Parser::currentPrecedence(){
    if(precedences.find(currToken.type) == precedences.end())
        return static_cast<int>(PrecedenceLevel::LOWEST);
    return precedences[currToken.type];
}

int Parser::peekPrecedence(){
    if(precedences.find(peekToken.type) == precedences.end())
        return static_cast<int>(PrecedenceLevel::LOWEST);
    return precedences[peekToken.type];
}

bool Parser::currentTokenIs(TokenType tokenType){
    return currToken.type == tokenType;
}

bool Parser::peekTokenIs(TokenType tokenType){
    return peekToken.type == tokenType;
}

bool Parser::expectPeek(TokenType tokenType){
    if(peekToken.type != tokenType){
        peekError(tokenType);
        return false;
    }
    nextToken();
    return true;
}

vector<string> Parser::getErrors(){
    return errors;
}

void Parser::putError(string err){
    errors.push_back(err);
}

void Parser::peekError(TokenType tokenType){
    string str = format("Expected next token to be ",
        ::to_string(tokenType),
        ", but got ",
        ::to_string(peekToken.type),
        " instead"
    );
    errors.push_back(str);
}

