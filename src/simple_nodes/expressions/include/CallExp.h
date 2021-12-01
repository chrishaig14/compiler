//
// Created by chris on 4/4/21.
//

#ifndef XLANG_CALLEXP_H
#define XLANG_CALLEXP_H


#include <vector>
#include "Exp.h"
#include "InstanceObject.h"

class sem::CallExp : public sem::Exp {
public:
    UExp function;
    std::vector<UExp> arguments;
    std::vector<std::unique_ptr<InstanceObject>> instances;
    CallExp(const sem::Exp& function, std::vector<UExp> arguments,
            std::vector<std::unique_ptr<InstanceObject>> instances);

    CallExp(const CallExp& other) : Exp(ExpType::CALL) {
        this->function = other.function->clone();
        for (auto& a: other.arguments) {
            this->arguments.emplace_back(a->clone());
        }
        for (auto& i: other.instances) {
            this->instances.emplace_back(std::make_unique<InstanceObject>(*i));
        }
    }

    std::unique_ptr<Call> to_call();

    bool equals(const Exp& o) const override;
    ~CallExp() = default;
    UExp clone() const override;
};


#endif //XLANG_CALLEXP_H
