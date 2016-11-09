// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef METRICS_LABEL_H
#define METRICS_LABEL_H

#include <ostream>
#include <string>

class MetricsLabel {
 public:
  MetricsLabel(const MetricsLabel* next, const std::string& key,
               const std::string& value)
      : next_(next), key_(key), value_(value) {
  }

  const MetricsLabel* GetNext() const {
    return next_;
  }

  void Print(std::ostream* output) const {
    *output << key_ << "=\"" << value_ << '"';
  }

 private:
  const MetricsLabel* const next_;
  std::string const key_;
  std::string const value_;
};

#endif
