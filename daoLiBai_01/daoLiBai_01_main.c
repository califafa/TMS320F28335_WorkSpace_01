#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

unsigned int maxV=65535;
unsigned int minV=0;
int pwm=0;


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

//myFilter

Uint16 lastPre=0;

float a=50;

Uint16 myFilter(Uint16 tmp){
    Uint16 t=(lastPre+tmp)/2;
    lastPre=2*t-lastPre;
    return t;
}


//kalmanFilter

Uint16 readValue();

float x_last=0;
float p_last=0.02;
float Q=0.018;
float R=0.542;
float kg;
float x_mid;
float x_now;
float p_mid;
float p_now;
float z_measure;

void kalmanInit(){
    z_measure=readValue();
    x_last=z_measure;
    x_mid=x_last;

}

Uint16 kalmanRealize(){
    x_mid=x_last;
    p_mid=p_last+Q;
    kg=p_mid/(p_mid+R);
    z_measure=(float)readValue();
    x_now=x_mid+kg*(z_measure-x_mid);
    p_now=(1-kg)*p_mid;

    p_last=p_now;
    x_last=x_now;

    return (Uint16)x_now;

}


//adc

void adcInit(){

    InitAdc();

    AdcRegs.ADCTRL1.bit.ACQ_PS=0xf; //     S/H CLK , 16 ADC CLK
    AdcRegs.ADCTRL3.bit.ADCCLKPS= 32 ;   // HSPCLK/(2*x)=
    AdcRegs.ADCTRL1.bit.SEQ_CASC=0;  // 1 Cascaded mode, 0 non-cascaded

    AdcRegs.ADCCHSELSEQ1.bit.CONV00=0;  //channel 0 use io A0

    AdcRegs.ADCMAXCONV.bit.MAX_CONV1=0; //  MAX_CONV1+1 , 0 use 1 channels, 1 use 2 channels , 2 use 3 channels
    AdcRegs.ADCTRL1.bit.CONT_RUN=1;     //continuous run

    AdcRegs.ADCTRL1.bit.SEQ_OVRD=0; // 0 allow loop in max_conv  , 1 not allow

    AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;
}

Uint16 readValue(){
    Uint16 tmp=0;

    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1; //trigger

    while(!AdcRegs.ADCST.bit.INT_SEQ1); //waiting for converse finished

    tmp=(AdcRegs.ADCRESULT0);    //AdcRegs.ADCRESULT0 returns 16bits.

    return tmp>>4;
}


//pid
long tmp_integral=0,tmp_voltage=0;

struct _pid{
    int SetSpeed;         //程序想要达到的速度
    int ActualSpeed;      //实际速度
    int err;              //当前误差
    int err_last;         //上一次pid的误差
    float Kp, Ki, Kd;       //比例系数、积分系数、微分系数
    int voltage;          //驱动电压
    int integral;         //累计误差（积分）
}pid;


int pidRealize(float speed){


    pid.SetSpeed=speed;
    pid.err=pid.SetSpeed-pid.ActualSpeed;

    tmp_integral+=pid.err;
    if((tmp_integral<=(long)0x7fff)&&(tmp_integral>=((long)-0x7fff))){
        pid.integral=tmp_integral;
    }else{
        if(tmp_integral>(long)0x7fff){
            pid.integral=0x7fff;
        }
        if(tmp_integral<(long)-0x7fff){
            pid.integral=-0x7fff;
        }
        tmp_integral=pid.integral;
    }

    tmp_voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);   //电压大小
    if((tmp_voltage<=(long)0x7fff)&&(tmp_voltage>=((long)-0x7fff))){
        pid.voltage=tmp_voltage;
    }else{
        if(tmp_voltage>(long)0x7fff){
            pid.voltage=0x7fff;
        }
        if(tmp_voltage<(long)-0x7fff){
            pid.voltage=-0x7fff;
        }
        tmp_voltage=pid.voltage;
    }
    pid.err_last=pid.err;
    pid.ActualSpeed=kalmanRealize();//readValue();                                      //读取实际的速度，本代码里使用函数模拟
    return pid.voltage;
}

void pidInit(){
    pid.SetSpeed=0;
    pid.ActualSpeed=0;
    pid.err=0;
    pid.err_last=0;
    pid.voltage=0;
    pid.integral=0;
    pid.Kp=0.4;
    pid.Ki=0.3;
    pid.Kd=-0.4;
}

//sci
void sciInit(){
    InitSciaGpio();
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
    SciaRegs.SCICCR.bit.SCICHAR=0b111;
}

void scia_send(int a){
    SciaRegs.SCITXBUF=a;
}




/**
 * main.c
 */
int main(void)
{

    InitSysCtrl();

    DINT;

    InitPieCtrl();

    IER=0;
    IFR=0;

    InitPieVectTable();

    EALLOW;
    SysCtrlRegs.HISPCP.all = 0x03;  // peripheral clk HSPCLK = SYSCLKOUT / 0x03;
    EDIS;

    init_testGPIO();

    adcInit();

    pidInit();
    sciInit();

    //test code
    while(1){
        //GpioDataRegs.GPADAT.all=~(readValue()>>10);

        //kalmanRealize read the balance position , average value is 184<<8 or 184.5<<8 __ 0xb8<<8 + 0x80
        //average balance value is 0xb880


        pwm=pidRealize(0xb88);


        //scia_send(pid.ActualSpeed>>4);


        scia_send(pwm>>8);
        delay_us(2500);
        //scia_send(pid.ActualSpeed);
        //delay_us(5000);
    }



	return 0;
}
