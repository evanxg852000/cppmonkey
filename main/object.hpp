#if !defined(OBJECT_H)
#define OBJECT_H

#include <string>
#include <memory>
#include <vector>
#include <any>
#include <functional>

enum class ObjectType {
    NIL,
    NUMBER,
    STRING,
    BOOLEAN,
    RETURN,
    ERROR,
    UNDEFINED, // Sorry for my weird Javascriptaculous behavior 
    FUNCTION,
    BUILTIN_OBJECT,
    BUILTIN_FUNCTION,
    ARRAY,
    HASH,

};

std::string to_string(const ObjectType& type);

class Object {
public:
    Object() = default;
    Object(ObjectType otype, std::any val): type{otype}, value{val} {};
    virtual ~Object() = default;

    virtual std::string inspect() const;
    virtual std::string getType() const{
        using std::to_string;
        return to_string(this->type);
    }
    virtual std::string hashKey() const;

    using BuiltInFunction = std::function< Object( std::vector<Object> )>;

    ObjectType type;
    std::string _tag;
    std::any value;
};

#endif // OBJECT_H