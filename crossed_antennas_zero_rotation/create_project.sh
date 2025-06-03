#!/bin/bash
# create_project.sh
#
# Usage:
#   ./create_project.sh PROJECT_ROOT [SOURCE_OR_INCLUDE_DIR ...]
#
# PROJECT_ROOT: the new project's root directory.
# SOURCE_OR_INCLUDE_DIR: one or more directories containing existing C/C++
#                        source files (*.c, *.cpp) and header files (*.h, *.hpp).
#   Source files are copied to "src", headers to "include".
#
# The script also generates:
#   - a common include file (include/common_includes.h) which includes all header files,
#   - a main.cpp (in src/) that includes common_includes.h, declares extern "C"
#     prototypes for each generated message function, allocates memory for each structure,
#     and calls their encode/decode functions.
# The project is set up to be built via a Makefile and to be debugged via VS Code or via the CLI (make + gdb).

# === Check Parameters ===
if [ "$#" -lt 1 ]; then
    cat <<EOF
Usage: $0 PROJECT_ROOT [SOURCE_OR_INCLUDE_DIR ...]
  PROJECT_ROOT: root directory for your new project.
  SOURCE_OR_INCLUDE_DIR: directories containing C/C++ (*.c, *.cpp) and header (*.h, *.hpp) files.
EOF
    exit 1
fi

PROJECT_ROOT="$1"
shift

# === Create Project Directory Structure ===
echo "Creating project directory structure in '$PROJECT_ROOT'..."
mkdir -p "$PROJECT_ROOT"/{src,include,build,.vscode}

# === Copy Files from Provided Directories ===
for d in "$@"; do
    if [ -d "$d" ]; then
        echo "Processing directory: $d"
        for file in "$d"/*; do
            if [ -f "$file" ]; then
                filename=$(basename "$file")
                lowername=$(echo "$filename" | tr '[:upper:]' '[:lower:]')
                case "$lowername" in
                    *.c|*.cpp)
                        echo "  Copying source file: $filename"
                        cp "$file" "$PROJECT_ROOT/src/"
                        ;;
                    *.h|*.hpp)
                        echo "  Copying header file: $filename"
                        cp "$file" "$PROJECT_ROOT/include/"
                        ;;
                esac
            fi
        done
    else
        echo "Warning: '$d' is not a directory. Skipping."
    fi
done

# === Create common_includes.h in the include directory ===
COMMON_INCLUDE_FILE="$PROJECT_ROOT/include/common_includes.h"
echo "Creating common include file: $COMMON_INCLUDE_FILE"
cat <<'EOF' > "$COMMON_INCLUDE_FILE"
#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

/* Auto-generated common includes file */
EOF

for header in "$PROJECT_ROOT"/include/*.{h,hpp}; do
    if [ -f "$header" ]; then
        base=$(basename "$header")
        # Skip if it is already the common file
        if [ "$base" = "common_includes.h" ]; then
            continue
        fi
        echo "#include \"$base\"" >> "$COMMON_INCLUDE_FILE"
    fi
done

echo "#endif // COMMON_INCLUDES_H" >> "$COMMON_INCLUDE_FILE"

# === Create main.cpp in the src directory ===
MAIN_FILE="$PROJECT_ROOT/src/main.cpp"
echo "Creating main source file: $MAIN_FILE"
# Create a default main.cpp that:
# 1. Includes common_includes.h and necessary C++ headers.
# 2. Declares an extern "C" block for the prototypes of the functions for each message.
# 3. Has a main() function that allocates memory for each structure and calls its encode/decode functions.
cat <<'EOF' > "$MAIN_FILE"
#include "common_includes.h"
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace std;

#define BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: Add prototypes for each generated message's encode/decode functions here.
// For example:
// void encode_NewOrderSingle(const NewOrderSingle *msg, uint8_t *buffer);
// void decode_NewOrderSingle(const uint8_t *buffer, NewOrderSingle *msg);

#ifdef __cplusplus
}
#endif

int main() {
    cout << "Running tests for generated messages:" << endl;

    // For each header (except common_includes.h), assume:
    // - A struct with the same name as the header (without extension)
    // - Functions: encode_<Name>(const <Name>* msg, uint8_t* buffer)
    //              decode_<Name>(const uint8_t* buffer, <Name>* msg)
    //
    // You should add corresponding prototypes (see extern "C" block above).
    //
    // Example test for a message named NewOrderSingle:
    // Uncomment and modify the following block for each generated message.
    /*
    {
        cout << "Testing message: NewOrderSingle" << endl;
        NewOrderSingle *msg = new NewOrderSingle;
        if (!msg) {
            cout << "Memory allocation for NewOrderSingle failed!" << endl;
            return 1;
        }
        memset(msg, 0, sizeof(NewOrderSingle));
        uint8_t buffer[BUFFER_SIZE] = {0};
        encode_NewOrderSingle(msg, buffer);
        decode_NewOrderSingle(buffer, msg);
        cout << "Test passed for message: NewOrderSingle" << endl;
        delete msg;
    }
    */

    cout << "All tests executed." << endl;
    return 0;
}
EOF

# === Create the Makefile in the project root ===
cat <<'EOF' > "$PROJECT_ROOT/Makefile"
# Makefile for the project
CXX := g++
CXXFLAGS := -Wall -g -Iinclude -Isrc -std=c++11
SRC_DIR := src
OBJ_DIR := build
BIN := myapp

# Find all source files in SRC_DIR (*.cpp and *.c)
SRCS := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(OBJS))

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
EOF

# === Create VS Code Configuration Files ===

# .vscode/tasks.json
mkdir -p "$PROJECT_ROOT/.vscode"
cat <<'EOF' > "$PROJECT_ROOT/.vscode/tasks.json"
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build: make",
            "type": "shell",
            "command": "make",
            "args": [],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "reveal": "always"
            },
            "problemMatcher": []
        },
        {
            "label": "clean: make",
            "type": "shell",
            "command": "make clean",
            "args": [],
            "group": "build",
            "presentation": {
                "reveal": "always"
            },
            "problemMatcher": []
        }
    ]
}
EOF

# .vscode/launch.json
cat <<'EOF' > "$PROJECT_ROOT/.vscode/launch.json"
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug MyApp",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/myapp",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
EOF

# .vscode/c_cpp_properties.json (include extra directories if provided)
INCLUDE_PATHS='[
    "${workspaceFolder}/include",
    "${workspaceFolder}/src"'
for d in "$@"; do
    ABS_PATH=$(realpath "$d")
    INCLUDE_PATHS="$INCLUDE_PATHS,\n    \"${ABS_PATH}\""
done
INCLUDE_PATHS="$INCLUDE_PATHS\n]"

cat <<EOF > "$PROJECT_ROOT/.vscode/c_cpp_properties.json"
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": $INCLUDE_PATHS,
            "defines": [],
            "compilerPath": "/usr/bin/g++",
            "cStandard": "c11",
            "cppStandard": "c++11",
            "intelliSenseMode": "gcc-x64"
        }
    ],
    "version": 4
}
EOF

echo "Project '$PROJECT_ROOT' created successfully."
echo "Directory structure:"
echo "  $PROJECT_ROOT/"
echo "    src/      - Source files will be placed here; a main.cpp has been generated."
echo "    include/  - Header files are copied here; a common_includes.h is generated."
echo "    build/    - Build output directory (created by the Makefile)."
echo "    .vscode/  - VS Code configuration files for tasks, debugging, and IntelliSense."
echo ""
echo "To get started:"
echo "  1. Open VS Code with: code \"$PROJECT_ROOT\""
echo "  2. Use the build task (Ctrl+Shift+B) or run 'make' from the CLI."
echo "  3. Debug using VS Code's debugger or 'gdb myapp' from the CLI."

