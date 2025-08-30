
// --importing required librares--
#include <iostream>
#include <string>
#include "load-patterns.h"
#include "args-handler.h"
#include "args-manager.h"
#include "func-helper.h"

// -- main function --
int main(int argc, char const *argv[])
{
	print_banner();

    Args args = handle_args(argc, argv);
	
    if(!args.valid) return 1;

    args.sandbox_path = create_sandbox();

    if(args.is_folder_scan) folder_to_sandbox( args.folder_path , args.sandbox_path );
    if(args.is_repo_scan) repo_to_sandbox( args.repo_url , args.sandbox_path );

    auto patterns = load_patterns(args.patterns_path);
    std::cout << "\n> [-] Total patterns loaded: " << patterns.size() << std::endl;

	return 0;
}

