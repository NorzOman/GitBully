#pragma once

#include <string>

void repo_to_sandbox( std::string repo_url , std::string sandbox_path );
void folder_to_sandbox( std::string folder_path , std::string sandbox_path);
std::string create_sandbox();

