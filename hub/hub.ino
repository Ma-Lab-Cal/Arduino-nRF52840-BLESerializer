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

/* 
 * Connection Handle Explanation
 * -----------------------------
 * The total number of connections is BLE_MAX_CONNECTION (20)
 * 
 */


#include <bluefruit.h>

typedef struct {
  char name[16+1];
  uint16_t conn_handle;
  BLEClientBas client_battery_info;
  BLEClientDis client_device_info;
  BLEClientUart client_uart;
} prph_info_t;

prph_info_t prphs[BLE_MAX_CONNECTION];


/**
 * Find the connection handle in the peripheral array
 * @param conn_handle Connection handle
 * @return array index if found, otherwise -1
 */
int findConnHandle(uint16_t conn_handle) {
  for(int id=0; id<BLE_MAX_CONNECTION; id+=1) {
    if (conn_handle == prphs[id].conn_handle) {
      return id;
    }
  }
  return -1;  
}

/**
 * Callback invoked when scanner picks up an advertising packet
 * @param report Structural advertising data
 */
void onScanHandler(ble_gap_evt_adv_report_t* report) {
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with bleuart service advertised  
  // Connect to the device with bleuart service in advertising packet
  Bluefruit.Central.connect(report);
}

/**
 * Callback invoked when an connection is established
 * @param conn_handle
 */
void onConnectHandler(uint16_t conn_handle) {
  // Find an available ID to use
  int id = findConnHandle(BLE_CONN_HANDLE_INVALID);

  // Eeek: Exceeded the number of connections !!!
  if (id < 0) return;
  
  prph_info_t* peer = &prphs[id];
  peer->conn_handle = conn_handle;
  
  Bluefruit.Connection(conn_handle)->getPeerName(peer->name, sizeof(peer->name)-1);

  // battery information
  if (peer->client_battery_info.discover(conn_handle)) {
    int battery = peer->client_battery_info.read();
  }
  
  if (peer->client_uart.discover(conn_handle)) {
    peer->client_uart.setTimeout(100);
    peer->client_uart.enableTXD();

    Bluefruit.Scanner.start(0);
  }
  else {
    Bluefruit.disconnect(conn_handle);
  }
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void onDisconnectHandler(uint16_t conn_handle, uint8_t reason) {
  // Mark the ID as invalid
  int id = findConnHandle(conn_handle);

  if (id < 0) return;

  prphs[id].conn_handle = BLE_CONN_HANDLE_INVALID;
}

void BLE_init() {
  Bluefruit.begin(0, 8);    // Peripheral = 0, Central = 4

  Bluefruit.setName("Hub");

  for (uint8_t id=0; id<BLE_MAX_CONNECTION; id+=1) {
    prphs[id].conn_handle = BLE_CONN_HANDLE_INVALID;

    prphs[id].client_battery_info.begin();

    prphs[id].client_device_info.begin();
    
    prphs[id].client_uart.begin();
  }

  Bluefruit.Central.setConnectCallback(onConnectHandler);
  Bluefruit.Central.setDisconnectCallback(onDisconnectHandler);
  
  Bluefruit.Scanner.setRxCallback(onScanHandler);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);       // in units of 0.625 ms
  Bluefruit.Scanner.filterUuid(BLEUART_UUID_SERVICE);
  Bluefruit.Scanner.useActiveScan(false);       // Don't request scan response data
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds
}

/**
 * Helper function to send a string to all connected peripherals
 */
void BLE_broadcast(const uint8_t* str, uint16_t size) {
  for(uint8_t id=0; id < BLE_MAX_CONNECTION; id+=1) {
    prph_info_t* peer = &prphs[id];

    if (peer->client_uart.discovered()) {
      peer->client_uart.write(str, size);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  while (!Serial) {}
  
  Serial.setTimeout(100);

  BLE_init();
}

void loop() {
  if (Bluefruit.Central.connected()) {
    if (Serial.available()) {
      
      uint8_t buffer[128] = {0};
      uint16_t size = Serial.readBytesUntil('\n', buffer, 128);
      BLE_broadcast(buffer, size);
      
      // we DO NOT need to send another newline character here.
      // `Serial.readBytesUntil()` will store the ending newline char into the buf.
    }
  }

  
  for (uint8_t id=0; id<BLE_MAX_CONNECTION; id+=1) {
    if (prphs[id].conn_handle == BLE_CONN_HANDLE_INVALID) {
      continue;
    }

    if (prphs[id].client_uart.available()) {
      uint8_t buffer[128];
      uint16_t size = prphs[id].client_uart.readBytesUntil('\n', buffer, 128);
      Serial.write(buffer, size);
      
      // we DO need to send another newline character here.
      // `uart_svc.readBytesUntil()` will NOT store the ending newline char into the buf.
      Serial.write('\n');
    }
  }
}
