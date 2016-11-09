// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "metrics_page.h"

void MetricsPage::PrintMetric(const std::string& name,
                              const std::map<std::string, std::string>& labels,
                              uint64_t value) {
  // Print metric name, including its labels.
  *ostream_ << name;
  if (!labels.empty()) {
    char separator = '{';
    for (const auto& label : labels) {
      *ostream_ << separator << label.first << "=\"" << label.second << '"';
      separator = ',';
    }
    *ostream_ << '}';
  }

  // Print metric value.
  *ostream_ << ' ' << value << std::endl;
}
