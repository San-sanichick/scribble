#!/bin/bash

git_path=$1


if [[ -z "$git_path" ]]; then
    echo "No path provided"
    exit 1
fi

repo_name=$(basename $git_path .git)

cd vendors

if [[ -d $repo_name ]]; then
    echo "Module '$repo_name' is already installed"
    exit 1
fi

echo "Installing submodule '$repo_name'..."
git submodule add $git_path

echo "Done."
echo "Now add the module to CMakeLists.txt"

