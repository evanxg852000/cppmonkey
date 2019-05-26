#if !defined(UTILS_H)
#define UTILS_H

#include <string>
#include <sstream>
#include <memory>
#include <cstdarg>

bool isLetter(const char&);

bool isDigit(const char&);

bool isWhitespace(const char&);

template <typename T>
std::string toStringWithPrecicion(const T val, const int n = 6){
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << val;
    return out.str();
}

// Can be used like: isInstanceOf<Program, AstNode>(node)
template <typename T, typename U>
bool isInstanceOf(std::shared_ptr<U> instance){
    return std::dynamic_pointer_cast<T>(instance) != nullptr;
}

std::string format();

template <typename T, typename... Types> 
std::string format(T v1, Types... v2) { 
    std::stringstream ss;
    ss << v1; 
    ss << format(v2...);
    return ss.str();
} 

#endif //  UTILS_H