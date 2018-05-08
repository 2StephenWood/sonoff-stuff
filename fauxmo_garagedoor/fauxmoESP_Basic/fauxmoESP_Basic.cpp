#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include "credentials.h"

#define SERIAL_BAUDRATE                 ( 115200 )
#define BUTTON                          ( 0 )
#define RELAY                           ( 12 )
#define LED                             ( 13 )
#define HEADER                          ( 14 )

fauxmoESP fauxmo;
bool      garageDoorOpen = false;
bool      globalState = false;

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

// -----------------------------------------------------------------------------
// Set State Callback
// -----------------------------------------------------------------------------
void toggleRelay() {
  digitalWrite(RELAY, HIGH);
  delay(1*1000);
  digitalWrite(RELAY, LOW);
}

// -----------------------------------------------------------------------------
// Set State Callback
// -----------------------------------------------------------------------------
bool setState(unsigned char device_id, const char * device_name, bool state) {
    Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");

    globalState = state;
    //TODO - figure out why delay() calls don't seem to work here.
}

// -----------------------------------------------------------------------------
// Get State Callback
// -----------------------------------------------------------------------------
bool getState(unsigned char device_id, const char * device_name) {
      return globalState;
}

void setup() {
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // Wifi
    wifiSetup();

    // Pins
    pinMode(BUTTON, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(RELAY, OUTPUT);
    pinMode(HEADER, OUTPUT);

    // Turn on LED
    digitalWrite(LED, LOW);

    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // Add virtual devices
    fauxmo.addDevice("garage door");
	//fauxmo.addDevice("switch two"); // You can add more devices
	//fauxmo.addDevice("switch three");

    // fauxmoESP 2.0.0 has changed the callback signature to add the device_id,
    // this way it's easier to match devices to action without having to compare strings.
    fauxmo.onSetState(setState);

    // Callback to retrieve current state (for GetBinaryState queries)
    fauxmo.onGetState(getState);
}

void loop() {
    // Since fauxmoESP 2.0 the library uses the "compatibility" mode by
    // default, this means that it uses WiFiUdp class instead of AsyncUDP.
    // The later requires the Arduino Core for ESP8266 staging version
    // whilst the former works fine with current stable 2.3.0 version.
    // But, since it's not "async" anymore we have to manually poll for UDP
    // packets
    fauxmo.handle();

    if(!digitalRead(BUTTON)) {
      globalState = !globalState;
      delay(1*1000); // 1 second switch debounce
    }

    if(globalState != garageDoorOpen) {
      toggleRelay();
      garageDoorOpen = !garageDoorOpen;
    }

    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

}
