// Copyright (c) 2016-2017 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <arpa/inet.h>

#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <forward_list>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
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
      MetricsLabel interface("interface", (*interfaces_)[i]);
      (*packet_counters_)[i].PrintMetrics(interface, &page);
    }
  }

 private:
  const std::vector<std::string>* const interfaces_;
  std::vector<PacketCounter>* const packet_counters_;
};

void usage() {
  std::cerr << "usage: promacct -i interface ... [-p httpport] "
               "[-r startaddr-endaddr[:key=value...] ...]"
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
  MetricsLabelsTerminator no_labels;
  std::forward_list<MetricsLabel> labels;
  std::forward_list<MetricsLabelsJoiner> joiners;
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
        // IP range: startaddr-endaddr[:key=value...].
        // Extract start address and end address.
        std::string_view arg(optarg);
        auto endaddr = std::find(arg.begin(), arg.end(), '-');
        if (endaddr == arg.end())
          usage();
        auto kvs = std::find(endaddr, arg.end(), ':');

        // Extract labels.
        const MetricsLabels* range_labels = &no_labels;
        for (auto key = kvs; key != arg.end();) {
          auto value = std::find(key, arg.end(), '=');
          if (value == arg.end())
            usage();
          auto next = std::find(value, arg.end(), ':');
          // TODO(ed): Use C++17 emplace_front().
          labels.emplace_front(std::string_view(key + 1, value - (key + 1)),
                               std::string_view(value + 1, next - (value + 1)));
          joiners.emplace_front(range_labels, &labels.front());
          range_labels = &joiners.front();
          key = next;
        }
        ranges.AddRange(range_labels,
                        parse_ipv4_address(std::string(arg.begin(), endaddr)),
                        parse_ipv4_address(std::string(endaddr + 1, kvs)));
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
    std::optional<std::string> error =
        pcap.Activate(interface, PacketParser::BytesNeededIPv4, 1 << 24);
    if (error) {
      std::cerr << "Failed to activate pcap for interface " << interface << ": "
                << *error << std::endl;
      std::exit(1);
    }
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
