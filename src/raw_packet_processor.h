// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef RAW_PACKET_PROCESSOR_H
#define RAW_PACKET_PROCESSOR_H

#include <cstddef>

class RawPacketProcessor {
 public:
  // TODO(ed): Should use std::basic_string_view<unsigned char>.
  virtual void ProcessPacket(const unsigned char* bytes,
                             std::size_t bytes_length,
                             std::size_t original_length) = 0;
};

#endif
