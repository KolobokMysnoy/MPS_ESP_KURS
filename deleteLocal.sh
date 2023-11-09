#!/bin/bash

# Directories to delete files from
directories=("esp32Center" "personChecker" "LuxChecker" "LED/led" "Driver/Driver")

# Files to be deleted
files_to_delete=("Connect32.hpp" "Connect32.cpp" "Connect.cpp" "Connect.hpp" "sendStructs.hpp")

# Function to delete files
delete_files() {
    local directory=$1
    local files=("${@:2}")

    # Check if the directory exists
    if [ -d "$directory" ]; then
        # Delete each specified file in the directory
        for file in "${files[@]}"; do
            file_path="$directory/$file"
            if [ -e "$file_path" ]; then
                rm "$file_path"
                echo "Deleted $file_path"
            else
                echo "File $file_path does not exist!"
            fi
        done
    else
        echo "Directory $directory does not exist!"
    fi
}

# Loop through each directory
for directory in "${directories[@]}"; do
    # Call the function to delete files
    delete_files "$directory" "${files_to_delete[@]}"
done
