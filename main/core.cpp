#include <iostream>
#include <string>
#include <memory>

#include "core.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "environment.hpp"
#include "evaluator.hpp"

using namespace std;

int add(int a, int b){
    return a + b;
}

void Runner::runRepl(string prompt){
const string MONKEY_FACE= R"STRING(
  .--.  .-"     "-.  .--.
  / .. \/  .-. .-.  \/ .. \
 | |  '|  /   Y   \  |'  | |
 | \   \  \ 0 | 0 /  /   / |
  \ '- ,\.-"`` ``"-./, -' /
   `'-' /_   ^ ^   _\ '-'`
       |  \._   _./  |
       \   \ `~` /   /
        '._ '-=-' _.'
           '~---~'
)STRING";

    string line;
    auto env = std::make_shared<Environment>();
    while(true){
        cout<< prompt;
        getline(cin, line);
        if(line == "exit")
            break;
        
        Lexer lexer{string(line)};
        Parser parser{lexer};
        shared_ptr<Program> program = parser.parseProgram();
        if(parser.getErrors().size() > 0) {
            cout<< MONKEY_FACE << endl;
            for(auto str : parser.getErrors()){
                cout<< str << endl;
            }
            continue;
        }

        Evaluator evaluator{program};
        auto evaluated = evaluator.execute(env);
        cout << evaluated.inspect() << endl;

        // Token token = lexer.nextToken();
        // while(token.type != TokenType::EOS){
        //     cout << tokenToString(token) <<endl;
        //     token = lexer.nextToken();
        // }
        //cout<< line << endl;
    }
}


void Runner::runFile(string file){
    //TODO
    cout<< "TODO: read and run file!" << endl;
}