// --importing required librares--
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "load-patterns.h"
#include "args-handler.h"
#include "args-manager.h"
#include "func-helper.h"
#include "files-scanner.h"

// -- main function --
int main(int argc, char const *argv[])
{
	print_banner();

    Args args = handle_args(argc, argv);
	
    if(!args.valid) return 1;

    args.sandbox_path = create_sandbox();

    if(args.is_folder_scan) folder_to_sandbox( args.folder_path , args.sandbox_path );
    if(args.is_repo_scan) repo_to_sandbox( args.repo_url , args.sandbox_path );

    std::vector<Pattern> patterns = load_patterns(args.patterns_path);
    std::cout << "\n> [-] Total patterns loaded: " << patterns.size() << std::endl;

    std::vector<std::string> files = collect_files(args.sandbox_path);

    std::vector<std::string> output_lines;
    regex_scanner(args.sandbox_path, files, patterns, output_lines, args.is_store_output);

    // Scan git diff if it's a repo scan
    if (args.is_repo_scan) {
        scan_full_git_history_diff(args.sandbox_path, patterns, output_lines, args.is_store_output);
    }

    if (args.is_store_output) {
        std::cout << "\n> [-] Storing output to: " << args.output_file << std::endl;
        std::ofstream output_file_stream(args.output_file);
        if (output_file_stream.is_open()) {
            for (const auto& line : output_lines) {
                output_file_stream << line << std::endl;
            }
            output_file_stream.close();
            std::cout << "> [✔] Output successfully stored." << std::endl;
        } else {
            std::cerr << "> [!] Error: Could not open output file: " << args.output_file << std::endl;
        }
    }

    std::cout << "Done" << std::endl;

	return 0;
}