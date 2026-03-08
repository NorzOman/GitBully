
#include "include/scan.h"
#include "include/utils.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>

// Remove (?i) from the string, add std::regex::icase to the constructor
std::regex assignment(R"((api|pass|key|secret|token|auth|email)[a-z0-9_]*\s*[:=]\s*["']([^"']{8,})["'])", std::regex::icase);
// std::regex raw_entropy(R"(([a-zA-Z0-9]{4,}[@#$!%^&*()-_+=][a-zA-Z0-9]{4,}))");

std::unordered_set<std::string> blacklist = {
    ".zip", ".7z", ".rar", ".tar", ".gz", ".bz2", ".xz", ".lzma", ".z", ".s7z", ".apk",
    ".jar", ".war", ".ear", ".cab", ".iso", ".img", ".dmg", ".pkg", ".deb", ".rpm",
    ".msi", ".cpio", ".shar", ".lzh", ".lha", ".iso9660", ".vcd", ".cue", ".bin",
    ".mp4", ".mkv", ".mov", ".avi", ".wmv", ".flv", ".webm", ".m4v", ".mpg", ".mpeg",
    ".m2v", ".ts", ".vob", ".qt", ".yuv", ".rm", ".rmvb", ".asf", ".amv", ".m4p",
    ".mpv", ".m2ts", ".mts", ".3gp", ".3g2", ".f4v", ".f4p", ".f4b", ".f4a",
    ".mp3", ".wav", ".flac", ".m4a", ".ogg", ".opus", ".aac", ".wma", ".aiff", ".alac",
    ".mid", ".midi", ".mka", ".mpc", ".ra", ".voc", ".au",
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".tif", ".webp", ".ico", ".svg",
    ".psd", ".ai", ".eps", ".indd", ".raw", ".cr2", ".nef", ".orf", ".sr2", ".dng",
    ".jxr", ".hdp", ".wdp", ".dds", ".heic", ".heif", ".jp2", ".j2k", ".jpf", ".jpm",
    ".exe", ".dll", ".so", ".a", ".o", ".lib", ".obj", ".out", ".app", ".elf",
    ".pyc", ".pyo", ".pyd", ".class", ".dex", ".ko", ".sys", ".vxd", ".com",
    ".pdf", ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".odt", ".ods", ".odp",
    ".epub", ".mobi", ".azw", ".chm",
    ".db", ".sqlite", ".sqlite3", ".mdb", ".accdb", ".frm", ".ibd", ".dbf", ".dat",
    ".pdb", ".sqlitedb", ".dump", ".rdb", ".bin",
    ".ttf", ".otf", ".woff", ".woff2", ".eot", ".fon",
    ".vmdk", ".vdi", ".vhd", ".vhdx", ".qcow", ".qcow2", ".ova", ".ovf",
    ".ds_store", ".thumbs.db", ".swp", ".lock", ".log", ".tmp", ".bak", ".old",
    ".gdoc", ".gsheet", ".gslides", ".cache", ".idx", ".pack"
};

void scan(std::filesystem::path path){
    std::vector<std::filesystem::path> files;
    std::cout << "// Ignored Files:" << "\n";
    for(const auto& it : std::filesystem::recursive_directory_iterator(path)) {
        if (!std::filesystem::is_regular_file(it.path())) continue;
        std::string ext = it.path().extension().string();
        if (std::filesystem::file_size(it) > 10 * 1024 * 1024 ||
            blacklist.find(ext) != blacklist.end() || it.path().string().find("/.git/") != std::string::npos) logger(it.path().string());
        else files.push_back(it);
    }

    for(const auto& t : files){
        std::ifstream file(t.string());
        std::string line;
        while (std::getline(file, line)) {
            std::smatch match;
            if (std::regex_search(line, match, assignment)) {
                std::cout << "[ * ] ASSIGNMENT FOUND: " << match[1] << " -> " << match[2] << "at" << t.string() << "\n" ;
            }
            // else if (std::regex_search(line, match, raw_entropy)) {
            //     std::cout << "[ * ] RAW SECRET FOUND: " << match[0] << "at" << t.string() << "\n";
            // }
        }
        file.close();
    }
}

void scanGithubRepo(std::string url){
    logger("Starting the scan for the repo");
    scan(url);
}

void scanLocalFolder(std::string path){
    logger("Starting the scan for the local folder");
    scan(path);
}
