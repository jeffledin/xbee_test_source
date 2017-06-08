#include <plib.h>
#include <xc.h>
#include <stdio.h>
#include "xbee_serial_protocol.h"

volatile unsigned char receiveFlag = 0;
volatile struct bufferStruct receiveBuffer;

void __ISR(_UART_1_VECTOR, ipl1) UART1Interrupt()
{   
    unsigned static char byteCount = 0;
    
    if(IFS0bits.U1RXIF)
    {
        IFS0bits.U1RXIF = 0;
        
        if(U1STAbits.URXDA) // IMPORTANT: why do we get two interrupts per byte?
        {
            receiveBuffer.buffer[receiveBuffer.currentIndex] = U1RXREG;
            //printf("0x%.2X\n", receiveBuffer.buffer[receiveBuffer.currentIndex]);

            switch(operatingMode)
            {
                case AT_MODE:
                    if(receiveBuffer.buffer[receiveBuffer.currentIndex] == '\r') receiveFlag = 1; 
                    break;
                case API_MODE_NO_ESCAPE:
        //            if(receiveBuffer.buffer[receiveBuffer.currentIndex] == 0x7E) byteCount = 0;//printf("Open response flag\n");
                    printf("0x%.2X\n", receiveBuffer.buffer[receiveBuffer.currentIndex]);
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
    
    
}


