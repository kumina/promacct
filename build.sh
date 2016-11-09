#!/bin/sh

set -e
c++ -std=gnu++1y -o promacct src/*.cc -Wall -Werror -lpcap -lpthread
clang-format -i src/*
