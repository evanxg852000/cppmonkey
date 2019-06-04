#include <iostream>
#include <array>
#include <tuple>
#include <any>



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

string hashKeyOf(ObjectType t, std::any v){
    return Object{t, v}.hashKey();
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

TEST_CASE("Test String", "[evaluator]"){
    using TestItem = std::pair<string, string>;
    std::array<TestItem, 2> tests{ {
        make_pair(" \"Hello World!\" ", "Hello World!"),
        make_pair(" \"Hello\" + \" \" + \"World!\" ", "Hello World!"), 
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == ObjectType::STRING);
        REQUIRE(any_cast<string>(evaluated.value ) == test.second);
    }

    const char* input = R"STRING( "Hello World!" )STRING";
    Object evaluated = testEval(string{input});
    REQUIRE(evaluated.type == ObjectType::STRING);
    REQUIRE(any_cast<string>(evaluated.value) == "Hello World!");
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
     std::array<TestItem, 11> tests{ {
        make_pair("5 + true;", "type mismatch: NUMBER + BOOLEAN"),
		make_pair("5 + true; 5;", "type mismatch: NUMBER + BOOLEAN"),
		make_pair("-true", "unknown operator: -BOOLEAN"),
		make_pair("true + false;", "unknown operator: BOOLEAN + BOOLEAN"),
		make_pair("5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"),
		make_pair("if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"),
		make_pair("if (10 > 1) { if (10 > 1) { return true + false; } return 1;}", "unknown operator: BOOLEAN + BOOLEAN"),
        make_pair("foobar", "identifier not found: foobar"),
        make_pair("true + false + true + false;", "unknown operator: BOOLEAN + BOOLEAN"),
        make_pair(" \"Hello\" - \"World\" ", "unknown operator: STRING - STRING"),
        make_pair(R"STRING({"name": "Monkey"}[fn(x) { x }];)STRING", "unusable as hash key: FUNCTION")
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

TEST_CASE("Test Builtins ", "[evaluator]"){
    using TestItem = std::pair<string, double>;
    std::array<TestItem, 5> tests{ {
        make_pair("PI;", 3.14),
		make_pair("PI + 10;", 13.14),
		make_pair("len(\"\");", 0),
        make_pair("len(\"four\");", 4),
        make_pair("len([2, 3 + 4])", 2),
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == ObjectType::NUMBER);
        REQUIRE(any_cast<double>(evaluated.value) == test.second);
    }

    using TestItemError = std::pair<string, string>;
    std::array<TestItemError, 2> errotTests{ {
        make_pair("len(1);", "argument to len not supported, got NUMBER"),
        make_pair("len(\"one\", \"two\"); ", "wrong number of argument. got=2, want=1")
    }};
    for(auto test : errotTests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == ObjectType::ERROR);
        REQUIRE(any_cast<string>(evaluated.value) == test.second);
    }   
}

TEST_CASE("Test Array Literal Evaluation", "[evaluator]"){
    Object evaluated = testEval("[ 1, 2 * 2, 3 + 3 ]");
    REQUIRE(evaluated.type == ObjectType::ARRAY);
    auto items = any_cast<vector<Object>>(evaluated.value);
    REQUIRE(items.size() == 3);

    REQUIRE(items[0].type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(items[0].value) == 1);
    REQUIRE(items[1].type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(items[1].value) == 4);
    REQUIRE(items[2].type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(items[2].value) == 6);
}

TEST_CASE("Test Array Index Expression Evaluation", "[evaluator]"){
    using TestItem = std::pair<string, Object>;
    std::array<TestItem, 10> tests{ {
        make_pair("[1, 2, 3][0]", Object{ObjectType::NUMBER, 1.0}),
		make_pair("[1, 2, 3][1]", Object{ObjectType::NUMBER, 2.0}),
		make_pair("[1, 2, 3][2]", Object{ObjectType::NUMBER, 3.0}),
		make_pair("let i = 0; [1][i];", Object{ObjectType::NUMBER, 1.0}),
		make_pair("[1, 2, 3][1 + 1];", Object{ObjectType::NUMBER, 3.0}),
		make_pair("let myArray = [1, 2, 3]; myArray[2];", Object{ObjectType::NUMBER, 3.0}),
		make_pair( "let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", Object{ObjectType::NUMBER, 6.0}),
		make_pair( "let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i];", Object{ObjectType::NUMBER, 2.0}),
		make_pair("[1, 2, 3][3]", Object{ObjectType::NIL, 0.0}),
		make_pair("[1, 2, 3][-1]", Object{ObjectType::NIL, 0.0})
    }};

    for(auto test : tests){
        Object evaluated = testEval(test.first);
        REQUIRE(evaluated.type == test.second.type);
        REQUIRE(any_cast<double>(evaluated.value) == any_cast<double>(test.second.value));
    }

}


TEST_CASE("Test Hash Literal Evaluation", "[evaluator]"){
    const char* input = R"STRING(
let two = "two";
{
    "one": 10 - 9,
    two: 1 + 1,
    "thr" + "ee": 6 / 2,
    4: 4,
    true: 5,
    false: 6
};
)STRING";

    Object evaluated = testEval(string{input});
    REQUIRE(evaluated.type == ObjectType::HASH);
    auto entries = any_cast<unordered_map<string, pair<Object, Object>>>(evaluated.value);
    REQUIRE(entries.size() == 6);

    auto key = hashKeyOf(ObjectType::STRING, string{"one"});
    REQUIRE(entries[key].second.type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(entries[key].second.value) == 1);

    key = hashKeyOf(ObjectType::STRING, string{"two"});
    REQUIRE(entries[key].second.type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(entries[key].second.value) == 2);

    key = hashKeyOf(ObjectType::STRING, string{"three"});
    REQUIRE(entries[key].second.type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(entries[key].second.value) == 3);

    key = hashKeyOf(ObjectType::NUMBER, 4.0);
    REQUIRE(entries[key].second.type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(entries[key].second.value) == 4);

    key = hashKeyOf(ObjectType::BOOLEAN, true);
    REQUIRE(entries[key].second.type == ObjectType::NUMBER);
    REQUIRE(any_cast<double>(entries[key].second.value) == 5);
}


TEST_CASE("Test Hash Index Expression Evaluation", "[evaluator]"){
    using TestItem = std::pair<const char*, Object>;
    std::array<TestItem, 7> tests{ {
        make_pair(R"STRING({"foo": 5}["foo"])STRING", Object{ObjectType::NUMBER, 5.0}),
	    make_pair(R"STRING({"foo": 5}["bar"])STRING", Object{ObjectType::NIL, 0.0}),
        make_pair(R"STRING(let key = "foo"; {"foo": 5}[key])STRING", Object{ObjectType::NUMBER, 5.0}),
        make_pair(R"STRING({}["foo"])STRING", Object{ObjectType::NIL, 0.0}),
        make_pair(R"STRING({5: 5}[5])STRING", Object{ObjectType::NUMBER, 5.0}),
        make_pair(R"STRING({true: 5}[true])STRING", Object{ObjectType::NUMBER, 5.0}),
        make_pair(R"STRING({false: 5}[false])STRING", Object{ObjectType::NUMBER, 5.0}),
    }};

    for(auto test : tests){
        Object evaluated = testEval(string{test.first});
        REQUIRE(evaluated.type == test.second.type);
        REQUIRE(any_cast<double>(evaluated.value) == any_cast<double>(test.second.value));
    }
	
}