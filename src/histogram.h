// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <array>
#include <cstdint>

template <unsigned int... Buckets>
class Histogram {
 public:
  Histogram() : count_(), sum_(), buckets_() {
  }

  void Record(uint64_t value) {
    ++count_;
    sum_ += value;
  }

 private:
  static constexpr std::array<unsigned int, sizeof...(Buckets)>
      bucket_boundaries_{Buckets...};

  uint64_t count_;
  uint64_t sum_;
  std::array<uint64_t, sizeof...(Buckets)> buckets_;
};

#endif
