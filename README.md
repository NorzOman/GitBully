# GitBully 🐂

**A C++ tool to bully your Git repositories into revealing their secrets.**

GitBully is a simple yet powerful command-line security tool written in C++. It scans a given Git repository for sensitive information, such as API keys, private keys, passwords, and other credentials that should not be publicly exposed. It checks for both sensitive filenames and sensitive content within files using regex patterns.

---
## Demo

https://github.com/user-attachments/assets/fcba8c1d-80fd-48ab-ad51-7aaf6dec6336

---

## Features

-   **Repository Cloning:** Automatically clones any public Git repository into a temporary `sandbox` directory for analysis.
-   **Sensitive Filename Detection:** Scans for a comprehensive list of potentially sensitive filenames (e.g., `.env`, `id_rsa`, `credentials.json`).
-   **Regex-Based Content Scanning:** Uses a customizable list of regular expressions from a `patterns.txt` file to find secrets within the code.
-   **Historical Diff Scan:** In addition to the current files, it performs a `git diff` between the first and latest commit to find secrets that may have been committed and removed later.
-   **Intelligent Filtering:** Excludes common binary, media, and dependency files (e.g., `.jpg`, `.mp4`, `.zip`, `node_modules`) to reduce noise and improve performance.
-   **User-Friendly Output:** Uses color-coded logs to clearly distinguish between successful operations, warnings, and critical findings.

---

## Prerequisites

Before you can compile and run GitBully, you need the following installed on your system.

-   **A Linux-based OS:** The tool relies on shell commands like `find`, `git`, `rm`, etc.
-   **g++ Compiler:** The standard C++ compiler. You can install it on Debian/Ubuntu with `sudo apt install build-essential`.
-   **Git:** The version control system. You can install it with `sudo apt install git`.

---

## Installation & Usage

Follow these steps to get GitBully up and running.

### 1. Create `patterns.txt`

GitBully loads its content-scanning rules from a file named `patterns.txt` in the same directory. You **must** create this file. Each line should be in the format `REGEX::LABEL`.

There is a `patterns.txt` already provided  to get you started.


### 2. Compile the Code

Open your terminal in the project directory and compile `main.cpp` using `g++`. We recommend using the `-std=c++11` flag to ensure compatibility and creating a named executable like `GitBully`.

```bash
g++ GitBully.cpp -std=c++11 -o GitBully
```

### 3. Run the Scanner

Execute the compiled program.

```bash
./GitBully
```

The program will then prompt you to enter the URL of the Git repository you want to scan.

```bash
oman@local:~/Desktop/projects/GitBully/GitBully$ ./GitBully
# ... ASCII Art ...
===============================================================
[*] GitBully Tool
[*] Linux-Only compatible build
[*] Working dir :: /home/oman/Desktop/projects/GitBully/GitBully
===============================================================

[-] Enter the repo url ➜ https://github.com/someuser/some-repo.git
```

The tool will then proceed with the scan and print its findings to the console.

---

## Understanding the Output

GitBully uses colors to make the output easy to read:

-   `[✔]` **(GREEN):** Indicates a successful action, like loading a file or a regex pattern.
-   `[#]` **(YELLOW):** Indicates a file was skipped because its extension was on the `unwanted_extensions` list.
-   `[✖]` **(RED):** A high-priority alert. This appears when a file with a **sensitive name** (e.g., `credentials.json`) is found.
-   `[!]` **(RED):** A high-priority alert. This appears when a **regex pattern matches content** inside a file. It will show the filename, line number, the rule that was triggered, and the line of code containing the secret.

At the end of the scan, a summary is provided:

```
===============================================================
[✔] Scanning completed
    ├─ Total lines        : 15032
    ├─ Sensitive files    : 2
    └─ Total detections   : 5
```

---

## How It Works

1.  **Initialization:** The tool starts, prints its banner, and prompts the user for a repository URL.
2.  **Sandbox Creation:** It deletes any old `sandbox` directory and creates a new one. The target repository is then cloned into `./sandbox/`.
3.  **File Enumeration:** It uses the `find` command to list all files in the repository, intelligently pruning common dependency directories like `.git`, `node_modules`, `dist`, and `build` to speed up the scan.
4.  **File Filtering & Loading:**
    -   Each file path is checked against the `unwanted_extensions` list. If it matches, the file is skipped.
    -   The filename is checked against the `sensitive_files` map. If it's a match, a "Sensitive file detected" alert is immediately raised.
    -   All other valid files are added to a list for content scanning.
5.  **Pattern Loading:** The tool reads `patterns.txt` line by line, compiling each regex string and storing it in memory along with its descriptive label.
6.  **Current State Scan:** It iterates through the list of loaded files. Each file is read line by line, and every regex pattern is tested against each line. If a match is found, it's reported instantly.
7.  **Historical Diff Scan:**
    -   It identifies the very first commit and the latest commit hash in the repository's history.
    -   It runs `git diff` between these two commits, which shows all the line-by-line changes made throughout the project's lifetime.
    -   This entire diff output is saved to a temporary file (`diff_output.txt`).
    -   The same regex scanner is then run on this diff file, which can uncover secrets that were added and later removed.

---

## Customization

You can easily customize GitBully's behavior by editing the `main.cpp` file:

-   **`sensitive_files`:** To add or remove filenames for the initial check, modify the `unordered_map<string, string> sensitive_files`.
-   **`unwanted_extensions`:** To scan more file types or ignore others, modify the `unordered_set<string> unwanted_extensions`.
-   **`patterns.txt`:** This is the most powerful way to customize the tool. Add your own regular expressions to detect proprietary or unique secret formats specific to your organization.
