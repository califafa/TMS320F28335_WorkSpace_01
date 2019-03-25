
//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

void scia_send(int a){
    //while(SciaRegs.SCIFFTX.bit.TXFFST!=0);

    SciaRegs.SCITXBUF=a;
}

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

void main(){

    InitSysCtrl();

    InitSciaGpio();

    DINT;

    InitPieCtrl();

    IER=0x0000;

    IFR=0x0000;

    InitPieVectTable();

    SciaRegs.SCICCR.all=0x0007;


    SciaRegs.SCICTL1.bit.TXENA=1;
    SciaRegs.SCICTL1.bit.RXENA=1;
    //SciaRegs.SCICTL1.all=0x0003;

    SciaRegs.SCICTL2.bit.TXINTENA=1;
    SciaRegs.SCICTL2.bit.RXBKINTENA=1;
    //SciaRegs.SCICTL2.all=0x0003;

    SciaRegs.SCICTL2.bit.TXINTENA=0;
    SciaRegs.SCICTL2.bit.RXBKINTENA=0;

    SciaRegs.SCIHBAUD=0x0001;
    SciaRegs.SCILBAUD=0x00E7;

    //SciaRegs.SCICTL1.bit.TXENA=1;
    //SciaRegs.SCICTL1.bit.RXENA=1;
    SciaRegs.SCICTL1.bit.SWRESET=1;
    //SciaRegs.SCICTL1.all=0x0023;

    while(1){
        scia_send(0x73);
        delay_us(10000);
    }

}
