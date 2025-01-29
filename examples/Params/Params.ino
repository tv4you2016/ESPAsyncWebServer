// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright 2016-2025 Hristo Gochkov, Mathieu Carbou, Emil Muratov

//
// Query parameters and body parameters
//

#include <Arduino.h>
#ifdef ESP32
#include <AsyncTCP.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(TARGET_RP2040)
#include <WebServer.h>
#include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>

static AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

#ifndef CONFIG_IDF_TARGET_ESP32H2
  WiFi.mode(WIFI_AP);
  WiFi.softAP("esp-captive");
#endif

  // Get query parameters
  //
  // curl -v http://192.168.4.1/?message=bob
  //
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam("message")) {
      message = request->getParam("message")->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Get form body parameters
  //
  // curl -v -H "Content-Type: application/x-www-form-urlencoded" -d "message=carl" -d "param=value" http://192.168.4.1/
  //
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    // display params
    size_t count = request->params();
    for (size_t i = 0; i < count; i++) {
      const AsyncWebParameter *p = request->getParam(i);
      Serial.printf("PARAM[%u]: %s = %s\n", i, p->name().c_str(), p->value().c_str());
    }

    // get message param
    String message;
    if (request->hasParam("message", true)) {
      message = request->getParam("message", true)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.begin();
}

// not needed
void loop() {}
