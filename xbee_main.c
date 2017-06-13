// Adds support for PIC32 Peripheral library functions and macros
#include <plib.h>
#include <xc.h>
#include <stdio.h>
#include "xbee_serial_protocol.h"
#include "configs.h"

#define _SUPPRESS_PLIB_WARNING 

void init(void);
void initTMR1(void);
void initTMR2(void);
void initIO(void);
void initUART1(unsigned char baudrate);
void initUART2(unsigned char baudrate);
void delay_ms(unsigned int milliseconds);

volatile struct bufferStruct receiveBuffer;

void main(void)
{
    unsigned char frameBuffer[50], dataPayload[50] = "TXDATA0C";
    unsigned char *parameters;

    init();
    
    printf("-------------------------\n");
    printf("XBee S2C -- API Mode\n");
    printf("-------------------------\n");

//    changeXBeeOperatingMode(AT_MODE); // go into AT Mode first to set a destination address and disable broadcast mode
//    changeXBeeBaudrate(BAUD_115200);
    
    changeXBeeOperatingMode(API_MODE_NO_ESCAPE);
    
    while(1)
    {
//        createTransmitRequestFrame(frameBuffer, TRANSMIT_REQUEST, 0x01, 0x0013A200, 0x41632F8D, 0xFFFE, 0x00, 0x00, 
//                               dataPayload, 8);
//    
//        printf("Frame sent\n\n");

        while(!fullPacketReceived);
        fullPacketReceived = 0;
        receiveBuffer.numBytes = 0;
        receiveBuffer.currentIndex = 0;
        printf("Packet received\n");

        processReceivedPacket();
        
        delay_ms(2000);
    }
    
    
    
    while(1);
    
    //createAPIFrame(TRANSMIT_REQUEST, )
    
    //sendData(dataBuffer, 255);
    
    while(1)
    {

    };
}

void init(void)
{
    DDPCONbits.JTAGEN = 0; // disable JTAG pins
    initIO();
//    initTMR1();
//    initTMR2();
    initUART1(BAUD_115200);
    initUART2(BAUD_115200);
    
    receiveBuffer.currentIndex = 0;
    receiveBuffer.numBytes = 0;
}

void initTMR1(void)
{
    // PBCLK = 80 MHz; don't use prescaler of 1:1 for this frequency 
    T1CONbits.ON = 0;
    T1CONbits.TCKPS = 0b01; // 1:8 prescaler
    PR1 = 10000;
    TMR1 = 0;
}

void initTMR2(void)
{
    asm volatile("di"); // disable global interrupts
    
    T2CONbits.ON = 0;
    T2CONbits.TCKPS = 0b111; // 1:256 prescaler
    PR2 = 31250; // 0.1s delay
    TMR2 = 0;
    
    IEC0bits.T2IE = 1;
    IFS0bits.T2IF = 0;
    INTCONbits.MVEC = 1; // enable multi-vectored interrupts
    IPC2bits.T2IP = 1; // priority 1
    
    asm volatile("ei"); // enable global interrupts
    
    T2CONbits.ON = 1;
}

void initIO(void)
{
    LATA = 0;
    TRISA = 0;
}

void delay_ms(unsigned int milliseconds)
{
    unsigned int i;
    T1CONbits.ON = 1;
    for(i = 0; i < milliseconds; i++)
    {
        while(!IFS0bits.T1IF);
        IFS0bits.T1IF = 0;
    }
}

void initUART1(unsigned char baud_select)
{
    // XBee Interface
    // Baudrate Calculations for 115200 bps
    // U2BRG = (Fpb / (4 * Baud Rate)) - 1
    U1MODEbits.ON = 0; // turn off UART module
    
    switch(baud_select)
    {
        case BAUD_9600:
            U1MODEbits.BRGH = 0;
            U1BRG = 520;
            break;
        case BAUD_115200:
            U1MODEbits.BRGH = 1;
            U1BRG = 172;
            break;
        default:
            break;
    }
    
    asm volatile("di"); // disable global interrupts
    
    INTCONbits.MVEC = 1; // enable multi-vectored interrupts
    IPC6bits.U1IP = 1; // interrupt priority = 1
    IEC0bits.U1RXIE = 1; // enable receive interrupts
    IEC0bits.U1TXIE = 0; // disable transmission interrupts
    IFS0bits.U1RXIF = 0; // reset interrupt flag
    IFS0bits.U1TXIF = 0; // reset interrupt flag
    U1STAbits.URXEN = 1; // enable RX
    U1STAbits.UTXEN = 1; // enable TX
    U1MODEbits.ON = 1; // turn on UART module
    
    asm volatile("ei"); // enable global interrupts
    
    delay_ms(50);
}

void initUART2(unsigned char baudrate)
{
    // Baudrate Calculations for 115200 bps
    // U2BRG = (Fpb / (4 * Baud Rate)) - 1
    U2MODEbits.ON = 0; // turn off UART module
    
    switch(baudrate)
    {
        case BAUD_9600:
            U2MODEbits.BRGH = 0;
            U2BRG = 520;
            break;
        case BAUD_115200:
            U2MODEbits.BRGH = 1;
            U2BRG = 172;
            break;
        default:
            break;
    }
    
    IFS1bits.U2RXIF = 0; // reset interrupt flag
    IFS1bits.U2TXIF = 0; // reset interrupt flag
    U2STAbits.URXEN = 1; // enable RX
    U2STAbits.UTXEN = 1; // enable TX
    U2MODEbits.ON = 1; // turn on UART module
    
    delay_ms(50);
}



