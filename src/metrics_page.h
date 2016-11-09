// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef METRICS_PAGE_H
#define METRICS_PAGE_H

#include <ostream>
#include <string>

class MetricsLabels;

class MetricsPage {
 public:
  MetricsPage(const std::string& prefix, std::ostream* output)
      : prefix_(prefix), output_(output) {
  }

  void PrintMetric(const std::string& name, const MetricsLabels* labels,
                   uint64_t value);

 private:
  const std::string prefix_;
  std::ostream* const output_;
};

#endif
