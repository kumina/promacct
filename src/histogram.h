// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <array>
#include <cstdint>

#include "metrics_label.h"
#include "metrics_page.h"

template <unsigned int... Buckets>
class Histogram {
 public:
  Histogram() : count_(), sum_(), buckets_() {
  }

  void Record(std::uint64_t value) {
    // Update scalar values.
    ++count_;
    sum_ += value;

    // Adjust bucket counters.
    const unsigned int boundaries[] = {Buckets...};
    for (unsigned int i = sizeof...(Buckets);
         i > 0 && value <= boundaries[i - 1]; --i)
      ++buckets_[i - 1];
  }

  void PrintMetrics(const std::string& name, const MetricsLabel* labels,
                    MetricsPage* output) const {
    if (count_ > 0) {
      // Print scalar values.
      output->PrintMetric(name + "_count", labels, count_);
      output->PrintMetric(name + "_sum", labels, sum_);

      // Print bucket counters.
      const unsigned int boundaries[] = {Buckets...};
      for (unsigned int i = 0; i < sizeof...(Buckets); ++i) {
        MetricsLabel le(labels, "le", std::to_string(boundaries[i]));
        output->PrintMetric(name + "_bucket", &le, buckets_[i]);
      }
      MetricsLabel le(labels, "le", "+Inf");
      output->PrintMetric(name + "_bucket", &le, count_);
    }
  }

 private:
  std::uint64_t count_;
  std::uint64_t sum_;
  std::array<std::uint64_t, sizeof...(Buckets)> buckets_;
};

#endif
