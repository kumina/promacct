// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <experimental/optional>
#include <iostream>  // TODO(ed): Remove!

#include "ipv4_ranges.h"
#include "parsed_packet_counter.h"

ParsedPacketCounter::ParsedPacketCounter(const IPv4Ranges* aggregation_ipv4)
    : aggregation_ipv4_(aggregation_ipv4) {
  // Prealllocate histograms for IPv4 address aggregation, so that
  // ProcessIPv4Packet() doesn't need to do any allocating.
  std::size_t ipv4_slots = aggregation_ipv4_->GetLength();
  packet_sizes_ipv4_tx_.resize(ipv4_slots);
  packet_sizes_ipv4_rx_.resize(ipv4_slots);
}

void ParsedPacketCounter::ProcessIPv4Packet(std::uint32_t src,
                                            std::uint32_t dst,
                                            std::size_t original_length) {
  packet_sizes_all_.Record(original_length);

  {
    std::experimental::optional<std::size_t> index =
        aggregation_ipv4_->GetIndex(src);
    if (index) {
      std::cout << "TX match " << src << std::endl;
      packet_sizes_ipv4_tx_[*index].Record(original_length);
    } else {
      std::cout << "TX mismatch " << src << std::endl;
    }
  }
  {
    std::experimental::optional<std::size_t> index =
        aggregation_ipv4_->GetIndex(dst);
    if (index) {
      std::cout << "RX match " << dst << std::endl;
      packet_sizes_ipv4_rx_[*index].Record(original_length);
    } else {
      std::cout << "RX mismatch " << dst << std::endl;
    }
  }
}

void ParsedPacketCounter::ProcessUnknownPacket(std::size_t original_length) {
  packet_sizes_all_.Record(original_length);
}
