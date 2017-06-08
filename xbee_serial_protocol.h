#ifndef XBEE_SERIAL_PROTOCOL_H
#define XBEE_SERIAL_PROTOCOL_H

#define AT_START_SEQUENCE 0
#define EXIT_COMMAND_MODE 1
#define GET_DESTINATION_ADDRESS_HIGH 2
#define GET_DESTINATION_ADDRESS_LOW 3
#define SET_DESTINATION_ADDRESS_HIGH 4
#define SET_DESTINATION_ADDRESS_LOW 5
#define API_MODE_NO_ESCAPE 6
#define API_MODE_WITH_ESCAPE 7
#define AT_MODE 8
#define READ_OPERATING_MODE 9
#define CHANGE_BAUD 10
#define APPLY_CHANGES 11
#define SAVE_THROUGH_RESET 12
#define READ_BAUD 13

// API IDs
#define TRANSMIT_REQUEST 0x10
#define EXPLICIT_COMMAND_FRAME 0x11
#define REMOTE_COMMAND_REQUEST 0x17
#define CREATE_SOURCE_ROUTE 0x21
#define AT_COMMAND_RESPONSE 0x88
#define MODEM_STATUS 0x8A
#define TRANSMIT_STATUS 0x8B

#define DELIMITER 0x7E

#define BAUD_9600   1
#define BAUD_115200 2

extern unsigned char broadcastTransmission, operatingMode;
extern volatile unsigned char receiveFlag;

extern struct bufferStruct
{
    unsigned char buffer[128];
    unsigned char numBytes;
    unsigned char currentIndex;
};

unsigned char changeXBeeOperatingMode(unsigned char mode);
unsigned char changeXBeeBaudrate(unsigned char baudrate);
void issue_AT_command(unsigned char command, unsigned char* parameters);
void sendData(unsigned char* dataBuffer, unsigned char messageLength);
void sendByte(unsigned char dataByte);
unsigned char readByte();
unsigned char calculateChecksum(unsigned char* data, unsigned char length);
unsigned char createAPIFrame(unsigned char frameType, unsigned char frameID, unsigned char frameParameters[], unsigned char frameParametersLength);
unsigned char createTransmitRequestFrame(unsigned char frameBuffer[], unsigned char frameType, unsigned char frameID, unsigned int destinationAddrHigh, unsigned int destinationAddrLow,
                                         unsigned int destinationNtwkAddr, unsigned char broadcastRadius, unsigned char options, unsigned char dataPayload[],
                                         unsigned char numDataPayloadBytes);

#endif

