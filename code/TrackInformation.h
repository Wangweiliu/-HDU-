#ifndef CODE_TRACKINFORMATION_H_
#define CODE_TRACKINFORMATION_H_

#include "zf_common_headfile.h"

enum roadType
{
    BaseHandle = 0,     // ����Ͷ�ֱ��
    BaseHandle_speedup, // ��ֱ���ͳ�ֱ��
    BaseHandle_down,
    BaseHandle_wan,
    Stop,               // ͣ��
    RuHuan_Small,       // �뻷
    HuanZhong_Small,    // ����
    ChuHuan_Small,      // ����
    RuHuan_Big,         // �뻷
    HuanZhong_Big,      // ����
    ChuHuan_Big,        // ����
    BigRCurve,          // С������
    Ramp,               // �µ�
        Ramp_fly,       // ���µ�
    ZebraCrossing,      // ������

    RescueScene,        //��Ԯ��
        RescueHandle_L, // ��Ԯ����ת
        RescueHandle_R, // ��Ԯ����ת
    DangerScene,        // Σ����
    RacingScene,        // ׷����
    SlowDown,
    SlowDown_big,
};


typedef struct
{
    uint16 LeftDaJiaoIN;
    uint16 RightDaJiaoIN;
    uint16 LeftDaJiaoOUT;
    uint16 RightDaJiaoOUT;
    float BeginDistance;
    float ForwardDistance;
    float ForwardDistance_Right;
    uint8  StopFinishflag;
    uint8 step;
    uint8 Direction;
    uint8 RescueHandle_LR_FLAG;
    uint16 StopFrame;
    uint16 S_sec;

    void(*Gddaojiao_judge)(void);
    void(*RescueReset)(void);
    void(*RescueInit)(void);

    enum RescueHandle_LR_Step
    {
        Nonee = 0,
        Forwardd,
        Stopp,
        Backk,
        Waitt,
    };
    enum Dir
    {
        middle = 0,
        left,
        right,
    };
}RescueHandle_LRmain;
extern RescueHandle_LRmain rescuehandle_lrmain;
void RescueInit(void);
void Gddaojiao_judge(void);
void RescueReset(void);


extern uint8 DataFromEdgeBoard;
extern int16 TrueDirError;
extern int16 LastTrueDirError;
extern int16 TrackState;
extern int16 LastTrackState;
extern int16 UpMissline;

extern uint8 ElemLists_Array[9];
extern uint8 ElemLists_Array_Index;
extern uint8 IFSend[3];

extern uint8 ZebraBeforeRunning;
extern uint8 ZebraBeforeStopping;
extern uint8 Zebra_Distance;

float SqrtByCarmack( float number );
void ZebraControl(void);
void ZebraCrossing_Process(void);

#endif
