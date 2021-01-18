//
// Created by chris on 2/8/20.
//

#include "ReturnError.h"

ReturnError::ReturnError(const TypeNode& expected_type, const TypeNode& actual_type) : runtime_error(
        "Expected to return " + expected_type.to_string() + ", actual: " +
        ((actual_type.kind == Kind::OBJECT && actual_type.object().id == ".None") ? "NULL"
                                                                                  : actual_type.to_string())
) {

}

bool ReturnError::operator==(const ReturnError& other) const {
    std::string a = this->what();
    std::string b = other.what();
    bool t = a == b;
    return t;
}
