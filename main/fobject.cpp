#include <string>
#include <memory>
#include <vector>
#include <any>

#include "token.hpp"
#include "ast.hpp"
#include "environment.hpp"
#include "fobject.hpp"

FunctionObject::FunctionObject(std::shared_ptr<FunctionLiteral> fn, std::shared_ptr<Environment> env):
    func{fn}, env{env} {
        
    };
    