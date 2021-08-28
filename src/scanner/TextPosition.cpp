//
// Created by chris on 3/1/21.
//

#include "TextPosition.h"

std::string text_pos_to_string(const std::string& __file__, TextPosition t) {
    return "" + __file__ + ":" + std::to_string(t.line + 1) + ":" + std::to_string(t.column + 1);
}

bool operator==(const TextPosition& a, const TextPosition& b) {
    return a.line == b.line && a.column == b.column;
}
