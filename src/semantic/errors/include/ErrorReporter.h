//
// Created by chris on 12/4/21.
//

#ifndef XLANG_ERRORREPORTER_H
#define XLANG_ERRORREPORTER_H


#include <string>
#include "../../../scanner/TextPosition.h"
#include "../../../ast/general/Type.h"
#include "../../../ast/expressions/include/Member.h"
#include "../../../ast/statements/Call.h"
#include "../../../scanner/CodeLines.h"
#include "../../../units/entities/Entity.h"
#include "Error.h"

#define FAIL_FIRST 0

enum class ErrorElement {
    BinopLeft, BinopRight, BinopOperator,
};

class ErrorReporter {
public:
    ErrorReporter(const CodeLines& code_lines);

    std::vector<std::unique_ptr<Error>> errors;
    const CodeLines& code_lines;
    bool failed;
    void fail(const std::string& msg, TextPosition pos);

    void fail(const std::string& msg) {
        this->fail(msg, TextPosition{1, 1});
    }

    std::string code_error_string(TextPosition start, TextPosition end);

    std::string current_class;
    std::string current_function;
    std::string __file__;

    void fail_ok(const std::string& pre_msg, const std::string& msg, TextPosition pos);
    void error(std::unique_ptr<Error> error);
    std::string context_string(TextPosition position);
    std::string code_context_string(TextPosition position);
};


#endif //XLANG_ERRORREPORTER_H
