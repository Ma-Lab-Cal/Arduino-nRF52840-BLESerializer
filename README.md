# Arduino-nRF52840-BLETransceiver

## Library Structure

There are three types of transceivers to use.

`BLETransceiverNode` is for the client-side node to extract or push raw data from and to the BLE communication stream.

`BLESerializerNode` is for the client-side node to receive or transmit normal data via BLE communication pipeline. It will encode the data to NLSM format before transmitting, and decode after receiving. 

`BLETransceiverHub` is for the central-side hub to extract or push raw data from and to the BLE communication pipeline. It can be used with a Python NLSM Serializer on PC side to form a complete BLE data stream.

## Note

This libraries is not very elegant --- both in terms of syntax, formatting, and code structure design. This is mainly due to time limitations on this project. The errata here is that the code can ONLY support ONE type of transceiver on one MCU, because there are global variable declared to avoid the hassle of passing non-static class member method as callback functions.
