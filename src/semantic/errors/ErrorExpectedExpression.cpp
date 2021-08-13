//
// Created by chris on 12/8/21.
//

#include "ErrorExpectedExpression.h"

Error* ErrorExpectedExpression::clone() const {
    return nullptr;
}

bool ErrorExpectedExpression::equal(const Error& other) const {
    return false;
}

std::string ErrorExpectedExpression::to_str() const {
    return std::string();
}

ErrorExpectedExpression::ErrorExpectedExpression(Entity entity, const Node& node) {

}
