#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

#pragma DATA_SECTION(bufR,"DMARAML4");
#pragma DATA_SECTION(bufS,"DMARAML5");

#define SIZE 16

unsigned int bufR[SIZE];
unsigned int bufS[SIZE];

void * DMADest;
void * DMASource;

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

void init_testGpio(){

    EALLOW;
    GpioCtrlRegs.GPADIR.bit.GPIO30=1;
    GpioCtrlRegs.GPADIR.bit.GPIO29=1;
    GpioCtrlRegs.GPADIR.bit.GPIO0=1;
    EDIS;
}

interrupt void dmaCH1_isr(){

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

}

/**
 * main.c
 */
int main(void)
{
    int i,j,k;
    InitSysCtrl();
    DINT;

    init_testGpio();

    InitPieCtrl();
    IER=0;
    IFR=0;
    InitPieVectTable();

    EALLOW;
    PieVectTable.DINTCH1=&dmaCH1_isr;
    EDIS;

    IER=M_INT7;
    EnableInterrupts();
    CpuTimer0Regs.TCR.bit.TSS=1;

    DMAInitialize();

    DMADest=&bufR[0];
    DMASource=&bufS[0];
    DMACH1AddrConfig(DMADest,DMASource);

    DMACH1BurstConfig(31,2,2);  //32-bit datasize, pointer step is 2(based on 16-bit)
    DMACH1TransferConfig(7,2,2);

    DmaRegs.CH1.BURST_COUNT.bit.BURSTCOUNT=(int)SIZE/2;
    DmaRegs.CH1.TRANSFER_COUNT=1;//.bit.TRANSFERCOUNT=1;
    DMACH1WrapConfig(0xFFFF,0,0xFFFF,0);

    DMACH1ModeConfig(DMA_TINT1,PERINT_ENABLE,ONESHOT_ENABLE,CONT_ENABLE,SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,THIRTYTWO_BIT,CHINT_END,CHINT_DISABLE);

    for(i=0;i<SIZE;i++){
        bufS[i]=i+10;
        bufR[i]=SIZE-i;
    }
    DmaRegs.CH1.CONTROL.bit.RUNSTS=0xffff;
    StartDMACH1();

    CpuTimer0Regs.TIM.half.LSW=512;
    CpuTimer0Regs.TCR.bit.SOFT=1;   //allow to free run even if halted
    CpuTimer0Regs.TCR.bit.FREE=1;
    CpuTimer0Regs.TCR.bit.TIE=1;    //Enable the timer0 interrupt signal
    CpuTimer0Regs.TCR.bit.TSS=0;   //restart the timer0


    CpuTimer1Regs.TIM.half.LSW=512;
    CpuTimer1Regs.TCR.bit.SOFT=1;   //allow to free run even if halted
    CpuTimer1Regs.TCR.bit.FREE=1;
    CpuTimer1Regs.TCR.bit.TIE=1;    //Enable the timer0 interrupt signal
    CpuTimer1Regs.TCR.bit.TSS=0;   //restart the timer0


    //DmaRegs.CH1.CONTROL.bit.PERINTFRC=1;
    delay_us(100000);


    for(i=0;i<SIZE;i++){
        GpioDataRegs.GPADAT.bit.GPIO30=1;
        for(j=0;j<bufR[i];j++){
            delay_us(1000);
        }
        GpioDataRegs.GPADAT.bit.GPIO30=0;
    }

	return 0;
}
