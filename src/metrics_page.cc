// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <string_view>

#include "metrics_labels.h"
#include "metrics_page.h"

void MetricsPage::PrintMetric(std::string_view name,
                              const MetricsLabels& labels,
                              std::uint64_t value) {
  // Print metric name, labels and its value.
  *output_ << prefix_ << name << '{';
  bool needs_comma = false;
  labels.Print(output_, &needs_comma);
  *output_ << "} " << value << std::endl;
}
