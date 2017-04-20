// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cassert>
#include <cstdint>
#include <experimental/optional>
#include <experimental/string_view>
#include <string>

#include "ipv4_ranges.h"

void IPv4Ranges::AddRange(std::experimental::string_view description,
                          uint32_t first, uint32_t last) {
  assert(first <= last && "Not a valid IPv4 range.");
  ranges_.push_back({std::string(description), first, last});
}

std::size_t IPv4Ranges::GetLength() const {
  std::size_t length = 0;
  for (const auto& range : ranges_)
    length += range.last - range.first + 1;
  return length;
}

std::experimental::optional<std::size_t> IPv4Ranges::GetIndexByAddress(
    std::uint32_t address) const {
  std::size_t offset = 0;
  for (const auto& range : ranges_) {
    if (address >= range.first && address <= range.last)
      return offset + address - range.first;
    offset += range.last - range.first + 1;
  }
  return {};
}

std::pair<std::experimental::string_view, std::uint32_t>
IPv4Ranges::GetAddressByIndex(std::size_t idx) const {
  for (const auto& range : ranges_) {
    if (idx <= range.last - range.first)
      return {range.description, range.first + idx};
    idx -= range.last - range.first + 1;
  }
  assert(0 && "GetAddressByIndex() out of bounds");
}
