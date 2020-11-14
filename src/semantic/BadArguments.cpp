//
// Created by chris on 2/8/20.
//

#include "BadArguments.h"

bool BadArguments::operator==(const BadArguments& other) const {
    return true;
}

BadArguments::BadArguments(VectorOfTypes ftype, VectorOfTypes args) : std::runtime_error("") {
    this->ftype = ftype;
    this->args = args;
    std::string sargs;
    std::string sparams;
    for (auto a:this->args) {
        sargs += a.to_string() + ", ";
    }
    sargs = sargs.substr(0, sargs.size() - 2);
    for (auto p:this->ftype) {
        sparams += p.to_string() + ", ";
    }
    sparams = sparams.substr(0, sparams.size() - 2);
    text = "Function called with arguments (" + sargs + ")" + " but expects (" + sparams + ")";
}
