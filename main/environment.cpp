

#include "object.hpp"
#include "environment.hpp"

using namespace std;

Object Environment::set(string name, Object value) {
    store[name]= value;
    return value; 
}


Object Environment::get(string name) {
    if (store.find(name) == store.end()){
        if(parent == nullptr)
            return Object{ObjectType::UNDEFINED, 0};
        return parent->get(name);
    }
    return store[name]; 
}