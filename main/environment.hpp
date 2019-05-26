#if !defined(ENVIRONMENT_H)
#define ENVIRONMENT_H

#include <string>
#include <memory>
#include <unordered_map>

#include "object.hpp"

class Environment {
public:
    Environment(): store{} {};
    Environment(std::shared_ptr<Environment> outer): store{}, parent{outer} {};
    virtual ~Environment() = default;
    Object set(std::string, Object);
    Object get(std::string);

private:
    std::unordered_map<std::string, Object> store;
    std::shared_ptr<Environment> parent;
};


#endif // ENVIRONMENT_H