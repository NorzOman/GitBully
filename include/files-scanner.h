#pragma once

#include <string>
#include <vector>
#include <regex>

// Define Pattern as a pair
using Pattern = std::pair<std::regex, std::string>;

// Collects all files from the sandbox path, excluding unwanted dirs/extensions
std::vector<std::string> collect_files(const std::string& sandbox_path);

// Scans files for regex patterns
void regex_scanner(const std::string& sandbox_path,
                   const std::vector<std::string>& files_to_scan,
                   const std::vector<Pattern>& patterns,
                   std::vector<std::string>& output_lines,
                   bool is_store_output);

// Scans the output of "git diff" for patterns
void scan_full_git_history_diff(const std::string& repo_path,
                   const std::vector<Pattern>& patterns,
                   std::vector<std::string>& output_lines,
                   bool is_store_output);