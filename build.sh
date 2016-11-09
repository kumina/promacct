#!/bin/sh

set -e
c++ -std=c++11 -o promacct src/*.cc -Wall -Werror -lpcap
clang-format -i src/*
