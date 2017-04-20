// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cstdint>
#include <experimental/optional>
#include <experimental/string_view>
#include <sstream>
#include <string>

#include "ipv4_ranges.h"
#include "packet_counter.h"

PacketCounter::PacketCounter(const IPv4Ranges* aggregation_ipv4)
    : aggregation_ipv4_(aggregation_ipv4),
      packet_size_bytes_ipv4_tx_(aggregation_ipv4_->GetLength()),
      packet_size_bytes_ipv4_rx_(aggregation_ipv4_->GetLength()) {
  // Histograms for IPv4 address aggregation are already preallocated,
  // so that ProcessIPv4Packet() doesn't need to do any resizing.
}

void PacketCounter::ProcessIPv4Packet(std::uint32_t src, std::uint32_t dst,
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

void PacketCounter::ProcessUnknownPacket(std::size_t original_length) {
  packet_size_bytes_all_.Record(original_length);
}

void PacketCounter::PrintMetrics(const MetricsLabels* labels,
                                 MetricsPage* output) {
  packet_size_bytes_all_.PrintMetrics("packet_size_bytes_all", labels, output);

  for (std::size_t i = 0; i < aggregation_ipv4_->GetLength(); ++i) {
    // Compute IP address string representation.
    std::pair<std::experimental::string_view, std::uint32_t> addr =
        aggregation_ipv4_->GetAddressByIndex(i);
    std::stringstream addr_ss;
    addr_ss << (addr.second >> 24) << '.' << (addr.second >> 16 & 0xff) << '.'
            << (addr.second >> 8 & 0xff) << '.' << (addr.second & 0xff);
    std::string addr_str = addr_ss.str();
    MetricsLabels ip(labels, "ip", addr_str);
    MetricsLabels ip_range(&ip, "ip_range", addr.first);

    // Print aggregated TX/RX statistics.
    packet_size_bytes_ipv4_tx_[i].PrintMetrics("packet_size_bytes_ipv4_tx",
                                               &ip_range, output);
    packet_size_bytes_ipv4_rx_[i].PrintMetrics("packet_size_bytes_ipv4_rx",
                                               &ip_range, output);
  }
}
