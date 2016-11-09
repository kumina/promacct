// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef IPV4_RANGES_H
#define IPV4_RANGES_H

#include <cstdint>
#include <experimental/optional>
#include <forward_list>

class IPv4Ranges {
 public:
  void AddRange(uint32_t first, uint32_t last);
  std::size_t GetLength() const;
  std::experimental::optional<std::size_t> GetIndex(
      std::uint32_t address) const;

 private:
  std::forward_list<std::pair<uint32_t, uint32_t>> ranges_;
};

#endif
