// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cassert>
#include <cstdint>
#include <experimental/string_view>

#include "packet_parser.h"
#include "parsed_packet_processor.h"

void PacketParser::ProcessPacket(
    std::experimental::basic_string_view<unsigned char> bytes,
    std::size_t original_length) {
  // Strip off the ethernet header and don't account for it in the
  // histograms. We're not interested in accounting the link layer.
  assert(bytes.size() >= BytesNeededEthernetHeader);
  assert(original_length >= bytes.size());
  bytes.remove_prefix(BytesNeededEthernetHeader);
  original_length -= BytesNeededEthernetHeader;

  if (bytes.size() >= 20 && (bytes[0] & 0xf0) == 0x40) {
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
