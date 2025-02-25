#!/bin/bash

# Directory to search, default is current directory
DIR="${1:-.}"

# Loop through each file in the directory and subdirectories
find "$DIR" -type f | while read -r file; do
    # Check if the file is a C/C++ source/header file (you can adjust the pattern)
    if [[ "$file" =~ \.cpp$|\.h$|\.c$ ]]; then
        echo "Processing $file..."
        # For macOS: sed -i '' -E for in-place replacement (empty string for backup)
        # For Linux: sed -i -E for in-place replacement
        if [[ "$OSTYPE" == "darwin"* ]]; then
            # macOS
            sed -i '' -E 's/#include <([^>]+)\.h"/#include <\1.h>/' "$file"
        else
            # Linux and other Unix-like systems
            sed -i -E 's/#include <([^>]+)\.h"/#include <\1.h>/' "$file"
        fi
    fi
done

echo "Script completed."
