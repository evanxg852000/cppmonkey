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
#include "main/object.hpp"
#include "main/fobject.hpp"
#include "main/environment.hpp"
#include "main/evaluator.hpp"


using namespace std;


Object testEval(string input){
        Lexer lexer{input};
        Parser parser{lexer};
        shared_ptr<Program> prog = parser.parseProgram();
        auto env = std::make_shared<Environment>();
        Evaluator evaluator{prog};
        return evaluator.execute(env);
}

TEST_CASE("Test Eval Integer Literal", "[evaluator]"){
    using TestItem = std::pair<string, double>;
     std::array<TestItem, 15> tests{ {
        make_pair("5;", 5), 
        make_pair("10", 10),
        make_pair("-5;", -5), 
        make_pair("-10", -10),
        make_pair("5 + 5 + 5 + 5 - 10", 10),
		make_pair("2 * 2 * 2 * 2 * 2", 32),
		make_pair("-50 + 100 + -50", 0),
		make_pair("5 * 2 + 10", 20),
		make_pair("5 + 2 * 10", 25),
		make_pair("20 + 2 * -10", 0),
		make_pair("50 / 2 * 2 + 10", 60),
		make_pair("2 * (5 + 10)", 30),
		make_pair("3 * 3 * 3 + 10", 37),
		make_pair("3 * (3 * 3) + 10", 37),
		make_pair("(5 + 10 * 2 + 15 / 3) * 2 + -10", 50)
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);

        REQUIRE(evaluated.type == ObjectType::NUMBER);
        REQUIRE(any_cast<double>(evaluated.value ) == test.second);
    }

}

TEST_CASE("Test Eval Boolean Literal", "[evaluator]"){
    using TestItem = std::pair<string, bool>;
     std::array<TestItem, 19> tests{ {
        make_pair("true;", true), 
        make_pair("false;", false),
        make_pair("1 < 2", true),
		make_pair("1 > 2", false),
		make_pair("1 < 1", false),
		make_pair("1 > 1", false),
		make_pair("1 == 1", true),
		make_pair("1 != 1", false),
		make_pair("1 == 2", false),
		make_pair("1 != 2", true),
		make_pair("true == true", true),
		make_pair("false == false", true),
		make_pair("true == false", false),
		make_pair("true != false", true),
		make_pair("false != true", true),
		make_pair("(1 < 2) == true", true),
		make_pair("(1 < 2) == false", false),
		make_pair("(1 > 2) == true", false),
		make_pair("(1 > 2) == false", true)
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);

        REQUIRE(evaluated.type == ObjectType::BOOLEAN);
        REQUIRE(any_cast<bool>(evaluated.value ) == test.second);
    }

}

TEST_CASE("Test Eval Bang Operator", "[evaluator]"){
    using TestItem = std::pair<string, bool>;
     std::array<TestItem, 6> tests{ {
        make_pair("!true;", false), 
        make_pair("!false;", true),
        make_pair("!5;", false),
        make_pair("!!true;", true),
        make_pair("!!false;", false),
        make_pair("!!5;", true)
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);

        REQUIRE(evaluated.type == ObjectType::BOOLEAN);
        REQUIRE(any_cast<bool>(evaluated.value ) == test.second);
    }
}


TEST_CASE("Test IfElse Expression", "[evaluator]"){
    using TestItem = std::pair<string, Object>;
     std::array<TestItem, 7> tests{ {
        make_pair("if (true) { 10 }", Object{ObjectType::NUMBER, 10.0}),
		make_pair("if (false) { 10 }", Object{ObjectType::NIL, 0.0}),
		make_pair("if (1) { 10 }", Object{ObjectType::NUMBER, 10.0}),
		make_pair("if (1 < 2) { 10 }", Object{ObjectType::NUMBER, 10.0}),
		make_pair("if (1 > 2) { 10 }", Object{ObjectType::NIL, 0.0}),
		make_pair("if (1 > 2) { 10 } else { 20 }", Object{ObjectType::NUMBER, 20.0}),
		make_pair("if (1 < 2) { 10 } else { 20 }", Object{ObjectType::NUMBER, 10.0})
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == test.second.type);
        REQUIRE(any_cast<double>(evaluated.value) == any_cast<double>(test.second.value));
    }

}

TEST_CASE("Test Return Statement", "[evaluator]"){
    using TestItem = std::pair<string, double>;
     std::array<TestItem, 6> tests{ {
        make_pair("return 10;", 10),
		make_pair("return 10; 9;", 10),
		make_pair("return 2 * 5; 9;", 10),
		make_pair("9; return 2 * 5; 9;", 10),
		make_pair("if (10 > 1) { return 10; }", 10),
        make_pair("if (10 > 1) { if (10 > 1) { return 10;} return 1;}", 10)
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == ObjectType::NUMBER);
        REQUIRE(any_cast<double>(evaluated.value) == test.second);
    }
}

TEST_CASE("Test Error Handling", "[evaluator]"){
    using TestItem = std::pair<string, std::string>;
     std::array<TestItem, 8> tests{ {
        make_pair("5 + true;", "type mismatch: NUMBER + BOOLEAN"),
		make_pair("5 + true; 5;", "type mismatch: NUMBER + BOOLEAN"),
		make_pair("-true", "unknown operator: -BOOLEAN"),
		make_pair("true + false;", "unknown operator: BOOLEAN + BOOLEAN"),
		make_pair("5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"),
		make_pair("if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"),
		make_pair("if (10 > 1) { if (10 > 1) { return true + false; } return 1;}", "unknown operator: BOOLEAN + BOOLEAN"),
        make_pair("foobar", "identifier not found: foobar")
        //make_pair("true + false + true + false;", "unknown operator: BOOLEAN + BOOLEAN"),
        //make_pair(" \"Hello\" - \"World\" ", "unknown operator: STRING - STRING"),
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == ObjectType::ERROR);
        REQUIRE(any_cast<string>(evaluated.value) == test.second);
    }
}

TEST_CASE("Test Let Statement", "[evaluator]"){
    using TestItem = std::pair<string, double>;
     std::array<TestItem, 4> tests{ {
        make_pair("let a = 5; a;", 5),
		make_pair("let a = 5 * 5; a;", 25),
		make_pair("let a = 5; let b = a; b;", 5),
		make_pair("let a = 5; let b = a; let c = a + b + 5; c;", 15)
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == ObjectType::NUMBER);
        REQUIRE(any_cast<double>(evaluated.value) == test.second);
    }
}

TEST_CASE("Test Function Object", "[evaluator]"){
    Object evaluated = testEval("fn(x){ x + 2; }; ");

    REQUIRE(evaluated.type == ObjectType::FUNCTION);
    auto fnLitPtr = any_cast<FunctionObject>(evaluated.value).func;

    REQUIRE(fnLitPtr->params->size() == 1);
    REQUIRE(fnLitPtr->params->at(0).tokenLiteral() == "x");
}


TEST_CASE("Test Function Call", "[evaluator]"){
    using TestItem = std::pair<string, double>;
     std::array<TestItem, 6> tests{ {
        make_pair("let identity = fn(x) { x; }; identity(5);", 5),
		make_pair("let identity = fn(x) { return x; }; identity(5);", 5),
		make_pair("let double = fn(x) { x * 2; }; double(5);", 10),
		make_pair("let add = fn(x, y) { x + y; }; add(5, 5);", 10),
		make_pair("let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20),
		make_pair("fn(x) { x; }(5)", 5)
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == ObjectType::NUMBER);
        REQUIRE(any_cast<double>(evaluated.value) == test.second);
    }
}


TEST_CASE("Test Closure", "[evaluator]"){
    const char* input = R"STRING(
let newAdder = fn(x) {
  fn(y) { x + y };
};

let addTwo = newAdder(2);
addTwo(2);
)STRING";

    Object evaluated = testEval(string{input});
    REQUIRE(evaluated.type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(evaluated.value) == 4);
}