# cdtags & fuzzy-search

A modern, fast, and flexible replacement for `cd` with configurable aliases and
fuzzy search capabilities. Written in C++ and Bash.

## Features
- **Tag-based directory navigation**: Assign short tags to long or hard-to-remember paths.
- **Fuzzy search**: Quickly find files, directories, or history entries using the new `fuzzy-search` tool (C++).
- **Shell integration**: Bash functions and keybindings for seamless workflow.
- **Minimal dependencies**: No Python required; fast startup even on NFS or slow home directories. 

---

## Why cdtags?

While [dtags](https://github.com/joowani/dtags) is fantastic, it can be slow to
initialize in some environments (e.g., NFS home directories). `cdtags` is
designed for speed and simplicity, with a Bash-first approach and a C++ backend
for performance-critical features.

## fuzzy-search

I'll admin, [fzf](https://github.com/junegunn/fzf) is likely a better tool with
better support, but it requires a go compiler, which may not be available in all
environments.   C++ & boost is generally available in most linux environments.
I wrote this mostly out of curiousity, but also to have a fzf-like tool on an
environment where the toolchain needed by fzf is not available.  The core
scoring algorithm are AI generated implementations of well-known algorithms.   A
bulk of the dcode for this is just input/output processing needed to make a
responsive application able to handle signifiant volumes of data that is
streamed in.

---

## Requirements
- conan 2.x (might not be required if boost-devel packages are provided)
- C++17 compatible compiler
- [Boost](https://www.boost.org/) (headers and iostreams)
- CMake >= 3.10

---

## Installation

```sh
# Install cmake/C++ toolchain
# See OS documentaiton on how to install cmake/c++ toolchain with apt/dnf
# Install conan
pip install conan
# Clone Repository
git clone <repo-url> cdtags
cd cdtags
# Install dependencies with conan
conan install . --output-folder=build/release --build=missing -s build_type=Release
# Build (Release mode recommended)
cd build/release
cmake -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DCMAKE_INSTALL_PREFIX+$(HOME)/.local \
    ../..
cmake --build . --parallel 
cmake --build . --target install
```

### Shell Integration
Add the following to your `~/.bashrc`:
```sh
source $HOME/.local//share/cdtags/cdtags-activate.sh
source $HOME/.local/share/cdtags/fuzzy-search/fuzzy-search-activate.sh
```

This enables tag navigation and fuzzy search keybindings in your shell.

---

# Usage

## Tag Management
- **Add a tag:**
  ```sh
  cdtags add <tag> <path>
  ```
- **Remove a tag:**
  ```sh
  cdtags remove <tag>
  ```
- **List tags:**
  ```sh
  cdtags list
  ```

## Directory Navigation
- **Jump to a tag or path:**
  ```sh
  cdt <tag-or-path>
  ```
  - Works with absolute, relative, or tag-relative paths.
  - Command-line completion is supported.

## Fuzzy Search Tool

The `fuzzy-search` tool provides fast, interactive fuzzy finding for files, directories, and shell history.

### Keybindings (Bash)
- <kbd>Ctrl+f</kbd>: Fuzzy-find and insert a file path at the cursor
- <kbd>Ctrl+t</kbd>: Fuzzy-find and insert a directory path at the cursor
- <kbd>Ctrl+r</kbd>: Fuzzy-search your shell history

### Example
```sh
# Fuzzy search for a file in the current directory tree
find . -type f | fuzzy-search --stdin

# Fuzzy search for a directory
find . -type d | fuzzy-search --stdin
```

---

## Advanced
- You can customize fuzzy-search keybindings in `fuzzy-search-activate.sh`.

---

## Contributing
Pull requests and issues are welcome! Please ensure your code is formatted and tested.

---

## License
MIT License. See [LICENSE.txt](LICENSE.txt).
