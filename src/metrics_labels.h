// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef METRICS_LABELS_H
#define METRICS_LABELS_H

#include <ostream>
#include <string_view>

// Utility class for attaching labels to metrics.
//
// This program models labels as being stored in a linked list,
// terminated with 'nullptr'. This approach allows us to efficiently
// propagate labels down to other functions on the call stack.
//
// For example, two labels on a metric can be configured as follows:
//
//     MetricsLabel l1(null, "key1", "value1");
//     MetricsLabel l2(&l1, "key1", "value1");
//     MetricsPage *p = ...;
//     p->PrintMetric("name", &l2, 123);
//
// This will result in 'name{key1="value1",key2="value"} 123' being
// emitted onto the metrics page. This class does not own the strings
// passed to the constructor.
class MetricsLabels {
 public:
  MetricsLabels(const MetricsLabels* inherit, std::string_view key,
                std::string_view value)
      : inherit_(inherit), key_(key), value_(value) {
  }

  // Prints the set of labels of a metric to the output stream.
  void Print(std::ostream* output) const {
    const MetricsLabels* label = this;
    for (;;) {
      *output << label->key_ << "=\"" << label->value_ << '"';
      if (label->inherit_ == nullptr)
        break;
      label = label->inherit_;
      *output << ',';
    }
  }

 private:
  const MetricsLabels* const inherit_;  // Next labels.
  std::string_view const key_;          // Key of this label.
  std::string_view const value_;        // Value of this label.
};

#endif
