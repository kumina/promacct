// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef IPV4_RANGES_H
#define IPV4_RANGES_H

#include <cstdint>
#include <optional>
#include <vector>

class MetricsLabels;

// Set of IPv4 address ranges.
//
// We want to be able to aggregate traffic based on IPv4 address ranges.
// This class implements a storage for such ranges, keeping track of
// them as pairs of first and last IPv4 address.
class IPv4Ranges {
 public:
  // Adds a new IPv4 address range to the store.
  void AddRange(const MetricsLabels* labels, std::uint32_t first,
                std::uint32_t last);

  // Returns the total number of IPv4 addresses stored within.
  std::size_t GetLength() const;

  // Given an IPv4 address, return a unique identifier. This is used by
  // ParsedPacketCounter to store all histograms contiguously.
  std::optional<std::size_t> GetIndexByAddress(std::uint32_t address) const;

  // Given a unique identifier, return the IPv4 address. This is used by
  // ParsedPacketCounter to reobtain the IPv4 address associated with a
  // histogram, used for printing metrics.
  std::pair<const MetricsLabels*, std::uint32_t> GetAddressByIndex(
      std::size_t) const;

 private:
  struct IPv4Range {
    const MetricsLabels* labels;
    std::uint32_t first;
    std::uint32_t last;
  };

  std::vector<IPv4Range> ranges_;
};

#endif
