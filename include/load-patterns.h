#pragma once
#include <string>
#include <vector>
#include <regex>
#include <utility>

using Pattern = std::pair<std::regex, std::string>;

std::vector<Pattern> load_patterns(const std::string& file_path);
