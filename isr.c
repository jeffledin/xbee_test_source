#include <plib.h>
#include <xc.h>
#include <stdio.h>

volatile unsigned char readTemperatureFlag = 0;

//void __ISR(0, ipl1) isr() // single vector
//{
//    if(IFS0bits.T2IF)
//    {
//        IFS0bits.T2IF = 0;
//        printf("Heya\n");
//    }
//}

void __ISR(_TIMER_2_VECTOR, ipl1) T2Interrupt() // multi-vector
{
    unsigned static int thirtySeconds = 0;
    IFS0bits.T2IF = 0;
    
    if(thirtySeconds >= 300)
    {
        thirtySeconds = 0;
        readTemperatureFlag = 1;
        printf("Reading temperature...\n");
    }
    else
    {
        ++thirtySeconds;
    }
}



