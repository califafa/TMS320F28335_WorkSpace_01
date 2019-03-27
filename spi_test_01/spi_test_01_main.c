#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File


void spi_xmit(Uint16 a){
    SpiaRegs.SPITXBUF=a;
}

void spi_init(){

    //SpiaRegs.SPICCR.all=0x000F; //Reset on , rising edge , 16-bit char bits

    //Enable master mode , normal phase , enable talk , and SPI int disabled.
    SpiaRegs.SPICTL.all=0x0006;
    SpiaRegs.SPIBRR=0x007F;
    SpiaRegs.SPICCR.all=0x0087; //Relinquish SPI from Reset , 8-bit char bits
    SpiaRegs.SPIPRI.bit.FREE=1; //Set so breakpoints don't disturb xmission

}

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

void init_testGPIO(){
    EALLOW;
    GpioCtrlRegs.GPADIR.all=0xff;
    EDIS;
}

/**
 * main.c
 */
int main(void)
{
    int i,j,k;
    InitSysCtrl();
    InitSpiaGpio();
    init_testGPIO();
    DINT;
    InitPieCtrl();
    IER=0;
    IFR=0;
    spi_init();

    while(1){
        spi_xmit(((i%8)+4)<<8);
        i++;

        /*
        while(SpiaRegs.SPIFFRX.bit.RXFFST !=1){

        }
        */
        delay_us(1000);

        GpioDataRegs.GPADAT.all=~((SpiaRegs.SPIRXBUF));

    }

	return 0;
}
