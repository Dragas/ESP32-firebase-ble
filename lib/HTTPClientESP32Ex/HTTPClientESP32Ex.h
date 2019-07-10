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

#ifndef HTTP_CLIENT_ESP32EX_H_
#define HTTP_CLIENT_ESP32EX_H_

#include <Arduino.h>
#include <HTTPClient.h>

#include <WiFiClient.h>
#include <WiFiClientSecure.h>

class HTTPClientESP32Ex : public HTTPClient
{
  public:
    HTTPClientESP32Ex();
    ~HTTPClientESP32Ex();

    /**
    * Initialization of new http connection.
    * \param host - Host name without protocols.
    * \param port - Server's port.
    * \param uri - The URI of resource.
    * \param CAcert - The Base64 encode root certificate string
    * \return True as default.
    * If no certificate string provided, use (const char*)NULL to CAcert param 
    */
    bool http_begin(const char *host, uint16_t port, const char *uri, const char *CAcert);

    /**
    * Check the http connection status.
    * \return True if connected.
    */
    bool http_connected();

    /**
    * Establish http connection if header provided and send it, send payload if provided.
    * \param header - The header string (constant chars array).
    * \param payload - The payload string (constant chars array), optional.
    * \return http status code, Return zero if new http connection and header and/or payload sent 
    * with no error or no header and payload provided. If obly payload provided, no new http connection was established.
    */
    int http_sendRequest(const char *header, const char *payload);

    /**
    * Send extra header without making new http connection (if http_sendRequest has been called)
    * \param header - The header string (constant chars array).
    * \return True if header sending success.
    * Need to call http_sendRequest with header first. 
    */
    bool http_sendHeader(const char *header);

    /**
    * Get the WiFi client pointer.
    * \return WiFi client pointer.
    */
    WiFiClient *http_getStreamPtr(void);

    bool http_connect(void);

    uint16_t tcpTimeout =  HTTPCLIENT_DEFAULT_TCP_TIMEOUT;

  protected:
    
    TransportTraitsPtr http_transportTraits;
    std::unique_ptr<WiFiClient> _tcp;

    char _host[200];
    char _uri[200];
    uint16_t _port = 0;
};

#endif /* HTTPClientESP32Ex_H_ */
