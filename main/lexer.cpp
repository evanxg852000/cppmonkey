#include <string>
#include <cstdlib>

#include "token.hpp"
#include "lexer.hpp"
#include "utils.hpp"

#include <iostream>

using namespace std;


Lexer::Lexer(string src): input{src} {
    keywords = makeKeywords();
    pos = 0;
    rpos = 0;
    ch = 0;
    readChar();
}

Lexer::~Lexer(){

}

Token Lexer::nextToken(){
    Token token;
    skipWhitespace();
    switch (ch){
        case '=':
            if(peekChar() == '='){
                string str{ch};
                readChar();
                str.push_back(ch);
                token = makeToken(TokenType::EQUAL_EQUAL, str);
            } else {
                token = makeToken(TokenType::EQUAL, ch);
            }
            break;
        case '+':
            token = makeToken(TokenType::PLUS, ch);
            break;
        case '-':
            token = makeToken(TokenType::MINUS, ch);
            break;
        case '*':
            token = makeToken(TokenType::STAR, ch);
            break;
        case '/':
            token = makeToken(TokenType::SLASH, ch);
            break;
        case '!':
            if(peekChar() == '='){
                string str{ch};
                readChar();
                str.push_back(ch);
                token = makeToken(TokenType::BANG_EQUAL, str);
            } else {
                token = makeToken(TokenType::BANG, ch);
            }
            break;
        case '<':
            token = makeToken(TokenType::LESS, ch);
            break;
        case '>':
            token = makeToken(TokenType::GREATER, ch);
            break;
        case ';':
            token = makeToken(TokenType::SEMICOLON, ch);
            break;
        case ',':
            token = makeToken(TokenType::COMMA, ch);
            break;
        case '(':
            token = makeToken(TokenType::LEFT_PAREN, ch);
            break;
        case ')':
            token = makeToken(TokenType::RIGHT_PAREN, ch);
            break;
        case '{':
            token = makeToken(TokenType::LEFT_BRACE, ch);
            break;
        case '}':
            token = makeToken(TokenType::RIGHT_BRACE, ch);
            break;
        case 0:
            token = makeToken(TokenType::EOS, "");
            break;
        default:
            if(isLetter(ch)){
                string word = readIdentifier();
                return makeToken(getTokenType(word), word);
            } else if(isDigit(ch)){
                return makeToken(TokenType::NUMBER, readNumber());
            } else {
                token = makeToken(TokenType::ILLEGAL, "");
            }
            break;
    }

    readChar();
    return token;
}


void Lexer::readChar(){
    if(rpos >= input.length()){
        ch = 0;
    } else {
        ch = input.at(rpos);
    }

    pos = rpos;
    rpos++;
}

char Lexer::peekChar(){
    if(rpos >= input.length())
        return 0;
    else 
        return input.at(rpos);
}

string Lexer::readIdentifier() {
    string str;
    while(isLetter(ch)){
        str.push_back(ch);
        readChar();
    }
    return str;
}

string Lexer::readNumber(){
    string str;
    while(isDigit(ch)){
        str.push_back(ch);
        readChar();
    }
    return str;
}

void Lexer::skipWhitespace(){
    while(isWhitespace(ch))
        readChar();
}

TokenType Lexer::getTokenType(string ident){
    auto it = keywords.find(ident);
    if(it != keywords.end())
        return (*it).second; //
    return TokenType::IDENT;
}

