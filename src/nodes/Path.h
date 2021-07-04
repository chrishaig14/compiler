//
// Created by chris on 3/7/21.
//

#ifndef XLANG_PATH_H
#define XLANG_PATH_H


#include "../types.h"

class Path {
private:
    VectorOfStrings path_parts;
    std::string s;
public:
    Path() = default;

    Path(Path p, const std::string& c);

    explicit Path(const std::string& s);

    explicit Path(const VectorOfStrings& p);

    std::string as_str() const;

    VectorOfStrings as_vec();
};


#endif //XLANG_PATH_H
