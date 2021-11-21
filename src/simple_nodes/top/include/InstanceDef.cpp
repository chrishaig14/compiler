//
// Created by chris on 21/11/21.
//

#include "InstanceDef.h"

bool sem::InstanceDef::equals(const sem::Top& o) const {
    return false;
}

sem::InstanceDef::InstanceDef(Path instance_name, Path base_type,
                              std::vector<std::unique_ptr<sem::FunctionDef>> methods)
        : Top(TopType::INSTANCE), instance_path(instance_name), base_type_path(base_type), methods(std::move(methods)) {
}
