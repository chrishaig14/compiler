//
// Created by chris on 12/4/21.
//

#ifndef XLANG_ERRORREPORTER_H
#define XLANG_ERRORREPORTER_H


#include <string>
#include "../../../scanner/TextPosition.h"
#include "../../../ast/Type.h"
#include "../../../ast/exp/include/Member.h"
#include "../../../ast/Call.h"
#include "../../../scanner/CodeLines.h"
#include "../../../units/Entity.h"
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
    std::string context_string(TextPosition position);
    std::string code_context_string(TextPosition position);
    std::string code_string(TextPosition start, TextPosition end);
    void fail(const std::string& msg, TextPosition pos);

    void fail(const std::string& msg) {
        this->fail(msg, TextPosition{1, 1});
    }

    std::string code_error_string(TextPosition start, TextPosition end);

    std::string current_class;
    std::string current_function;
    std::string __file__;

    void fail_ok(const std::string& pre_msg, const std::string& msg, TextPosition pos);
    void module_no_member(const ast::Type& t, const std::string& member, TextPosition pos, ast::Node& obj,
                          TextPosition member_start, TextPosition member_end);
    void object_no_member(const ast::Type& t, const ast::Member& obj);
    void module_no_member(std::string mod_name, const std::string& member, TextPosition pos, ast::Node& obj,
                          TextPosition member_start, TextPosition member_end);
    void package_no_member(std::string pack_name, const std::string& member, TextPosition pos, ast::Node& obj,
                           TextPosition member_start, TextPosition member_end);
    void error(std::unique_ptr<Error> error);
    void package_no_member(Package* pack, const std::string& member, TextPosition pos, ast::Node& obj,
                           TextPosition member_start, TextPosition member_end);
};


#endif //XLANG_ERRORREPORTER_H
