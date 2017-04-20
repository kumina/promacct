#!/bin/sh

docker run -i -v `pwd`:/promacct alpine:edge /bin/sh << 'EOF'
set -ex

# Install prerequisites for the build process.
apk update
apk add g++ libpcap-dev

cd /promacct
c++ -static -std=gnu++1z -o promacct src/*.cc -Wall -Werror -lpcap
EOF
