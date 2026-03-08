
#include "include/utils.h"
#include "include/scan.h"
#include <cstdlib>
#include <string>

int main(int argc , char* argv[]){
    showBanner();
    if(argc == 1 || (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1])=="--help")) || argc > 2){
        showHelp();
        exit(1);
    }
    args buildArgs = verify(std::string(argv[1]));

    if(!buildArgs.isValid){
        logger("The path provided doesnt exists on the system or doesnt match any github repo url.",true);
        exit(-1);
    }

    if(buildArgs.isGithubUrl){
        scanGithubRepo(buildArgs.path);
    }else{
        scanLocalFolder(buildArgs.path);
    }

}
