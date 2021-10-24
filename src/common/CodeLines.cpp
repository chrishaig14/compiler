//
// Created by chris on 31/12/20.
//

#include <cassert>
#include "CodeLines.h"


std::string CodeLines::get_line(size_t i) const {
    assert(i < this->line_offsets.size());
    Range line_range = this->line_offsets[i];
    return this->text.substr(line_range.offset, line_range.length - 1);
}
