#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

#define slaveAddr 0xd0>>1
#define msgLen 1

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

void I2CA_Init(){

    I2caRegs.I2CSAR=slaveAddr;
    I2caRegs.I2CPSC.all=14;

    I2caRegs.I2CCLKL = 10;      // NOTE: must be non zero
    I2caRegs.I2CCLKH = 5;       // NOTE: must be non zero
    I2caRegs.I2CIER.all = 0x24; // Enable SCD & ARDY interrupts

    //
    // Take I2C out of reset
    // Stop I2C when suspended
    //
    I2caRegs.I2CMDR.all = 0x0020;

    I2caRegs.I2CFFTX.all = 0x6000;  // Enable FIFO mode and TXFIFO
    I2caRegs.I2CFFRX.all = 0x2040;  // Enable RXFIFO, clear RXFFINT,
}

Uint16 I2CA_write(Uint16 addr,Uint16 data){

    if(I2caRegs.I2CMDR.bit.STP==1){
        return I2C_STP_NOT_READY_ERROR;
    }

    //set slave addr
    I2caRegs.I2CSAR=slaveAddr;

    if(I2caRegs.I2CSTR.bit.BB==1){
        return I2C_BUS_BUSY_ERROR;
    }

    //set num of bytes to send msg + addr
    I2caRegs.I2CCNT=msgLen+1;

    //set data to send
    I2caRegs.I2CDXR=addr;
    I2caRegs.I2CDXR=data;

    I2caRegs.I2CMDR.all=0x6E20;

    return I2C_SUCCESS;

}


Uint16 I2CA_read(Uint16 addr){

    if(I2caRegs.I2CMDR.bit.STP==1){
        return I2C_STP_NOT_READY_ERROR;
    }

    I2caRegs.I2CSAR = slaveAddr;

    if(I2caRegs.I2CSTR.bit.BB==1){
        return I2C_BUS_BUSY_ERROR;
    }

    I2caRegs.I2CCNT=1;
    I2caRegs.I2CDXR=addr;
    I2caRegs.I2CMDR.all=0x2620;

    delay_us(50);
    I2caRegs.I2CCNT=msgLen;
    I2caRegs.I2CMDR.all=0x2C20; //send SCL and read


    //return I2C_SUCCESS;
    return I2caRegs.I2CDRR;

}

interrupt void i2c_int1a_isr(){
    GpioDataRegs.GPADAT.all=~I2CA_read(0);


    //
    // Enable future I2C (PIE Group 8) interrupts
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
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
    InitSysCtrl();

    InitI2CGpio();

    DINT;

    InitPieCtrl();

    IER=0;
    IFR=0;

    InitPieVectTable();

    EALLOW;
    PieVectTable.I2CINT1A=&i2c_int1a_isr;
    EDIS;
    init_testGPIO();
    I2CA_Init();

    PieCtrlRegs.PIEIER8.bit.INTx1=1;

    //IER |= M_INT8;
    //EINT;

    while(1){

        GpioDataRegs.GPADAT.all=~I2CA_read(0);

        //I2CA_read(0);
        //I2CA_write(0,4);
        delay_us(100000);

    }

	return 0;
}
