// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef WEBSERVER_REQUEST_HANDLER_H
#define WEBSERVER_REQUEST_HANDLER_H

#include <ostream>

class WebserverRequestHandler {
 public:
  virtual void HandleRequest(std::ostream* output) = 0;
};

#endif
