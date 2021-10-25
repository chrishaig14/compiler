#ifndef ERROR_FORMATTER_H
#define ERROR_FORMATTER_H

#include "all_errors.h"

class ErrorFormatter {
public:
    std::string format(const error::Error& err) const;

    virtual std::string format(const error::BadReturn& err) const = 0;
    virtual std::string format(const error::TypeMismatch& err) const = 0;
    virtual ~ErrorFormatter() = default;
};

#endif // ERROR_FORMATTER_H