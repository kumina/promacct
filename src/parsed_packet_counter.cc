// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cstdint>
#include <experimental/optional>
#include <sstream>

#include "ipv4_ranges.h"
#include "parsed_packet_counter.h"

ParsedPacketCounter::ParsedPacketCounter(const IPv4Ranges* aggregation_ipv4)
    : aggregation_ipv4_(aggregation_ipv4) {
  // Prealllocate histograms for IPv4 address aggregation, so that
  // ProcessIPv4Packet() doesn't need to do any allocating.
  std::size_t ipv4_slots = aggregation_ipv4_->GetLength();
  packet_size_bytes_ipv4_tx_.resize(ipv4_slots);
  packet_size_bytes_ipv4_rx_.resize(ipv4_slots);
}

void ParsedPacketCounter::ProcessIPv4Packet(std::uint32_t src,
                                            std::uint32_t dst,
                                            std::size_t original_length) {
  packet_size_bytes_all_.Record(original_length);

  // Aggregation on source IPv4 address.
  {
    std::experimental::optional<std::size_t> index =
        aggregation_ipv4_->GetIndexByAddress(src);
    if (index)
      packet_size_bytes_ipv4_tx_[*index].Record(original_length);
  }

  // Aggregation on destination IPv4 address.
  {
    std::experimental::optional<std::size_t> index =
        aggregation_ipv4_->GetIndexByAddress(dst);
    if (index)
      packet_size_bytes_ipv4_rx_[*index].Record(original_length);
  }
}

void ParsedPacketCounter::ProcessUnknownPacket(std::size_t original_length) {
  packet_size_bytes_all_.Record(original_length);
}

void ParsedPacketCounter::PrintMetrics(const MetricsLabel* labels,
                                       MetricsPage* output) {
  packet_size_bytes_all_.PrintMetrics("packet_size_bytes_all", labels, output);

  for (std::size_t i = 0; i < aggregation_ipv4_->GetLength(); ++i) {
    // Compute IP address string representation.
    std::uint32_t addr_num = aggregation_ipv4_->GetAddressByIndex(i);
    std::stringstream addr_str;
    addr_str << (addr_num >> 24) << '.' << (addr_num >> 16 & 0xff) << '.'
             << (addr_num >> 8 & 0xff) << '.' << (addr_num & 0xff);
    MetricsLabel ip(labels, "ip", addr_str.str());

    // Print aggregated TX/RX statistics.
    packet_size_bytes_ipv4_tx_[i].PrintMetrics("packet_size_bytes_ipv4_tx", &ip,
                                               output);
    packet_size_bytes_ipv4_rx_[i].PrintMetrics("packet_size_bytes_ipv4_rx", &ip,
                                               output);
  }
}
