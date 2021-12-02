//
// Created by chris on 4/4/21.
//

#ifndef XLANG_DECLARATION_H
#define XLANG_DECLARATION_H

#include <string>
#include "Common.h"
#include "../../expressions/include/Exp.h"
#include <cassert>

class sem::Declaration : public sem::Common {
    UExp _expression;
public:
    std::string identifier;
    Exp& expression;

    Declaration(const std::string& identifier, UExp _expression);

    Declaration(const Declaration& other)
            : sem::Common(CommonType::DECLARATION), _expression(other.expression.clone()), identifier(other.identifier),
              expression(*_expression) {
    }

    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;
};


#endif //XLANG_DECLARATION_H
