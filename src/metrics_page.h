// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef METRICS_PAGE_H
#define METRICS_PAGE_H

#include <map>
#include <ostream>
#include <string>

class MetricsPage {
 public:
  MetricsPage(std::ostream* ostream) : ostream_(ostream) {
  }

  void PrintMetric(const std::string& name,
                   const std::map<std::string, std::string>& labels,
                   uint64_t value);

 private:
  std::ostream* const ostream_;
};

#endif
