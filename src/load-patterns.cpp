#include "load-patterns.h"
#include <fstream>
#include <iostream>

std::vector<Pattern> load_patterns(const std::string& file_path) {
    std::vector<Pattern> patterns;
    std::ifstream infile(file_path);

    if (!infile.is_open()) {
        std::cerr << "\n> [!] Could not open patterns file: " << file_path << std::endl;
        return patterns;
    }

    std::string line;
    int line_count = 0;

    std::cout << "> [-] Now loading the patterns file ( any errors will be shown below )" << std::endl;

    while (std::getline(infile, line)) {
        ++line_count;

        size_t pos = line.find("::");
        if (pos == std::string::npos) {
            std::cerr << "> [?] Malformed pattern found at : Line " << line_count << std::endl;
            continue;
        }

        std::string regex_str = line.substr(0, pos);
        std::string label = line.substr(pos + 2);

        try {
            patterns.emplace_back(std::regex(regex_str), label);
            //std::cout << "> [-] Loaded regex for: " << label << std::endl;
        } catch (const std::regex_error& e) {
            std::cerr << "> [!] Invalid regex on line " << line_count << ": " << regex_str << " (" << e.what() << ")" << std::endl;
        }
    }

    return patterns;
}
