
#include <iostream>
#include <filesystem>
#include "include/utils.h"

void showHelp(){
    std::cout << "Usage: ./gitbully <url/folder>" << "\n";
    std::cout << "       -its that simple" << "\n";
}

void showBanner(){
    std::cout << "GITBULLY V0.1" << "\n";
}

void logger(std::string query, bool error){
    if(error) std::cout << "[!] " << query << std::endl;
    else std::cout << "[-] " << query << std::endl;
}

args verify(std::string path){
    args a;
    if(path.empty()) return a;
    a.path = path;
    if(path.find("https://github.com/") != std::string::npos){
        a.isGithubUrl = true;
        a.isValid = true;
        std::string s = path.substr(path.length()-4,4);
        if(s != ".git") path+= ".git";
        a.path = path;
    }else{
        if(std::filesystem::exists(path) && std::filesystem::is_directory(path)){
            a.isValid = true;
        }
    }
    return a;
}
