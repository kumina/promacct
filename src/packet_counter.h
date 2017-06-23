// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PACKET_COUNTER_H
#define PACKET_COUNTER_H

#include <cstdint>
#include <vector>

#include "histogram.h"
#include "parsed_packet_processor.h"
#include "protocol_histogram.h"

class IPv4Ranges;
class MetricsLabels;
class MetricsPage;

// Counts network packets, aggregating them by IPv4 source/destination
// address.
class PacketCounter : public ParsedPacketProcessor {
 public:
  explicit PacketCounter(const IPv4Ranges* aggregation_ipv4);

  // Counts an IPv4 packet.
  void ProcessIPv4Packet(std::uint32_t src, std::uint32_t dst,
                         std::uint8_t protocol,
                         std::size_t original_length) override;
  // Counts a network packet of an unknown type.
  void ProcessUnknownPacket(std::size_t original_length) override;

  // Prints all of the stored histograms to the metrics page output.
  void PrintMetrics(const MetricsLabels& labels, MetricsPage* output);

 private:
  typedef Histogram<64, 128, 256, 512, 1024, 2048> PacketSizeHistogram;
  typedef ProtocolHistogram<PacketSizeHistogram> ProtocolPacketSizeHistogram;

  const IPv4Ranges* const aggregation_ipv4_;

  // Histogram storing the sizes of all incoming network packets.
  PacketSizeHistogram packet_size_bytes_all_;

  // Per-IPv4 address histogram storing the sizes of all packets having
  // the address as its source.
  std::vector<ProtocolPacketSizeHistogram> packet_size_bytes_ipv4_tx_;

  // Per-IPv4 address histogram storing the sizes of all packets having
  // the address as its destination.
  std::vector<ProtocolPacketSizeHistogram> packet_size_bytes_ipv4_rx_;
};

#endif
