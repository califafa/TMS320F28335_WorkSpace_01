
//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

void main(){

    InitSysCtrl();

    unsigned long i=0;
    unsigned long j=1;
    EALLOW;
    GpioCtrlRegs.GPADIR.all=0xffffffff;
    EDIS;

    GpioDataRegs.GPADAT.bit.GPIO30=0;
    delay_us(1000000);

    GpioDataRegs.GPATOGGLE.bit.GPIO30=1;

    while(1){
        /*
        DSP28x_usDelay(100);
        //GpioDataRegs.GPADAT.all=~(1<<(i%6));
        i++;
        j=i*1000;
        GpioDataRegs.GPADAT.bit.GPIO30=1;
        delay_us(j);
        GpioDataRegs.GPADAT.bit.GPIO30=0;
        */
    }

}
