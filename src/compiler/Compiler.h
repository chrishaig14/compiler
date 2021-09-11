//
// Created by chris on 25/5/21.
//

#ifndef XLANG_COMPILER_H
#define XLANG_COMPILER_H


#include <string>
#include <fstream>
#include "../units/Package.h"
#include "../units/Module.h"
#include "../utils.h"

#define REQUIREMENTS_FILE "requirements.txt"

void
write_cmakelists(const std::string& cmake_output_path, const std::string& output_name, const std::string& all_files,
                 const std::string& all_libraries, bool is_lib);
std::map<std::string, std::string> read_requirements(const std::string& filepath);
void add_local_path_to_module(Module& module, Path path, Package& top_package);
void add_path_to_module(Module& module, Path path, Package& top_package);
void add_path_with_alias_to_module(Module& module, const std::string& alias, Path path, Package& root_package);
void analyze_module(Module& module, Package& top_package);
void analyze_all_modules(Package& package, Package& top_package);

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

public:
    ~Compiler() {
        std::cout << "Called compiler destructor " << std::endl;
        // delete this->top_package; // TODO this should not be allocated on the heap
        // delete this->root_package; // TODO this should not be allocated on the heap
    }

    Compiler(const std::string& project_dir, const std::string& project_output_dir, const std::string& output_name,
             const std::string& lib_path, bool is_lib, const std::string& version);
    VectorOfStrings load_requirements(const std::string& filepath);
    void preprocess_package(Package& package);
    void add_global_path_to_module(Module& module, Path path);


    void parse_module(Module& module);
    void parse_package(Package& package);
    // void transpile_all_modules(Package& package, const std::string& output_dir, bool is_top);
    // void transpile_one_module(Module& module, std::string& package_header, const std::string& output_package_dir,
    //                           Package& package, std::string static_initializations, std::string& static_cleanups);

    void load_module(Package& package, const std::string& d_name);

    void load_package(Package& package, int level);
    void load_library(const std::string& name, const std::string& lib_version);
    void load_top_unit(const std::string& name, const std::string& m_version, bool m_is_lib);
    // void transpile_one_module(Module& module, std::string& package_header, const std::string& output_package_dir,
    //                           Package& package);

    void main();
    std::string top_package_name;
    void pre();
    Package root_package;
    Package top_package;
};


#endif //XLANG_COMPILER_H
