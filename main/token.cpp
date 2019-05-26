#include <string>
#include <sstream>
#include <map>

#include <cstdlib>

#include "token.hpp"

using namespace std;

std::string to_string(const TokenType& type){
    switch (type){
    case TokenType::LEFT_PAREN:
        return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN:
        return "RIGHT_PAREN"; 
    case TokenType::LEFT_BRACE:
        return "LEFT_BRACE"; 
    case TokenType::RIGHT_BRACE:
        return "RIGHT_BRACE";
	case TokenType::COMMA:
        return "COMMA";
    case TokenType::DOT:
        return "DOT";
    case TokenType::MINUS:
        return "MINUS"; 
    case TokenType::PLUS:
        return "PLUS"; 
    case TokenType::SEMICOLON:
        return "SEMICOLON"; 
    case TokenType::SLASH:
        return "SLASH"; 
    case TokenType::STAR:
        return "STAR";
	case TokenType::BANG:
        return "BANG";
    case TokenType::BANG_EQUAL:
        return "BANG_EQUAL";
	case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::EQUAL_EQUAL:
        return "EQUAL_EQUAL";
	case TokenType::GREATER:
        return "GREATER"; 
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
	case TokenType::LESS:
        return "LESS";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::IDENT:
        return "IDENT";
    case TokenType::STRING:
        return "STRING";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::INT:
        return "INT";
    case TokenType::FUNC:
        return "FUNC";
    case TokenType::LET:
        return "LET";
    case TokenType::TRUE:
        return "TRUE"; 
    case TokenType::FALSE:
        return "FALSE";
    case TokenType::IF:
        return "IF";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::ILLEGAL:
        return "ILLEGAL";
    case TokenType::EOS:
        return "EOS";
    default:
        return "";
    }
}

Token makeToken(TokenType type, std::string literal){
    Token token{
        type,
        literal,
        literal,
        0,
        0
    };
    if(type == TokenType::NUMBER)
        token.value = (double)atoi(literal.c_str());
    else if(type == TokenType::TRUE)
        token.value = true;
    else if(type == TokenType::FALSE)
        token.value = false;
    return token;
}

Token makeToken(TokenType type, char ch){
    return makeToken(type, string(1, ch));
}

string tokenToString(const Token& token){
    stringstream ss;
    ss << "{";
    ss << " type: " << ::to_string(token.type) << ",";
    ss << " literal: " << token.literal << " ";
    ss << "}";
    return ss.str();
}

unordered_map<string, TokenType> makeKeywords(){
    unordered_map<string, TokenType> words;
    words["fn"] = TokenType::FUNC;
    words["let"] = TokenType::LET;
    words["true"] = TokenType::TRUE;
    words["false"] = TokenType::FALSE;
    words["if"] = TokenType::IF;
    words["else"] = TokenType::ELSE;
    words["return"] = TokenType::RETURN;

    return words;
}