// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include "metrics_page.h"
#include "metrics_labels.h"

void MetricsPage::PrintMetric(const std::string& name,
                              const MetricsLabels* labels, uint64_t value) {
  // Print metric name, including its labels.
  *output_ << prefix_ << name;
  if (labels != nullptr) {
    *output_ << '{';
    labels->Print(output_);
    *output_ << '}';
  }

  // Print metric value.
  *output_ << ' ' << value << std::endl;
}
