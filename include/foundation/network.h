/*

Copyright 2016 Green Mountain Computing Systems, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "mqtt.h"

//////////////////////////////////////// HTTP

class HttpPost : public Consumer<xstring> {
 private:
  TRANSPORT_BASE& client;
  const char *host, *path;
  short port;
  uint8_t headerc;
  const char **headers;
  strbuf sbuf;

  void readResponse() {
    unsigned bytes = 0;
    unsigned long endTime = millis() + 25000;

    do {
      if (client.available()) {
	while (client.available()) {
	  char c = client.read();
	  Serial.write(c);
	  bytes++;
	}
	break;
      }
    } while (millis() < endTime);
    client.stop();
    if (bytes > 0)
      Serial.println();
    else
      Serial.println("HTTP client timeout.");
  }

 public:
  HttpPost(TRANSPORT_BASE& client, const char *host, short port, const char *path, uint8_t headerc, const char **headers) : client(client), host(host), path(path), port(port), headerc(headerc), headers(headers), sbuf(1024) {
  }
  void onData(RxNode *publisher, void *value) {
    IPAddress remote_addr = IPAddress(54,164,253,124);
    Serial.println(host);
    Serial.println(port);
    //    if (!WiFi.hostByName(host, remote_addr))
    //      Serial.println("Can't resolve IP addr");
    if (client.connect(remote_addr, port)) {
      sbuf.reset();
      ((xstring *)value)->writeto(sbuf);

      client.print("POST ");
      client.print(path);
      client.print(" HTTP/1.1\r\n");
      client.print("Host: ");
      client.print(host);
      client.print("\r\n");
      client.print("Accept: */*\r\n");
      client.print("Connection: close\r\n");
      client.print("User-Agent: RxFusion\r\n");
      for (uint8_t i = 0; i < headerc; i++) {
	client.print(headers[i]);
	client.print("\r\n");
      }
      client.print("Content-Length: ");
      client.print((int)strlen(sbuf.c_str()));
      client.print("\r\n\r\n");
      client.print(sbuf.c_str());

      readResponse();
    } else
      Serial.println("Connection failed");
  }
};

//////////////////////////////////////// MQTT

class MqttPub : public Consumer<xstring> {
 private:
  TRANSPORT_BASE& trans;
  const char *host, *clientId, *user, *password, *topic;
  short port;
  unsigned short keepAlive;
  strbuf sbuf;
  MqttPacket pkt;
  bool connected;

  unsigned readBytes(uint8_t *buf, uint16_t len) {
    unsigned bytes = 0;
    unsigned long endTime = millis() + 25000;

    do {
      while (trans.available() && bytes < len) {
	buf[bytes] = trans.read();
	bytes++;
      }
      if (bytes == len) break;
      delay(100);
    } while (millis() < endTime);

    return bytes;
  }

  bool connect() {
    uint8_t res[2];

    if (!trans.connect(host, port)) {
      sbuf.reset("Failed to connect ");
      sbuf.append(host);
      sbuf.append(" ");
      sbuf.append(port);
      error.push(sbuf.c_str());
      return false;
    }

    pkt.makeConnect(clientId, user, password, keepAlive);
    trans.write(pkt.data(), pkt.length());

    // RESPONSE
    if (readBytes(res, 2) != 2) { // FIXED HEADER
      error.push("MQTT connect error: no response");
      goto mqtterror;
    }
    if (res[0] != 0x20 || res[1] != 0x02) { // CONNACK, length 2
      sbuf.reset("MQTT connect error: unexpected reponse ");
      sbuf.append(res[0]);
      error.push(sbuf.c_str());
      goto mqtterror;
    }
    if (readBytes(res, 2) != 2) { // VARIABLE HEADER
      error.push("MQTT connect error: incomplete response");
      goto mqtterror;
    }
    if (res[0] != 0x00 || res[1] != 0x00) { // SP, code
      sbuf.reset("MQTT connect error: code ");
      sbuf.append(res[1]);
      error.push(sbuf.c_str());
      goto mqtterror;
    }

    return true;

  mqtterror:
    trans.flush();
    trans.stop();
    return false;
  }

 public:
  EmbeddedOrigin<const char *> error;

  MqttPub(TRANSPORT_BASE& trans, const char *host, short port, const char *clientId, const char *user, const char *password, const char *topic, unsigned short keepAlive=0x40) : trans(trans), host(host), port(port), clientId(clientId), user(user), password(password), topic(topic), keepAlive(keepAlive), sbuf(64) { connected = false; }

  void onData(RxNode *publisher, void *value) {
    if (!connected)
      connected = connect();
    if (connected) {
      sbuf.reset();
      ((xstring *)value)->writeto(sbuf);
      pkt.makePub(topic, sbuf.c_str());
      trans.write(pkt.data(), pkt.length());
      //Serial.println("sent pub");
    }
  }
};
