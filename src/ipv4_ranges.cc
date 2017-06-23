// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cassert>
#include <cstdint>
#include <optional>

#include "ipv4_ranges.h"

void IPv4Ranges::AddRange(const MetricsLabels* labels, std::uint32_t first,
                          std::uint32_t last) {
  assert(first <= last && "Not a valid IPv4 range.");
  ranges_.push_back({labels, first, last});
}

std::size_t IPv4Ranges::GetLength() const {
  std::size_t length = 0;
  for (const auto& range : ranges_)
    length += range.last - range.first + 1;
  return length;
}

std::optional<std::size_t> IPv4Ranges::GetIndexByAddress(
    std::uint32_t address) const {
  std::size_t offset = 0;
  for (const auto& range : ranges_) {
    if (address >= range.first && address <= range.last)
      return offset + address - range.first;
    offset += range.last - range.first + 1;
  }
  return {};
}

std::pair<const MetricsLabels*, std::uint32_t> IPv4Ranges::GetAddressByIndex(
    std::size_t idx) const {
  for (const auto& range : ranges_) {
    if (idx <= range.last - range.first)
      return {range.labels, range.first + idx};
    idx -= range.last - range.first + 1;
  }
  assert(0 && "GetAddressByIndex() out of bounds");
}
