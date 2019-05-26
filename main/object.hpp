#if !defined(OBJECT_H)
#define OBJECT_H

#include <string>
#include <memory>
#include <vector>
#include <any>

enum class ObjectType {
    NIL,
    NUMBER,
    BOOLEAN,
    RETURN,
    ERROR,
    UNDEFINED, // Sorry for my weird Javascriptaculous behavior 
    FUNCTION
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

    ObjectType type;
    std::any value;
};


#endif // OBJECT_H