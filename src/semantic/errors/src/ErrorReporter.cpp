#include "../include/ErrorReporter.h"
#include "../../ModuleChecker.h"

using namespace error;

void ErrorReporter::fail(const std::string& msg, TextPosition pos) {
    this->failed = true;
    std::string out = this->context_string(pos) + msg + this->code_context_string(pos);
    std::cout << out << std::endl;
    if (FAIL_FIRST) {
        throw std::runtime_error("Error");
    }
}

std::string substring(const std::string& s, TextPosition start, TextPosition end) {
    assert(start.line == end.line);
    return s.substr(start.column, end.column - start.column);
}


std::map<ErrorElement, fmt::text_style> styles;

void init_styles() {
    styles[ErrorElement::BinopLeft] = fmt::fg(fmt::terminal_color::green) | fmt::emphasis::bold;
    styles[ErrorElement::BinopOperator] = fmt::fg(fmt::terminal_color::red) | fmt::emphasis::bold;
    styles[ErrorElement::BinopRight] = fmt::fg(fmt::terminal_color::blue) | fmt::emphasis::bold;
}

std::string ErrorReporter::context_string(TextPosition position) {
    std::string msg = E_HLT(text_pos_to_string(this->__file__, position)) + E_FMT(" In function ") +
                      E_HLT((this->current_class == "" ? "" : this->current_class + ".") + this->current_function) +
                      E_FMT(": ");
    return msg;
}

std::string ErrorReporter::code_context_string(TextPosition position) {
    // return this->code_lines.get_line(position.line);
    std::string str = "\n" + this->code_lines.get_line(position.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(position.column, ' ') + std::string(1, '^'));
    return str;
}

std::string ErrorReporter::code_error_string(TextPosition start, TextPosition end) {
    size_t length = end.column - start.column + 1;
    std::string str = "\n" + this->code_lines.get_line(start.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(start.column, ' ') + std::string(length, '^'));
    return str;
}

void ErrorReporter::error(std::unique_ptr<Error> error) {
    this->failed = true;
    std::cout << "---- Semantic Error ----" << std::endl;
    std::cout << error->to_str(this->code_lines) << std::endl;
    this->errors.emplace_back(std::move(error));
    assert(this->errors.back() != nullptr);
    std::cout << "------------------------" << std::endl;
}

ErrorReporter::ErrorReporter(const CodeLines& code_lines) : code_lines(code_lines) {
    this->failed = false;
    init_styles();
}

void ErrorReporter::fail(const std::string& msg) {
    this->fail(msg, TextPosition{1, 1});
}

bool ErrorReporter::ok() {
    return not this->failed;
}
