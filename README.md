# Arduino-nRF52840-BLETransceiver

## Library Structure

There are three types of Tranceiver to use.

`BLETransceiverNode` is for the client-side node to extract or push raw data from and to the BLE communication stream.

`BLESerializerNode` is for the client-side node to receive or transmit normal data via BLE communication pipeline. It will encode the data to NLSM format before transmitting, and decode after receiving. 

`BLETransceiverHub` is for the central-side hub to extract or push raw data from and to the BLE communication pipeline. It can be used with a Python NLSM Serializer on PC side to form a complete BLE data stream.
