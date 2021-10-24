//
// Created by chris on 3/1/21.
//

#ifndef UNTITLED1_TEXTPOSITION_H
#define UNTITLED1_TEXTPOSITION_H

#include <string>

struct TextPosition {
    size_t line;
    size_t column;
};

bool operator==(const TextPosition& a, const TextPosition& b);

std::string text_pos_to_string(const std::string& __file__, TextPosition t);

#endif //UNTITLED1_TEXTPOSITION_H
