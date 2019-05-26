#include "vendor/catch2.hpp"

#include "main/core.hpp"
#include "main/token.hpp"
#include "main/lexer.hpp"

using namespace std;

TEST_CASE("Basic lexing", "[lexer]"){
    Token tokens[] = {
        makeToken(TokenType::EQUAL, "="),
        makeToken(TokenType::PLUS,  "+"),
        makeToken(TokenType::LEFT_PAREN, "("),
        makeToken(TokenType::RIGHT_PAREN, ")"),
        makeToken(TokenType::LEFT_BRACE, "{"),
        makeToken(TokenType::RIGHT_BRACE, "}"),
        makeToken(TokenType::COMMA, ","),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::EOS, ""),
    };

    //string input = ;
    Lexer lexer{"=+(){},;"};
    for(auto expect: tokens){
        auto tok = lexer.nextToken();
        REQUIRE(expect.type == tok.type);
        REQUIRE(expect.literal == tok.literal);
    }

}

TEST_CASE("Advanced lexing", "lexer"){
    const char* input = R"STRING(
let five = 5;
let ten = 10;

let add = fn(x, y) {
    x + y;
};

let result = add(five, ten);
!-/*5;
5 < 10 > 5;

if (5 < 10) {
	return true;
} else {
	return false;
};

10 == 10;
10 != 9;
)STRING";

/*
"foobar"
"foo bar"
[1, 2];
{"foo": "bar"}
*/

    Token tokens[] = {
        makeToken(TokenType::LET, "let"),
        makeToken(TokenType::IDENT, "five"),
        makeToken(TokenType::EQUAL, "="),
        makeToken(TokenType::NUMBER, "5"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::LET, "let"),
        makeToken(TokenType::IDENT, "ten"),
        makeToken(TokenType::EQUAL, "="),
        makeToken(TokenType::NUMBER, "10"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::LET, "let"),
        makeToken(TokenType::IDENT, "add"),
        makeToken(TokenType::EQUAL, "="),
        makeToken(TokenType::FUNC, "fn"),
        makeToken(TokenType::LEFT_PAREN, "("),
        makeToken(TokenType::IDENT, "x"),
        makeToken(TokenType::COMMA, ","),
        makeToken(TokenType::IDENT, "y"),
        makeToken(TokenType::RIGHT_PAREN, ")"),
        makeToken(TokenType::LEFT_BRACE, "{"),
        makeToken(TokenType::IDENT, "x"),
        makeToken(TokenType::PLUS, "+"),
        makeToken(TokenType::IDENT, "y"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::RIGHT_BRACE, "}"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::LET, "let"),
        makeToken(TokenType::IDENT, "result"),
        makeToken(TokenType::EQUAL, "="),
        makeToken(TokenType::IDENT, "add"),
        makeToken(TokenType::LEFT_PAREN, "("),
        makeToken(TokenType::IDENT, "five"),
        makeToken(TokenType::COMMA, ","),
        makeToken(TokenType::IDENT, "ten"),
        makeToken(TokenType::RIGHT_PAREN, ")"),
        makeToken(TokenType::SEMICOLON, ";"),

        makeToken(TokenType::BANG, "!"),
        makeToken(TokenType::MINUS, "-"),
        makeToken(TokenType::SLASH, "/"),
        makeToken(TokenType::STAR, "*"),
        makeToken(TokenType::NUMBER, "5"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::NUMBER, "5"),
        makeToken(TokenType::LESS, "<"),
        makeToken(TokenType::NUMBER, "10"),
        makeToken(TokenType::GREATER, ">"),
        makeToken(TokenType::NUMBER, "5"),
        makeToken(TokenType::SEMICOLON, ";"),

        makeToken(TokenType::IF, "if"),
        makeToken(TokenType::LEFT_PAREN, "("),
        makeToken(TokenType::NUMBER, "5"),
        makeToken(TokenType::LESS, "<"),
        makeToken(TokenType::NUMBER, "10"),
        makeToken(TokenType::RIGHT_PAREN, ")"),
        makeToken(TokenType::LEFT_BRACE, "{"),
        makeToken(TokenType::RETURN, "return"),
        makeToken(TokenType::TRUE, "true"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::RIGHT_BRACE, "}"),
        makeToken(TokenType::ELSE, "else"),
        makeToken(TokenType::LEFT_BRACE, "{"),
        makeToken(TokenType::RETURN, "return"),
        makeToken(TokenType::FALSE, "false"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::RIGHT_BRACE, "}"),
        makeToken(TokenType::SEMICOLON, ";"),

        makeToken(TokenType::NUMBER, "10"),
        makeToken(TokenType::EQUAL_EQUAL, "=="),
        makeToken(TokenType::NUMBER, "10"),
        makeToken(TokenType::SEMICOLON, ";"),
        makeToken(TokenType::NUMBER, "10"),
        makeToken(TokenType::BANG_EQUAL, "!="),
        makeToken(TokenType::NUMBER, "9"),
        makeToken(TokenType::SEMICOLON, ";"),

        makeToken(TokenType::EOS, ""),
    };

    Lexer lexer{string(input)};
    for(auto expect: tokens){
        auto tok = lexer.nextToken();
        REQUIRE(expect.type == tok.type);
        REQUIRE(expect.literal == tok.literal);
    }

}