#!/bin/sh

set -e
c++ -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -static -std=gnu++1y -o promacct src/*.cc -Wall -Werror -lpcap
clang-format -i src/*
