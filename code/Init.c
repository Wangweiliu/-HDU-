#include<code/Init.h>

void Init_Parameter()
{
    PWMOut = 0;
    PidParameter_Init_AfterRunning(&CurveDirectionPID);
    PidParameter_Init_AfterRunning(&SpeedPID);
    AngleofRoundabout=0;//���������ǻ�������
    RoadLength=0;//�������������Ȼ�������

    TrackState = -1;//��·״������
    ZebraBeforeStopping = 0;//������ͣ����־λ����

    SpeedPID.kp = KpofCommon;//�ٶȻ���ʼ��
    SpeedPID.ki = KiofCommon;
    SpeedPID.kd = KdofCommon;
    flyFrame = 0;
    RescueReset();
}
