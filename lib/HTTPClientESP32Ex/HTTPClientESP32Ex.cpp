/*
 * Customized version of ESP32 HTTPClient Library. To allow custom header and payload string sending
 * for http and https connection.
 *
 * Use as part of my Firebase-ESP32 and LineNotify-ESP32 libraries for Arduino.
 * 
 * The MIT License (MIT)
 * Copyright (c) 2019 K. Suwatchai (Mobizt)
 * 
 * HTTPClient Arduino library for ESP32
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the HTTPClient for Arduino.
 * Port to ESP32 by Evandro Luis Copercini (2017), 
 * changed fingerprints to CA verification. 	
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/

#ifndef HTTPClientESP32Ex_CPP
#define HTTPClientESP32Ex_CPP


#include "HTTPClientESP32Ex.h"

class TransportTraits
{
  public:
    virtual ~TransportTraits(){}

    virtual std::unique_ptr<WiFiClient> create()
    {
      return std::unique_ptr<WiFiClient>(new WiFiClient());
    }

    virtual bool verify(WiFiClient& client, const char* host)
    {
      return true;
    }
};

class TLSTraits : public TransportTraits
{
  public:
    TLSTraits(const char* CAcert, const char* clicert = nullptr, const char* clikey = nullptr):
      _cacert(CAcert), _clicert(clicert), _clikey(clikey){}

    std::unique_ptr<WiFiClient> create() override
    {
      return std::unique_ptr<WiFiClient>(new WiFiClientSecure());
    }

    bool verify(WiFiClient& client, const char* host) override
    {
      WiFiClientSecure& wcs = static_cast<WiFiClientSecure&>(client);
      wcs.setCACert(_cacert);
      wcs.setCertificate(_clicert);
      wcs.setPrivateKey(_clikey);
      return true;
    }

  protected:
    const char* _cacert;
    const char* _clicert;
    const char* _clikey;
};


HTTPClientESP32Ex::HTTPClientESP32Ex() {}

HTTPClientESP32Ex::~HTTPClientESP32Ex()
{
  if (_tcp) _tcp->stop();
}

bool HTTPClientESP32Ex::http_begin(const char* host, uint16_t port, const char* uri, const char* CAcert)
{
  http_transportTraits.reset(nullptr);
  memset(_host, 0, sizeof _host);
  strcpy(_host, host);
  _port = port;
  memset(_uri, 0, sizeof _uri);
  strcpy(_uri, uri);
  http_transportTraits = TransportTraitsPtr(new TLSTraits(CAcert));
  return true;
}


bool HTTPClientESP32Ex::http_connected()
{
  if (_tcp) return ((_tcp->available() > 0) || _tcp->connected());
  return false;
}


bool HTTPClientESP32Ex::http_sendHeader(const char* header)
{
  if (!http_connected())return false;
  return (_tcp->write(header, strlen(header)) == strlen(header));
}

int HTTPClientESP32Ex::http_sendRequest(const char* header, const char* payload)
{
  size_t size = strlen(payload);
  if(strlen(header)>0){
	if (!http_connect()) return HTTPC_ERROR_CONNECTION_REFUSED;  
	if (!http_sendHeader(header))return HTTPC_ERROR_SEND_HEADER_FAILED;
  }
  if (size > 0)
    if (_tcp->write(&payload[0], size) != size)
      return HTTPC_ERROR_SEND_PAYLOAD_FAILED;
  return 0;
}


WiFiClient* HTTPClientESP32Ex::http_getStreamPtr(void)
{
  if (http_connected()) return _tcp.get();
  return nullptr;
}

bool HTTPClientESP32Ex::http_connect(void)
{
  if (http_connected()) {
    while (_tcp->available() > 0) _tcp->read();
    return true;
  }

  if (!http_transportTraits)
     return false;

  _tcp = http_transportTraits->create();

  if (!http_transportTraits->verify(*_tcp, _host)) {
    _tcp->stop();
    return false;
  }

  if (!_tcp->connect(_host, _port)) return false;
  
  return http_connected();
}
#endif
