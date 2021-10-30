//
// Created by chris on 24/10/21.
//

#ifndef XLANG_MYERRORFORMATTER_H
#define XLANG_MYERRORFORMATTER_H

#include <semantic/errors/include/ErrorFormatter.h>
#include <semantic/errors/include/all_errors.h>

class MyErrorFormatter : public ErrorFormatter {
public:
    std::string format(const error::BadReturn& err) const override;
    std::string format(const error::TypeMismatch& err) const override;
    std::string format(const error::TypeclassNotFound& err) const override;
};

#endif //XLANG_MYERRORFORMATTER_H
