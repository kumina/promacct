// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <cstddef>

#include "raw_packet_processor.h"

class ParsedPacketProcessor;

class PacketParser : public RawPacketProcessor {
 public:
  explicit PacketParser(ParsedPacketProcessor* processor)
      : processor_(processor) {
  }

  void ProcessPacket(const unsigned char* bytes, std::size_t bytes_length,
                     std::size_t original_length) override;

  static constexpr std::size_t BytesNeededIPv4 = 34;

 private:
  ParsedPacketProcessor* const processor_;
};

#endif
