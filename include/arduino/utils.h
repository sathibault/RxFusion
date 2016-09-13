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

#ifdef WIFI_H
void ConnectWifi(const char *ssid, const char *pass) {
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi not present");
    abort();
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);

  int status = WL_IDLE_STATUS;
  while ( status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(1000);
  }

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
#endif

#ifdef RTC_ZERO_H
void NetworkTime(Client& client) {
#if DBG_NET > 0
  Serial.println("Daytime request time-nw.nist.gov");
#endif
  if (client.connect("time-nw.nist.gov", 13)) {
    unsigned long startTime = millis();
    do {
      if (client.available()) {
	client.parseInt(); // Skip JJJJJ
	uint8_t year = client.parseInt();
	client.read(); // skip -
	uint8_t month = client.parseInt();
	client.read(); // skip -
	uint8_t day = client.parseInt();
	uint8_t hours = client.parseInt();
	uint8_t minutes = client.parseInt();
	uint8_t seconds = client.parseInt();
	rtc.setDate(day, month, year);
	rtc.setTime(hours, minutes, seconds);
	Serial.print("Daytime is ");
	Serial.println(rtc.getEpoch());
	while (client.available())
	  char c = client.read();
	break;
      }
    } while ((millis()-startTime) < 25000);
    client.stop();
    Serial.println("");
  } else {
    Serial.println("Daytime client timeout");
  }
}
#endif
