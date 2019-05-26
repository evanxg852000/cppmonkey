#include <cctype>

#include "utils.hpp"

bool isLetter(const char& ch){
    return std::isalpha(ch) || ch == '_';
}

bool isDigit(const char& ch){
    return std::isdigit(ch);
}

bool isWhitespace(const char& ch){
    return std::isspace(ch);
}

std::string format() { 
    return ""; 
}