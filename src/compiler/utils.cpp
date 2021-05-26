//
// Created by chris on 25/5/21.
//

#include <iostream>
#include <getopt.h>
#include <sys/stat.h>
#include "utils.h"


void parse_args(int argc, char** argv, bool& is_lib, std::string& project_dir, std::string& project_output_dir,
                std::string& global_name) {
    int opt;
    while ((opt = getopt(argc, argv, "i:o:n:l")) != -1) {
        switch (opt) {
            case 'i':
                project_dir = optarg;
                break;
            case 'l':
                is_lib = true;
                break;
            case 'o':
                project_output_dir = optarg;
                break;
            case 'n':
                global_name = optarg;
                break;
            case ':':
                printf("option needs a value\n");
                exit(1);
            case '?':
                printf("unknown option: %c\n", optopt);
                exit(1);
        }
    }
    for (; optind < argc; optind++) {
        printf("extra arguments: %s\n", argv[optind]);
    }
}

void init(int argc, char** argv, bool& is_lib, std::string& project_dir, std::string& project_output_dir,
          std::string& global_name) {
    parse_args(argc, argv, is_lib, project_dir, project_output_dir, global_name);

    std::string command = "rm -rf " + project_output_dir;
    system(command.c_str());

    int x = mkdir(project_output_dir.c_str(), 0700);
    if (x != 0) {
        std::cout << "failed to create application dir" << std::endl;
        exit(0);
    }
}
