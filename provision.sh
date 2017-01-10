#! /usr/bin/env bash

set -e

# Some dependencies
sudo apt-get update
sudo apt-get install -y make g++ gdb git cmake libgtest-dev clang-3.5 python-pip

# For coverage
sudo pip install gcovr==3.2

pushd /usr/src/gtest
    sudo CXX=g++ CC=gcc cmake CMakeLists.txt
    sudo CXX=g++ CC=gcc make
    find /usr/src/gtest -name '*.a' | xargs -I^ sudo ln -sf ^ /usr/lib
popd
