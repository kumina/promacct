// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <arpa/inet.h>

#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>

#include "ipv4_ranges.h"
#include "metrics_page.h"
#include "packet_counter.h"
#include "packet_parser.h"
#include "pcap.h"
#include "webserver.h"
#include "webserver_request_handler.h"

namespace {
class PacketCounterServer : public WebserverRequestHandler {
 public:
  PacketCounterServer(const std::vector<std::string>* interfaces,
                      std::vector<PacketCounter>* packet_counters)
      : interfaces_(interfaces), packet_counters_(packet_counters) {
  }

  void HandleRequest(std::ostream* output) override {
    MetricsPage page("promacct_", output);
    for (size_t i = 0; i < interfaces_->size(); ++i) {
      MetricsLabels interface(nullptr, "interface", (*interfaces_)[i]);
      (*packet_counters_)[i].PrintMetrics(&interface, &page);
    }
  }

 private:
  const std::vector<std::string>* const interfaces_;
  std::vector<PacketCounter>* const packet_counters_;
};

void usage() {
  std::cerr << "usage: promacct -i interface ... [-p httpport] "
               "[-r description:startaddr-endaddr ...]"
            << std::endl;
  std::exit(1);
}

std::uint32_t parse_ipv4_address(const std::string& str) {
  struct in_addr addr;
  if (inet_pton(AF_INET, str.c_str(), &addr) != 1)
    usage();
  return ntohl(addr.s_addr);
}
}  // namespace

int main(int argc, char* argv[]) {
  // Parse command line arguments.
  int ch;
  std::vector<std::string> interfaces;
  std::uint16_t httpport = 9112;
  IPv4Ranges ranges;
  while ((ch = getopt(argc, argv, "i:p:r:")) != -1) {
    switch (ch) {
      case 'i':
        // Network interface.
        interfaces.push_back(optarg);
        break;
      case 'p':
        // Port number on which to bind the HTTP server.
        httpport = std::stoi(optarg);
        break;
      case 'r': {
        // IP range: description:startaddr-endaddr.
        std::experimental::string_view arg(optarg);
        auto split1 = std::find(arg.begin(), arg.end(), ':');
        if (split1 == arg.end())
          usage();
        auto split2 = std::find(split1, arg.end(), '-');
        if (split2 == arg.end())
          usage();
        ranges.AddRange(
            std::experimental::string_view(arg.begin(), split1 - arg.begin()),
            parse_ipv4_address(std::string(split1 + 1, split2)),
            parse_ipv4_address(std::string(split2 + 1, arg.end())));
        break;
      }
      default:
        usage();
    }
  }
  argc -= optind;
  argv += optind;
  if (argc != 0 || interfaces.empty())
    usage();

  // Create pcap handles and allocate histograms.
  std::vector<Pcap> pcaps;
  std::vector<PacketCounter> packet_counters;
  for (const std::string& interface : interfaces) {
    // TODO(ed): Use C++17 emplace_back().
    // Pcap& pcap = pcaps.emplace_back();
    pcaps.emplace_back();
    Pcap& pcap = pcaps.back();
    pcap.Activate(interface, PacketParser::BytesNeededIPv4, 1 << 24);
    packet_counters.emplace_back(&ranges);
  }

  // Create HTTP server that returns metrics for all interfaces.
  PacketCounterServer packet_counter_server(&interfaces, &packet_counters);
  Webserver webserver(&packet_counter_server);
  webserver.BindAndListen(httpport);

  // Spawn a small number of worker threads for HTTP GET requests.
  std::vector<std::thread> webserver_workers;
  for (int i = 0; i < 5; ++i) {
    webserver_workers.push_back(std::thread([&webserver]() {
      for (;;)
        webserver.Dispatch();
    }));
  }

  // Count incoming network packets in the main thread at a fixed 10 Hz
  // rate. This has the advantage of reducing CPU load significantly, as
  // libpcap tends to already unblock when a very small number of
  // packets are available for processing.
  for (;;) {
    for (std::size_t i = 0; i < pcaps.size(); ++i) {
      PacketParser parser(&packet_counters[i]);
      pcaps[i].Dispatch(&parser);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
