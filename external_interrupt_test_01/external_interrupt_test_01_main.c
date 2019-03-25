
//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

interrupt void xint1_isr(){
    GpioDataRegs.GPATOGGLE.bit.GPIO30=1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

__interrupt void xint2_isr(){
    GpioDataRegs.GPATOGGLE.bit.GPIO29=1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

void main(){

    InitSysCtrl();

    DINT;

    InitPieCtrl();

    IER=0x0000;
    IFR=0x0000;

    InitPieVectTable();

    EALLOW;
    PieVectTable.XINT1=&xint1_isr;
    PieVectTable.XINT2=&xint2_isr;
    EDIS;

    PieCtrlRegs.PIECTRL.bit.ENPIE=1;
    PieCtrlRegs.PIEIER1.bit.INTx4=1;
    PieCtrlRegs.PIEIER1.bit.INTx5=1;

    IER |= M_INT1;
    EINT;

    EALLOW;
    GpioCtrlRegs.GPADIR.bit.GPIO30=1;
    GpioCtrlRegs.GPADIR.bit.GPIO29=1;
    GpioCtrlRegs.GPADIR.bit.GPIO1=1;
    EDIS;

    //GpioCtrlRegs.GPAQSEL1.bit.GPIO1=2;

    EALLOW;

    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL=15; //select GPIO15
    GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL=2; //select GPIO2

    XIntruptRegs.XINT1CR.bit.POLARITY=1; //xint1 Rising Edge
    XIntruptRegs.XINT2CR.bit.POLARITY=0; //xint2 Falling Edge

    XIntruptRegs.XINT1CR.bit.ENABLE=1;
    XIntruptRegs.XINT2CR.bit.ENABLE=1;

    GpioCtrlRegs.GPAQSEL1.bit.GPIO1=2;  // 6 samples as a sample
    GpioCtrlRegs.GPACTRL.bit.QUALPRD0=0xFF; //2*255 clk period a sample

    EDIS;

    while(1){
        GpioDataRegs.GPATOGGLE.bit.GPIO1=1;
    }
}
