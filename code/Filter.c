#include <code/Filter.h>

uint8 IfOpenKalman = 1;
KFP_Typedef CarSpeedAfterFilter;
KFP_Typedef GyroZAfterFilter;
KFP_Typedef GyroYAfterFilter;

void Kalman_Init(KFP_Typedef *kfp)
{
    kfp->Kg=0;//����������
    kfp->LastP=0.02;
    kfp->Now_P=0;
    kfp->Q=0.05;//��������
    kfp->R=1.0;//�۲�����
    kfp->out=0;
}

float KalmanFilter(KFP_Typedef *kfp,float input)
{
    //Ԥ��Э����̣�kʱ��ϵͳ����Э���� = k-1ʱ�̵�ϵͳЭ���� + ��������Э����
    kfp->Now_P = kfp->LastP + kfp->Q;

    //���������淽�̣����������� = kʱ��ϵͳ����Э���� / ��kʱ��ϵͳ����Э���� + �۲�����Э���
    kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);

    //��������ֵ���̣�kʱ��״̬����������ֵ = ״̬������Ԥ��ֵ + ���������� * ������ֵ - ״̬������Ԥ��ֵ��
    kfp->out = kfp->out + kfp->Kg * (input -kfp->out);//��Ϊ��һ�ε�Ԥ��ֵ������һ�ε����ֵ

    //����Э�����: ���ε�ϵͳЭ����� kfp->LastP Ϊ��һ������׼����
    kfp->LastP = (1-kfp->Kg) * kfp->Now_P;

    return kfp->out;
}
