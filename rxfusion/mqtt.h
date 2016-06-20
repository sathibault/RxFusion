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

#define MQTT_PROTOCOL_LEVEL 0x04

class MqttPacket {
 private:
  bytebuf pkt;
  
  void addFixed(uint8_t type, uint8_t flags, uint16_t len) {
    pkt.append((type<<4)|flags);
    do {
      uint8_t byte = (uint8_t)(len&0x7f);
      len = len >> 7;
      if (len > 0)
	byte |= 0x80;
      pkt.append(byte);
    } while (len > 0);
  }
  
  void addString(const char *s) {
    uint16_t len = strlen(s);
    pkt.append((uint8_t)(len>>8));
    pkt.append((uint8_t)(len&0xff));
    pkt.append(s, len);
  }
 public:
  MqttPacket() : pkt(128) {}

  const uint8_t *data() { return pkt.data(); }
  uint16_t length() { return pkt.length(); }

  void makeConnect(const char *clientId, const char *user, const char *password, unsigned short keepAlive) {
    uint8_t flags;
    uint16_t remaining;

    flags = 0x02; // CleanSession
    remaining = 10;
    remaining += 2 + strlen(clientId);
    if (user != NULL) {
      flags |= 0x80;
      remaining += 2 + strlen(user);
    }
    if (password != NULL) {
      flags |= 0x40;
      remaining += 2 + strlen(password);
    }

    pkt.reset();
    // FIXED HEADER
    addFixed(1, 0, remaining);

    // VARIABLE HEADER
    addString("MQTT"); // protocol name
    pkt.append(MQTT_PROTOCOL_LEVEL); // protocol level
    pkt.append(flags); // flags
    pkt.append((uint8_t)(keepAlive>>8)); // keep alive MSB
    pkt.append((uint8_t)(keepAlive&0xff)); // keep alive LSB

    // PAYLOAD
    addString(clientId);
    if (user != NULL)
      addString(user);
    if (password != NULL)
      addString(password);
  }

  void makePub(const char *topic, const char *msg) {
    uint16_t remaining;
    uint16_t len = strlen(msg);

    remaining = 2 + strlen(topic);
    remaining += len;
    
    pkt.reset();
    // FIXED HEADER
    addFixed(3, 0, remaining);

    // VARIABLE HEADER
    addString(topic);

    // PAYLOAD
    pkt.append(msg, len);
  }

  void makeDisconnect() {
    pkt.reset();
    // FIXED HEADER
    addFixed(14, 0, 0);
  }
};
