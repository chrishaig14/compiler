//
// Created by chris on 31/12/20.
//

#include "CodeLines.h"


std::string CodeLines::get_line(size_t i) const {
    if (i > this->line_offsets.size()) {
        throw std::runtime_error("LINE OUT OF RANGE");
    }
    Range line_range = this->line_offsets[i];
    return this->text.substr(line_range.offset, line_range.length - 1);
}
