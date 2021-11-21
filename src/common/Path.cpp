//
// Created by chris on 3/7/21.
//

#include "Path.h"

Path::Path(Path p, const std::string& c) {
    this->path_parts = p.as_vec();
    this->path_parts.push_back(c);
    for (const auto& cc: this->path_parts) {
        this->s += cc + ".";
    }
    this->s = this->s.substr(0, this->s.size() - 1);
}

Path::Path(const std::string& s) {
    this->s = s;
    size_t k = 0;
    this->path_parts.push_back("");
    for (char i : s) {
        if (i == '.') {
            this->path_parts.push_back("");
            k++;
        } else {
            this->path_parts[k] += i;
        }
    }
}

Path::Path(const VectorOfStrings& p) {
    this->path_parts = p;
    for (const auto& cc: this->path_parts) {
        this->s += cc + ".";
    }
    this->s = this->s.substr(0, this->s.size() - 1);
}

std::string Path::as_str() const {
    return this->s;
}

VectorOfStrings Path::as_vec() const {
    return this->path_parts;
}

std::string Path::basname() const {
    return path_parts.back();
}

bool Path::operator==(const Path& other) const {
    return this->as_str() == other.as_str();
}
