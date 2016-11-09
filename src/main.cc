// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <chrono>
#include <ostream>
#include <thread>

#include "ipv4_ranges.h"
#include "packet_parser.h"
#include "parsed_packet_counter.h"
#include "parsed_packet_processor.h"
#include "pcap.h"
#include "webserver.h"
#include "webserver_request_handler.h"

namespace {
class HelloWorld : public WebserverRequestHandler {
 public:
  void HandleRequest(std::ostream* output) override {
    *output << "Hello world\n";
  }
};
}

int main() {
  Pcap p;
  p.Activate("enp3s0", PacketParser::BytesNeededIPv4, 16 * 1024 * 1024);

  IPv4Ranges ir;
  ir.AddRange(0x0a000000, 0x0affffff);
  ParsedPacketCounter pc(&ir);
  PacketParser pa(&pc);

  HelloWorld h;
  Webserver webserver(&h);
  webserver.BindAndListen(7227);

  // Spawn a small number of worker threads for HTTP GET requests.
  std::vector<std::thread> webserver_workers;
  for (int i = 0; i < 5; ++i) {
    webserver_workers.push_back(std::thread([&webserver]() {
      for (;;)
        webserver.Dispatch();
    }));
  }

  // Count incoming network packets in the main thread.
  for (;;) {
    p.Dispatch(&pa);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
