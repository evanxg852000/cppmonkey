#include <string>
#include <memory>
#include <vector>
#include <any>
#include <typeinfo>

#include <iostream>

#include "utils.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "fobject.hpp"
#include "evaluator.hpp"
#include "environment.hpp"

using namespace std;


Object Evaluator::eval(std::shared_ptr<AstNode> node, std::shared_ptr<Environment> env) {
    // Program
    if(auto program = dynamic_pointer_cast<Program>(node);  program != nullptr)
        return evalProgram(program->statements, env);

    // Statements
    if(auto exprStmt = dynamic_pointer_cast<ExpressionStatement>(node); exprStmt != nullptr)
        return eval(exprStmt->expression, env);

    // Block
    if(auto blockStmt = dynamic_pointer_cast<BlockStatement>(node); blockStmt != nullptr)
        return evalBlockStatement(blockStmt->statements, env);

    // Function Literal
    if(auto funLit = dynamic_pointer_cast<FunctionLiteral>(node); funLit != nullptr)
        return Object{ObjectType::FUNCTION, FunctionObject{funLit, env}};

    // Call
    if(auto callExpr = dynamic_pointer_cast<CallExpression>(node); callExpr != nullptr){
        auto function = eval(callExpr->function, env);
        if(function.type == ObjectType::ERROR)
            return function;
        
        auto args = evalExpressions(callExpr->arguments, env);
        if(args.size() == 1 && args[0].type == ObjectType::ERROR)
            return args[0];

        return applyFunction(function, args);
    }

    // Let 
    if(auto letStmt = dynamic_pointer_cast<LetStatement>(node); letStmt != nullptr){
        auto value = eval(letStmt->value, env);
        if(value.type == ObjectType::ERROR)
            return value;
        env->set(letStmt->name.value, value);
        return value;
    }

    // Identifier
    if(auto ident = dynamic_pointer_cast<Identifier>(node); ident != nullptr){
        auto value = env->get(ident->value);
        if(value.type == ObjectType::UNDEFINED)
            return raiseError(format("identifier not found: ", ident->value));
        return value;
    }

    // If Else
    if(auto ifExpr = dynamic_pointer_cast<IfExpression>(node); ifExpr != nullptr)
        return evalIfExpression(ifExpr, env);

    // Return
    if(auto returnStmt = dynamic_pointer_cast<ReturnStatement>(node); returnStmt != nullptr){
        auto value = eval(returnStmt->value, env);
        if(value.type == ObjectType::ERROR)
            return value;
        return Object{ObjectType::RETURN, value};
    }

    // Number
    if(auto number = dynamic_pointer_cast<NumberLiteral>(node); number != nullptr)
        return Object{ObjectType::NUMBER, number->value};

    // Boolean
    if(auto boolean = dynamic_pointer_cast<BooleanLiteral>(node); boolean != nullptr)
        return boolean->value? TRUE_OBJ : FALSE_OBJ;

    // Prefix
    if(auto prefixExpr = dynamic_pointer_cast<PrefixExpression>(node); prefixExpr != nullptr){
        auto right = eval(prefixExpr->right, env);
        if(right.type == ObjectType::ERROR)
            return right;
        return evalPrefixExpression(prefixExpr->oprator, right);
    }

    // Infix 
    if(auto infixExpr = dynamic_pointer_cast<InfixExpression>(node); infixExpr != nullptr){
        auto left = eval(infixExpr->left, env);
        if(left.type == ObjectType::ERROR)
            return left;
        auto right = eval(infixExpr->right, env);
        if(right.type == ObjectType::ERROR)
            return right;
        return evalInfixExpression(infixExpr->oprator, left, right);
    }


    
    
    return NIL_OBJ;
}


Object Evaluator::evalProgram(std::vector<std::shared_ptr<StatementNode>> stmts, std::shared_ptr<Environment> env){
    Object result;
    for(auto stmt : stmts){
        result = eval(stmt, env);

        if(result.type == ObjectType::RETURN)
            return any_cast<Object>(result.value);

        if(result.type == ObjectType::ERROR)
            return result;
    }
    return result;
}

Object Evaluator::evalBlockStatement(std::vector<std::shared_ptr<StatementNode>> stmts, std::shared_ptr<Environment> env){
    Object result;
    for(auto stmt : stmts){
        result = eval(stmt, env);

        if(result.type == ObjectType::RETURN || result.type == ObjectType::ERROR)
            return result;
    }
    return result;
}

Object Evaluator::evalPrefixExpression(std::string oprator, Object right){
    if(oprator == "!")
        return evalBangOperatorExpression(right);
    if(oprator == "-")
        return evalMinusOperatorExpression(right);

    return raiseError(format("unknown operator: ", oprator, right.getType()));
}

Object Evaluator::evalInfixExpression(std::string oprator, Object left, Object right){
    if(left.type == ObjectType::NUMBER && right.type == ObjectType::NUMBER)
        return evalIntegerInfixExpression(oprator, left, right); 
    



    // std::addressof(left) cannot work as these values are passed by value
    // therefore copied around
    if(oprator == "=="){
        auto r = left.type == ObjectType::BOOLEAN && left.type == right.type 
            && any_cast<bool>(left.value) == any_cast<bool>(right.value);
        return nativeToBoolean(r);
    }

    if(oprator == "!="){
        return nativeToBoolean(std::addressof(left) != std::addressof(right));
        auto r = (left.type == ObjectType::BOOLEAN || right.type == ObjectType::BOOLEAN) 
            && left.type != right.type || any_cast<bool>(left.value) != any_cast<bool>(right.value);
        return nativeToBoolean(r);
    }

    if(left.type != right.type)
        return raiseError(format("type mismatch: ", left.getType(), " ", oprator, " ", right.getType()));
        
    return raiseError(format("unknown operator: ", left.getType(), " ", oprator, " ", right.getType()));
}

Object Evaluator::evalIfExpression(std::shared_ptr<IfExpression> expr, std::shared_ptr<Environment> env){
    auto condition = eval(expr->condition, env);
    if(condition.type == ObjectType::ERROR)
        return condition;

    if(isTruthy(condition))
        return eval(expr->consequence, env);
    else if(expr->alternative != nullptr)
        return eval(expr->alternative, env);
    
    return NIL_OBJ;
}

std::vector<Object> Evaluator::evalExpressions(std::shared_ptr<std::vector<std::shared_ptr<ExpressionNode>>> arguments, std::shared_ptr<Environment> env){
    std::vector<Object> args{};
    auto t = arguments.get();
    for(auto arg : *arguments){
        auto value = eval(arg, env);
        if(value.type == ObjectType::ERROR)
            return std::vector<Object>{ Object{ObjectType::ERROR, 0} };
        args.push_back(value);
    }
    return args;
}



// helpers

Object Evaluator::evalBangOperatorExpression(Object right){
    if(right.type == TRUE_OBJ.type 
        && any_cast<bool>(right.value) == any_cast<bool>(TRUE_OBJ.value))
        return FALSE_OBJ;
    if(right.type == FALSE_OBJ.type 
        && any_cast<bool>(right.value) == any_cast<bool>(FALSE_OBJ.value))
        return TRUE_OBJ;

    if(right.type == NIL_OBJ.type 
        && any_cast<double>(right.value) == any_cast<double>(NIL_OBJ.value))
        return TRUE_OBJ;

    return FALSE_OBJ;
}

Object Evaluator::nativeToBoolean(bool value){
    if(value)
        return TRUE_OBJ;
    return FALSE_OBJ;
}

Object Evaluator::evalMinusOperatorExpression(Object right){
    if(right.type != ObjectType::NUMBER)
        return raiseError(format("unknown operator: -", right.getType()));

    return Object{ObjectType::NUMBER, -(any_cast<double>(right.value))};
}

Object Evaluator::evalIntegerInfixExpression(std::string oprator, Object left, Object right){
    auto leftVal = any_cast<double>(left.value);
    auto rightVal = any_cast<double>(right.value);
    
    if(oprator == "+")
        return Object{ObjectType::NUMBER, (leftVal + rightVal)};
    
    if(oprator == "-")
        return Object{ObjectType::NUMBER, (leftVal - rightVal)};
    
    if(oprator == "*")
        return Object{ObjectType::NUMBER, (leftVal * rightVal)};

    if(oprator == "/")
        return Object{ObjectType::NUMBER, (leftVal / rightVal)};
    
    if(oprator == ">")
        return nativeToBoolean(leftVal > rightVal);
    
    if(oprator == "<")
        return nativeToBoolean(leftVal < rightVal);

    if(oprator == "==")
        return nativeToBoolean(leftVal == rightVal);

    if(oprator == "!=")
        return nativeToBoolean(leftVal != rightVal);

    return raiseError(format("unknown operator: ", left.getType(), " ", oprator, " ", right.getType()));
}

Object Evaluator::applyFunction(Object func, std::vector<Object> args){
    if(func.type != ObjectType::FUNCTION)
        return raiseError(format("not a function ", func.getType()));
    
    // create new env & bind params values
    auto funcObject = any_cast<FunctionObject>(func.value);
    auto env = std::make_shared<Environment>(funcObject.env);
    int i = 0;
    for(auto param: *funcObject.func->params){
        env->set(param.value, args[i++]);
    }

    auto value = eval(funcObject.func->body, env);
    if(value.type == ObjectType::RETURN)
        return any_cast<Object>(value);
    return value;
}

Object Evaluator::raiseError(std::string msg) {
    return Object{ObjectType::ERROR, msg};
}

bool Evaluator::isTruthy(Object obj){
    if(obj.type == ObjectType::NIL)
        return false;

    if(obj.type == ObjectType::BOOLEAN)
        return any_cast<bool>(obj.value);

    return true;
}

