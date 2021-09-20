//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERRORCLASSNOMETHODFOROP_H
#define XLANG_ERRORCLASSNOMETHODFOROP_H


#include "Error.h"

class ErrorClassNoMethodForOp : public Error {
public:
    ErrorClassNoMethodForOp(std::string class_name, std::string op, const ast::CommonNode& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str(const CodeLines& code) const override;

    std::string class_name;
    std::string op;
    const ast::CommonNode& node;
};


#endif //XLANG_ERRORCLASSNOMETHODFOROP_H
