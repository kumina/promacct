// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PARSED_PACKET_COUNTER_H
#define PARSED_PACKET_COUNTER_H

#include <cstdint>
#include <vector>

#include "histogram.h"
#include "parsed_packet_processor.h"

class IPv4Ranges;
class MetricsLabel;
class MetricsPage;

class ParsedPacketCounter : public ParsedPacketProcessor {
 public:
  explicit ParsedPacketCounter(const IPv4Ranges* aggregation_ipv4);

  void ProcessIPv4Packet(std::uint32_t src, std::uint32_t dst,
                         std::size_t original_length) override;
  void ProcessUnknownPacket(std::size_t original_length) override;

  void PrintMetrics(const MetricsLabel* labels, MetricsPage* output);

 private:
  typedef Histogram<64, 128, 256, 512, 1024, 2048> PacketSizeHistogram;

  const IPv4Ranges* const aggregation_ipv4_;

  PacketSizeHistogram packet_size_bytes_all_;
  std::vector<PacketSizeHistogram> packet_size_bytes_ipv4_tx_;
  std::vector<PacketSizeHistogram> packet_size_bytes_ipv4_rx_;
};

#endif
