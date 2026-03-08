
#pragma once

#include <iostream>
#include <string>

struct args{
    std::string path = "";
    bool isGithubUrl = false;
    bool isValid = false;
};

void showHelp();
void showBanner();
void logger(std::string,bool=false);
args verify(std::string);
