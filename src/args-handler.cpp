#include "args-handler.h"
#include "func-helper.h"
#include <iostream>

Args handle_args(int argc, char const *argv[]) {
    Args args;

    if (argc == 1 || argc > 7) {
        std::cerr << "> [!] Invalid number of arguments. Use `--help`." << std::endl;
        args.valid = false;
        return args;
    }

    if (argc == 2 && std::string(argv[1]) == "--help") {
        print_usage();
        args.valid = false;
        return args;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--folder") {
            if (i + 1 < argc) {
                args.is_folder_scan = true;
                args.folder_path = argv[++i];
            } else {
                std::cerr << "> [!] Missing value for `--folder`" << std::endl;
                args.valid = false;
                return args;
            }
        }
        else if (arg == "--repo") {
            if (i + 1 < argc) {
                args.is_repo_scan = true;
                args.repo_url = argv[++i];
            } else {
                std::cerr << "> [!] Missing value for `--repo`" << std::endl;
                args.valid = false;
                return args;
            }
        }
        else if (arg == "--patterns") {
            if (i + 1 < argc) {
                args.is_patterns_list = true;
                args.patterns_path = argv[++i];
            } else {
                std::cerr << "> [!] Missing value for `--patterns`" << std::endl;
                args.valid = false;
                return args;
            }
        }
        else if (arg == "--output") {
            if (i + 1 < argc) {
                args.is_store_output = true;
                args.output_file = argv[++i];
            } else {
                std::cerr << "> [!] Missing value for `--output`" << std::endl;
                args.valid = false;
                return args;
            }
        }
        else {
            std::cerr << "> [!] Unknown option: " << arg << std::endl;
            args.valid = false;
            return args;
        }
    }

    if (args.is_folder_scan && args.is_repo_scan) {
        std::cerr << "> [!] Cannot scan both folder and repo at the same time." << std::endl;
        args.valid = false;
    }

    if (!args.is_folder_scan && !args.is_repo_scan) {
        std::cerr << "> [!] No scan type specified. Use `--folder` or `--repo`." << std::endl;
        args.valid = false;
    }

    if (!args.is_patterns_list) {
        std::cerr << "> [!] Patterns list not provided. Use `--patterns <file>`." << std::endl;
        args.valid = false;
    }

    return args;
}
