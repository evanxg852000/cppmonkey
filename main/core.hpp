#if !defined(CORE_H)
#define CORE_H

#include <string>

int add(int, int);

class Runner {
private:
    bool hasError;
    void run();

public:
    void runRepl(std::string);
    void runFile(std::string);
};

#endif // CORE_H
