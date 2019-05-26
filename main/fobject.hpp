#if !defined(FUNCTION_OBJECT_H)
#define FUNCTION_OBJECT_H

#include <memory>

class Environment;
class FunctionLiteral;

class FunctionObject {
public:
    FunctionObject(std::shared_ptr<FunctionLiteral>, std::shared_ptr<Environment>);
    virtual ~FunctionObject() = default;

    std::shared_ptr<FunctionLiteral> func;
    std::shared_ptr<Environment> env;
};


#endif // FUNCTION_OBJECT_H