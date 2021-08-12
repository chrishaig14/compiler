#include "utils.h"
#include "Compiler.h"


int main(int argc, char* argv[]) {
    bool is_lib;
    std::string project_dir;
    std::string project_output_dir;
    std::string output_name;
    char* xlpath = getenv("XLPATH");
    assert(xlpath != nullptr);
    std::string lib_path = xlpath;
    std::string version;
    // std::cout << "argc = " << argc << std::endl;
    // for (size_t i = 0; i < (size_t) argc; i++) {
    //     std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    // }
    init(argc, argv, is_lib, project_dir, project_output_dir, output_name, version);

    Compiler compiler(project_dir, project_output_dir, output_name, lib_path, is_lib, version);
    compiler.pre();
    compiler.main();
}