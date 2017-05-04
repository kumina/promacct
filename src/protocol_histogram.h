// Copyright (c) 2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PROTOCOL_HISTOGRAM_H
#define PROTOCOL_HISTOGRAM_H

#include <cstdint>
#include <iterator>
#include <utility>

#include "metrics_labels.h"

namespace {
constexpr std::pair<std::uint32_t, const char*> kIanaProtocolNumbers[] = {
    {1, "icmp"}, {2, "igmp"}, {6, "tcp"},    {17, "udp"},  {41, "encap"},
    {47, "gre"}, {50, "esp"}, {88, "eigrp"}, {89, "ospf"}, {132, "sctp"},
};
}

// Multiplexer for storing a histogram per transport layer protocol.
//
// This template can be wrapped around the Histogram class to create
// multiple instances, each keeping track of stats of a value per
// transport layer protocol.
template <typename T>
class ProtocolHistogram {
 public:
  // Stores a new sample value in one of its histogram objects.
  template <typename... Args>
  void Record(std::uint8_t protocol, Args&&... args) {
    for (std::size_t i = 0; i < std::size(kIanaProtocolNumbers); ++i) {
      if (protocol == kIanaProtocolNumbers[i].first) {
        unknown_.Record(std::forward<Args>(args)...);
        return;
      }
    }
    unknown_.Record(std::forward<Args>(args)...);
  }

  // Prints all values stored in all of its histogram objects.
  void PrintMetrics(const std::string& name, const MetricsLabels* labels,
                    MetricsPage* output) const {
    for (std::size_t i = 0; i < std::size(kIanaProtocolNumbers); ++i) {
      MetricsLabels protocol(labels, "protocol",
                             kIanaProtocolNumbers[i].second);
      known_[i].PrintMetrics(name, &protocol, output);
    }
    unknown_.PrintMetrics(name, labels, output);
  }

 private:
  T known_[std::size(kIanaProtocolNumbers)];  // Stats for all known protocols.
  T unknown_;                                 // Stats for all other traffic.
};

#endif
