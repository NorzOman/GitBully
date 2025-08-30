
#include "args-manager.h"
#include "func-helper.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void folder_to_sandbox(std::string folder_path,std::string sandbox_path) {
    try {
        if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
            std::cerr << "\n> [!] Folder does not exist: " << folder_path << std::endl;
            return;
        }

        if (!fs::exists(sandbox_path)) {
            std::cerr << "\n> [!] Sandbox folder doesnt exist" << std::endl;
            exit(1);
        }

        fs::copy(folder_path, sandbox_path, fs::copy_options::recursive | fs::copy_options::overwrite_existing);

        std::cout << "\n> [-] Copied folder: " << folder_path << " -> " << sandbox_path  << "\n" << std::endl;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "\n> [!] Error copying folder: " << e.what() << std::endl;
        exit(1);
    }
}

void repo_to_sandbox(std::string repo_url,std::string sandbox_path) {
    if (!fs::exists(sandbox_path)) {
        std::cerr << "\n> [!] Sandbox folder doesnt exist" << std::endl;
        exit(1);
    }

    std::string command = "git clone " + repo_url + " " + sandbox_path;

    std::cout << "\n> [-] Cloning repo: " << repo_url << "\n" << std::endl;
    std::string output = run(command);

    std::cout << output << std::endl;
    std::cout << "\n> [-] Repo cloned succesfully\n" << std::endl;
}


std::string create_sandbox(){
    std::cout << "\n> [-] Now creating sandbox folder..." << std::endl;
    run("rm -rf sandbox/");
    std::string t = run("pwd");
    run("mkdir sandbox");
    t = t + "/sandbox/";
    std::cout << "> [-] Created the sandbox folder :" << t << std::endl;
    return t;
}