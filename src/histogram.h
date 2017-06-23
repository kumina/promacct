// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <array>
#include <cstdint>
#include <string>

#include "metrics_labels.h"
#include "metrics_page.h"

// Prometheus-style histogram.
//
// Prometheus has a standard convention for processing histograms. This
// templated class provides a convenient way of creating such objects.
// The template arguments can be used to specify the bucket cutoffs.
//
// A histogram containing four buckets ("32", "64", "128" and "+Inf")
// can be created as follows:
//
//     Histogram<32, 64, 128> my_histogram;
template <unsigned int... Buckets>
class Histogram {
 public:
  Histogram() : count_(), sum_(), buckets_() {
  }

  // Stores a new sample value in the histogram object.
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

  // Prints all values stored in the histogram object.
  void PrintMetrics(const std::string& name, const MetricsLabels& labels,
                    MetricsPage* output) const {
    if (count_ > 0) {
      // Print scalar values.
      output->PrintMetric(name + "_count", labels, count_);
      output->PrintMetric(name + "_sum", labels, sum_);

      // Print bucket counters.
      const unsigned int boundaries[] = {Buckets...};
      for (unsigned int i = 0; i < sizeof...(Buckets); ++i) {
        std::string boundary = std::to_string(boundaries[i]);
        MetricsLabel le("le", boundary);
        MetricsLabelsJoiner joiner(&labels, &le);
        output->PrintMetric(name + "_bucket", joiner, buckets_[i]);
      }
      MetricsLabel le("le", "+Inf");
      MetricsLabelsJoiner joiner(&labels, &le);
      output->PrintMetric(name + "_bucket", joiner, count_);
    }
  }

 private:
  std::uint64_t count_;  // Number of samples.
  std::uint64_t sum_;    // Sum of all samples.
  std::array<std::uint64_t, sizeof...(Buckets)> buckets_;  // Histogram buckets.
};

#endif
