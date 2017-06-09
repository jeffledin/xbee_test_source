#include <xc.h>
#include "xbee_serial_protocol.h"

unsigned char broadcastTransmission = 1, operatingMode;
volatile struct bufferStruct receiveBuffer;

// TODO: Return error if verification fails
unsigned char changeXBeeOperatingMode(unsigned char mode)
{
    unsigned char* parameters;
    unsigned char errorStatus = 0;
    
    switch(mode)
    {
        case AT_MODE:
            operatingMode = mode;
            delay_ms(1000);
            printf("Sending AT Start Sequence...");
            issue_AT_command(AT_START_SEQUENCE, parameters);
            delay_ms(1000);
            printf("Switching to AT Mode...");
            issue_AT_command(AT_MODE, parameters);
            printf("Setting Destination Address High...");
            issue_AT_command(SET_DESTINATION_ADDRESS_HIGH, "0013A200");
            printf("Setting Destination Address Low...");
            issue_AT_command(SET_DESTINATION_ADDRESS_LOW, "41632F8D");
            broadcastTransmission = 0; // no longer in broadcast mode
            printf("Getting Destination Address High...");
            issue_AT_command(GET_DESTINATION_ADDRESS_HIGH, parameters);
            printf("Getting Destination Address Low...");
            issue_AT_command(GET_DESTINATION_ADDRESS_LOW, parameters);
            printf("Verifying changes...");
            issue_AT_command(READ_OPERATING_MODE, parameters);
            printf("Committing changes and exiting Command Mode...");
            issue_AT_command(EXIT_COMMAND_MODE, parameters);
            break;
        case API_MODE_NO_ESCAPE:
            operatingMode = AT_MODE;
            broadcastTransmission = 0; // no longer in broadcast mode
            delay_ms(1000);
            printf("Sending AT Start Sequence...");
            issue_AT_command(AT_START_SEQUENCE, parameters);
            delay_ms(1000);
            printf("Switching to API Mode (No Escape Characters)...");
            issue_AT_command(API_MODE_NO_ESCAPE, parameters);
            printf("Verifying changes...");
            issue_AT_command(READ_OPERATING_MODE, parameters);
            printf("Committing changes and exiting Command Mode...");
            issue_AT_command(EXIT_COMMAND_MODE, parameters);
            operatingMode = API_MODE_NO_ESCAPE;
            break;
        case API_MODE_WITH_ESCAPE:
            operatingMode = AT_MODE;
            broadcastTransmission = 0; // no longer in broadcast mode
            delay_ms(1000);
            printf("Sending AT Start Sequence...");
            issue_AT_command(AT_START_SEQUENCE, parameters);
            delay_ms(1000);
            printf("Switching to API Mode (With Escape Characters)...");
            issue_AT_command(API_MODE_WITH_ESCAPE, parameters);
            printf("Verifying changes...");
            issue_AT_command(READ_OPERATING_MODE, parameters);
            printf("Committing changes and exiting Command Mode...");
            issue_AT_command(EXIT_COMMAND_MODE, parameters);
            operatingMode = API_MODE_WITH_ESCAPE;
            break;
        default:
            break;
    }
    
    printf("\n");
    
    return errorStatus;
}

// TODO: Error status
unsigned char changeXBeeBaudrate(unsigned char baud_select)
{
    unsigned char* parameters;
    unsigned char errorStatus = 0;
    
    switch(baud_select)
    {
        case BAUD_9600:
            printf("Changing XBee baudrate to 9600 bps...\n");
            delay_ms(1000);
            printf("Sending AT Start Sequence...");
            issue_AT_command(AT_START_SEQUENCE, parameters);
            delay_ms(1000);
            printf("Switching to AT Mode...");
            issue_AT_command(AT_MODE, parameters);
            printf("Writing baudrate settings...");
            issue_AT_command(CHANGE_BAUD, "3");
            printf("Making sure changes save through power reset...");
            issue_AT_command(SAVE_THROUGH_RESET, parameters);
            printf("Verifying changes...");
            issue_AT_command(READ_BAUD, parameters);
            printf("Applying changes...");
            issue_AT_command(APPLY_CHANGES, parameters);
            printf("Matching PIC baudrate to XBee baudrate...\n");
            break;
        case BAUD_115200:
            printf("Changing XBee baudrate to 115200 bps...\n");
            delay_ms(1000);
            printf("Sending AT Start Sequence...");
            issue_AT_command(AT_START_SEQUENCE, parameters);
            delay_ms(1000);
            printf("Switching to AT Mode...");
            issue_AT_command(AT_MODE, parameters);
            printf("Writing baudrate settings...");
            issue_AT_command(CHANGE_BAUD, "7");
            printf("Making sure changes save through power reset...");
            issue_AT_command(SAVE_THROUGH_RESET, parameters);
            printf("Verifying changes...");
            issue_AT_command(READ_BAUD, parameters);
            printf("Applying changes...");
            issue_AT_command(APPLY_CHANGES, parameters);
            printf("Matching PIC baudrate to XBee baudrate...\n");
            break;
        default:
            break;
    }
    
    initUART1(baud_select); // consider putting this into non-volatile memory
    
    delay_ms(1000);
    
    printf("Exiting command mode...");
    issue_AT_command(EXIT_COMMAND_MODE, parameters);
    
    printf("\n");
    
    return errorStatus;
}
void issue_AT_command(unsigned char command, unsigned char* parameters)
{
    unsigned char outBuffer[25], messageLength, i;
    
    switch(command)
    {
        case AT_START_SEQUENCE:
            messageLength = 3;
            outBuffer[0] = '+';
            outBuffer[1] = '+';
            outBuffer[2] = '+';
            break;
        case AT_MODE:
            messageLength = 6;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'A';
            outBuffer[3] = 'P';
            outBuffer[4] = '0';
            outBuffer[5] = '\r';
            break;
        case API_MODE_NO_ESCAPE:
            messageLength = 6;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'A';
            outBuffer[3] = 'P';
            outBuffer[4] = '1';
            outBuffer[5] = '\r';
            break;
        case API_MODE_WITH_ESCAPE:
            messageLength = 6;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'A';
            outBuffer[3] = 'P';
            outBuffer[4] = '2';
            outBuffer[5] = '\r';
            break;
        case READ_OPERATING_MODE:
            messageLength = 5;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'A';
            outBuffer[3] = 'P';
            outBuffer[4] = '\r';
            break;
        case SAVE_THROUGH_RESET:
            messageLength = 5;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'W';
            outBuffer[3] = 'R';
            outBuffer[4] = '\r';
            break;
        case APPLY_CHANGES:
            messageLength = 5;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'A';
            outBuffer[3] = 'C';
            outBuffer[4] = '\r';
            break;
        case EXIT_COMMAND_MODE:
            messageLength = 5;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'C';
            outBuffer[3] = 'N';
            outBuffer[4] = '\r';
            break;
        case GET_DESTINATION_ADDRESS_HIGH:
            messageLength = 5;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'D';
            outBuffer[3] = 'H';
            outBuffer[4] = '\r';
            break;
        case GET_DESTINATION_ADDRESS_LOW:
            messageLength = 5;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'D';
            outBuffer[3] = 'L';
            outBuffer[4] = '\r';
            break;
        case SET_DESTINATION_ADDRESS_HIGH:
            messageLength = 13;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'D';
            outBuffer[3] = 'H';
            outBuffer[4] = parameters[0];
            outBuffer[5] = parameters[1];
            outBuffer[6] = parameters[2];
            outBuffer[7] = parameters[3];
            outBuffer[8] = parameters[4];
            outBuffer[9] = parameters[5];
            outBuffer[10] = parameters[6];
            outBuffer[11] = parameters[7];
            outBuffer[12] = '\r';
            break;
        case SET_DESTINATION_ADDRESS_LOW:
            messageLength = 13;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'D';
            outBuffer[3] = 'L';
            outBuffer[4] = parameters[0];
            outBuffer[5] = parameters[1];
            outBuffer[6] = parameters[2];
            outBuffer[7] = parameters[3];
            outBuffer[8] = parameters[4];
            outBuffer[9] = parameters[5];
            outBuffer[10] = parameters[6];
            outBuffer[11] = parameters[7];
            outBuffer[12] = '\r';
            break;
        case CHANGE_BAUD:
            messageLength = 6;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'B';
            outBuffer[3] = 'D';
            outBuffer[4] = parameters[0];
            outBuffer[5] = '\r';
            break;
        case READ_BAUD:
            messageLength = 5;
            outBuffer[0] = 'A';
            outBuffer[1] = 'T';
            outBuffer[2] = 'B';
            outBuffer[3] = 'D';
            outBuffer[4] = '\r';
            break;
        default:
            break;
    }
    
    for(i = 0; i < messageLength; i++)
    {
        sendByte(outBuffer[i]);
        if(broadcastTransmission) // if coordinator is in broadcast mode, then 
        {
            delay_ms(1); // be careful with this delay -- too long and you will time out of command mode
        }
    }
    
    while(!receiveFlag); // wait for response
    receiveFlag = 0;

    for(i = 0; i < receiveBuffer.numBytes - 1; i++)
    {
        printf("%c", receiveBuffer.buffer[i]);
    }
    printf("\n");
    
    receiveBuffer.numBytes = 0;
    receiveBuffer.currentIndex = 0;
}

unsigned char processReceivedPacket(struct bufferStruct receiveBuffer)
{
    
}

void sendData(unsigned char* dataBuffer, unsigned char messageLength)
{
    unsigned int i;
    
    for(i = 0; i < messageLength; i++)
    {
        sendByte(dataBuffer[i]);
        delay_ms(1);
    }
}

void sendByte(unsigned char dataByte)
{
    while(!IFS0bits.U1TXIF);
    IFS0bits.U1TXIF = 0;
    U1TXREG = dataByte;
}

unsigned char readByte()
{
    while(!U1STAbits.URXDA);
    IFS0bits.U1RXIF = 0;
    return U1RXREG;
}

unsigned char createAPIFrame(unsigned char frameType, unsigned char frameID, unsigned char frameParameters[], unsigned char frameParametersLength)
{
    unsigned char length = 0;
    
    switch(frameType)
    {
        case TRANSMIT_REQUEST:
            break;
        default:
            break;
    }
    
    calculateChecksum(1, 1);
    
    return 1;
}

unsigned char createTransmitRequestFrame(unsigned char frameBuffer[], unsigned char frameType, unsigned char frameID, unsigned int destinationAddrHigh, unsigned int destinationAddrLow,
                                         unsigned int destinationNtwkAddr, unsigned char broadcastRadius, unsigned char options, unsigned char dataPayload[],
                                         unsigned char numDataPayloadBytes)
{
    unsigned char bufferIndex = 0;
    unsigned int length = 14 + numDataPayloadBytes; // exclude first three bytes of frame
    
    frameBuffer[bufferIndex++] = DELIMITER;
    frameBuffer[bufferIndex++] = (length & 0xFF00) >> 8;
    frameBuffer[bufferIndex++] = length & 0x00FF;
    frameBuffer[bufferIndex++] = frameType;
    frameBuffer[bufferIndex++] = frameID;
    frameBuffer[bufferIndex++] = (destinationAddrHigh & 0xFF000000) >> 24;
    frameBuffer[bufferIndex++] = (destinationAddrHigh & 0x00FF0000) >> 16;
    frameBuffer[bufferIndex++] = (destinationAddrHigh & 0x0000FF00) >> 8;
    frameBuffer[bufferIndex++] = destinationAddrHigh & 0x000000FF;
    frameBuffer[bufferIndex++] = (destinationAddrLow & 0xFF000000) >> 24;
    frameBuffer[bufferIndex++] = (destinationAddrLow & 0x00FF0000) >> 16;
    frameBuffer[bufferIndex++] = (destinationAddrLow & 0x0000FF00) >> 8;
    frameBuffer[bufferIndex++] = destinationAddrLow & 0x000000FF;
    frameBuffer[bufferIndex++] = (destinationNtwkAddr & 0xFF00) >> 8;
    frameBuffer[bufferIndex++] = destinationNtwkAddr & 0x00FF;
    frameBuffer[bufferIndex++] = broadcastRadius;
    frameBuffer[bufferIndex++] = options;
    
    int i = 0;
    for(i = 0; i < numDataPayloadBytes; i++)
    {
        frameBuffer[bufferIndex++] = dataPayload[i];
    }
    frameBuffer[bufferIndex++] = calculateChecksum(frameBuffer, length);
    
    printf("Constructed Frame: \n");
    for(i = 0; i < bufferIndex; i++)
    {
        printf("0x%.2X\n", frameBuffer[i]);
    }
    
    sendData(frameBuffer, bufferIndex);
}

// Add all bytes of the packet, except start delimiter and the length. Keep lowest 8 bits. Subtract from 0xFF.
unsigned char calculateChecksum(unsigned char* data, unsigned char length)
{
    unsigned char sum = 0;
    unsigned char checksum;
    unsigned int i;
    
    for(i = 3; i < length + 3; i++)
    {
        sum += data[i];
    }
    
    checksum = 0xFF - sum;
    
    return checksum;
}
