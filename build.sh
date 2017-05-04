#!/bin/sh

set -e
c++ -O2 -std=gnu++1z -o promacct src/*.cc -Wall -Werror -lpcap
clang-format -i src/*
