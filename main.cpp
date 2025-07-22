// import all the required libraries
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <array>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <regex>


using namespace std;

// Defining a bunch of color to make shit blob of text look pretty
#define RESET   "\033[0m"
#define RED     "\033[93m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[93m"
#define CYAN    "\033[36m"

// Big line of filenames to consider
unordered_map<string, string> sensitive_files = {    {"id_rsa", "SSH private key"},    {"id_dsa", "SSH private key"},    {"id_ecdsa", "SSH private key"},    {"id_ed25519", "SSH private key"},    {".env", "Environment file"},    {".npmrc", "NPM config with auth tokens"},    {".git-credentials", "Git plaintext credentials"},    {".pgpass", "PostgreSQL password file"},    {"credentials", "AWS credentials file"},    {"accessTokens.json", "Azure CLI tokens"},    {"gcloud.json", "Google Cloud key file"},    {"secrets.yml", "Rails secrets"},    {"master.key", "Rails master decryption key"},    {"credentials.yml.enc", "Rails encrypted credentials"},    {"terraform.tfstate", "Terraform state with secrets"},    {"_rsa.pem", "PEM-formatted RSA private key"},    {".pem", "Generic PEM-formatted private key"},    {".key", "Generic private key"},    {".ppk", "PuTTY private key"},    {"/etc/shadow", "Linux hashed password file"},    {"/etc/gshadow", "Linux group shadow file"},    {".netrc", "Netrc credentials"},    {"_netrc", "Netrc credentials"},    {".dockercfg", "Legacy Docker auth config"},    {"config.json", "Docker config with auth tokens"},    {"wp-config.php", "WordPress config with DB credentials"},    {"settings.py", "Django config with secrets"},    {".bash_history", "Bash command history"},    {".zsh_history", "Zsh command history"},    {"psql_history", "PostgreSQL history"},    {".psql_history", "PostgreSQL history"},    {".mysql_history", "MySQL history"},    {"sftp.json", "VSCode SFTP plugin credentials"},    {"sftp-config.json", "SFTP plugin credentials"},    {"WebServers.xml", "JetBrains IDE encoded passwords"},    {".s3cfg", "S3cmd config with keys"},    {"config", "AWS config"},    {"database.yml", "Database config"},    {"application.properties", "Spring Boot config"},    {"application.yml", "Spring Boot config"},    {"mongoid.yml", "Mongoid DB config"},    {"robomongo.json", "Robomongo connection file"},    {"filezilla.xml", "FileZilla server list"},    {"recentservers.xml", "FileZilla recent servers"},    {"WinSCP.ini", "WinSCP config"},    {"dbeaver-data-sources.xml", "DBeaver DB connections"},    {"proftpdpasswd", "ProFTPd password file"},    {".bashrc", "Shell profile with possible secrets"},    {".bash_profile", "Shell profile with possible secrets"},    {".zshrc", "Shell profile with possible secrets"},    {".history", "Generic shell history"},    {"logins.json", "Firefox password export"},    {"secrets.json", "Credential file"},    {"credentials.json", "Credential file"},    {"keystore.jks", "Java KeyStore"},    {"terraform.tfvars", "Terraform secret variables"},    {"kubeconfig", "Kubernetes config with tokens"},        {"*.sql", "Database dump file"},    {"*.dump", "Database dump file"},    {"*.sql.gz", "Compressed database dump"}};


// Extensions not useful for source code scanning — either noisy or risky.
// Generated using copilot to avoid noise in results
unordered_set<string> unwanted_extensions = {
    // Lowercase
    ".ai", ".avi", ".bmp", ".flac", ".gif", ".heic", ".ico", ".jpeg", ".jpg", ".m4a",
    ".mkv", ".mov", ".mp3", ".mp4", ".mpeg", ".mpg", ".oga", ".ogg", ".ogv", ".png",
    ".psd", ".svg", ".tiff", ".wav", ".webm", ".webp", ".wma", ".wmv", ".jfif" ,
    
    // Uppercase
    ".AI", ".AVI", ".BMP", ".FLAC", ".GIF", ".HEIC", ".ICO", ".JPEG", ".JPG", ".M4A",
    ".MKV", ".MOV", ".MP3", ".MP4", ".MPEG", ".MPG", ".OGA", ".OGG", ".OGV", ".PNG",
    ".PSD", ".SVG", ".TIFF", ".WAV", ".WEBM", ".WEBP", ".WMA", ".WMV" , ".JFIF", 

    ".eot", ".otf", ".ttf", ".woff", ".woff2",

    ".7z", ".a", ".ar", ".bz2", ".cab", ".deb", ".dmg", ".gz", ".iso", ".jar", ".rar",
    ".rpm", ".tar", ".tgz", ".war", ".xz", ".zip",

    ".app", ".bat", ".bin", ".cmd", ".com", ".dll", ".exe", ".msi", ".out", ".pyd",
    ".scr", ".so", ".rss",

    ".class", ".elc", ".ko", ".lib", ".o", ".obj", ".pyc", ".pyo",

    ".ckpt", ".h5", ".joblib", ".mlmodel", ".npy", ".npz", ".onnx", ".pb", ".p",
    ".pkl", ".pickle", ".pt", ".pth", ".rdata", ".rda", ".sav", ".tflite",

    ".doc", ".docx", ".epub", ".mobi", ".odt", ".pdf", ".ppt", ".pptx", ".xls", ".xlsx",

    ".db", ".db3", ".log", ".mdb", ".sqlite", ".sqlite3", ".sqlitedb",

    ".css", ".min.css", ".min.js", ".sass", ".scss",

    ".DS_Store", ".bak", ".csproj", ".d", ".ilk", ".pdb", ".sln", ".suo",
    ".swp", ".tmp", ".vcxproj", ".vs",".lock"
};



// Declaring some global variablees
int sensitive_files_no = 0;
vector<pair<regex,string>> patterns;


// function that runs command with option to stream output display , defaulted to false
string run(string command , bool show=false){

	// Create a buffer block with cpp way to delcare aray
	array<char , 128> buffer;
	string output;

	// Run the command and pipe it to the cariable to read it like a file
	// since popen is c thats why we cant just stringstream read it

	// popen is a c levle command , before strings came through so instead of string it expects
	// char *ch pointer pointing to the start ofthe string
	// Since we have string we can use the c_Str function that returns that *ch pointing to the start of the string
	FILE* pipe = popen(command.c_str(),"r");

	// Failed command
	if(!pipe) return "Failed to run command :: " + command;

	// template expexts fgets(data to put , size of data , where to get data)
	while(fgets(buffer.data() , buffer.size() , pipe)){
		if(show){
			cout << buffer.data();
		}
		output += buffer.data();
	}

	pclose(pipe);

    // Remove trailing newline
    if (!output.empty() && output.back() == '\n') {
        output.pop_back();
    }

    return output;
}

// funciton roe return base file name
string basename(const string& path) {
    size_t pos = path.find_last_of('/');
    if (pos == string::npos) return path; // no slash found
    return path.substr(pos + 1);
}


void regex_scanner(vector<string> files_to_scan){
	cout << YELLOW << "===============================================================" << RESET << endl;
	// Now we start the regex scan
	int total_detection = 0;
	int total_lines = 0;
	for(string filename : files_to_scan){
		string file_path = "sandbox/" + filename;
		ifstream infile(file_path);

		if(!infile){
			cout << RED <<  "[✖] Error processing file: " << file_path << RESET << endl;
			continue;
		}

		cout << CYAN << "[➜] Scanning: " << filename  << RESET << endl;

		string line;
		int line_count = 0;
		while(getline(infile , line)){
			++line_count;
			++total_lines;
			for(const auto& [re , label] : patterns){
				smatch match;
				if(regex_search(line , match , re)){
					++total_detection;
					cout << RED << "\n[!] Match found in " << filename
					     << " at line " << line_count
					     << " [" << label << "]:\n"
					     << "    " << line <<  RESET << '\n' ;
				}

			}

		}

		cout << GREEN << "[✔] Scan complete: " << filename << RESET << endl;
	}

	cout << YELLOW << "===============================================================" << RESET << endl;
	cout << GREEN
	     << "[✔] Scanning completed\n"
	     << "    ├─ Total lines        : " << total_lines << '\n'
	     << "    ├─ Sensitive files    : " << sensitive_files_no << '\n'
	     << "    └─ Total detections   : " << total_detection
	     << RESET << endl;
}


int main(int argc, char const *argv[])
{
    string working_dir = run("pwd");
    // ASCII Art Banner
    cout << RED << "\n";
    cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣾⡟⠷⢦⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⡶⢶⡦⣄⡀⠀⠀⠀⢀⣤⠞⠻⠛⠃⠀⠈⠛⢦⡀⠀⠀⠀⠀⠀⢀⣀⣀⡀\n";
    cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠉⠒⠬⢍⡉⠉⣹⠃⠀⠀⠀⠀⠀⠀⠀⠀⢹⠒⢶⣶⡾⠛⠛⠁⠀⠙\n";
    cout << "⠀⠀⠀⠀⠀⠀⠀⢀⣠⠾⠋⠉⠉⠉⠛⠲⠤⢤⣀⣀⣀⡘⣧⡀⠀⠀⠀⠀⠈⣹⠁⠀⠀⠀⠀⠀⠀⠀⢀⠀⠸⡦⠊⠁⠀⠀⠀⠀⢄⡼\n";
    cout << "⠀⠀⠀⢀⣀⡤⠶⠿⠅⠀⢀⠀⠔⠊⠀⠀⠀⠀⠀⠀⠈⠉⠙⠛⠮⣄⣀⣠⣼⣧⡄⠀⠀⠀⠆⠀⠀⣤⣼⠲⣄⢀⡀⢀⣀⣤⣤⠴⠋⠀\n";
    cout << "⠀⠀⢀⡟⠁⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢘⣧⠀⠀⠀⠀⠀⠀⠉⠀⢰⠃⠈⠉⠉⠉⠁⠀⠀⠀⠀\n";
    cout << "⠀⠀⣸⠁⠀⠀⡰⠁⠀⠀⠀⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡴⠀⠀⠀⠀⠀⣸⠃⠀⠀⠀⠀⠀⠁⠀⠀⣼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⢰⣿⠀⠀⣰⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠞⠀⠀⠀⠀⠀⣰⡃⠀⠀⠀⠀⠀⠀⠀⢀⣴⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⢠⠇⣿⠀⢠⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠎⠀⠀⠀⠀⠀⠀⢿⢻⡍⢠⡄⠀⠀⢠⣠⠎⣼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⡸⢰⡏⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡏⠀⠀⠀⠀⠀⠀⠀⠈⠻⣗⣚⣿⣴⣺⠟⢁⡼⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⣇⣸⡇⠀⠸⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠀⠀⠀⢀⡔⠀⠀⠀⠀⠀⠀⠉⠉⠉⠀⢀⡞⠁⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⣇⢸⡇⠀⠀⢿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠇⠀⠀⠀⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠊⠀⠀⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⢹⣸⡇⠀⠀⠘⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠞⠁⠀⠀⠀⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠈⡏⢷⠀⠀⠀⠘⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠃⠀⠀⠀⠀⣼⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠘⣼⡀⠀⠀⠀⣼⢷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠈⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⢻⡇⠀⠀⢠⢿⠀⠉⠛⢷⠶⠤⢤⣄⣀⣀⣀⣹⠰⠀⠀⠀⠀⣠⠄⠀⠀⠀⠀⠀⣀⡀⠀⢠⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⢸⠁⠀⠀⡏⢸⠀⠀⠀⡿⠀⠀⠀⠀⠀⠀⠈⠙⡇⠀⠀⠀⢰⣇⠀⠀⠀⢀⡠⢾⠉⠀⠀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⢺⠀⠀⢸⠇⢸⠀⠀⣼⠃⠀⠀⠀⠀⠀⠀⠀⠀⢻⡀⠀⠀⠀⡏⠓⠦⠴⠋⠁⢸⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⢸⡄⠀⡾⠀⡞⠀⢰⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⠀⠀⠀⡇⠀⠀⠀⠀⠀⣼⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⠘⠃⢀⡇⢰⠁⠀⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⢰⠃⠀⠀⠀⠀⠀⢻⠀⠀⢰⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⢀⡀⠈⣇⡸⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⢸⠀⠀⠀⠀⠀⠀⢸⡀⠀⡜⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⣼⠀⠀⢸⡇⠀⠠⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⠀⠀⢸⠀⠀⠀⠀⠀⠀⢸⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⢰⡟⠀⠀⠚⡧⣄⣀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡆⠀⣼⠀⠀⠀⠀⠀⠀⣸⠀⠀⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⠻⠤⠤⠖⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⢹⡀⠀⠀⠀⠀⠀⢹⡀⠀⠸⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠃⠀⠈⣇⠀⠀⠀⠀⠀⠀⣧⡀⠐⠻⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡞⠒⠀⠐⠻⡄⠀⠀⠀⠀⠀⠙⠒⠒⠊⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠳⣤⣠⠤⠴⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    cout << RESET << endl;
    cout << YELLOW << "===============================================================" << RESET << endl;
    cout << GREEN << "[*] GitBully Tool" << RESET << endl;
    cout << YELLOW << "[*] Linux-Only compatible build" << RESET << endl;
    cout << YELLOW << "[*] Working dir :: " << CYAN << working_dir << RESET << endl;
    cout << YELLOW << "===============================================================\n\n" << RESET << endl;

    string url;
    cout << CYAN << "[-] Enter the repo url ➜ " << RESET;
    getline(cin , url);

    // Remove old sandboxess
    run("rm -rf sandbox");

    run("mkdir sandbox");
    cout << GREEN << "[✔] Created a working directory to work with" << RESET << endl;

    // Clone the url onto the system
    string clone_command = "git clone " + url + " sandbox/";
    run(clone_command,true);

    cout << YELLOW << "===============================================================" << RESET << endl;
    cout << CYAN << "\n\n[*] Loading files onto the scanner..." << RESET << endl;


	// long ahh cmomand to get all the files in the sandbox folder in the correct format as needed
	//string files = run("bash -c \"find sandbox/ -path 'sandbox/.git' -prune -o -type f -printf '%P\\n'\"");
	// updated cmmand that skips some redundant files
	string files = run(R"(bash -c '
	  find sandbox/ \
	    \( \
	      -name node_modules -o \
	      -name .git -o \
	      -name build -o \
	      -name dist -o \
	      -name .next \
	    \) -type d -prune -o \
	    -type f \
	    ! -name package-lock.json \
	    ! -name package.json \
	    -printf "%P\n"
	')");

	vector<string> files_to_scan;

	// Create a stringstream to load the file onto the vector
	stringstream fileread(files);

	string temp;

	while (getline(fileread, temp)) {

	    // Extract extension
	    size_t pos = temp.rfind('.');
	    if (pos != string::npos) {
	        string ext = temp.substr(pos);
	        if (unwanted_extensions.count(ext)) {
	            cout << YELLOW << "[#] Skipped: " << temp << RESET << endl;
	            continue;
	        }
	    }

	    // Check for sensitive files
	    if (sensitive_files.count(basename(temp))) {
	        ++sensitive_files_no;
	        cout << RED << "[✖] Sensitive file detected: " << temp << RESET << endl;
	        files_to_scan.push_back(temp);
	        continue;
	    }

	    cout << GREEN << "[✔] Loaded: " << temp << RESET << endl;
	    files_to_scan.push_back(temp);
	}


	// Load the patterns into the map
	ifstream infile("patterns.txt");
	string line;
	int line_count = 0;


	cout << YELLOW << "\n\n\n===============================================================" << RESET << endl;
	cout << CYAN << "[+] Now loading regex for the scanner" << RESET << endl;


	while(getline(infile,line)){
		++line_count;
		int pos = line.find("::");

		// If something weird is encountered then  report it to the user
		if(pos == string::npos){
			// cout << "[!] An error occured processing the regex at line : " << line_count << endl;
			continue;
		}

		string regex_str = line.substr(0,pos);
		string label = line.substr(pos+2);

		// now create the regex object and push in into the pair and into the vecot
		try{
			patterns.push_back(make_pair(regex(regex_str) , label));
			cout << GREEN << "[✔] Loaded regex for: " << label << RESET << endl;
		} // if it fails to convert it into object then we raise erro
		catch (const regex_error& e){
        	// One of template way to show errors weird looking
        	cerr << RED << "[-] Invalid regex: " << regex_str << " (" << e.what() << ")" << RESET << endl;
        }
	}


	cout << CYAN << "\n\n[+] Now starting current latest commit file scan..." << RESET << endl;

	regex_scanner(files_to_scan);



	cout << CYAN << "\n\n[+] Now starting diff file scan..." << RESET << endl;

	string first_commit = run("cd sandbox/ && git rev-list --max-parents=0 HEAD");

	string latest_commit = run("cd sandbox/ && git rev-parse HEAD");

	string diff_command = "cd sandbox/ && git diff " + first_commit + " " + latest_commit + " > diff_output.txt";

	system(diff_command.c_str());

	vector<string> old_to_scan = {"diff_output.txt"};

	regex_scanner(old_to_scan);

	return 0;
}
