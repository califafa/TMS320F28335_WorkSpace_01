#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

void delay_us(Uint32 t)
{
    Uint32 i = t*30;
    DSP28x_usDelay(i);
}

struct _pid{
    float SetSpeed;         //������Ҫ�ﵽ���ٶ�
    float ActualSpeed;      //ʵ���ٶ�
    float err;              //��ǰ���
    float err_last;         //��һ��pid�����
    float Kp, Ki, Kd;       //����ϵ��������ϵ����΢��ϵ��
    float voltage;          //������ѹ
    float integral;         //�ۼ������֣�
}pid;

float readSpeed(){
    return (pid.voltage)/3-100;     //�����ٶȺ͵�ѹ�Ĺ�ϵ��������
}

float PID_realize(float speed){
    pid.SetSpeed=speed;
    pid.err=pid.SetSpeed-pid.ActualSpeed;
    pid.integral+=pid.err;
    pid.voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);   //��ѹ��С
    pid.err_last=pid.err;
    pid.ActualSpeed=readSpeed();                                      //��ȡʵ�ʵ��ٶȣ���������ʹ�ú���ģ��
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
    int a[10]={30,40,50,60,70,80,90,100,110,120};   //Ϊ�˲��Է��㣬 ��������a�� һ��10������ ���Բ���10�Ρ�

    InitSysCtrl();
    init_testGPIO();
    PID_init();


    for(j=0;j<10;j++){                  //һ������10���� �� ��a[0] �� a[9]��
        for(i=0;i<128;i++){             //ÿ��������128�Ρ�
            speed=PID_realize(a[j]);    //�ڵ���128�κ� �� speed ��ܽӽ� �趨������a[n])��
        }

        GpioDataRegs.GPADAT.bit.GPIO30=1;
        for(i=0;i<speed;i++){
            delay_us(1000);                     //���ﵽ�����ٶȣ������Ⱦ��ж��ٺ��롣
        }
        GpioDataRegs.GPADAT.bit.GPIO30=0;
    }
}

//
// End of File
//
