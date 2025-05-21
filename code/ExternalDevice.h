#ifndef CODE_EXTERNALDEVICE_H_
#define CODE_EXTERNALDEVICE_H_

#include "zf_common_headfile.h"

//���Ŷ���
#define KEY_LIST               {P33_4, P33_6, P33_5, P33_8, P33_7, P13_0, P13_1}

#define ServoMotorPWM          ATOM0_CH1_P21_3
#define ServoMotorFREQ         100//��Χ 50-300

#define PWMRetreat             ATOM1_CH2_P21_4
#define PWMAdvance             ATOM1_CH3_P21_5
#define MotorEnble             P21_6

#define BUZZ                   P33_13

#define LED0p                  P33_9
#define LED1p                  P33_10
#define LED2p                  P33_11
#define LED3p                  P33_12
//���Ŷ���


/***************************���****************************/
extern float BatteryVoltage;
void Get_Battery(void);
/***************************���****************************/


/***************************���****************************/
extern float _ServoDutyfactor;
void Servo_Init(void);
void Servo_Constrain(float* angle);
void Set_Angel(uint16 angle);
/***************************���****************************/


/***************************���****************************/
extern uint8 MotorFlag;
extern uint8 MotorPower;
extern uint32 PWMOut;
extern int32 PWMMax;
extern int32 PWMMin;
void Motor_Init(void);
void Set_MoterSpeed(int32 MotorSpeed);
/***************************���****************************/


/***************************������****************************/
#define EncoderValue_to_RealSpeed  1.0
extern int16 EncoderValue;
extern float CarSpeed;
extern float RoadLength;
void Cal_RoadLength(float T);
void Encoder_Init(void);
void EncoderValue_Get(void);
/***************************������****************************/


/***************************������****************************/
void Get_GyroValue(void);
void Cal_Angel(float T);
int Ramp_Detection(void);
extern float AngleofRoundabout;//��һȦΪ282~289(���������˲�)
extern float CourseAngle;
extern float alpha; // �����˲�ϵ�����ɸ�����Ҫ����
extern float Gyro_X;
extern float Gyro_Y;
extern float Gyro_Z;
extern float Acc_X;
extern float Acc_Y;
extern float Acc_Z;
/***************************������****************************/


/***************************������****************************/
typedef enum
{
    BUZZ0=0,

} BUZZn_e;
typedef enum
{
    BUZZ_OFF=0,  //��
    BUZZ_ON=1, //����
    BUZZ_RVS=2, //��ת
}BUZZs_e;
void Buzz_Init(void);
void Buzz_Ctrl(BUZZn_e BUZZno, BUZZs_e sta);
void ElemBuzz(void);
/***************************������****************************/


/***************************LED****************************/
typedef enum
{
    LED0=0,
    LED1=1,
    LED2=2,
    LED3=3,
    LEDALL=4
} LEDn_e;
typedef enum
{
    ON=0,  //��
    OFF=1, //��
    RVS=2, //��ת
}LEDs_e;
void Led_Init(void);
void Led_Ctrl(LEDn_e LEDno, LEDs_e sta);
/***************************LED****************************/

#endif
