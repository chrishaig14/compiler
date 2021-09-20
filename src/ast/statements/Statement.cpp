//
// Created by chris on 1/8/20.
//

#include <stdexcept>
#include "Statement.h"

using namespace ast;

bool ast::Statement::operator==(const ast::Statement& other) const {
    if (this->ntype != other.ntype) {
        return false;
    }
    return this->equal(other);
}

bool ast::Statement::operator!=(const ast::Statement& other) const {
    return !(this->operator==(other));
}

Statement::~Statement() {
    // std::cout << "Destructor for " << this << " called" << std::endl;
}

bool ast::Statement::equal(const ast::Statement& other) const {
    // std::cout << "BASE NODE for " << this << " EQUAL CALLED!" << std::endl;
    return false;
}

Statement::Statement(StatementType ntype, TextPosition start, TextPosition end) : ntype(ntype) {
    this->start = start;
    this->end = end;
}
