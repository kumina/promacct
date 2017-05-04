// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <cstdint>

class WebserverRequestHandler;

// A simple HTTP web server.
//
// This web server is not capable of parsing actual HTTP requests. It
// invokes WebserverRequestHandler::HandleRequest() for every incoming
// request and returns the string generated, serving it back with MIME
// type 'text/plain'.
class Webserver {
 public:
  Webserver(WebserverRequestHandler* handler) : handler_(handler), fd_(-1) {
  }
  ~Webserver();

  // Binds the web server to a given port number and starts listening
  // for incoming requests.
  void BindAndListen(std::uint16_t port);

  // Blocks and processes a single incoming HTTP request.
  void Dispatch();

 private:
  WebserverRequestHandler* const handler_;
  int fd_;
};

#endif
