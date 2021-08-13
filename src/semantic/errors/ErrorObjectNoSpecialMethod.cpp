//
// Created by chris on 12/8/21.
//

#include "ErrorObjectNoSpecialMethod.h"

Error* ErrorObjectNoSpecialMethod::clone() const {
    return nullptr;
}

bool ErrorObjectNoSpecialMethod::equal(const Error& other) const {
    return false;
}

std::string ErrorObjectNoSpecialMethod::to_str() const {
    return std::string();
}

ErrorObjectNoSpecialMethod::ErrorObjectNoSpecialMethod(const TypeNode& type, const std::string& method_name,
                                                       const SubscriptNode& node) {

}
