// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <pcap/pcap.h>

#include <cassert>
#include <cstddef>
#include <string>

#include "pcap.h"
#include "raw_packet_processor.h"

void Pcap::Activate(const std::string& device, std::size_t snapshot_length,
                    std::size_t buffer_length) {
  assert(pcap_ == nullptr && "Cannot activate twice.");

  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_ = pcap_create(device.c_str(), errbuf);
  assert(pcap_ != nullptr && "Failed to create device.");

  pcap_set_promisc(pcap_, false);
  pcap_set_snaplen(pcap_, snapshot_length);
  pcap_set_buffer_size(pcap_, buffer_length);

  if (pcap_activate(pcap_) != 0)
    assert(false && "Failed to activate");
}

Pcap::~Pcap() {
  if (pcap_ != nullptr)
    pcap_close(pcap_);
}

void Pcap::Callback_(unsigned char* user, const struct pcap_pkthdr* header,
                     const unsigned char* bytes) {
  RawPacketProcessor* processor = (RawPacketProcessor*)user;
  processor->ProcessPacket(bytes, header->caplen, header->len);
}

void Pcap::Dispatch(RawPacketProcessor* processor) {
  assert(pcap_ != nullptr && "Cannot dispatch before activating.");
  pcap_dispatch(pcap_, 0, &Pcap::Callback_, (unsigned char*)processor);
}
