#! /usr/bin/env bash

set -e

# Some dependencies
sudo apt-get update
sudo apt-get install -y make g++ gdb git cmake libgtest-dev

pushd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    find /usr/src/gtest -name '*.a' | xargs -I^ sudo ln -sf ^ /usr/lib
popd
