#include "utils.h"
#include "Compiler.h"


int main(int argc, char* argv[]) {
    bool is_lib;
    std::string project_dir;
    std::string project_output_dir;
    std::string output_name;
    std::string lib_path = getenv("XLPATH");
    std::string version;

    init(argc, argv, is_lib, project_dir, project_output_dir, output_name, version);

    Compiler compiler(project_dir, project_output_dir, output_name, lib_path, is_lib, version);
    compiler.main();
}