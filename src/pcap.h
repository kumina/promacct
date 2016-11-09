// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef PCAP_H
#define PCAP_H

#include <pcap/pcap.h>

#include <cstddef>
#include <string>

class RawPacketProcessor;

// Network packet capturer based on libpcap.
class Pcap {
 public:
  Pcap() : pcap_() {
  }

  ~Pcap();

  // Create a pcap handle for a network device and start capturing.
  void Activate(const std::string& device, std::size_t snapshot_length,
                std::size_t buffer_length);

  // Read the next batch of packets from the pcap handle and forward its
  // contents over to the raw packet processor.
  unsigned int Dispatch(RawPacketProcessor* processor);

 private:
  pcap_t* pcap_;

  // Callback invoked by libpcap.
  static void Callback_(unsigned char* user, const struct pcap_pkthdr* header,
                        const unsigned char* bytes);
};

#endif
