#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <any>

#include "object.hpp"

using namespace std;

std::string to_string(const ObjectType& type){
    switch (type){
    case ObjectType::NIL:
        return "NIL";
    case ObjectType::NUMBER:
        return "NUMBER";
    case ObjectType::STRING:
        return "STRING";
    case ObjectType::BOOLEAN:
        return "BOOLEAN";
    case ObjectType::RETURN:
        return "RETURN";
    case ObjectType::UNDEFINED:
        return "UNDEFINED";
    case ObjectType::FUNCTION:
        return "FUNCTION";
    case ObjectType::BUILTIN_OBJECT:
        return "BUILTIN OBJECT";
    case ObjectType::BUILTIN_FUNCTION:
        return "BUILTIN FUNCTION";
    case ObjectType::ARRAY:
        return "ARRAY";
    case ObjectType::HASH:
        return "HASH";
    default:
        return "";
    }
}

string Object::inspect() const {
    stringstream ss;
    switch (type){
    case ObjectType::NIL:
        ss << "Nil";
        break;
    case ObjectType::NUMBER:
        ss << any_cast<double>(value);  
        break;
    case ObjectType::STRING:
        ss << any_cast<string>(value);
        break;
    case ObjectType::BOOLEAN:
        ss << (any_cast<bool>(value)? "True" : "False");
        break;
    case ObjectType::RETURN:
        ss << any_cast<Object>(value).inspect();
        break;
    case ObjectType::ERROR:
        ss << any_cast<string>(value);
        break;
    case ObjectType::UNDEFINED:
        ss << "Undefined";
        break;
    case ObjectType::FUNCTION:
        /* TODO: Could refactor to get funtion body
        auto fnObj = any_cast<FunctionObject>(value);
        fnObj.func->toString();
        */
        ss << "<function: " << _tag << ">";
        break;
    case ObjectType::BUILTIN_OBJECT:
         ss << "<builtin: " << any_cast<Object>(value).inspect() << ">";
        break;
    case ObjectType::BUILTIN_FUNCTION:
        ss << "<builtin-function: " << _tag << ">";
        break;
    case ObjectType::ARRAY:{
            auto items = any_cast<std::vector<Object>>(value);
            ss << "[";
            bool isFirst = true;
            for(auto item: items){
                if(isFirst)
                    isFirst = false;
                else 
                    ss << ", ";
                ss << item.inspect();
            }
            ss << "]";
            break;
        }
    case ObjectType::HASH:{
            auto entries = any_cast<unordered_map<string, pair<Object, Object>>>(value);
            ss << "{";
            bool isFirst = true;
            for(auto item: entries){
                if(isFirst)
                    isFirst = false;
                else 
                    ss << ", ";
                ss << item.second.first.inspect() << ":" << item.second.second.inspect();
            }
            ss << "}";
            break;
        }
    default:
        ss << "";
    }
    return ss.str();
}


string Object::hashKey() const{
    stringstream ss;
    switch (type){
    case ObjectType::NUMBER:
        ss << any_cast<double>(value);  
        break;
    case ObjectType::STRING:
        ss << any_cast<string>(value);
        break;
    case ObjectType::BOOLEAN:
        ss << (any_cast<bool>(value)? "1" : "0");
        break;
    
    default:
        ss << "";
    }
    return ss.str();
}