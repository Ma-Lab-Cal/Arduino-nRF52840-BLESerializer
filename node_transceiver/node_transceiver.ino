
#include "dotBLETransceiver.h"

using namespace rath;

void setup() {
  Serial.begin(115200);

  // while (!Serial) {}

  BLETransceiverNode.init("ClientNode0");
  
  Serial.setTimeout(100);
  BLETransceiverNode.setTimeout(100);
  
  BLETransceiverNode.startAdvertising();
}

void loop() {
  // Serial -> BLE
  if (Serial.available()) {
    uint8_t buffer[128];    
    uint16_t size = Serial.readBytesUntil(NLSM_END, buffer, 128);
    
    if (size != 0) {
      BLETransceiverNode.transmit(buffer, size);
    }
  }


  // BLE -> Serial
  uint8_t buffer[128];
  uint16_t size = BLETransceiverNode.receive(buffer, 128, 0);
  if (size != 0) {
    Serial.write(buffer, size);
    Serial.write(NLSM_END);
  }
}
