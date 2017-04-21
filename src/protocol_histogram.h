// Copyright (c) 2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PROTOCOL_HISTOGRAM_H
#define PROTOCOL_HISTOGRAM_H

#include <cstdint>
#include <utility>

#include "metrics_labels.h"

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
    switch (protocol) {
      case 1:
        icmp_.Record(std::forward<Args>(args)...);
        break;
      case 2:
        igmp_.Record(std::forward<Args>(args)...);
        break;
      case 6:
        tcp_.Record(std::forward<Args>(args)...);
        break;
      case 17:
        udp_.Record(std::forward<Args>(args)...);
        break;
      case 41:
        encap_.Record(std::forward<Args>(args)...);
        break;
      case 89:
        ospf_.Record(std::forward<Args>(args)...);
        break;
      case 132:
        sctp_.Record(std::forward<Args>(args)...);
        break;
      default:
        unknown_.Record(std::forward<Args>(args)...);
        break;
    }
  }

  // Prints all values stored in all of its histogram objects.
  void PrintMetrics(const std::string& name, const MetricsLabels* labels,
                    MetricsPage* output) const {
    {
      MetricsLabels protocol(labels, "protocol", "icmp");
      icmp_.PrintMetrics(name, &protocol, output);
    }
    {
      MetricsLabels protocol(labels, "protocol", "igmp");
      igmp_.PrintMetrics(name, &protocol, output);
    }
    {
      MetricsLabels protocol(labels, "protocol", "tcp");
      tcp_.PrintMetrics(name, &protocol, output);
    }
    {
      MetricsLabels protocol(labels, "protocol", "udp");
      udp_.PrintMetrics(name, &protocol, output);
    }
    {
      MetricsLabels protocol(labels, "protocol", "encap");
      encap_.PrintMetrics(name, &protocol, output);
    }
    {
      MetricsLabels protocol(labels, "protocol", "ospf");
      ospf_.PrintMetrics(name, &protocol, output);
    }
    {
      MetricsLabels protocol(labels, "protocol", "sctp");
      sctp_.PrintMetrics(name, &protocol, output);
    }
    unknown_.PrintMetrics(name, labels, output);
  }

 private:
  T icmp_;     // Stats for the Internet Control Message Protocol.
  T igmp_;     // Stats for the Internet Group Management Protocol.
  T tcp_;      // Stats for the Transmission Control Protocol.
  T udp_;      // Stats for the User Datagram Protocol.
  T encap_;    // Stats for the IPv6 encapsulation protocol.
  T ospf_;     // Stats for the Open Shortest Path First protocol.
  T sctp_;     // Stats for the Stream Control Transmission Protocol.
  T unknown_;  // Stats for all other traffic.
};

#endif
