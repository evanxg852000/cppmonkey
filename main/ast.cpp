#include <iostream>
#include <string>
#include <sstream>
#include <vector>


#include "token.hpp"
#include "ast.hpp"

using namespace std;

// Program class
string Program::tokenLiteral() {
    if(statements.size() > 0)
        return statements[0]->tokenLiteral();
    else
        return "";
}
string Program::toString(){
    stringstream ss{};
    for(auto stmt : statements)
        ss << stmt->toString();
    return ss.str();
}

// Identifier class
void Identifier::expressionNode(){
}
string Identifier::toString(){
    return value;
}

// LetStatement class
void LetStatement::statementNode(){
}
string LetStatement::toString(){
    stringstream ss{};
    ss << tokenLiteral() << " "
       << name.toString() << " = ";
    if(value != nullptr)
        ss << value->toString();
    ss << ";";
    return ss.str();
}

// ReturnStatement class
void ReturnStatement::statementNode(){
}
string ReturnStatement::toString(){
    stringstream ss{};
    ss << tokenLiteral() << " ";
    if(value != nullptr)
       ss << value->toString(); 
    ss << ";";
    return ss.str();
}

// ExpressionStatement class
void ExpressionStatement::statementNode(){
}
string ExpressionStatement::toString(){
    if(expression != nullptr)
        return expression->toString();
    return "";
}

// BlockStatement class
void BlockStatement::statementNode(){
}
string BlockStatement::toString(){
    stringstream ss;
    for(auto stmt: statements){
        ss << stmt->toString();
    }
    return ss.str();
}

// NumberLiteral class 
void NumberLiteral::expressionNode(){

}
string NumberLiteral::toString(){
    /* Note: 
       std::to_string(value) should be used, but for precision issues
    */
    return tokenLiteral();
}

// StringLiteral class
void StringLiteral::expressionNode(){

}

string StringLiteral::toString(){
    return tokenLiteral();
}


// BooleanLiteral class
void BooleanLiteral::expressionNode(){

}
string BooleanLiteral::toString(){
    /* Note: 
       std::to_string(value) should be used, boolean is converted to int in c++
       maybe write a template specialization
    */
    return tokenLiteral();
}

// ArrayLiteral 
void ArrayLiteral::expressionNode(){

}
string ArrayLiteral::toString(){
    stringstream ss;
    ss << "[";
    bool isFirst= true;
    for(auto item: items){
        if(isFirst)
            isFirst = false;
        else
            ss << ", ";

        ss << item->toString();
    }
    ss << "]";
    return ss.str();
}

// HashLiteral
void HashLiteral::expressionNode(){

}
string HashLiteral::toString(){
    stringstream ss;
    ss << "{";
    bool isFirst= true;
    for(auto entry: entries){
        if(isFirst) 
            isFirst = false; 
        else 
            ss << ", ";
    
        ss << entry.first->toString() << ":" << entry.second->toString();
    }
    ss << "}";
    return ss.str();
}

// PrefixExpression class
void PrefixExpression::expressionNode(){

}
string PrefixExpression::toString(){
    stringstream ss;
    ss << "(" << oprator << right->toString() << ")";
    return ss.str();
}

// InfixExpression class
void InfixExpression::expressionNode(){

}
string InfixExpression::toString(){
    stringstream ss;
    ss << "(" << left->toString() << " " << oprator << " " 
        << right->toString() << ")";
    return ss.str();
}

// IfExpression class
void IfExpression::expressionNode(){    
}
string IfExpression::toString(){
    stringstream ss;
    ss << "if" << condition->toString() << " " << consequence->toString();
    if(alternative != nullptr)
        ss << "else " << alternative->toString();
    return ss.str();
}

// FunctionLiteral class
void FunctionLiteral::expressionNode(){  
}
string FunctionLiteral::toString(){
    stringstream ss;
    ss << tokenLiteral() << "(" ;
    for(auto p = params->begin(); p != params->end(); ++p){
        ss << p->toString();
        if(p != params->end()-1)
            ss << ",";
    }
    ss << ")";
    ss << body->toString();
    return ss.str();
}

// CallExpression class
void CallExpression::expressionNode(){ 
}
string CallExpression::toString(){
    stringstream ss;
    ss << function->toString() << "(" ;
    for(auto p = arguments->begin(); p != arguments->end(); ++p){
        ss << p->get()->toString();
        if(p != arguments->end()-1)
            ss << ", ";
    }
    ss << ")";
    return ss.str();
}

// IndexExpression class
void IndexExpression::expressionNode(){

}
string IndexExpression::toString(){
    stringstream ss;
    ss << "(" << left->toString() << "[" << index->toString() << "])";
    return ss.str();
}