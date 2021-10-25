#include "../include/ErrorFormatter.h"

std::string ErrorFormatter::format(const error::Error& err) const {
    switch (err.error_type) {
        case error::ErrorType::bad_return:
            return this->format(static_cast<const error::BadReturn&>(err));
        case error::ErrorType::type_mismatch:
            return this->format(static_cast<const error::TypeMismatch&>(err));
        case error::ErrorType::missing:
            throw std::runtime_error("missing formatter for this error");
    }
    return "Error";
}
