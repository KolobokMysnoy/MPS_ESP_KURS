#!/bin/bash

# Esp8266 copy all files from one location to folders of esp8266
source_directories_esp8266=("./connection/esp8266" "./sendStructs")
destination_directories_esp8266=("personChecker" "LuxChecker" "LED/led" "Driver/Driver")

# Esp32 copy all files from one location to folder of esp32
source_directories_esp32=("./connection/esp32" "./sendStructs")
destination_directories_esp32=("esp32Center")

# Function to perform the copy
copy_files() {
    local source_dir=$1
    local dest_dirs=("${@:2}")

    # Loop through each file in the source directory
    for file in "$source_dir"/*; do
        # Check if the item is a file (not a directory)
        if [ -f "$file" ]; then
            # Copy the file to each destination directory if it exists
            for dest_dir in "${dest_dirs[@]}"; do
                if [ -d "$dest_dir" ]; then
                    cp "$file" "./$dest_dir"
                else
                    echo "Destination directory $dest_dir does not exist!"
                fi
            done
        fi
    done
}

# Loop through each source directory for esp8266
for source_dir in "${source_directories_esp8266[@]}"; do
    # Check if source directory exists
    if [ ! -d "$source_dir" ]; then
        echo "Source directory $source_dir does not exist!"
        continue
    fi

    # Call the function to copy files
    copy_files "$source_dir" "${destination_directories_esp8266[@]}"
done

# Loop through each source directory for esp32
for source_dir in "${source_directories_esp32[@]}"; do
    # Check if source directory exists
    if [ ! -d "$source_dir" ]; then
        echo "Source directory $source_dir does not exist!"
        continue
    fi

    # Call the function to copy files
    copy_files "$source_dir" "${destination_directories_esp32[@]}"
done
