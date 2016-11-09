// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PARSED_PACKET_PROCESSOR_H
#define PARSED_PACKET_PROCESSOR_H

#include <cstdint>

class ParsedPacketProcessor {
 public:
  virtual void ProcessIPv4Packet(std::uint32_t src, std::uint32_t dst,
                                 std::size_t original_length) = 0;
  virtual void ProcessUnknownPacket(std::size_t original_length) = 0;
};

#endif
