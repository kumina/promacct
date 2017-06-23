// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef METRICS_LABELS_H
#define METRICS_LABELS_H

#include <ostream>
#include <string_view>

// Rope/cord-like class for holding a set of metrics labels.
//
// These classes can be used to temporarily construct metrics labels on
// the stack, so that they can be printed by class MetricsPage. To
// prevent any heap allocations, these classes use a tree-like
// structure, so labels from multiple sources can be combined.
class MetricsLabels {
 public:
  MetricsLabels() {
  }
  virtual ~MetricsLabels() {
  }

  virtual void Print(std::ostream* output, bool* needs_comma) const = 0;

  MetricsLabels& operator=(const MetricsLabels&) = delete;
  MetricsLabels(const MetricsLabels&) = delete;
};

// Singleton key-value pair, representing a single label.
class MetricsLabel : public MetricsLabels {
 public:
  MetricsLabel(std::string_view key, std::string_view value)
      : key_(key), value_(value) {
  }

  void Print(std::ostream* output, bool* needs_comma) const override {
    if (*needs_comma)
      *output << ',';
    *needs_comma = true;
    *output << key_ << "=\"" << value_ << '"';
  }

 private:
  std::string_view key_;
  std::string_view value_;
};

// The empty set: no labels.
class MetricsLabelsTerminator : public MetricsLabels {
 public:
  void Print(std::ostream* output, bool* needs_comma) const override {
  }
};

// Union of two sets of labels.
class MetricsLabelsJoiner : public MetricsLabels {
 public:
  MetricsLabelsJoiner(const MetricsLabels* left, const MetricsLabels* right)
      : left_(left), right_(right) {
  }

  void Print(std::ostream* output, bool* needs_comma) const override {
    left_->Print(output, needs_comma);
    right_->Print(output, needs_comma);
  }

 private:
  const MetricsLabels* const left_;
  const MetricsLabels* const right_;
};

#endif
