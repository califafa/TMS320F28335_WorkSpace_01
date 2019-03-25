
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File


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

    InitSysCtrl();

    EALLOW;
    SysCtrlRegs.HISPCP.all = 0x03;  // peripheral clk HSPCLK = SYSCLKOUT / 0x03;
    EDIS;

    init_testGPIO();
    //GpioDataRegs.GPATOGGLE.bit.GPIO30=1;

    DINT;

    InitPieCtrl();

    IER=0;
    IFR=0;

    InitPieVectTable();

    InitAdc();

    AdcRegs.ADCTRL1.bit.ACQ_PS=0xf; //     S/H CLK , 16 ADC CLK
    AdcRegs.ADCTRL3.bit.ADCCLKPS= 1 ;   // HSPCLK/(2*1)=12.5
    AdcRegs.ADCTRL1.bit.SEQ_CASC=0;  // 1 Cascaded mode, 0 non-cascaded


    AdcRegs.ADCCHSELSEQ1.bit.CONV00=0;  //channel 0 use io A0
    //AdcRegs.ADCCHSELSEQ2.bit.CONV04=0;    //channel 4 use io A0;

    AdcRegs.ADCMAXCONV.bit.MAX_CONV1=2; //  MAX_CONV1+1 , 0 use 1 channels, 1 use 2 channels , 2 use 3 channels
    AdcRegs.ADCTRL1.bit.CONT_RUN=1;     //continuous run
    //AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=0x01;

    AdcRegs.ADCTRL1.bit.SEQ_OVRD=0; // 0 allow loop in max_conv  , 1 not allow

    AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;

    while(1){

        AdcRegs.ADCTRL2.bit.SOC_SEQ1=1; //trigger

        while(!AdcRegs.ADCST.bit.INT_SEQ1); //waiting for converse finished

        GpioDataRegs.GPADAT.all=~((AdcRegs.ADCRESULT2)>>10);    //AdcRegs.ADCRESULT0 returns 16bits.


        GpioDataRegs.GPATOGGLE.bit.GPIO30=1;
        delay_us(400000);
    }
   // while(1);

    return 0;
}
