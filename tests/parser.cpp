#include <iostream>
#include <array>
#include <tuple>


#include "vendor/catch2.hpp"

#include "main/core.hpp"
#include "main/token.hpp"
#include "main/lexer.hpp"
#include "main/ast.hpp"
#include "main/parser.hpp"
#include "main/utils.hpp"

using namespace std;

void checkParseError(Parser& p){
    if(p.getErrors().size() > 0) {
        for(auto str : p.getErrors())
            WARN(str);
    }
    REQUIRE(p.getErrors().size() == 0);
}

void checkNumberLiteral(shared_ptr<ExpressionNode> expr, double value){
    auto number = std::dynamic_pointer_cast<NumberLiteral>(expr);
    REQUIRE(number->value == value);
    REQUIRE(number->tokenLiteral() == ::toStringWithPrecicion(value, 0));
}

void checkBooleanLiteral(shared_ptr<ExpressionNode> expr, bool value){
    auto boolean = std::dynamic_pointer_cast<BooleanLiteral>(expr);
    REQUIRE(boolean->value == value);
    REQUIRE(boolean->tokenLiteral() == ((value)? "true" : "false"));
}

TEST_CASE("Test LetStatement", "[parser]"){
    const char* input = R"STRING(
let x = 5;
let y = 10;
let foobar = 838383;
    )STRING";

    string ids[] = {"x", "y", "foobar"};
    double vals[] = {5, 10, 838383};

    Lexer lexer{string{input}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 3);
    for(size_t i = 0; i < prog->statements.size(); ++i){
        auto stmt = std::dynamic_pointer_cast<LetStatement>(prog->statements[i]);
        REQUIRE(stmt != nullptr);
        REQUIRE(stmt->name.tokenLiteral() == ids[i]);

        auto expr = std::dynamic_pointer_cast<NumberLiteral>(stmt->value);
        REQUIRE(expr != nullptr);
        REQUIRE(expr->value == vals[i]);
    }
}

TEST_CASE("Test returnStatement", "[parser]"){
    const char* input = R"STRING(
return 5;
return 10;
return 993322;
    )STRING";

    double vals[] = {5, 10, 993322};
    
    Lexer lexer{string{input}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 3);
    for(size_t i = 0; i < prog->statements.size(); ++i){
        auto retStmt = std::dynamic_pointer_cast<ReturnStatement>(prog->statements[i]);
        REQUIRE(retStmt != nullptr);
        REQUIRE(retStmt->tokenLiteral() == "return");

        auto expr = std::dynamic_pointer_cast<NumberLiteral>(retStmt->value);
        REQUIRE(expr != nullptr);
        REQUIRE(expr->value == vals[i]);
    }
}

TEST_CASE("Test IdentifierExpression", "[parser]"){
    Lexer lexer{string{"foobar;"}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "foobar");
    auto ident = std::dynamic_pointer_cast<Identifier>(stmt->expression);
    REQUIRE(ident->value == "foobar");
    REQUIRE(ident->tokenLiteral() == "foobar");
}


TEST_CASE("Test NumberLiteral", "[parser]"){
    Lexer lexer{string{"5;"}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);
    
    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "5");
    auto number = std::dynamic_pointer_cast<NumberLiteral>(stmt->expression);
    REQUIRE(number->value == 5);
    REQUIRE(number->tokenLiteral() == "5");
}

TEST_CASE("Test StringLiteral", "[parser]"){
    const char* input = R"STRING( "Hello World!" )STRING";
    Lexer lexer{string{input}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);
    
    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "Hello World!");
    auto str = std::dynamic_pointer_cast<StringLiteral>(stmt->expression);
    REQUIRE(str->value == "Hello World!");
    REQUIRE(str->tokenLiteral() == "Hello World!");
}

TEST_CASE("Test PrefixExpression", "[parser]"){
    using TestItem = std::tuple<string, string, double>;
     std::array<TestItem, 4> tests{ {
        make_tuple("!5;", "!", 5), 
        make_tuple("-15;", "-", 15),
        make_tuple("!false;", "!", false),
        make_tuple("!true;", "!", true)
    }};

    for(auto test : tests){
        Lexer lexer{string{std::get<0>(test)}};
        Parser parser{lexer};
        shared_ptr<Program> prog = parser.parseProgram();
        checkParseError(parser);

        REQUIRE(prog != nullptr);
        REQUIRE(prog->statements.size() == 1);

        auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
        REQUIRE(stmt != nullptr);
        auto expr = std::dynamic_pointer_cast<PrefixExpression>(stmt->expression);
        REQUIRE(expr->oprator == std::get<1>(test));
        if(std::dynamic_pointer_cast<NumberLiteral>(expr->right) != nullptr)
            checkNumberLiteral(expr->right, std::get<2>(test));
        else
            checkBooleanLiteral(expr->right, std::get<2>(test));
    } 
}

TEST_CASE("Test InfixExpression", "[parser]"){
    using TestItem = std::tuple<string, double, string, double>;
    std::array<TestItem, 11> tests{ {
        make_tuple("5 + 5;", 5, "+", 5),
        make_tuple("5 - 5;", 5, "-", 5),
        make_tuple("5 * 5;", 5, "*", 5),
        make_tuple("5 / 5;", 5, "/", 5),
        make_tuple("5 > 5;", 5, ">", 5),
        make_tuple("5 < 5;", 5, "<", 5),
        make_tuple("5 == 5;", 5, "==", 5),
        make_tuple("5 != 5;", 5, "!=", 5),
        make_tuple("true == true", true, "==", true),
        make_tuple("true != false", true, "!=", false),
        make_tuple("false == false", false, "==", false)
    } };

    for(auto test : tests){
        Lexer lexer{string{std::get<0>(test)}};
        Parser parser{lexer};
        shared_ptr<Program> prog = parser.parseProgram();
        checkParseError(parser);

        REQUIRE(prog != nullptr);
        REQUIRE(prog->statements.size() == 1);

        auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
        REQUIRE(stmt != nullptr);
        auto expr = std::dynamic_pointer_cast<InfixExpression>(stmt->expression);
        
        //check left
        if(std::dynamic_pointer_cast<NumberLiteral>(expr->left) != nullptr)
            checkNumberLiteral(expr->left, std::get<1>(test));
        else
            checkBooleanLiteral(expr->left, std::get<1>(test));

        REQUIRE(expr->oprator == std::get<2>(test));

        // //check rigth
        if(std::dynamic_pointer_cast<NumberLiteral>(expr->right) != nullptr)
            checkNumberLiteral(expr->right, std::get<3>(test));
        else
            checkBooleanLiteral(expr->right, std::get<3>(test));
    } 
}

TEST_CASE("Test Operator Precedence Parsing", "[parser]"){
    std::array<std::pair<string, string>, 26> tests = {{
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a + b) + c)"},
        {"a + b - c", "((a + b) - c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"true", "true"},
        {"false", "false"},
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
        {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
        {"(5 + 5) * 2", "((5 + 5) * 2)"},
        {"2 / (5 + 5)", "(2 / (5 + 5))"},
        {"-(5 + 5)", "(-(5 + 5))"},
        {"!(true == true)", "(!(true == true))"},
        {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
        {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
        {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
        {"a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
        {"add(a * b[2], b[1], 2 * [1, 2][1])", "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"}
    }};

    for(auto test : tests){
        Lexer lexer{test.first};
        Parser parser{lexer};
        shared_ptr<Program> prog = parser.parseProgram();
        checkParseError(parser);

        REQUIRE(prog != nullptr);
        REQUIRE(prog->toString() == test.second);
    }
}

TEST_CASE("Test If Expression Parsing", "[parser]"){
    Lexer lexer{string{"if (x < y) { x }"}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);
    
    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);  
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "if"); 
    auto expr = dynamic_pointer_cast<IfExpression>(stmt->expression); 
    REQUIRE(expr != nullptr);

    REQUIRE(expr->condition != nullptr);
    REQUIRE(expr->condition->toString() == "(x < y)");

    REQUIRE(expr->consequence != nullptr);
    REQUIRE(expr->consequence->statements.size() == 1);
    REQUIRE(expr->consequence->statements[0]->toString() == "x");

    REQUIRE(expr->alternative == nullptr);
}

TEST_CASE("Test If Else Expression  Parsing", "[parser]"){
    Lexer lexer{string{"if (x < y) { x } else { y }"}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);
    
    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);  
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "if"); 
    auto expr = dynamic_pointer_cast<IfExpression>(stmt->expression); 
    REQUIRE(expr != nullptr);

    REQUIRE(expr->condition != nullptr);
    REQUIRE(expr->condition->toString() == "(x < y)");

    REQUIRE(expr->consequence != nullptr);
    REQUIRE(expr->consequence->statements.size() == 1);
    REQUIRE(expr->consequence->statements[0]->toString() == "x");

    REQUIRE(expr->alternative != nullptr);
    REQUIRE(expr->alternative->statements.size() == 1);
    REQUIRE(expr->alternative->statements[0]->toString() == "y");
}

TEST_CASE("Test Function Literal Expression  Parsing", "[parser]"){
    Lexer lexer{string{"fn (x , y) { x + y; }"}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);  
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "fn");
    auto func = dynamic_pointer_cast<FunctionLiteral>(stmt->expression); 
    REQUIRE(func != nullptr);

    REQUIRE(func->params->size() == 2);
    REQUIRE(func->params->at(0).toString() == "x");
    REQUIRE(func->params->at(1).toString() == "y");

    REQUIRE(func->body->statements.size() == 1);
    REQUIRE(func->body->toString() == "(x + y)");
    auto bodyStmt = std::dynamic_pointer_cast<ExpressionStatement>(func->body->statements[0]);
    REQUIRE(bodyStmt != nullptr);

    auto expr = std::dynamic_pointer_cast<InfixExpression>(bodyStmt->expression);
    REQUIRE(expr->left->toString() == "x");
    REQUIRE(expr->oprator == "+");
    REQUIRE(expr->right->toString() == "y");
}

TEST_CASE("Test Function Literal Expression Params Parsing", "[parser]"){
    using TestItem = std::pair<string, std::vector<string>>;
    std::array<TestItem, 3> tests{ {
        { "fn() {};", vector<string>{} },
        { "fn(x) {};", vector<string>{ "x" } },
        { "fn(x, y, z) {};", vector<string>{"x", "y", "z"} }
    } };

    for(auto test : tests){
        Lexer lexer{test.first};
        Parser parser{lexer};
        shared_ptr<Program> prog = parser.parseProgram();
        checkParseError(parser);
        
        REQUIRE(prog != nullptr);
        REQUIRE(prog->statements.size() == 1);

        auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
        REQUIRE(stmt != nullptr);
        auto func = std::dynamic_pointer_cast<FunctionLiteral>(stmt->expression);
        REQUIRE(func != nullptr);
        REQUIRE(func->params->size() == test.second.size());
        auto p = func->params->begin();
        int i = 0;
        while(p != func->params->end()){
            REQUIRE(p->tokenLiteral() == test.second[i]);
            i++;
            p++;
        }
    }
}

TEST_CASE("Test Call Expression Parsing", "[parser]"){
    Lexer lexer{string{"add(1, 2 * 3, 4 + 5);"}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);  
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "add");
    auto call = dynamic_pointer_cast<CallExpression>(stmt->expression); 
    REQUIRE(call != nullptr);

    auto func = dynamic_pointer_cast<Identifier>(call->function);
    REQUIRE(func != nullptr);
    REQUIRE(func->value == "add");

    REQUIRE(call->arguments->size() == 3);
    //REQUIRE(func->params->at(0).toString() == "x");
    //REQUIRE(func->params->at(1).toString() == "y");
}

TEST_CASE("Test Call Expression Argument Parsing", "[parser]"){
using TestItem = std::tuple<string, string, int>;
    std::array<TestItem, 3> tests{ {
        make_tuple("add(x, y);", "add", 2),
        make_tuple("sum(w, x, y, z);", "sum", 4),
        make_tuple("foo();", "foo", 0)
    } };

    for(auto test : tests){
        Lexer lexer{std::get<0>(test)};
        Parser parser{lexer};
        shared_ptr<Program> prog = parser.parseProgram();
        checkParseError(parser);

        REQUIRE(prog != nullptr);
        REQUIRE(prog->statements.size() == 1);  
        auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
        REQUIRE(stmt != nullptr);
        REQUIRE(stmt->tokenLiteral() == std::get<1>(test));
        auto call = dynamic_pointer_cast<CallExpression>(stmt->expression); 
        REQUIRE(call != nullptr);

        auto func = dynamic_pointer_cast<Identifier>(call->function);
        REQUIRE(func != nullptr);
        REQUIRE(func->value == std::get<1>(test));
        REQUIRE(call->arguments->size() == std::get<2>(test));
    }

}

TEST_CASE("Test Array Literal", "[parser]"){
    Lexer lexer{"[ 1, 2 * 2, 3 + 3 ]"};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);  
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "[");
    auto array = dynamic_pointer_cast<ArrayLiteral>(stmt->expression); 
    REQUIRE(array != nullptr);

    REQUIRE(array->items.size() == 3);
    //TODO ... check more internal
}


TEST_CASE("Test Hash Literal", "[parser]"){
    const char* input = R"STRING( { "one": 1, "two": 2, "three": 3 } )STRING";
    Lexer lexer{string{input}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);  
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "{");
    auto map = dynamic_pointer_cast<HashLiteral>(stmt->expression); 
    REQUIRE(map != nullptr);

    REQUIRE(map->entries.size() == 3);
    //TODO ... check more internal

}

TEST_CASE("Test Index Expression", "[parser]"){
    Lexer lexer{string{"arr[2 * 3];"}};
    Parser parser{lexer};
    shared_ptr<Program> prog = parser.parseProgram();
    checkParseError(parser);

    REQUIRE(prog != nullptr);
    REQUIRE(prog->statements.size() == 1);  
    auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(prog->statements[0]);
    REQUIRE(stmt != nullptr);
    REQUIRE(stmt->tokenLiteral() == "arr");
    auto indexExpr = dynamic_pointer_cast<IndexExpression>(stmt->expression); 
    REQUIRE(indexExpr != nullptr);

    auto left = dynamic_pointer_cast<Identifier>(indexExpr->left);
    REQUIRE(left != nullptr);
    REQUIRE(left->toString() == "arr");

    auto idx = dynamic_pointer_cast<InfixExpression>(indexExpr->index);
    REQUIRE(idx != nullptr);
}