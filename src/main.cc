// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <iostream>

#include "packet_parser.h"
#include "parsed_packet_processor.h"
#include "pcap.h"

namespace {
class PacketPrinter : public ParsedPacketProcessor {
 public:
  void ProcessIPv4Packet(std::uint32_t src, std::uint32_t dst,
                         std::size_t original_length) {
    std::cout << "Got IPv4 packet! " << std::hex << src << " " << dst << " "
              << std::dec << original_length << std::endl;
  }

  void ProcessUnknownPacket(std::size_t original_length) {
    std::cout << "Got unknown packet! " << original_length << std::endl;
  }
};
}

int main() {
  Pcap p;
  p.Activate("enp3s0", PacketParser::BytesNeededIPv4, 16 * 1024 * 1024);
  for (;;) {
    PacketPrinter pr;
    PacketParser pa(&pr);
    p.Dispatch(&pa);
  }
}
