#ifndef FILTER_H_
#define FILTER_H_

#include "zf_common_headfile.h"

typedef struct
{
    float LastP;//�ϴι���Э���� ��ʼ��ֵΪ0.02
    float Now_P;//��ǰ����Э���� ��ʼ��ֵΪ0
    float out;//�������˲������ ��ʼ��ֵΪ0
    float Kg;//���������� ��ʼ��ֵΪ0
    float Q;//��������Э���� ��ʼ��ֵΪ0.001
    float R;//�۲�����Э���� ��ʼ��ֵΪ0.543
}KFP_Typedef;//Kalman Filter parameter

extern uint8 IfOpenKalman;
extern KFP_Typedef CarSpeedAfterFilter;
extern KFP_Typedef GyroZAfterFilter;
extern KFP_Typedef GyroYAfterFilter;
void Kalman_Init(KFP_Typedef *kfp);
float KalmanFilter(KFP_Typedef *kfp,float input);

#endif


