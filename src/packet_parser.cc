// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cstdint>

#include "packet_parser.h"
#include "parsed_packet_processor.h"

void PacketParser::ProcessPacket(const unsigned char* bytes,
                                 std::size_t bytes_length,
                                 std::size_t original_length) {
  if (bytes_length >= BytesNeededIPv4 && (bytes[14] & 0xf0) == 0x40) {
    std::uint32_t src = (std::uint32_t)bytes[26] << 24 |
                        (std::uint32_t)bytes[27] << 16 |
                        (std::uint32_t)bytes[28] << 8 | bytes[29];
    std::uint32_t dst = (std::uint32_t)bytes[30] << 24 |
                        (std::uint32_t)bytes[31] << 16 |
                        (std::uint32_t)bytes[32] << 8 | bytes[33];
    processor_->ProcessIPv4Packet(src, dst, original_length);
  } else {
    processor_->ProcessUnknownPacket(original_length);
  }
}
