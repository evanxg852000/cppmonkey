#if !defined(EVALUATOR_H)
#define EVALUATOR_H


#include <string>
#include <memory>
#include <vector>
#include <any>

#include "ast.hpp"
#include "object.hpp"
#include "environment.hpp"

class Evaluator {
public:
    Evaluator(std::shared_ptr<Program> ast): program{ast} {};
    virtual ~Evaluator() = default;
    Object execute(std::shared_ptr<Environment> env){ return eval(this->program, env); }
    
private:
    const Object NIL_OBJ = Object{ObjectType::NIL, 0.0};
    const Object TRUE_OBJ = Object{ObjectType::BOOLEAN, true};
    const Object FALSE_OBJ = Object{ObjectType::BOOLEAN, false};

    std::shared_ptr<Program> program;

    Object eval(std::shared_ptr<AstNode>, std::shared_ptr<Environment>);
    Object evalProgram(std::vector<std::shared_ptr<StatementNode>>, std::shared_ptr<Environment>);
    Object evalBlockStatement(std::vector<std::shared_ptr<StatementNode>>, std::shared_ptr<Environment>);
    Object evalPrefixExpression(std::string, Object);
    Object evalInfixExpression(std::string, Object, Object);
    Object evalIfExpression(std::shared_ptr<IfExpression>, std::shared_ptr<Environment>);
    std::vector<Object> evalExpressions(std::shared_ptr<std::vector<std::shared_ptr<ExpressionNode>>>, 
        std::shared_ptr<Environment>);

    Object evalBangOperatorExpression(Object);
    Object nativeToBoolean(bool);
    Object evalMinusOperatorExpression(Object);
    Object evalIntegerInfixExpression(std::string, Object, Object);
    Object applyFunction(Object, std::vector<Object>);
    Object raiseError(std::string);
    bool isTruthy(Object);
    
    
};



#endif // EVALUATOR_H