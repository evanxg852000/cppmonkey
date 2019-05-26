#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <any>

#include "object.hpp"
//#include "ast.hpp"
//#include "fobject.hpp"

using namespace std;

std::string to_string(const ObjectType& type){
    switch (type){
    case ObjectType::NIL:
        return "NIL";
    case ObjectType::NUMBER:
        return "NUMBER";
    case ObjectType::BOOLEAN:
        return "BOOLEAN";
    case ObjectType::RETURN:
        return "RETURN";
    case ObjectType::UNDEFINED:
        return "UNDEFINED";
    case ObjectType::FUNCTION:
        return "FUNCTION";


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
    // case ObjectType::FUNCTION:{
    //     auto fnObj = any_cast<FunctionObject>(value);
    //     ss << fnObj.func->toString();
    //     break;
    // }

    
    default:
        ss << "";
    }
    return ss.str();
}
