// Copyright (c) 2016 Kumina, https://kumina.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef WEBSERVER_REQUEST_HANDLER_H
#define WEBSERVER_REQUEST_HANDLER_H

#include <ostream>

// Interface for handlers for HTTP requests.
//
// This implementation does not yet allow you to distinguish between
// URLs, HTTP methods, extract headers, etc. It can only be used to
// return a single response, which is good enough for what we need.
class WebserverRequestHandler {
 public:
  // Processes an incoming HTTP request, by writing the response body to
  // the output stream.
  virtual void HandleRequest(std::ostream* output) = 0;
};

#endif
