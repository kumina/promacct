#!/bin/sh

docker run -i -v `pwd`:/promacct alpine:edge /bin/sh << 'EOF'
set -ex

# Install prerequisites for the build process.
apk update
apk add g++ libpcap-dev

cd /promacct
#c++ -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -static -std=gnu++1y -o promacct src/*.cc -Wall -Werror -lpcap
c++ -static -std=gnu++1y -o promacct src/*.cc -Wall -Werror -lpcap
EOF
