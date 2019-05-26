#include <iostream>
#include <string>

#include "core.hpp"

using namespace std;


int main(int argc, char const *argv[]){
	Runner runner;

	if(argc <= 1){
		runner.runRepl("cMK/> ");
	} else {
		string file = argv[1];
		runner.runFile(file);
	}
    return 0;
}