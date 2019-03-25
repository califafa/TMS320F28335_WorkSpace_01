#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

#define CPU_FRQ_150MHZ 1
//
// Defines
//
#if (CPU_FRQ_150MHZ)
    #define CPU_CLK   150e6
#endif
#if (CPU_FRQ_100MHZ)
    #define CPU_CLK   100e6
#endif

//
// 5kHz (300rpm) EPWM1 frequency. Frequency can be changed here
//
#define PWM_CLK   5e3
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E      // up-down count, timebase=SYSCLKOUT

void InitEPwm1(){

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC=0;    //stop all the TB clocks
    EDIS;



    //Setup Sync
    EPwm1Regs.TBCTL.bit.SYNCOSEL=TB_SYNC_DISABLE;    //Pass through

    EPwm1Regs.TBCTR=0;

    EPwm1Regs.TBCTL.bit.HSPCLKDIV=7;
    EPwm1Regs.TBCTL.bit.CLKDIV=0;

    //allow each timer to be sync'ed
    EPwm1Regs.TBCTL.bit.PHSEN=TB_DISABLE;    //相位使能
    EPwm1Regs.TBPHS.half.TBPHS=0; //时间基准 相位 寄存器
    EPwm1Regs.TBCTL.bit.PRDLD=1;
    EPwm1Regs.TBPRD=15000;//PWM1_TIMER_TBPRD;周期
    //EPwm1Regs.TBCTL.bit.FREE_SOFT=
    //EPwm1Regs.TBCTL.bit.PHSDIR=0;
    EPwm1Regs.TBCTL.bit.CTRMODE=TB_COUNT_UPDOWN; //count up-down

    //EPwm1Regs.CMPA.half.
    EPwm1Regs.CMPCTL.bit.SHDWAMODE=0; //CC_SHADOW
    EPwm1Regs.CMPCTL.bit.SHDWBMODE=0;

    EPwm1Regs.CMPCTL.bit.LOADAMODE=0x00; //CC_CTR_ZERO
    EPwm1Regs.CMPCTL.bit.LOADBMODE=0x10;
    EPwm1Regs.CMPA.half.CMPA=3000;

    EPwm1Regs.AQCTLA.bit.CAU=0x02;//AQ_SET
    EPwm1Regs.AQCTLA.bit.CAD=0x01;//AQ_CLEAR
    EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;          // Set PWM1A on Zero
    EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;


    EPwm1Regs.DBCTL.bit.OUT_MODE=0x00;  //00 directly out ,
    EPwm1Regs.DBCTL.bit.IN_MODE=0x00;
    EPwm1Regs.DBCTL.bit.POLSEL=0x01;

    EPwm1Regs.DBFED=0;//EPWM1_MIN_DB    , falling delay
    EPwm1Regs.DBRED=0;                 // rising delay

    EPwm1Regs.ETSEL.bit.INTSEL=ET_CTR_ZERO; //Select INT on ZERO event
    EPwm1Regs.ETSEL.bit.INTEN=1;    //Enable INT
    EPwm1Regs.ETPS.bit.INTPRD=ET_3RD;   //Generate INT on 3rd event

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC=1;
    EDIS;

}

Uint32 pwmCount=0;

interrupt void epwm1_isr(){

    pwmCount+=3;

    EPwm1Regs.ETCLR.bit.INT = 1;

    GpioDataRegs.GPATOGGLE.bit.GPIO30=1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

}

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

void init_testGPIO(){
    EALLOW;
    GpioCtrlRegs.GPADIR.all=0xffffffff;
    EDIS;
    GpioDataRegs.GPADAT.all=0xff;
}

/**
 * main.c
 */
int main(void)
{

    int i=0;

    InitSysCtrl();

    init_testGPIO();

    DINT;

    InitPieCtrl();

    IER=0x0000;
    IFR=0x0000;

    InitPieVectTable();

    EALLOW;
    PieVectTable.EPWM1_INT=&epwm1_isr;
    EDIS;

    EALLOW;
    //pievector
    EDIS;

    InitEPwm1Gpio();
    InitEPwm1();

    IER|=M_INT3;

    PieCtrlRegs.PIEIER3.bit.INTx1=1;

    EINT;
    ERTM;

    while(1){
       i++;
       //EPwm1Regs.TBPRD=3000+(i%6000);

       // __asm("          NOP");
    }

    return 0;
}
