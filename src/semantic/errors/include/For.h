//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORFOR_H
#define XLANG_ERRORFOR_H


#include "Error.h"

class error::For : public error::Error {
public:
    For(const Entity& entity, TextPosition position);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;
    std::unique_ptr<Entity> entity;
    TextPosition position;
};


#endif //XLANG_FOR_H
