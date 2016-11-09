// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cstdint>
#include <experimental/optional>

#include "ipv4_ranges.h"

void IPv4Ranges::AddRange(uint32_t first, uint32_t last) {
  ranges_.push_back({first, last});
}

std::size_t IPv4Ranges::GetLength() const {
  std::size_t length = 0;
  for (const auto& range : ranges_)
    length += range.second - range.first + 1;
  return length;
}

std::experimental::optional<std::size_t> IPv4Ranges::GetIndex(
    std::uint32_t address) const {
  std::size_t offset = 0;
  for (const auto& range : ranges_) {
    if (address >= range.first && address <= range.second)
      return offset + address - range.first;
    offset += range.second - range.first + 1;
  }
  return {};
}
