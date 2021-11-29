//
// Created by chris on 21/11/21.
//

#ifndef XLANG_INSTANCEDEF_H
#define XLANG_INSTANCEDEF_H

#include <common/Path.h>
#include "Top.h"
#include "FunctionDef.h"

class sem::InstanceDef : public sem::Top {
public:
    Path instance_path;
    Path base_type_path;
    std::vector<std::unique_ptr<sem::FunctionDef>> methods;
    std::vector<std::unique_ptr<sem::FunctionDef>> static_methods;
    InstanceDef(Path instance_path, Path base_path, std::vector<std::unique_ptr<sem::FunctionDef>> methods,std::vector<std::unique_ptr<sem::FunctionDef>> static_methods);
    bool equals(const Top& o) const override;

};


#endif //XLANG_INSTANCEDEF_H
