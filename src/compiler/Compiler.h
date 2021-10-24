//
// Created by chris on 25/5/21.
//

#ifndef XLANG_COMPILER_H
#define XLANG_COMPILER_H


#include <string>
#include <fstream>
#include <units/infos/Package.h>
#include <units/infos/Module.h>
#include <util/utils.h>

#define REQUIREMENTS_FILE "requirements.txt"

void add_local_path_to_module(Module& module, Path path, Package& top_package);
void add_path_to_module(Module& module, Path path, Package& top_package);
void add_path_with_alias_to_module(Module& module, const std::string& alias, Path path, Package& root_package);
bool check_module(Module& module, Package& top_package);
bool check_package(Package& package, Package& top_package);
bool preprocess_package(Package& package);
bool parse_module(Module& module);
bool parse_package(Package& package);
void resolve_module_imports(Module& module, Package& top_package);

class Compiler {
    std::string project_dir;
    std::string project_output_dir;
    std::string output_name;
    std::vector<std::unique_ptr<Module>> my_modules;
    std::string lib_path;
    std::map<std::string, bool> loaded_top_units;
    std::string static_initializations;
    std::string static_cleanups;
    VectorOfStrings all_modules;
    bool is_lib;
    std::string version;
    bool ok;
public:
    void load_project();
    Compiler(const std::string& project_dir, const std::string& project_output_dir, const std::string& output_name,
             const std::string& lib_path, bool is_lib, const std::string& version);
    VectorOfStrings load_requirements(const std::string& filepath);
    void add_global_path_to_module(Module& module, Path path);
    void load_library(const std::string& name, const std::string& lib_version);
    void load_top_unit(const std::string& name, const std::string& m_version, bool m_is_lib);
    bool main();
    bool pre();
    Package root_package;
    Package top_package;
    std::map<std::string, std::string> instances;
};


#endif //XLANG_COMPILER_H
