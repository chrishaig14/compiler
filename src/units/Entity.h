//
// Created by chris on 6/4/21.
//

#ifndef XLANG_ENTITY_H
#define XLANG_ENTITY_H

enum class E_TYPE {
    CLASS, CONST_FUNCTION, FUNCTION_VALUE, OBJECT_VALUE, PACKAGE, MODULE
};

class Entity {
public:
    E_TYPE type;

    Entity(E_TYPE t);
};

#endif //XLANG_ENTITY_H
