//
// Created by chris on 24/10/21.
//

#ifndef XLANG_MYERRORFORMATTER_H
#define XLANG_MYERRORFORMATTER_H

#include <semantic/errors/include/ErrorFormatter.h>
#include <semantic/errors/include/all_errors.h>

class MyErrorFormatter : public ErrorFormatter {
public:
    const std::string& __file__;
    const CodeLines& code_lines;
    explicit MyErrorFormatter(const std::string& __file__, const CodeLines& code_lines);

    std::string format(const error::BadReturn& err) const override;
    std::string format(const error::TypeMismatch& err) const override;
    std::string format(const error::TypeclassNotFound& err) const override;
    std::string context_string(TextPosition position) const;
    std::string format(const error::GenericError& err) const override;
    std::string code_context_string(TextPosition position) const;
    std::string format(const error::Redeclared& err) const override;
    std::string format(const error::NoMember& err) const override;
    std::string format(const error::NoMemberSuggestions& err) const override;
    std::string format(const error::ObjectNoSpecialMethod& err) const override;
    std::string format(const error::ClassNoMethodForOp& err) const override;
    std::string format(const error::NotDeclared& err) const override;
};

#endif //XLANG_MYERRORFORMATTER_H
