#include "files-scanner.h"
#include "func-helper.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

static const std::unordered_set<std::string> unwanted_dirs = {
    ".git", ".hg", ".svn", "node_modules", "bower_components", "vendor",
    "build", "dist", "out", "bin", "obj", ".next", ".nuxt", "target",
    "coverage", "logs", "tmp", "temp", "cache", ".cache", "__pycache__",
    ".idea", ".vscode", ".project", ".settings", "Pods", "Carthage",
    "DerivedData", "public", "static", "assets", "spec", "test", "tests"
};

static const std::unordered_set<std::string> unwanted_extensions = {
    ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".svg", ".webp", ".tiff", ".ico",
    ".mp4", ".mkv", ".avi", ".mov", ".wmv", ".flv", ".webm",
    ".mp3", ".wav", ".flac", ".ogg", ".aac", ".wma",
    ".zip", ".tar", ".gz", ".bz2", ".7z", ".rar", ".xz",
    ".ttf", ".otf", ".woff", ".woff2", ".eot",
    ".exe", ".dll", ".so", ".dylib", ".bin", ".pyd", ".lib", ".a", ".o", ".class", ".jar",
    ".db", ".sqlite", ".sqlite3", ".sql", ".log", ".mdb",
    ".doc", ".docx", ".ppt", ".pptx", ".xls", ".xlsx", ".pdf", ".odt", ".rtf",
    ".min.js", ".min.css",
    "package-lock.json", "yarn.lock", "composer.lock", "Gemfile.lock", "Pipfile.lock", "poetry.lock",
    ".pkl", ".h5", ".model", ".pb", ".joblib", ".csv", ".json",
    ".bak", ".swp", ".swo", ".DS_Store", ".gitignore", ".gitattributes", ".dockerignore"
};

static bool has_unwanted_extension(const fs::path& path) {
    if (unwanted_extensions.count(path.filename().string())) {
        return true;
    }
    return unwanted_extensions.count(path.extension().string());
}

// Helper to check unwanted directories
static bool is_unwanted_dir(const fs::path& path) {
    return unwanted_dirs.count(path.filename().string());
}

// Collect candidate files to scan
std::vector<std::string> collect_files(const std::string& sandbox_path) {
    std::vector<std::string> files;
    std::cout << "\n> [-] Building list of files to scan..." << std::endl;
    try {
        for (auto it = fs::recursive_directory_iterator(
                 sandbox_path, fs::directory_options::skip_permission_denied);
             it != fs::recursive_directory_iterator(); ++it) {
            const auto& entry_path = it->path();
            if (it->is_directory()) {
                if (is_unwanted_dir(entry_path)) {
                    it.disable_recursion_pending();
                }
                continue;
            }
            if (!it->is_regular_file() || has_unwanted_extension(entry_path)) {
                continue;
            }
            files.push_back(fs::relative(entry_path, sandbox_path).string());
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "> [!] Filesystem error while collecting files: " << e.what() << std::endl;
    }
    std::cout << "> [-] Total candidate files found: " << files.size() << std::endl;
    return files;
}

// Scan collected files for regex matches
void regex_scanner(const std::string& sandbox_path,
                   const std::vector<std::string>& files_to_scan,
                   const std::vector<Pattern>& patterns,
                   std::vector<std::string>& output_lines,
                   bool is_store_output) {
    std::cout << "\n> [-] Starting regex scan on collected files..." << std::endl;
    int total_detections = 0;
    for (const auto& relative_file : files_to_scan) {
        fs::path full_path = fs::path(sandbox_path) / relative_file;
        std::ifstream infile(full_path);
        if (!infile.is_open()) {
            std::cerr << "> [!] Could not open file: " << full_path << std::endl;
            continue;
        }
        std::string line;
        int line_number = 0;
        while (std::getline(infile, line)) {
            ++line_number;
            for (const auto& pattern : patterns) {
                if (std::regex_search(line, pattern.first)) {
                    total_detections++;
                    std::string output = "> [!] Match in " + relative_file +
                                      " at line " + std::to_string(line_number) +
                                      " [" + pattern.second + "]: " + line;
                    std::cout << output << std::endl;
                    if (is_store_output) {
                        output_lines.push_back(output);
                    }
                }
            }
        }
    }
    std::cout << "\n> [✔] Regex scan complete." << std::endl;
    std::cout << "> [-] Total detections found in files: " << total_detections << std::endl;
}

void scan_full_git_history_diff(const std::string& repo_path,
                                const std::vector<Pattern>& patterns,
                                std::vector<std::string>& output_lines,
                                bool is_store_output) {
    std::cout << "\n> [-] Scanning full git history for secrets..." << std::endl;

    std::string first_commit_cmd = "git -C " + repo_path + " rev-list --max-parents=0 HEAD";
    std::string first_commit = run(first_commit_cmd);

    std::string latest_commit_cmd = "git -C " + repo_path + " rev-parse HEAD";
    std::string latest_commit = run(latest_commit_cmd);

    if (first_commit.empty() || latest_commit.empty()) {
        std::cerr << "> [!] Could not retrieve git commit history. Is this a valid git repository?" << std::endl;
        return;
    }

    std::string diff_command = "git -C " + repo_path + " diff " + first_commit + " " + latest_commit;
    std::string diff_output = run(diff_command);

    if (diff_output.empty()) {
        std::cout << "> [-] No git history diff to scan." << std::endl;
        return;
    }

    int git_detections = 0;
    std::istringstream diff_stream(diff_output);
    std::string line;
    std::string current_file = "Unknown";

    while (std::getline(diff_stream, line)) {
        if (line.rfind("+++ b/", 0) == 0) {
            current_file = line.substr(6);
            continue;
        }

        if (line.rfind('+', 0) != 0) {
            continue;
        }

        for (const auto& pattern : patterns) {
            if (std::regex_search(line, pattern.first)) {
                git_detections++;
                std::string output = "> [!] Match in git history (file: " + current_file + ") [" + pattern.second + "]: " + line.substr(1);
                std::cout << output << std::endl;
                if (is_store_output) {
                    output_lines.push_back(output);
                }
            }
        }
    }

    std::cout << "> [✔] Git history scan complete." << std::endl;
    std::cout << "> [-] Total detections in git history: " << git_detections << std::endl;
}