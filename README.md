
# vcs - A Minimal Version Control System in C++

This is a simple project I implemented to understand the core concepts behind how Version Control Systems (like Git) work under the hood.

It mimics Git in spirit and demonstrates ideas like object storage, indexing, staging, commits, branching, and status tracking.

---

## 🚀 Motivation

This project was built purely for educational purposes — to deepen my understanding of how version control tools like Git manage files, history, and metadata under the hood.

---

## ⚙️ Build and Run Instructions

### 1. Build the Project

Ensure you have a **C++17-compatible compiler** installed.

```bash
mkdir build && cd build
cmake ..
make
```

### 2. Run the CLI

You can run commands from the build directory:

```bash
./build/Debug/vcs <command>
```

**Optional Tip:** Add `vcs.exe` (or the built binary path) to your system's environment variables to use it globally from the terminal like `git`.

---

## 🧪 Available Commands 

⚠️ If vcs.exe is not added to your environment variables, replace vcs with the path to the executable (e.g., ./build/Debug/vcs).

### `vcs init`
Initializes a new repository by creating the internal `.vcs/` directory.

---

### `vcs add <files or directories>`
Adds one or more files/folders to the index (i.e., staging area) to track changes.

> Example:
> `vcs add file.txt src/`

---

### `vcs rm <files>`
Removes one or more files from the index (stops tracking them).

> Example:
> `vcs rm old_config.txt`

---

### `vcs commit <message>`
Commits the currently staged files (those added via `vcs add`) with the provided commit message.

> Example:
> `vcs commit "Implemented login logic"`

---

### `vcs restore <filename>`
Restores file from the latest commit (undoes local changes).

> Example:
> `vcs restore textfile.txt`

### `vcs status`
Displays the working directory state:
- Changes staged for commit
- Changes not staged for commit
- Untracked files

---

### `vcs branch <branch-name>`
Creates a new branch from the current commit.

> Example:
> `vcs branch feature`

---

### `vcs checkout <branch-name>`
Switches to the specified branch and updates the working directory accordingly.

> Example:
> `vcs checkout master`

---

### `vcs log`
Shows a list of commits made in the current branch.

---

## Ignore Files 

This VCS supports `.vcsignore`, similar to `.gitignore`.
- Add paths to .vcsignore (one per line) to exclude files/folders from being tracked.

Example:
```
!keep_text.txt # Negations must always be at the top 
build/
*.cpp
```
