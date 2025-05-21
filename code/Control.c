#include <code/Control.h>
/***************************PID*****************************/
PID_struct SpeedPID;
PID_struct CurveDirectionPID;


float ConstrainFloat(float amt, float low, float high)
{
    return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}

void PidValue(void)
{
    CurveDirectionPID.kp = 2.4;
    CurveDirectionPID.ki = 0;
    CurveDirectionPID.kd = 12.0;
    CurveDirectionPID.imax = 200;

    SpeedPID.kp = 100.0;
    SpeedPID.ki = 5.00;
    SpeedPID.kd = 0;
}

void Pid_Init(PID_struct * pid)
{
    pid->kp        = 0;
    pid->ki        = 0;
    pid->kd        = 0;
    pid->imax      = 0;
    pid->out_p     = 0;
    pid->out_i     = 0;
    pid->out_d     = 0;
    pid->out       = 0;
    pid->integrator= 0;
    pid->last_error= 0;
    pid->last_derivative   = 0;
    pid->last_t    = 0;
}

void PidParameter_Init_AfterRunning(PID_struct * pid)
{
    pid->out_p     = 0;
    pid->out_i     = 0;
    pid->out_d     = 0;
    pid->out       = 0;
    pid->integrator= 0;
    pid->last_error= 0;
    pid->last_derivative   = 0;
    pid->last_t    = 0;
}

float Pid_LocCtrl(PID_struct * pid, float error)//λ��ʽ
{
    pid->integrator += error;
    ConstrainFloat(pid->integrator, -pid->imax, pid->imax);//�����޷�
    pid->out_p = pid->kp * error;
    pid->out_i = pid->ki * pid->integrator;
    pid->out_d = pid->kd * (error - pid->last_error);
    pid->last_error = error;
    pid ->out = pid->out_p + pid->out_i + pid->out_d;
    return pid->out;
}


float Pid_IncCtrl(PID_struct * pid, float error)//����ʽ
{
    pid->out_p = pid->kp * (error - pid->last_error);
    pid->out_i = pid->ki * error;
    pid->out_d = pid->kd * ((error - pid->last_error) - pid->last_derivative);
    pid->last_derivative = error - pid->last_error;
    pid->last_error = error;
    pid->out += pid->out_p + pid->out_i + pid->out_d;
    return pid->out;
}

void Pid_update(PID_struct* pid ,float error)
{
    //���ַ���
    if(error >= 70) pid->ki = 1.2;
    else if(error >= 50) pid->ki = KiofCommon/3 * 1;
    else if(error >= 30) pid->ki = KiofCommon/3 * 2;
    else pid->ki = KiofCommon/3 * 3;
    //���ٻ���


}
/***************************PID*****************************/


/***************************ǰ��*****************************/
uint8 IfOpenFF = 1;
FF_struct SpeedFF;

void FF_Init(FF_struct *FF,float KP,float KD,float Filter_Coe)
{
    FF->KP=KP;
    FF->KD=KD;
    FF->Filter_Coe=Filter_Coe;
    FF->In=0;
    FF->Last_In=0;
    FF->Filter_In=0;
    FF->Out=0;
}

void FF_Calculate(FF_struct *FF,float Input)
{
    FF->In=Input;
    FF->Filter_In=FF->Filter_Coe*FF->Filter_In+(1-FF->Filter_Coe)*FF->Last_In;
    FF->Out=FF->In*FF->KP+(FF->In-FF->Filter_In)*FF->KD;
    FF->Last_In=FF->In;
}
/***************************ǰ��*****************************/


/***************************ģ������*****************************/
uint8 IfOpenFuzzy=1;
int rule_p[7][7]=
{
    { 6 , 5 , 4 , 4 , 3 , 1 , 0},//-36
    { 5 , 4 , 3 , 3 , 2 , 0 , 0},//-24
    { 3 , 3 , 2 , 1 , 0 , 1 , 2},//-12
    { 2 , 1 , 1 , 0 , 1 , 1 , 2},//0
    { 2 , 1 , 0 , 1 , 2 , 3 , 3},//12
    { 0 , 0 , 2 , 3 , 3 , 4 , 5},//24
    { 0 , 1 , 3 , 4 , 4 , 5 , 6},//36
};//ģ������� P
//int rule_d[7] = { 6 , 5 , 3 , 2 , 3 , 5 , 6};//ģ������� D
int rule_d[7] = { 6 , 4 , 2 , 1 , 2 , 4 , 6};//ģ������� D

float EFF[7]={-30,-20,-14,0,14,20,30};
float DFF[7]={-10,-5,-3,0,3,5,10};

//float Kp_UFF[7]={1.34, 1.34, 2.66, 4.0, 5.34, 6.66, 8.0};
//float Kd_UFF[7]={5.34, 5.34, 10.66, 16.0, 21.34, 26.64, 32.0};
float Kp_UFF[7]={0.67, 0.67, 1.33, 2.0, 2.67, 3.33, 4.0};
float Kd_UFF[7]={2.67, 2.67, 5.33, 8.0, 10.67, 13.33, 16.0};

float KP_Fuzzy(float E,float EC)
{
    float U=0;
    float PF[2]={0},DF[2]={0},UF[4]={0};
    /*ƫ��,ƫ��΢���Լ����ֵ��������*/
    int Pn=0,Dn=0,Un[4]={0};
    float t1=0,t2=0,t3=0,t4=0;
    /*�����ȵ�ȷ��*/
    /*����PD��ָ������ֵ�����Ч������*/
    if(E>EFF[0] && E<EFF[6])
    {
        if(E<=EFF[1])
        {
            Pn=-2;
            PF[0]=(EFF[1]-E)/(EFF[1]-EFF[0]);
        }
        else if(E<=EFF[2])
        {
            Pn=-1;
            PF[0]=(EFF[2]-E)/(EFF[2]-EFF[1]);
        }
        else if(E<=EFF[3])
        {
            Pn=0;
            PF[0]=(EFF[3]-E)/(EFF[3]-EFF[2]);
        }
        else if(E<=EFF[4])
        {
            Pn=1;
            PF[0]=(EFF[4]-E)/(EFF[4]-EFF[3]);
        }
        else if(E<=EFF[5])
        {
            Pn=2;
            PF[0]=(EFF[5]-E)/(EFF[5]-EFF[4]);
        }
        else if(E<=EFF[6])
        {
            Pn=3;
            PF[0]=(EFF[6]-E)/(EFF[6]-EFF[5]);
        }
    }

    else if(E<=EFF[0])
    {
        Pn=-2;
        PF[0]=1;
    }
    else if(E>=EFF[6])
    {
        Pn=3;
        PF[0]=0;
    }

    PF[1]=1-PF[0];


    //�ж�D��������
    if(EC>DFF[0]&&EC<DFF[6])
    {
        if(EC<=DFF[1])
        {
            Dn=-2;
            DF[0]=(DFF[1]-EC)/(DFF[1]-DFF[0]);
        }
        else if(EC<=DFF[2])
        {
            Dn=-1;
            DF[0]=(DFF[2]-EC)/(DFF[2]-DFF[1]);
        }
        else if(EC<=DFF[3])
        {
            Dn=0;
            DF[0]=(DFF[3]-EC)/(DFF[3]-DFF[2]);
        }
        else if(EC<=DFF[4])
        {
            Dn=1;
            DF[0]=(DFF[4]-EC)/(DFF[4]-DFF[3]);
        }
        else if(EC<=DFF[5])
        {
            Dn=2;
            DF[0]=(DFF[5]-EC)/(DFF[5]-DFF[4]);
        }
        else if(EC<=DFF[6])
        {
            Dn=3;
            DF[0]=(DFF[6]-EC)/(DFF[6]-DFF[5]);
        }
    }
    //���ڸ�����������
    else if (EC<=DFF[0])
    {
        Dn=-2;
        DF[0]=1;
    }
    else if(EC>=DFF[6])
    {
        Dn=3;
        DF[0]=0;
    }
    DF[1]=1-DF[0];



    Un[0]=rule_p[Pn+2][Dn+2];
    Un[1]=rule_p[Pn+3][Dn+2];
    Un[2]=rule_p[Pn+2][Dn+3];
    Un[3]=rule_p[Pn+3][Dn+3];

    UF[0]=PF[0]*DF[0];
    UF[1]=PF[1]*DF[0];
    UF[2]=PF[0]*DF[1];
    UF[3]=PF[1]*DF[1];

    t1=UF[0]*Kp_UFF[Un[0]];
    t2=UF[1]*Kp_UFF[Un[1]];
    t3=UF[2]*Kp_UFF[Un[2]];
    t4=UF[3]*Kp_UFF[Un[3]];
    U = t1+t2+t3+t4;

    return U;
}

float Kd_Fuzzy(float EC)
{
    float out=0;
    uint8 i=0;
    float degree_left = 0,degree_right = 0;
    uint8 degree_left_index = 0,degree_right_index = 0;

    if(EC<DFF[0])
    {
        degree_left = 1;
        degree_right = 0;
        degree_left_index = 0;
    }
    else if (EC>DFF[6]) {
        degree_left = 1;
        degree_right = 0;
        degree_left_index = 6;
    }
    else {
        for(i=0;i<6;i++)
        {
            if(EC>=DFF[i]&&EC<DFF[i+1])
            {
                degree_left = (float)(DFF[i+1] - EC)/(DFF[i+1] - DFF[i]);
                degree_right = 1 - degree_left;
                degree_left_index = i;
                degree_right_index = i+1;
                break;
            }
        }
    }

    out = Kd_UFF[rule_d[degree_left_index]]*degree_left+Kd_UFF[rule_d[degree_right_index]]*degree_right;

    return out;
}
/***************************ģ������*****************************/


/***************************ADRC����*****************************/

//����������11��������Ҫ�û������Ĳ���
/****************TD**********/
float r = 50,//���ٸ�������
      h = 0.005;//�˲�����,ϵͳ���ò������˲����Ӿ���ȡֵΪADRC�������ڣ�����5ms����һ�Σ�h����0.005

/**************ESO**********/
float b       = 0,//ϵͳϵ��
      delta   = 0,//deltaΪfal��e��alpha��delta������������������
      belta01 = 1,//����״̬�۲�����������1
      belta02 = 1,//����״̬�۲�����������2
      belta03 = 1;//����״̬�۲�����������3

/**************NLSEF*******/
float alpha1 = 0.1,//
      alpha2 = 0,//
      belta1 = 1,//���������ź�����
      belta2 = 0,//����΢���ź�����,����ջ�����PI����
      belta3 = 0.1;//�����ź�����

/************************************sign����***************************/
float _sign(float x)//���ź���
{
    if(x>0)
        return 1;
    else if(x<0)
        return -1;
    else
        return 0;
}
/*****************************fhan����*********************************/
float fhan(float x1,float x2,float r,float h)
{
 /*****************************��һ��************************/
//  float d    = 0,
//        a    = 0,
//        a0   = 0,
//        a1   = 0,
//        a2   = 0,
//        y    = 0,
//        fhan = 0;
//
//  d    =  r*h*h;
//  a0   =  h*x2;
//  y    =  x1+a0;
//  a1   =  sqrtf(d*(d+8*fabsf(y)));
//  a2   =  a0 + sign(y)*(a1 - d)/2.0;
//  a    =  (a0+y)*(sign(y+d)-sign(y-d))/2.0  + a2*(1-(sign(y+d)-sign(y-d))/2.0);
//  fhan = -r*(a/d)*(sign(y+d)-sign(y-d))/2.0 - r*sign(a)*(1-(sign(a+d)-sign(a-d))/2.0);
 /******************************�ڶ���**********************/
    float deltaa  =0,
          deltaa0 =0,
          y       =0,
          a0      =0,
          a       =0,
          fhan    =0;

    deltaa = r*h;
    deltaa0 = deltaa*h;
    y=x1+x2*h;
    a0 = sqrtf(deltaa*deltaa+8*r*fabsf(y));
    if(fabsf(y)<=deltaa0)
        a=x2+y/h;
    else
        a=x2+0.5*(a0-deltaa)*_sign(y);
    if(fabsf(a)<=deltaa)
        fhan = -r*a/deltaa;
    else
        fhan = -r*_sign(a);

  return fhan;
}
/*******************************fal����**********************************/
float fal(float e,float alpha,float delta)
{
  float result = 0,fabsf_e = 0;

  fabsf_e = fabsf(e);

  if(delta>=fabsf_e)
    result = e/powf(delta,1.0-alpha);
  else //if(delta<fabsf_e)
    result = powf(fabsf_e,alpha)*_sign(e);

 return result;
}
//�м������������Ҫ�û������Լ���ֵ
/****************TD*******************/
float x1 = 0,//��������
      x2 = 0,//���������΢��
/****************ESO******************/
      e  = 0,//���
      z1 = 0,//���ٷ���ֵ
      z2 = 0,//���ٷ���ֵ�Ķ�΢��
      z3 = 0,//����ϵͳ���Ŷ������Ŷ���
/**************NLSEF******************/
      u = 0,//���ֵ
      ui = 0; // ������
/********************************ADRC************************************/
float ADRC(float v,float y)  //  ������v�������Ŀ��ֵ��  y������ֵ
{
  float u0 = 0,
        e1 = 0,
        e2 = 0;
/******************************TD****************************************/
  x1 = x1 + h*x2;
  x2 = x2 + h*fhan(x1-v,x2,r,h);
/******************************ESO***************************************/
  e = z1 - y;
  z1 = z1 + h*(z2-belta01*e);
  z2 = z2 + h*(z3-belta02*fal(e,0.5,delta)+b*u);
  z3 = z3 + h*(-belta03*fal(e,0.25,delta));
/******************�޷���ADRC�����Ļ�����ﵽ�޷�����********************/
  if(z1>=30000) z1=30000;
  if(z1<=-30000) z1 = -30000;
  if(z2>=30000) z2=30000;
  if(z2<=-30000) z2 = -30000;
  if(z3>=30000) z3=30000;
  if(z3<=-30000) z3 = -30000;
/******************************NLSEF*************************************/
  e1 = x1 - z1;
  e2 = x2 - z2;
  ui += h*e;

  u0 = belta1*fal(e1,alpha1,delta) + belta2*fal(e2,alpha2,delta) + belta3*ui;//����0<alpha1<1<alpha2

  u = u0 - z3/b;

  return u;
}
/***************************ADRC����*****************************/

/***************************���ջ��л�*****************************/
uint8 OpenOrClosedFlag=1;
/***************************���ջ��л�*****************************/
