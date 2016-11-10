// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cassert>
#include <cstdint>

#include "packet_parser.h"
#include "parsed_packet_processor.h"

void PacketParser::ProcessPacket(const unsigned char* bytes,
                                 std::size_t bytes_length,
                                 std::size_t original_length) {
  // Strip off the ethernet header and don't account for it in the
  // histograms. We're not interested in accounting the link layer.
  assert(bytes_length >= BytesNeededEthernetHeader);
  bytes += BytesNeededEthernetHeader;
  bytes_length -= BytesNeededEthernetHeader;
  original_length -= BytesNeededEthernetHeader;

  if (bytes_length >= 20 && (bytes[0] & 0xf0) == 0x40) {
    // Proper IPv4 packet. Extract source and destination addresses.
    std::uint32_t src = (std::uint32_t)bytes[12] << 24 |
                        (std::uint32_t)bytes[13] << 16 |
                        (std::uint32_t)bytes[14] << 8 | bytes[15];
    std::uint32_t dst = (std::uint32_t)bytes[16] << 24 |
                        (std::uint32_t)bytes[17] << 16 |
                        (std::uint32_t)bytes[18] << 8 | bytes[19];
    processor_->ProcessIPv4Packet(src, dst, original_length);
  } else {
    // Unknown packet type.
    processor_->ProcessUnknownPacket(original_length);
  }
}
