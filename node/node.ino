/*********************************************************************
  This is an example for our nRF52 based Bluefruit LE modules

  Pick one up today in the adafruit shop!

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  MIT license, check LICENSE for more information
  All text above, and the splash screen below must be included in
  any redistribution
*********************************************************************/
#include <bluefruit.h>

#define DEVICE_NAME     "ClientNode0"

BLEDfu  ble_dfu;            // OTA DFU service
BLEDis  ble_device_info;   // device information
BLEBas  ble_battery_info;    // battery
BLEUart ble_uart;           // uart over ble

BLEConnection* connection;

void onConnectHandler(uint16_t conn_handle) {
  // Get the reference to current connection
  connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = {0};
  connection->getPeerName(central_name, sizeof(central_name));
}

void onDisconnectHandler(uint16_t conn_handle, uint8_t reason) {
}

void BLE_init(void) {
  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behavior, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName(DEVICE_NAME); // useful testing with multiple central connections

  Bluefruit.Periph.setConnectCallback(onConnectHandler);
  Bluefruit.Periph.setDisconnectCallback(onDisconnectHandler);

  ble_dfu.begin();

  ble_device_info.setManufacturer("Rath Robotics");
  ble_device_info.setModel("nRF52840");
  ble_device_info.begin();

  ble_battery_info.begin();
  ble_battery_info.write(100);
  
  ble_uart.begin();
}

void BLE_startAdvertising(void) {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(ble_uart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void setup() {
  Serial.begin(115200);

  // while (!Serial) {}

  Serial.setTimeout(100);
  ble_uart.setTimeout(100);
  
  BLE_init();

  BLE_startAdvertising();
}

void loop() {
  // Forward data from HW Serial to BLEUART
  if (Serial.available()) {
    uint8_t buffer[128];
    
    uint16_t size = Serial.readBytesUntil('\n', buffer, 128);
    ble_uart.write(buffer, size);
    
    // we DO NOT need to send another newline character here.
    // `Serial.readBytesUntil()` will store the ending newline char into the buf.
  }

  // Forward from BLEUART to HW Serial
  if (ble_uart.available()) {
    uint8_t buffer[128];
    uint16_t size = ble_uart.readBytesUntil('\n', buffer, 128);
    Serial.write(buffer, size);
    
    // we DO need to send another newline character here.
    // `uart_svc.readBytesUntil()` will NOT store the ending newline char into the buf.
    Serial.write('\n');
  }

  delay(1);
  char test_str[] = "hello3___________________3\n";
  ble_uart.write(test_str, 27);
}
