#include <plib.h>
#include <xc.h>
#include <stdio.h>
#include "xbee_serial_protocol.h"

volatile unsigned char receiveFlag = 0;
volatile struct bufferStruct receiveBuffer;
volatile unsigned char fullPacketReceived = 0;
volatile unsigned int receivedPacketLength;

#define OPEN_FLAG_QUERY_STATE 0
#define GENERIC_RECEIVE_STATE 1
#define GET_PACKET_LENGTH_STATE 2 

void receiverFSM(void);

void __ISR(_UART_1_VECTOR, ipl1) UART1Interrupt()
{   
    IFS0bits.U1RXIF = 0;

    if(U1STAbits.URXDA) // IMPORTANT: why do we get two interrupts per byte?
    {
        receiveBuffer.buffer[receiveBuffer.currentIndex] = U1RXREG;

        switch(operatingMode)
        {
            case AT_MODE:
                if(receiveBuffer.buffer[receiveBuffer.currentIndex] == '\r') receiveFlag = 1; 
                break;
            case API_MODE_NO_ESCAPE:
                printf("0x%.2X\n", receiveBuffer.buffer[receiveBuffer.currentIndex]);
                receiverFSM();
                break;
            case API_MODE_WITH_ESCAPE:
                break;
            default:
                break;
        }

        ++receiveBuffer.numBytes;
        ++receiveBuffer.currentIndex;
    }
}

void receiverFSM(void)
{
    unsigned static char state = OPEN_FLAG_QUERY_STATE, currentFieldBytes = 0;
    
    switch(state)
    {
        case OPEN_FLAG_QUERY_STATE:
            if(receiveBuffer.buffer[receiveBuffer.currentIndex] == 0x7E)
            {
                state = GET_PACKET_LENGTH_STATE;
                //printf("Open flag received\n");
            }
            break;
        case GET_PACKET_LENGTH_STATE:
            if(currentFieldBytes == 0)
            {
                receivedPacketLength = receiveBuffer.buffer[receiveBuffer.currentIndex] << 8;
                ++currentFieldBytes;
                state = GET_PACKET_LENGTH_STATE;
            }
            else if(currentFieldBytes == 1)
            {
                receivedPacketLength &= 0xFF00;
                receivedPacketLength |= receiveBuffer.buffer[receiveBuffer.currentIndex];
                //printf("Received message length: %d\n", receivedMessageLength);
                state = GENERIC_RECEIVE_STATE;
            }
            break;
        case GENERIC_RECEIVE_STATE: // get all message parameters (including checksum), as specified by the packet length field in previous state
        {
            unsigned static char localByteCount = 0;
            if(localByteCount < receivedPacketLength) 
            {
                state = GENERIC_RECEIVE_STATE;
            }
            else
            {
                fullPacketReceived = 1;
                printf("Checksum: 0x%.2X\n/n", receiveBuffer.buffer[receiveBuffer.currentIndex]);
                state = OPEN_FLAG_QUERY_STATE;
            }
            ++localByteCount;
            break;
        }
        default:
            break;
    }
}

