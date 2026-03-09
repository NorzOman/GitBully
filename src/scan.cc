
#include "include/scan.h"
#include "include/utils.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <unordered_set>
#include <regex>

std::regex secrets(
    R"(()" // Start Match
    R"(\b(?:api|pass|key|secret|token|auth|email|aws|db|firebase)[a-z0-9_]*\b\s*[:=]\s*['"]?([^'\"\s\n]{8,})['\"]?|)"
    R"(gh[pousr]_[A-Za-z0-9_]{36,255}|)"       // GitHub
    R"(sk-(?:proj-)?[A-Za-z0-9\-_]{48,}|)"     // OpenAI
    R"(AKIA[0-9A-Z]{16}|)"                     // AWS Access Key
    R"(AIza[0-9A-Za-z_\-]{35}|)"               // Google/Firebase
    R"(xox[baprs]-[0-9]{10,13}-[a-zA-Z0-9\-]+|)" // Slack
    R"(eyJ[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}\.[A-Za-z0-9_\-]{10,}|)" // JWT
    R"([a-z0-9+._%+-]+@[a-z0-9.-]+\.[a-z]{2,6}[:=][^@\s'\"#]{5,}|)" // email:pass
    R"([a-z0-9+]+:\/\/[^:\s'\"#]+:[^@\s'\"#]+@[^\s'\"#]+)"          // db://user:pass@host
    R"())", // End Match
    std::regex::icase | std::regex::optimize
);

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
    ".gdoc", ".gsheet", ".gslides", ".cache", ".idx", ".pack", ".css"
};

void scan(std::filesystem::path path){
    std::vector<std::filesystem::path> files;
    std::cout << "\n\n// Ignored Files:" << "\n";

    for(const auto& it : std::filesystem::recursive_directory_iterator(path)) {
        if (!std::filesystem::is_regular_file(it.path())) continue;
        std::string ext = it.path().extension().string();
        if (std::filesystem::file_size(it) > 10 * 1024 * 1024 ||
            blacklist.find(ext) != blacklist.end() || it.path().string().find("/.git/") != std::string::npos) logger(it.path().string());
        else files.push_back(it);
    }

    for(const auto& t : files){
        int lineCount = 0;
        std::ifstream file(t.string());
        std::string line;
        while (std::getline(file, line)) {
            std::smatch match;
            if (std::regex_search(line, match, secrets)) {
                std::cout << "[*] Assignment Found: " 
                        << match[1] << " -> " << match[2] 
                        << " | File: " << t.string() 
                        << " | Line: " << lineCount << std::endl;
            }
            ++lineCount;
        }
        file.close();
    }
}

std::string generateUID(size_t length = 16) {
    const std::string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string uid;
    for (size_t i = 0; i < length; i++) {
        uid += chars[dist(gen)];
    }

    return uid;
}

void scanGithubRepo(std::string url){ 
    std::string uuid = generateUID();
    std::string path = "/tmp/" + uuid;
    logger("Cloning the repo to destination path:");
    logger(path);
    std::string command = "git clone " + url + " " + path;
    system(command.c_str());
    logger("Verifying the cloning process");
    if(!std::filesystem::is_directory(path)){
        logger("Failed to verify; There is chance the cloning process failed" , true);
        exit(1);
    }
    logger("Verified Successfully");
    scan(path);
    logger("Scan successfull , now removing the cloned repo");
    command = "rm -r " + path;
    system("command");
}

void scanLocalFolder(std::string path){
    logger("Starting the scan for the local folder");
    scan(path);
}
