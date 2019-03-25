#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

struct _pid{
    float SetSpeed;         //程序想要达到的速度
    float ActualSpeed;      //实际速度
    float err;              //当前误差
    float err_last;         //上一次pid的误差
    float Kp, Ki, Kd;       //比例系数、积分系数、微分系数
    float voltage;          //驱动电压
    float integral;         //累计误差（积分）
}pid;

float readSpeed(){
    return (pid.voltage)/3-100;     //假设速度和电压的关系是这样的
}

float PID_realize(float speed){
    pid.SetSpeed=speed;
    pid.err=pid.SetSpeed-pid.ActualSpeed;
    pid.integral+=pid.err;
    pid.voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);   //电压大小
    pid.err_last=pid.err;
    pid.ActualSpeed=readSpeed();                                      //读取实际的速度，本代码里使用函数模拟
    return pid.ActualSpeed;
}

void PID_init(){
    pid.SetSpeed=0;
    pid.ActualSpeed=0;
    pid.err=0;
    pid.err_last=0;
    pid.voltage=0;
    pid.integral=0;
    pid.Kp=0.2;
    pid.Ki=0.3;
    pid.Kd=-0.2;
}

void init_testGPIO(){
    EALLOW;
    GpioCtrlRegs.GPADIR.bit.GPIO30=1;
    EDIS;
}

//
// Main
//
void main(void)
{
    int i=0;
    int j=0;
    float speed=0;
    int a[10]={30,40,50,60,70,80,90,100,110,120};   //为了测试方便， 定义数组a， 一共10个数， 可以测试10次。

    InitSysCtrl();
    init_testGPIO();
    PID_init();


    for(j=0;j<10;j++){                  //一共测试10个数 ， 从a[0] 到 a[9]。
        for(i=0;i<128;i++){             //每个数迭代128次。
            speed=PID_realize(a[j]);    //在迭代128次后 ， speed 会很接近 设定的数（a[n])。
        }

        GpioDataRegs.GPADAT.bit.GPIO30=1;
        for(i=0;i<speed;i++){
            delay_us(1000);                     //最后达到多大的速度，脉冲宽度就有多少毫秒。
        }
        GpioDataRegs.GPADAT.bit.GPIO30=0;
    }
}

//
// End of File
//
