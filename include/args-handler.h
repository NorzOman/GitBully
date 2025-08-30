#pragma once
#include <string>

struct Args {
    bool valid = true;
    
    bool is_repo_scan = false;
    bool is_folder_scan = false;
    bool is_store_output = false;
    bool is_patterns_list = false;

    std::string repo_url;
    std::string folder_path;
    std::string output_file;
    std::string patterns_path;
    std::string sandbox_path;
};

Args handle_args(int argc, char const *argv[]);