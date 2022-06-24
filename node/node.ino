
#include "dotBLETransceiver.h"

using namespace rath;

void setup() {
  Serial.begin(115200);

  // while (!Serial) {}

  BLESerializerNode.init("ClientNode0");
  
  Serial.setTimeout(100);
  BLESerializerNode.setTimeout(100);
  
  BLESerializerNode.startAdvertising();
}

void loop() {

  // BLE -> Serial
  uint8_t buffer[128];
  uint16_t size = BLESerializerNode.receive(buffer, 128);
  if (size != 0) {
    Serial.write(buffer, size);
    Serial.write(NLSM_END);
  }
  
  char test_str[] = "hello\n\n\n\nworld!";
  BLESerializerNode.transmit((uint8_t *)test_str, 27);

  delay(10);
}
