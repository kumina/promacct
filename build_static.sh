#!/bin/sh

docker run -i -v `pwd`:/promacct alpine:edge /bin/sh << 'EOF'
set -ex

# Install prerequisites for the build process.
apk update
apk add g++ libpcap-dev

# Compatibility with C++17. This can be removed when Alpine Linux
# switches to GCC 7, which will include these headers by default.
cat > /usr/include/optional << EOF2
#include <experimental/optional>
namespace std {
using std::experimental::optional;
}
EOF2
cat > /usr/include/string_view << EOF2
#include <experimental/string_view>
namespace std {
using std::experimental::basic_string_view;
using std::experimental::string_view;
}
EOF2

cd /promacct
c++ -O2 -static -std=gnu++1z -o promacct src/*.cc -Wall -Werror -lpcap
strip promacct
EOF
