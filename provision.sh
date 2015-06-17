#! /usr/bin/env bash

# Some dependencies
sudo apt-get update
sudo apt-get install -y libjudy-dev make g++ gdb git

(
    cd /vagrant/
    git submodule update --init --recursive
)
