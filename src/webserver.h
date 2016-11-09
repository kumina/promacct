// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <cstdint>

class WebserverRequestHandler;

class Webserver {
 public:
  Webserver(WebserverRequestHandler* handler) : handler_(handler), fd_(-1) {
  }
  ~Webserver();

  void BindAndListen(uint16_t port);
  void Dispatch();

 private:
  WebserverRequestHandler* const handler_;
  int fd_;
};

#endif
