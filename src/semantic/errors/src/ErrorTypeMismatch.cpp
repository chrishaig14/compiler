//
// Created by chris on 8/8/21.
//

#include "../include/ErrorTypeMismatch.h"
#include <json/json.hpp>

std::string mmmcode(const CodeLines& code_lines, TextPosition start, TextPosition end) {
    size_t length = end.column - start.column + 1;
    std::string str = "\n" + code_lines.get_line(start.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(start.column, ' ') + std::string(length, '^'));
    return str;
}

std::string highlight_one(const std::string& s, size_t start_col, size_t end_col) {
    return s.substr(0, start_col) + E_HLT(s.substr(start_col, end_col - start_col)) + s.substr(end_col, s.size());
}

ErrorTypeMismatch::ErrorTypeMismatch(const sem::Type& expected, const ast::ExpNode& value_node, const Entity& actual)
        : expected(expected.clone()), actual(actual.clone()), start(value_node.start), end(value_node.end) {
}

std::string ErrorTypeMismatch::to_str(const CodeLines& code) const {
    std::string as = entity_to_string(*actual);
    std::string pre_msg = "Expected " + E_HLT(expected->to_string()) + ", got " + E_HLT(as) + ":\n";
    // pre_msg += " -- node: " + to_string(this->value_node.to_json());
    // std::string msg = highlight_one(value_node);
    std::string line = code.get_line(this->start.line);
    pre_msg += highlight_one(line, this->start.column, this->end.column);
    return pre_msg;
}

ErrorTypeMismatch::ErrorTypeMismatch(const sem::Type& expected, TextPosition start, TextPosition end,
                                     const Entity& actual)
        : expected(expected.clone()), actual(actual.clone()), start(start), end(end) {
}

bool ErrorTypeMismatch::equal(const Error& other) const {
    const auto& o = (const ErrorTypeMismatch&) other;
    bool act = *o.actual == *this->actual && o.actual->get_value().type == this->actual->get_value().type;
    bool exp = *o.expected == *this->expected;
    bool val = o.start == this->start && o.end == this->end;
    return act && exp && val;
}

Error* ErrorTypeMismatch::clone() const {
    return new ErrorTypeMismatch(*this->expected, this->start, this->end, *this->actual);
}
