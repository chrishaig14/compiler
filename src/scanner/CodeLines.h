//
// Created by chris on 31/12/20.
//

#ifndef UNTITLED1_CODELINES_H
#define UNTITLED1_CODELINES_H


#include <vector>
#include <string>
#include <stdexcept>

struct Range {
    size_t offset;
    size_t length;
};

class CodeLines {
public:
    std::string text;

    std::string get_line(int i) {
        if (i > this->line_offsets.size()) {
            throw std::runtime_error("LINE OUT OF RANGE");
        }
        Range line_range = this->line_offsets[i];
        return this->text.substr(line_range.offset, line_range.length-1);
    }

    std::vector<Range> line_offsets;
};


#endif //UNTITLED1_CODELINES_H
