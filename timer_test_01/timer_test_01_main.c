
//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File



void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}



interrupt void timer0_isr(){

    //GpioDataRegs.GPATOGGLE.bit.GPIO30=1;
    GpioDataRegs.GPADAT.bit.GPIO30=1;
    delay_us(300000);
    GpioDataRegs.GPADAT.bit.GPIO30=0;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;


}

interrupt void timer1_isr(){

    //GpioDataRegs.GPATOGGLE.bit.GPIO29=1;

    GpioDataRegs.GPADAT.bit.GPIO29=1;
    delay_us(300000);
    GpioDataRegs.GPADAT.bit.GPIO29=0;

    //PieCtrlRegs.PIEACK.all = PIEACK_GROUP13;

}

interrupt void timer2_isr(){

    //GpioDataRegs.GPATOGGLE.bit.GPIO0=1;

    GpioDataRegs.GPADAT.bit.GPIO0=1;
    delay_us(300000);
    GpioDataRegs.GPADAT.bit.GPIO0=0;

    //PieCtrlRegs.PIEACK.all = PIEACK_GROUP14;

}

void init_testGpio(){

    EALLOW;
    GpioCtrlRegs.GPADIR.bit.GPIO30=1;
    GpioCtrlRegs.GPADIR.bit.GPIO29=1;
    GpioCtrlRegs.GPADIR.bit.GPIO0=1;
    EDIS;
}

void main(){

    InitSysCtrl();

    init_testGpio();

    DINT;

    InitPieCtrl();

    IER=0x0000;
    IFR=0x0000;

    InitPieVectTable();

    EALLOW;
    PieVectTable.TINT0=&timer0_isr;
    PieVectTable.XINT13=&timer1_isr;
    PieVectTable.TINT2=&timer2_isr;
    EDIS;

    InitCpuTimers();

    ConfigCpuTimer(&CpuTimer0,150,630000);  //630000 us ==630 ms
    ConfigCpuTimer(&CpuTimer1,150,700000);
    ConfigCpuTimer(&CpuTimer2,150,600000);

    CpuTimer0Regs.TCR.all=0x4000;
    CpuTimer1Regs.TCR.all=0x4000;
    CpuTimer2Regs.TCR.all=0x4000;

    IER |= M_INT1;
    IER |= M_INT13;
    IER |= M_INT14;

    PieCtrlRegs.PIEIER1.bit.INTx7=1;

    EINT;
    ERTM;

    while(1){
        ;
    }

}
