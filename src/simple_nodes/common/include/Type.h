//
// Created by chris on 1/8/20.
//

#ifndef Type_H
#define Type_H


#include <string>
#include <vector>

#include "../../../types.h"
#include <iostream>
#include "../../sem.h"
#include "../../../json/json_fwd.hpp"

enum class sem::Kind {
    OBJECT, FUNCTION, UNKNOWN
};

class UnknownType;

class Class;

class sem::Type {
public:
    Kind kind;

    virtual std::string to_string() const = 0;
    virtual std::string actual_to_string() const = 0;

    virtual bool equal(const sem::Type& other) const = 0;

    virtual Type* clone() const = 0;

    virtual ast::Type* to_ast() const = 0;

    virtual TypeObject& object();

    virtual TypeFunction& function();

    virtual const TypeFunction& function() const;

    virtual const TypeObject& object() const;

    bool operator==(const sem::Type& other) const;

    bool operator!=(const sem::Type& other) const;

    virtual ~Type() = default;

    bool generic;
    bool is_generic_param;

    virtual nlohmann::json to_json() const = 0;

    virtual bool is_generic() const;
};

std::ostream& operator<<(std::ostream& o, const sem::Type& t);

#endif //Type_H
