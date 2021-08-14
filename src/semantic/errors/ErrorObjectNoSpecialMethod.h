//
// Created by chris on 12/8/21.
//

#ifndef XLANG_ERROROBJECTNOSPECIALMETHOD_H
#define XLANG_ERROROBJECTNOSPECIALMETHOD_H


#include "Error.h"
#include "../../nodes/SubscriptNode.h"

class ErrorObjectNoSpecialMethod : public Error {
public:
    ErrorObjectNoSpecialMethod(const TypeNode& type, const std::string& method_name, const SubscriptNode& node);
    Error* clone() const override;
    bool equal(const Error& other) const override;
    std::string to_str() const override;

};


#endif //XLANG_ERROROBJECTNOSPECIALMETHOD_H
